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
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <sstream>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/Pool.h>

using namespace std;
UTILS_NAMESPACE_USE;

class poolapp : public DistilleryApplication
{
  public:
    virtual int run(const vector<string>& remainings_args)
    {
        Pool<int> mypool(10);

        int* p = mypool.pull();

        if (!p) {
            p = new int;
        }

        *p = 2;

        cout << *p << endl;

        mypool.push(p);

        p = 0;

        p = mypool.pull();

        cout << *p << endl;

        for (int i = 0; i < 20; i++) {
            mypool.push(new int);
        }

        return 0;
    }
};

MAIN_APP(poolapp);
