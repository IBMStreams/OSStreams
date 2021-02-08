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

#include <sys/time.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/Thread.h>
#include <iostream>

using namespace std;
UTILS_NAMESPACE_USE;

class looper : public Thread
{
  protected:
    unsigned long long _loop_count;

  public:
    looper(unsigned long long lc)
      : _loop_count(lc)
    {}

    static void do_loop(unsigned long long loop_count)
    {
        volatile unsigned long long i = 0;
        while (i < loop_count) {
            i++;
        }
    }

    virtual void* run(void* threadArgs)
    {
        SPCDBG(L_INFO, "Thread looping...", "cpuguess");

        do_loop(_loop_count);
        return NULL;
    }
};

class cpu_guess : public DistilleryApplication
{
  protected:
  public:
    cpu_guess() {}

    virtual ~cpu_guess() {}

    virtual ostream& printDescription(ostream& o) const
    {
        o << "cpu_guess";

        return o;
    }

    double getts()
    {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + (double)tv.tv_usec / 1000000.0;
    }

    unsigned long long find_good_loop_count()
    {
        unsigned long long loop_count = 1024;

        for (;;) {
            double begin = getts();

            looper::do_loop(loop_count);

            double end = getts();

            SPCDBG(L_INFO, "Looking for a good loop count: " << loop_count << " - " << end - begin,
                   "cpuguess");

            if ((end - begin) > 0.5) {
                SPCDBG(L_INFO, "Picked " << loop_count, "cpuguess");
                return loop_count;
            }

            loop_count *= 2;
        }
    }

    virtual int run(const arg_vector_t& args)
    {
        unsigned long long loop_count = find_good_loop_count();

        double base = getts();
        looper::do_loop(loop_count);
        base = getts() - base;

        SPCDBG(L_INFO, "base: " << loop_count << " / " << base, "cpuguess");

        for (int i = 2; i <= 8; i++) {
            double begin = getts();

            looper** threads = new looper*[i];
            for (int j = 0; j < i; j++) {
                threads[j] = new looper(loop_count);
                threads[j]->create();
            }

            for (int j = 0; j < i; j++) {
                threads[j]->join();
                delete threads[j];
            }

            double end = getts();

            cout << i << ": " << (end - begin) / base << endl;
        }

        return 0;
    }
};

MAIN_APP(cpu_guess);
