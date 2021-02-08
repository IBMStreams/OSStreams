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
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Core/TransportStats.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HashMapHelpers.h>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;

class TransportStatsTest : public CompilerApp
{
  public:
    TransportStatsTest() {}

    virtual int run(const std::vector<std::string>& remains)
    {
        // Compile the input program
        CompilerApp::run(remains);
        string transport("tcp");
        const char* env = getenv("STREAMS_USE_TRANSPORT");
        if (NULL != env) {
            transport = env;
        }

        _transportStats->loadStats();

        TransportStats& ts = getTransportStats();
        // Print the table
        ts.print(cout);

        cout << "The bogomips is " << ts.getBogoMips() << endl;

        // Grab some known stats
        TransportStats::Data d16k = ts.estimate(16 * 1024);
        d16k.print(cout);
        FASSERT(d16k.getTupleSize() == 16384);
        if (transport == "tcp") {
            FASSERT(d16k.getMaxRate() == 118195961);
            FASSERT(d16k.getSenderCPUPercent() == 4);
            FASSERT(d16k.getReceiverCPUPercent() == 10);
        } else if (transport == "llm_rum_tcp") {
            FASSERT(d16k.getMaxRate() == 118182705);
            FASSERT(d16k.getSenderCPUPercent() == 9);
            FASSERT(d16k.getReceiverCPUPercent() == 11);
        } else {
            FASSERT(d16k.getMaxRate() == 2188337942LL);
            FASSERT(d16k.getSenderCPUPercent() == 100);
            FASSERT(d16k.getReceiverCPUPercent() == 100);
        }

        // Another known one
        TransportStats::Data d8k = ts.estimate(8 * 1024);
        d8k.print(cout);
        FASSERT(d8k.getTupleSize() == 8192);
        if (transport == "tcp") {
            FASSERT(d8k.getMaxRate() == 118082680);
        } else if (transport == "llm_rum_tcp") {
            FASSERT(d8k.getMaxRate() == 116982706);
        } else {
            FASSERT(transport == "llm_rum_ib");
            FASSERT(d8k.getMaxRate() == 2872803605LL);
        }

        // now one in the middle;
        TransportStats::Data d12k = ts.estimate(12 * 1024);
        d12k.print(cout);

        // And one past the top
        TransportStats::Data d128 = ts.estimate(128 * 1024);
        d128.print(cout);

        // And one at the beginning
        TransportStats::Data d4 = ts.estimate(4);
        d4.print(cout);

        TransportStats::Data d1 = ts.estimate(1);
        d1.print(cout);

        return EXIT_SUCCESS;
    }

  private:
    string fileName_;
};

MAIN_APP(TransportStatsTest)
