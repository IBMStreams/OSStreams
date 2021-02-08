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

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryException.h>
#include <iostream>
#include <sstream>

#include <UTILS/DistilleryApplication.h>
#include <UTILS/Formatter.h>
#include <UTILS/PieList.h>

using namespace std;
UTILS_NAMESPACE_USE;

class pie : public DistilleryApplication
{

  public:
    pie(void) {}

    //   void getArguments(option_vector_t& options)
    //   {
    //     option_t args[] = {
    //       { 'g', "glob", 0, "", "Use glob type of matching",
    //         INT_OPT(regex::setGlob)  },
    //     } ;

    //     APPEND_OPTIONS(options, args) ;
    //   }

    //   void setGlob(const option_t* option, int value)
    //   {
    //     _glob = true ;
    //   }

    enum states
    {
        FREE,
        STATE1,
        STATE2,
        TOTAL
    };

#define DO(cmd)                                                                                    \
    do {                                                                                           \
        cmd;                                                                                       \
        cout << Format("%-40s", #cmd) << " --> " << p;                                             \
        if (p.size(0) > 0) {                                                                       \
            cout << "   {" << p.first(0) << "," << p.last(0) << "}";                               \
        }                                                                                          \
        cout << endl;                                                                              \
        usleep(100000);                                                                            \
    } while (0)

    virtual int run(const vector<string>& remainings_args)
    {
        PieList<int> p(TOTAL, 10);

        for (uint32_t i = 0; i < p.size(); i++) {
            p[i] = i;
        }

        cout << p << endl << endl;

        for (;;) {
            DO(p.shift(FREE) += 1000);
            DO(p.shift(FREE) += 1000);
            DO(p.shift(STATE1));
            DO(p.shift(STATE1));
            DO(p.shift(STATE2) -= 1000);
            DO(p.shift(STATE2) -= 1000);
        }

        return 0;
    }
};

MAIN_APP(pie);
