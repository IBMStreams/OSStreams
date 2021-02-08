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

#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/times.h>

#include <iostream>
#include <sstream>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>

using namespace std;
UTILS_NAMESPACE_USE;

volatile bool done = false;

void alarm_handler(int sig)
{
    done = true;
}

#define MEMCPY(dst, src, size) memcpy(dst, src, size)

class memorybench : public DistilleryApplication
{

  public:
    int _size;
    int _time;
    bool _csv;
    bool _malloc;

    memorybench(void)
      : _size(4096)
      , _time(10)
      , _csv(false)
      , _malloc(false)
    {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 's', "size", ARG, "", "Size of the block to allocate in bytes",
              INT_OPT(memorybench::setSize) },
            { 't', "time", ARG, "", "Duration of the test in seconds",
              INT_OPT(memorybench::setTime) },
            { 'c', "csv", 0, "", "Output in CSV format", INT_OPT(memorybench::setCSV) },
            { 'm', "malloc", 0, "", "Use libc malloc", INT_OPT(memorybench::setMalloc) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setSize(const option_t* option, int value) { _size = value; }

    void setTime(const option_t* option, int value) { _time = value; }

    void setCSV(const option_t* option, int value) { _csv = true; }

    void setMalloc(const option_t* option, int value) { _malloc = true; }

    virtual int run(const vector<string>& remainings_args)
    {
        signal(SIGALRM, alarm_handler);

        void* static_block = malloc(_size);

        alarm(_time);

        unsigned long long total_size = 0LL;
        unsigned long long total_count = 0LL;

        struct timeval tv;

        gettimeofday(&tv, NULL);
        double begin = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        if (_malloc && !total_count) {
            while (!done) {
                void* block = malloc(_size);
                MEMCPY(block, static_block, _size);
                free(block);
                total_size += _size;
                total_count++;
            }
        }

        if (!total_count) {
            while (!done) {
                unsigned char* block = new unsigned char[_size];
                MEMCPY(block, static_block, _size);
                delete[] block;
                total_size += _size;
                total_count++;
            }
        }

        gettimeofday(&tv, NULL);
        double end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        struct tms tms;
        times(&tms);

#define DATA(name, value)                                                                          \
    do {                                                                                           \
        if (_csv) {                                                                                \
            cout << value << ",";                                                                  \
        } else {                                                                                   \
            cout << name " = " << value << endl;                                                   \
        }                                                                                          \
    } while (0)

        DATA("Allocation unit", _size);
        DATA("total_size", (double)total_size);
        DATA("total_count", (double)total_count);
        DATA("alloc/s", (double)total_count);
        DATA("byte/s", (double)total_size / (end - begin));
        DATA("user", ((double)tms.tms_utime / sysconf(_SC_CLK_TCK)) / (end - begin));
        DATA("system", ((double)tms.tms_stime / sysconf(_SC_CLK_TCK)) / (end - begin));

        if (_csv) {
            cout << endl;
        }

        return 0;
    }
};

MAIN_APP(memorybench);
