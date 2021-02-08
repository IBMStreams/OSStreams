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

#include <UTILS/DistilleryException.h>
#include <iostream>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/TSharedQueue.h>
#include <UTILS/Thread.h>

using namespace std;
UTILS_NAMESPACE_USE;

class speedtest : public DistilleryApplication
{
  protected:
    int _loops;

  public:
    speedtest()
      : _loops(0)
    {}

    virtual ~speedtest() {}

    void setLoopArgument(const option_t* option, int value) { _loops = value; }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'l', "loop", ARG, "", "loop option", INT_OPT(speedtest::setLoopArgument) },
        };

        APPEND_OPTIONS(options, args);
    }

    virtual ostream& printDescription(ostream& o) const
    {
        o << "This is the speedtest";

        return o;
    }

    class dequeue_thread : public Thread
    {
      protected:
        TSharedQueue<int*>& _q;
        int _loops;

      public:
        dequeue_thread(TSharedQueue<int*>& q, int loops)
          : _q(q)
          , _loops(loops)
        {}

        virtual void* run(void* threadArgs)
        {
            SPCDBG(L_INFO, "Starting dequeue thread", "x");

            unsigned long long ts_begin = debug::TimeStamp::TOD_us();

            for (int i = 0; i < _loops; i++) {
                int* mb = NULL;
                _q.dequeue(&mb);
                SPCDBG(L_DEBUG, "Dequeuing " << mb[0], "x");
                assert(i == mb[0]);
                delete[] mb;
            }

            unsigned long long ts_end = debug::TimeStamp::TOD_us();

            SPCDBG(L_ERROR, "dequeue time: " << (ts_end - ts_begin) << "us", "x");
            SPCDBG(L_ERROR,
                   "dequeue speed: " << (double)_loops / (double)((ts_end - ts_begin) / 1000000.0)
                                     << " item/s",
                   "x");

            return NULL;
        }
    };

    class enqueue_thread : public Thread
    {
      protected:
        TSharedQueue<int*>& _q;
        int _loops;

      public:
        enqueue_thread(TSharedQueue<int*>& q, int loops)
          : _q(q)
          , _loops(loops)
        {}

        virtual void* run(void* threadArgs)
        {
            SPCDBG(L_INFO, "Starting enqueue thread", "x");

            unsigned long long ts_begin = debug::TimeStamp::TOD_us();

            for (int i = 0; i < _loops; i++) {
                SPCDBG(L_DEBUG, "Enqueuing " << i, "x");
                int* mb = new int[4];
                mb[0] = i;
                _q.enqueue(mb);
            }

            unsigned long long ts_end = debug::TimeStamp::TOD_us();

            SPCDBG(L_ERROR, "enqueue time: " << (ts_end - ts_begin) << "us", "x");
            SPCDBG(L_ERROR,
                   "dequeue speed: " << (double)_loops / (double)((ts_end - ts_begin) / 1000000.0)
                                     << " item/s",
                   "x");

            return NULL;
        }
    };

    virtual int run(const arg_vector_t& args)
    {
        TSharedQueue<int*> q(64);
        enqueue_thread e(q, _loops);
        dequeue_thread d(q, _loops);

        int ret = d.create();
        if (ret < 0) {
            throw "thread creation failed";
        }

        ret = e.create();
        if (ret < 0) {
            throw "thread creation failed";
        }

        ret = d.join();
        if (ret < 0) {
            throw "thread creation failed";
        }
        ret = e.join();
        if (ret < 0) {
            throw "thread creation failed";
        }

        return 0;
    }
};

MAIN_APP(speedtest);
