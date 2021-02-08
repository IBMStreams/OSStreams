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

#include <iostream>
#include <limits.h>
#include <stdlib.h>

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/VariableThreadPool.h>

using namespace std;
UTILS_NAMESPACE_USE;

class vthreadpool : public DistilleryApplication
{
  public:
    int _hi_threads;
    int _lo_threads;
    int _items;
    int _max_rand;
    int _sleep;

    vthreadpool()
      : _max_rand(INT_MAX)
      , _sleep(0)
    {}

    virtual ~vthreadpool() {}

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 0, "hithread", ARG | REQUIRED, "", "number of threads (HI)",
              INT_OPT(vthreadpool::setHiThreadCount) },
            { 0, "lothread", ARG | REQUIRED, "", "number of threads (LO)",
              INT_OPT(vthreadpool::setLoThreadCount) },
            { 0, "items", ARG | REQUIRED, "", "number of items",
              INT_OPT(vthreadpool::setItemCount) },
            { 0, "rand", ARG, "", "max number of volatile loop", INT_OPT(vthreadpool::setMaxRand) },
            { 0, "sleep", ARG, "", "sleep before shutdown", INT_OPT(vthreadpool::setSleep) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setHiThreadCount(const option_t* option, int value) { _hi_threads = value; }

    void setLoThreadCount(const option_t* option, int value) { _lo_threads = value; }

    void setItemCount(const option_t* option, int value) { _items = value; }

    void setMaxRand(const option_t* option, int value) { _max_rand = value; }

    void setSleep(const option_t* option, int value) { _sleep = value; }

    virtual ostream& printDescription(ostream& o) const
    {
        o << "This is the vthreadpool application";

        return o;
    }

    class Factory : public VariableThreadPool::ThreadFactory
    {
      public:
        Factory() {}

        virtual VariableThreadPool::WThread* createThread(VariableThreadPool* pool)
        {
            SPCDBG(L_INFO, "Creating new thread", "test");

            return new VariableThreadPool::WThread(pool);
        }
    };

    class WI : public WorkItem
    {
      public:
        Mutex* _m;
        // string _msg ;
        volatile unsigned long long _loop;

        WI(Mutex* m, unsigned long long loop /*const string& msg*/)
          : _m(m)
          , _loop(loop) // _msg(msg)
        {}

        virtual void satisfy()
        {
            // AutoMutex lck(*_m) ;
            // cout << _msg << endl ;

            _loop *= 1;

            while (_loop) {
                _loop--;
            }
        }
    };

    virtual int run(const arg_vector_t& args)
    {
        Factory fact;
        Mutex m;
        VariableThreadPool pool(_lo_threads, _hi_threads, &fact);

        // sleep(2) ;

        srand(getpid());

        for (int i = 0; i < _items; i++) {
            // stringstream strm ;
            // strm << "request " << i ;
            // pool.submitWork(new WI(&m, strm.str( ))) ;
            pool.submitWork(new WI(&m, rand() % _max_rand));
        }

        if (_sleep) {
            sleep(_sleep);

            pool.submitWork(new WI(&m, 0));
            sleep(1);
        }

        pool.shutdown();

        // pool.submitWork(new WI(&m, 0)) ;

        return 0;
    }
};

MAIN_APP(vthreadpool);
