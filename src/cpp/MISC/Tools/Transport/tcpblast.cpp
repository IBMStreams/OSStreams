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
#include <UTILS/ConsoleReader.h>
#include <UTILS/DistilleryApplication.h>
#include <iostream>
#include <memory>

using namespace std;
UTILS_NAMESPACE_USE;

volatile bool done = false;

void alarm_handler(int sig)
{
    done = true;
}

class datablast : public DistilleryApplication
{

  protected:
    unsigned int _size;
    unsigned int _count;
    unsigned int _duration;
    bool _csv;
    bool _latency;
    bool _secure;
    unsigned int _submitratemult;
    bool _progress;

  public:
    datablast(void)
      : _size(1024)
      , _count(0)
      , _duration(10)
      , _csv(false)
      , _latency(false)
      , _secure(false)
      , _submitratemult(1)
      , _progress(false)
    {
        setKeepRemainingArguments();
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 's', "size", ARG, "", "Size of each transmit (1KB default)",
              INT_OPT(datablast::setSize) },
            { 'n', "count", ARG, "", "Number of iterations", INT_OPT(datablast::setCount) },
            { 't', "time", ARG, "", "Time to run in seconds (10s default)",
              INT_OPT(datablast::setTime) },
            { 'c', "csv", 0, "", "Use CSV output", INT_OPT(datablast::setCSV) },
            { 0, "latency", ARG, "", "Latency measurements", INT_OPT(datablast::setLatency) },
            { 0, "progress", 0, "", "Display Progress", INT_OPT(datablast::setProgress) },
            { 0, "secure", 0, "TCPSERVER_SECURE", "Use TLS TCP", INT_OPT(datablast::setSecure) }
        };

        APPEND_OPTIONS(options, args);
    }

    void setSize(const option_t* option, int value) { _size = value; }

    void setCount(const option_t* option, int value)
    {
        _count = value;
        _duration = 0;
    }

    void setTime(const option_t* option, int value)
    {
        _duration = value;
        _count = 0;
    }

    void setCSV(const option_t* option, int value) { _csv = true; }

    void setLatency(const option_t* option, int value)
    {
        _latency = true;
        _submitratemult = value;
    }

    void setSecure(const option_t* option, int value) { _secure = true; }

    void setProgress(const option_t* option, int value) { _progress = true; }

    void printStat(double d, uint64_t count)
    {
        double t = d / 1000000.0;
        uint64_t bits = (unsigned long long)count * (unsigned long long)_size;
        bits *= 8LL;

        if (_csv) {
            cout << t << "," << _size << "," << count << "," << (double)count / t << ","
                 << (double)bits / t << endl;
        } else {
            cout << "[tcpblast] Time : " << t << " sec"
                 << " " << count << " pkt" << endl;
            cout << "[tcpblast] Count: " << (double)count / t << " pkt/s" << endl;
            cout << "[tcpblast] Bit  : " << (double)bits / t << " bit/s" << endl;
        }
    }

    static inline double systime()
    {
        struct timespec ts;
        clock_gettime(CLOCK_REALTIME, &ts);
        return ts.tv_sec + (ts.tv_nsec * 1e-9);
    }

    virtual int run(const vector<string>& args)
    {
        signal(SIGPIPE, SIG_IGN);
        signal(SIGALRM, alarm_handler);

        if (args.size() != 1 && args.size() != 2) {
            cerr << "[tcpblast] Missing arguments: '<host port> or <ns label>'" << endl;
            exit(1);
        }

        auto_ptr<DataSender> data_sender;

        if (args.size() == 2) {
            TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
            TCPInstance::instance()->setSecurityDirectory("./");
            data_sender.reset(TCPInstance::instance()->newSender(args[0], atoi(args[1].c_str()),
                                                                 TCPInstance::Static));
        } else {
            TCPInstance::instance()->setSecureMethod(_secure ? "TLSv1.2" : "none");
            TCPInstance::instance()->setSecurityDirectory("./");
            data_sender.reset(TCPInstance::instance()->newSender(
              DataSender::PortDescription(args[0]), TCPInstance::Static));
        }

        data_sender->connect();

        if (_duration) {
            alarm(_duration);
        }

        unsigned int count = 0;

        unsigned long long ts_begin = debug::TimeStamp::TOD_us();
        if (_duration) {
            if (_latency) {
                double timestamp, nexttime = 0;
                int submitrate = 1024 * _submitratemult;
                double minsep = 1.0 / submitrate;
                while (!done) {
                    while (1) {
                        timestamp = systime();
                        if (timestamp >= nexttime) {
                            break;
                        }
                        sched_yield();
                    }
                    data_sender->write(&timestamp, sizeof(double));
                    nexttime = timestamp + minsep;
                    count++;
                }
                data_sender->write("quit", sizeof("quit") + 1);
            } else {
                char* buffer = new char[_size];

                for (unsigned int i = 0; i < _size; i++) {
                    buffer[i] = 'a' + (i % 26);
                }

                if (_progress) {
                    while (_duration) {
                        ts_begin = debug::TimeStamp::TOD_us();
                        count = 0;
                        done = false;
                        alarm(1);

                        while (!done) {
                            data_sender->write(buffer, _size);
                            count++;
                        }

                        printStat(debug::TimeStamp::TOD_us() - ts_begin, count);

                        _duration--;
                    }
                } else {
                    while (!done) {
                        data_sender->write(buffer, _size);
                        count++;
                    }
                }

                delete[] buffer;
            }
        } else {
            if (_latency) {
                cerr << "[tcpblast] Not implemented yet" << endl;
            } else {
                char* buffer = new char[_size];

                for (unsigned int x = 0; x < _size; x++) {
                    buffer[x] = 'a' + (x % 26);
                }

                for (unsigned int i = 0; i < _count; i++) {
                    data_sender->write(buffer, _size);
                }

                delete[] buffer;
            }
        }

        printStat(debug::TimeStamp::TOD_us() - ts_begin, count);

        data_sender->shutdown();
        usleep(1000000); // linger while transport is shutting down

        return 0;
    }
};

MAIN_APP(datablast);
