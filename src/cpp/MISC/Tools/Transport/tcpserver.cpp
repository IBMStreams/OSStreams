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

#include <TRANS/TCPInstance.h>
#include <TRANS/TCPReceiver.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/auto_array.h>
#include <boost/algorithm/string.hpp>
#include <iostream>
#include <memory>

using namespace std;
UTILS_NAMESPACE_USE;

class datareceiver
  : public DistilleryApplication
  , public DataReceiver::Callback
{

  protected:
    unsigned short _tcp_port;
    set<string> _senders;
    vector<DataReceiver::port_data_t> _ports;
    bool _quiet;
    uint32_t _port_flag;
    bool _no_wait;
    bool _latency;
    bool _secure;
    uint64_t _warmup_count;
    uint64_t _latency_count;
    double _latency_total;
    double _latency_min;
    double _latency_max;
    double _starttime;
    double _endtime;
    DataReceiver* _data_recv;

  public:
    datareceiver(void)
      : _tcp_port(0)
      , _senders()
      , _quiet(false)
      , _port_flag(0)
      , _no_wait(false)
      , _latency(false)
      , _secure(false)
      , _warmup_count(0)
      , _latency_count(0)
      , _latency_total(0)
      , _latency_min(1024 * 1024 * 1024)
      , _latency_max(0)
      , _starttime(0)
      , _endtime(0)
      , _data_recv(NULL)
    {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'p', "port", ARG, "", "Use the specified TCP port",
              INT_OPT(datareceiver::setTCPPort) },
            { 'c', "clients", ARG, "", "Comma-separated list of clients to expect",
              STR_OPT(datareceiver::setClients) },
            { 'n', "name", ARG, "", "Register with the name server",
              STR_OPT(datareceiver::setName) },
            { 'q', "quiet", 0, "", "Be quiet", INT_OPT(datareceiver::setQuiet) },
            { 'w', "no-wait", 0, "", "Dont call wait", INT_OPT(datareceiver::setNoWait) },
            { 0, "latency", 0, "", "Latency measurements", INT_OPT(datareceiver::setLatency) },
            { 0, "secure", 0, "TCPSERVER_SECURE", "Use TLS TCP", INT_OPT(datareceiver::setSecure) }
        };

        APPEND_OPTIONS(options, args);
    }

    void setTCPPort(const option_t* option, int value) { _tcp_port = value; }

    void setClients(const option_t* option, const char* value)
    {
        /*
         * Parse the string of senders in the format: sender1,sender2,sender3.
         */
        vector<string> senders;
        boost::split(senders, value, boost::is_any_of(","), boost::token_compress_on);
        /*
         * Insert all names in the sender set.
         */
        vector<string>::iterator it;
        for (it = senders.begin(); it != senders.end(); it++) {
            _senders.insert(*it);
        }
    }

    void setName(const option_t* option, const char* value)
    {
        DataReceiver::port_data_t pd;
        memset(&pd, 0, sizeof(pd));
        pd.ns_label = StrDup(value);
        pd.user_data.u32 = _port_flag;
        pd.recipients = _senders;
        pd.callback = this;
        _ports.push_back(pd);
        _port_flag++;
    }

    void setQuiet(const option_t* option, int value) { _quiet = true; }

    void setNoWait(const option_t* option, int value) { _no_wait = true; }

    void setLatency(const option_t* option, int value) { _latency = true; }

    void setSecure(const option_t* option, int value) { _secure = true; }

    static inline double systime()
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec + (ts.tv_nsec * 1e-9);
    }

    virtual void onConnected(const unsigned long long portId, PortLabel const& pl, const bool opt)
    {}

    virtual void onDisconnected(const unsigned long long portId,
                                PortLabel const& pl,
                                const bool opt)
    {}

    virtual void onMessage(void* buffer, uint32_t size, const DataReceiver::user_data_t& user_data)
    {
        if (!_quiet) {
            cout << "[tcpserver] <" << user_data.u32 << "> " << (char*)buffer << " <" << size << ">"
                 << endl;
        }

        if (strcmp((char*)buffer, "quit") == 0) {
            _data_recv->shutdown();
            return;
        }

        if (_latency) {
            // Skip the first 1024 packets
            _warmup_count++;
            if (_warmup_count > 1024) {
                double t1 = systime();
                double* t0 = (double*)buffer;
                if (t1 > *t0) {
                    double l = (t1 - *t0) * 1e6;
                    _latency_count++;
                    _latency_total += l;
                    if (l < _latency_min) {
                        _latency_min = l;
                    }
                    if (l > _latency_max) {
                        _latency_max = l;
                    }
                }
            }
        }
    }

    virtual int run(const vector<string>& remainings_args)
    {
        auto_ptr<DataReceiver> data_recv;

        TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
        TCPInstance::instance()->setSecurityDirectory("./");
        data_recv.reset(new TCPReceiver(_ports));

        cout << "[tcpserver] Listening on several ports" << endl;

        _data_recv = data_recv.get();

        if (!_no_wait) {
            data_recv->wait();
        }

        cout << "[tcpserver] Ready" << endl;

        data_recv->run();

        if (_latency) {
            cout << "[tcpserver] Count: " << _latency_count << endl;
            cout << "[tcpserver] Avg  : " << _latency_total / _latency_count << " us" << endl;
            cout << "[tcpserver] Min  : " << _latency_min << " us" << endl;
            cout << "[tcpserver] Max  : " << _latency_max << " us" << endl;
        }

        cout << "[tcpserver] Exiting" << endl;

        for (unsigned int i = 0; i < _ports.size(); i++) {
            delete[] _ports[i].ns_label;
        }

        return 0;
    }
};

MAIN_APP(datareceiver);
