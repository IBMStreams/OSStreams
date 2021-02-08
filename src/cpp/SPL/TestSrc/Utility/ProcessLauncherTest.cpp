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
#include <SPL/Core/CompilerException.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>
#include <stdlib.h>
#include <string>

using namespace std;
using namespace SPL;
using namespace Distillery;

class ProcessLaucherTest : public DistilleryApplication
{
  public:
    ProcessLaucherTest() {}

    void getArguments(option_vector_t& options)
    {
        setKeepRemainingArguments();
        option_t args[] = {};
        APPEND_OPTIONS(options, args);
    }
    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        ostringstream outStr;
        ostringstream errStr;
        vector<string> args;
        args.push_back("1");
        args.push_back("1");

        int res;
        const string bashScript = "./process.sh";
        cerr << "1- (bashScript, args)" << endl;
        res = ProcessLauncher::runProcess(bashScript, args);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        cerr << "2- (bashScript, args, false, false, &outStr, &errStr)" << endl;
        res = ProcessLauncher::runProcess(bashScript, args, false, false, &outStr, &errStr);
        FASSERT(res == 0);
        FASSERT(outStr.str() == "bugra,mark,howard,henrique\n");
        FASSERT(errStr.str() == "robert,martin,vibhore\n");
        cerr.flush();
        cout.flush();
        outStr.str("");
        errStr.str("");

        cerr << "3- (bashScript, args, true, false)" << endl;
        res = ProcessLauncher::runProcess(bashScript, args, true, false);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        cerr << "4- (bashScript, args, false, true)" << endl;
        res = ProcessLauncher::runProcess(bashScript, args, false, true);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        const string perlScript = "./process.pl";
        cerr << "5- (perlScript, args)" << endl;
        res = ProcessLauncher::runProcess(perlScript, args);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        cerr << "6- (perlScript, args, false, false, &outStr, &errStr)" << endl;
        res = ProcessLauncher::runProcess(perlScript, args, false, false, &outStr, &errStr);
        FASSERT(res == 0);
        FASSERT(outStr.str() == "bugra,mark,howard,henrique\n");
        FASSERT(errStr.str() == "robert,martin,vibhore\n");
        cerr.flush();
        cout.flush();

        cerr << "7- (perlScript, args, true, false)" << endl;
        res = ProcessLauncher::runProcess(perlScript, args, true, false);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        cerr << "8- (perlScript, args, false, true)" << endl;
        res = ProcessLauncher::runProcess(perlScript, args, false, true);
        FASSERT(res == 0);
        cerr.flush();
        cout.flush();

        return 0;
    }
};

MAIN_APP(ProcessLaucherTest)
