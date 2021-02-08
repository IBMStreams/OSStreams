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
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryException.h>

using namespace std;
UTILS_NAMESPACE_USE;

class myApp : public DistilleryApplication
{
  public:
    virtual int run(const arg_vector_t& args);

    myApp();
    virtual ~myApp();

  protected:
    string cool;
};

myApp::myApp()
  : cool("<default>")
{
    setKeepRemainingArguments();
}

myApp::~myApp()
{
    cout << "bye bye myApp" << endl;
}

int myApp::run(const arg_vector_t& args)
{
    char longstr[5000];
    memset(longstr, 'A', 4999);
    longstr[4999] = '\0';
    printArgs();

    cout << "run ps and grep for pid " << getpid() << endl;
    sleep(5);

    replaceArgv(2, "IBM");
    printArgs();
    sleep(5);

    setMyProcessTableName("today is not a sunny day");
    printArgs();
    setMyProcessTableState("starting up");
    printArgs();

    cout << "run ps and grep for pid " << getpid() << endl;

    sleep(5);
    setMyProcessTableState("ready");
    printArgs();
    sleep(5);

    replaceArgv(1, "BigBlue");
    printArgs();

    cout << "run ps and grep for pid " << getpid() << endl;
    sleep(5);
    setMyProcessTableName("IAmReady");
    printArgs();
    cout << "run ps and grep for pid " << getpid() << endl;
    sleep(5);
    setMyProcessTableName(longstr);
    printArgs();
    cout << "run ps and grep for pid " << getpid() << endl;
    sleep(5);
    printArgs();
    setMyProcessTableState("failed");

    return 0;
}

MAIN_APP(myApp);
