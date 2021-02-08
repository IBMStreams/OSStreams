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

#include "../Utility/UtlTestCommon.h"

#include <SPL/Runtime/Utility/RWMutex.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {
RWMutex mutex_;
volatile uint64_t value = 0;

class Worker : public SPL::Thread
{
  public:
    Worker(bool read)
      : reader_(read)
      , shutdown_(false)
    {}
    void* run(void* /*args*/)
    {
        while (!shutdown_) {
            if (reader_) {
                reader();
            } else {
                writer();
            }
        }
        return 0;
    }
    void reader()
    {
        mutex_.readLock();
        FASSERT(value % 2 == 0);
        mutex_.unlock();
    }
    void writer()
    {
        mutex_.writeLock();
        value += 1;
        value += 1;
        value += 1;
        value += 1;
        FASSERT(value % 2 == 0);
        mutex_.unlock();
    }
    void shutdown() { shutdown_ = true; }

  private:
    bool reader_;
    volatile bool shutdown_;
};

class RWMutexTest : public DistilleryApplication
{
  public:
    RWMutexTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        Worker r1(true);
        Worker r2(true);
        Worker w1(false);
        Worker w2(false);
        r1.create();
        r2.create();
        w1.create();
        w2.create();
        while (true) {
            uint64_t v;
            mutex_.readLock();
            v = value;
            mutex_.unlock();
            FASSERT(v % 2 == 0);
            if (v >= 1000000) {
                break;
            }
            sleep(1);
        }
        r1.shutdown();
        r2.shutdown();
        w1.shutdown();
        w2.shutdown();
        r1.join();
        r2.join();
        w1.join();
        w2.join();
        cerr << "Success!" << endl;
        return 0;
    }
};

};

MAIN_APP(SPL::RWMutexTest)
