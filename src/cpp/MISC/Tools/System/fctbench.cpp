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

volatile unsigned long long counter = 0LL;

static void static_fct()
{
    counter++;
}

class tester
{
  public:
    void fct() { counter++; }
};

class vtester
{
  public:
    virtual void virtual_fct() = 0;
    virtual ~vtester() {}
};

class subtester : public vtester
{
  public:
    virtual void virtual_fct() { counter++; }
    virtual ~subtester() {}
};

class fctbench : public DistilleryApplication
{

  public:
    int _time;
    bool _csv;

    fctbench(void) : _time(10), _csv(false) {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 't', "time", ARG, "", "Duration of the test in seconds", INT_OPT(fctbench::setTime) },
            { 'c', "csv", 0, "", "Output in CSV format", INT_OPT(fctbench::setCSV) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setTime(const option_t* option, int value) { _time = value; }

    void setCSV(const option_t* option, int value) { _csv = true; }

    virtual int run(const vector<string>& remainings_args)
    {
        signal(SIGALRM, alarm_handler);

        double results[4];

#define DOIT(a)                                                                                    \
    a // ; a ; a ; a ; // a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a
      // ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a
      // ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a ; a
      // ; a ; a ; a ; a ;

        struct timeval tv;

        /***************************************************************/

        counter = 0;
        alarm(_time / 4);
        done = false;

        gettimeofday(&tv, NULL);
        double begin = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        while (!done) {
            DOIT(static_fct());
        }

        gettimeofday(&tv, NULL);
        double end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        results[0] = (double)counter / (end - begin);

        /***************************************************************/

        counter = 0;
        alarm(_time / 4);
        done = false;
        void (*fct_ptr)(void) = static_fct;

        gettimeofday(&tv, NULL);
        begin = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        while (!done) {
            DOIT(fct_ptr());
        }

        gettimeofday(&tv, NULL);
        end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        results[1] = (double)counter / (end - begin);

        /***************************************************************/

        counter = 0;
        tester t;
        alarm(_time / 4);
        done = false;

        gettimeofday(&tv, NULL);
        begin = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        while (!done) {
            DOIT(t.fct());
        }

        gettimeofday(&tv, NULL);
        end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        results[2] = (double)counter / (end - begin);

        /***************************************************************/

        counter = 0;
        vtester* st = new subtester;
        alarm(_time / 4);
        done = false;

        gettimeofday(&tv, NULL);
        begin = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        while (!done) {
            DOIT(st->virtual_fct());
        }

        gettimeofday(&tv, NULL);
        end = (double)tv.tv_sec + (double)tv.tv_usec / 1000000;

        results[3] = (double)counter / (end - begin);

        /***************************************************************/

#define DATA(name, value)                                                                          \
    do {                                                                                           \
        if (_csv) {                                                                                \
            cout << value << ",";                                                                  \
        } else {                                                                                   \
            cout << name " = " << value << endl;                                                   \
        }                                                                                          \
    } while (0)

        DATA("static call/s", results[0]);
        DATA("pointer call/s", results[1]);
        DATA("method call/s", results[2]);
        DATA("virtual call/s", results[3]);

        return 0;
    }
};

MAIN_APP(fctbench);
