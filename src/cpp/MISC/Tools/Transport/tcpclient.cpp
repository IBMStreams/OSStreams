/*
 * Copyright 2021 IBM Corporation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <TRANS/DynDataSender.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/ViewStreamSender.h>
#include <UTILS/ConsoleReader.h>
#include <UTILS/DistilleryApplication.h>

#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <locale>
#include <memory>
#include <utility>
#include <vector>

using namespace std;
UTILS_NAMESPACE_USE;

class ConnectionCallback : public DataSender::ConnectionCallback
{

  private:
    string _cnName;
    int _nBrokenConnections;
    bool _isConnecting;
    bool _announce;

  public:
    ConnectionCallback(const string& cnName, bool announce = true)
      : _cnName(cnName)
      , _nBrokenConnections(0)
      , _isConnecting(false)
      , _announce(announce)
    {}

    ~ConnectionCallback()
    {
        _announce&& cout << "[tcpclient] CBK cnName:" << _cnName
                         << " - callback ctor nBrokenConnections:" << _nBrokenConnections
                         << ", isConnecting:" << _isConnecting << endl;
    }

    void onConnecting(bool isReconnect, bool isOptional)
    {
        if (_isConnecting) {
            _announce&& cout << "[tcpclient] CBK ERROR: already isConnecting! "; // prefix next msg
        }
        _announce&& cout << "[tcpclient] CBK cnName:" << _cnName
                         << " - onConnecting(isReconnect:" << isReconnect
                         << ", isOptional:" << isOptional << ")" << endl;
        isReconnect&& _nBrokenConnections++;
        _isConnecting = true;
    }

    void onConnected(const std::string& ns_label)
    {
        if (!_isConnecting) {
            _announce&& cout << "[tcpclient] CBK ERROR: not is isConnecting! "; // prefix next msg
        }
        _announce&& cout << "[tcpclient] CBK cnName:" << _cnName << " - onConnected()" << endl;
        _isConnecting = false;
    }

    void onDisconnected() { _isConnecting = false; }

    void onClosed() { _isConnecting = false; }
};

/** Special locale that treats commas as white space */
struct csv_reader : std::ctype<char>
{
    csv_reader()
      : std::ctype<char>(get_table())
    {}
    static std::ctype_base::mask const* get_table()
    {
        static std::vector<std::ctype_base::mask> rc(table_size, std::ctype_base::mask());

        rc[','] = std::ctype_base::space;
        rc['\n'] = std::ctype_base::space;
        return &rc[0];
    }
};

class datasender : public DistilleryApplication
{

  protected:
    vector<string> _entries;
    bool _dynamic;
    bool _secure;
    bool _callbacks;
    bool _viewMode; ///< if true, then encode a View Tuple header for the outgoing message
    std::vector<unsigned long long> _viewIds;
    int _channelIndex;
    std::vector<int32_t> _allChannels;

    // dynamic connection labels for which sending tuples is allowed
    set<string> _filters;
    map<DataSender::ConnectionId, string> _filterIds;
    vector<DataSender::ConnectionId> _staticConnectionIds;
    typedef map<DataSender::ConnectionId, string>::iterator FilterIterator;

  public:
    datasender(void)
      : _dynamic(false)
      , _secure(false)
      , _callbacks(false)
      , _viewMode(false)
      , _channelIndex(-1)
    {
        setKeepRemainingArguments();
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'n', "name", ARG, "", "Use glob type of matching", STR_OPT(datasender::setName) },
            { 'd', "dyn", 0, "", "Use dynamic client", INT_OPT(datasender::setDynamic) },
            { 0, "secure", 0, "TCPCLIENT_SECURE", "Use TLS TCP", INT_OPT(datasender::setSecure) },
            { 0, "cbk", 0, "TCPCLIENT_CBK", "Announce callbacks",
              INT_OPT(datasender::setCallbacks) },
            { 0, "filter", ARG, "",
              "Filter in tuples sent to connections identified by the argument.  Works only with a "
              "dynamic client.",
              STR_OPT(datasender::setFilter) },
            { 0, "view", ARG, "", "Encode a View Tuple header for the outgoing data",
              STR_OPT(datasender::setView) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setName(const option_t* option, const char* value) { _entries.push_back(value); }

    void setDynamic(const option_t* option, int value) { _dynamic = true; }

    void setSecure(const option_t* option, int value) { _secure = true; }

    void setCallbacks(const option_t* option, int value) { _callbacks = true; }

    /**
     * Set the view argument.
     *
     * @param value has the format ['c'N,]id1,id2,id3...  where the optional
     * character 'c' and number N indicate the parallel channel number,
     * whereas id1,id2,id3... are the view ids to be associated with each tuple.
     *
     * Example:  c3,2,3,1 = channel 2, view ids 2, 3, 1
     * Example:  3,5,9    = no channel number, view ids 3, 5, 9.
     */
    void setView(const option_t* option, const char* value)
    {
        bool readChannelValue = false;
        stringstream views;

        if (value[0] == 'c') {
            value++;
            readChannelValue = true;
        }

        // imbue() a stream with the csv_reader locale to read view Id values
        views.imbue(std::locale(std::locale(), new csv_reader()));
        views << value;

        // copy parsed values into viewIds
        std::istream_iterator<unsigned long long> eos; // end-of-stream iterator
        for (std::istream_iterator<unsigned long long> it(views); it != eos; it++) {
            if (readChannelValue) {
                _channelIndex = (int)(*it);
                readChannelValue = false;
            } else {
                _viewIds.push_back(*it);
            }
        }

        std::cout << "viewIds" << viewToString(_viewIds) << endl;
        _viewMode = true;
    }

    std::string viewToString(const std::vector<unsigned long long>& viewIds) const
    {
        std::ostringstream s;
        s << "<" << viewIds.size() << ">=<";
        for (std::vector<unsigned long long>::const_iterator i = viewIds.begin();
             i != viewIds.end(); i++) {
            if (i != viewIds.begin()) {
                s << ", ";
            }
            s << *i;
        }
        s << ">";
        return s.str();
    }

    void setFilter(const option_t* option, const char* value) { _filters.insert(string(value)); }

    virtual int run(const vector<string>& remainings_args)
    {
        signal(SIGPIPE, SIG_IGN);

        if (_viewMode) {
            runAsViewSender(remainings_args);
            return 0;
        }

        DataSender* data_sender = NULL;

        if (remainings_args.size() == 2 && _entries.size() == 0) {
            TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
            TCPInstance::instance()->setSecurityDirectory("./");
            data_sender = TCPInstance::instance()->newSender(
              remainings_args[0], atoi(remainings_args[1].c_str()),
              _dynamic ? TCPInstance::Dynamic : TCPInstance::Static);
        } else {
            if (remainings_args.size() == 0 && _entries.size() > 0) {
                if (_entries.size() == 1) {
                    DataSender::PortDescription pd(
                      _entries[0], boost::shared_ptr<DataSender::ConnectionCallback>(
                                     new ConnectionCallback(_entries[0], _callbacks)));
                    TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
                    TCPInstance::instance()->setSecurityDirectory("./");
                    data_sender = TCPInstance::instance()->newSender(
                      pd, _dynamic ? TCPInstance::Dynamic : TCPInstance::Static);
                } else {
                    vector<DataSender::PortDescription> pds;
                    for (unsigned i = 0; i < _entries.size(); i++) {
                        DataSender::PortDescription pd(
                          _entries[i], boost::shared_ptr<DataSender::ConnectionCallback>(
                                         new ConnectionCallback(_entries[i], _callbacks)));
                        pds.push_back(pd);
                    }

                    TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
                    TCPInstance::instance()->setSecurityDirectory("./");
                    data_sender = TCPInstance::instance()->newSender(
                      pds, _dynamic ? TCPInstance::Dynamic : TCPInstance::Static);
                }
            } else {
                cerr << "Missing arguments: '<ip> <port>' or '-n <ns label>'" << endl;
                exit(1);
            }
        }

        if (_dynamic) {
            data_sender = new DynDataSender(data_sender);
        }

        if (_filters.size() > 0) {
            data_sender->getAllConnectionIds(_staticConnectionIds);
            cout << "Static connection ids: "
                 << convert(UTILS_NAMESPACE::toString(_staticConnectionIds)) << std::endl;
        }

        auto_ptr<DataSender> data_sender_auto(data_sender);
        data_sender->connect();

        ConsoleReader cr;
        char* ptr = cr.getCommandPTR("> ");
        while (ptr) {
            if (strncmp(ptr, "big ", 4) == 0) {
                unsigned int size = atoi(ptr + 4);
                char* blob = new char[size];
                memset(blob, 'a', size - 1);
                blob[size] = 0;
                data_sender->write(blob, size);
                delete[] blob;
            } else {
                if (_dynamic && strncmp(ptr, "add ", 4) == 0) {
                    string lbl = ptr + 4;
                    DynDataSender* s = dynamic_cast<DynDataSender*>(data_sender);
                    if (s == NULL) {
                        abort();
                    }
                    DataSender::PortDescription pd(
                      lbl, boost::shared_ptr<DataSender::ConnectionCallback>(
                             new ConnectionCallback(lbl, _callbacks)));
                    DataSender::ConnectionId id = s->addConnection(pd);

                    // If the new connection label is among the ones that are filtered in,
                    // then add the connection id to filters
                    if (_filters.find(lbl) != _filters.end()) {
                        _filterIds.insert(std::make_pair(id, lbl));
                    }

                    if (_filters.size() > 0) {
                        cout << "Connection id " << id << " added for label " << lbl << std::endl;
                        printAllKeys(_filterIds);
                    }
                } else {
                    if (_dynamic && strncmp(ptr, "del ", 4) == 0) {
                        string lbl = ptr + 4;
                        DynDataSender* s = dynamic_cast<DynDataSender*>(data_sender);
                        if (s == NULL) {
                            abort();
                        }

                        s->removeConnection(lbl);

                        // Delete all connection ids for this label
                        if (_filters.find(lbl) != _filters.end()) {
                            vector<DataSender::ConnectionId> ids;
                            findKeys(_filterIds, lbl, ids);
                            for (vector<DataSender::ConnectionId>::iterator it = ids.begin();
                                 it != ids.end(); it++) {
                                _filterIds.erase(*it);
                            }
                        }

                        if (_filters.size() > 0) {
                            printAllKeys(_filterIds);
                        }
                    } else {
                        if (_filters.size() > 0) {
                            // we may have filtered connections: use the filtered write function

                            printOpenConnectionIds(data_sender);

                            // create vector with all the static connections and append the dynamic
                            // connections that are filtered in
                            vector<DataSender::ConnectionId> ids(_staticConnectionIds);
                            copyAllKeys(_filterIds, ids);
                            cout << "Writing to connection ids: "
                                 << convert(UTILS_NAMESPACE::toString(ids)) << std::endl;
                            data_sender->write(ids, ptr, strlen(ptr) + 1);
                        } else {
                            // unfiltered write
                            data_sender->write(ptr, strlen(ptr) + 1);
                        }
                    }
                }
            }

            if (strcmp(ptr, "quit") == 0) {
                // Wait 500ms before shutting down; this allows the receiver enough
                // time to process the "quit" message and initiate shutdown.
                usleep(500000);
                data_sender->shutdown();
                usleep(1000000); // linger while transport is shutting down
                break;
            }

            ptr = cr.getCommandPTR("> ");
        }

        return 0;
    }

    int runAsViewSender(const vector<string>& remainings_args)
    {
        assert(_viewMode);
        validateViewMode();
        std::auto_ptr<ViewStreamSender> sender(ViewStreamSender::newInstance(_entries[0], "none"));
        cout << "<tcpclient> Sending view tuples to name service label " << _entries[0] << endl;

        // write console lines as view tuples until reading "quit" from the console
        unsigned long long tupleSeq = 0;
        ConsoleReader cr;
        for (char* ptr = cr.getCommandPTR("> "); ptr != NULL; ptr = cr.getCommandPTR("> ")) {
            sender->write(_viewIds.data(), _viewIds.size(), _channelIndex, _allChannels.data(),
                          _allChannels.size(), tupleSeq++, ptr, strlen(ptr) + 1);
            if (strcmp(ptr, "quit") == 0) {
                sender->close();
                usleep(1000000); // linger while transport is shutting down
                break;
            }
        }
        return 0;
    }

    void validateViewMode()
    {
        bool valid = true;

        if (_entries.size() != 1) {
            valid = false;
            cerr << "View mode only works when using a name label (see --name option)." << endl;
        }
        if (_dynamic) {
            valid = false;
            cerr << "View mode does not work with the dynamic client." << endl;
        }

        if (!valid) {
            printUsage(cerr, false);
            cerr << endl;
            UTILS_NAMESPACE::DistAppTerminate(1);
        }
    }

    // Copy into vector 'ids' the conn ids from _filterIds
    static void copyAllKeys(map<DataSender::ConnectionId, string>& sourceMap,
                            vector<DataSender::ConnectionId>& dest)
    {
        for (FilterIterator it(sourceMap.begin()); it != sourceMap.end(); it++) {
            dest.push_back(it->first);
        }
    }

    // Find all the map values equal to the specified value and copy their keys into the given
    // vector
    static void findKeys(map<DataSender::ConnectionId, string>& map,
                         string& value,
                         vector<DataSender::ConnectionId>& keys)
    {
        for (FilterIterator it(map.begin()); it != map.end(); it++) {
            if (value == it->second) {
                keys.push_back(it->first);
            }
        }
    }

    // Print all the keys of the given map
    static void printAllKeys(map<DataSender::ConnectionId, string>& map)
    {
        vector<DataSender::ConnectionId> ids;
        copyAllKeys(map, ids);
        cout << "Valid filter connection ids: " << convert(UTILS_NAMESPACE::toString(ids))
             << std::endl;
    }

    static void printOpenConnectionIds(DataSender* sender)
    {
        cout << "Open connection ids: ";
        vector<DataSender::ConnectionId> openIds;
        sender->getAllConnectionIds(openIds);
        cout << convert(UTILS_NAMESPACE::toString(openIds)) << std::endl;
    }

    // Replace all occurrences of [] and {} with <> to simplify regex match
    static string convert(string input)
    {
        string s(input);
        replace(s.begin(), s.end(), '{', '<');
        replace(s.begin(), s.end(), '}', '>');
        replace(s.begin(), s.end(), '[', '<');
        replace(s.begin(), s.end(), ']', '>');
        return s;
    }
};

MAIN_APP(datasender);
