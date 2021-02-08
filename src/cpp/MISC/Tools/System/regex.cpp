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
#include <UTILS/RegEx.h>

using namespace std;
UTILS_NAMESPACE_USE;

class regex : public DistilleryApplication
{

  public:
    bool _glob;

    regex(void)
      : _glob(false)
    {
        setKeepRemainingArguments();
    }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 'g', "glob", 0, "", "Use glob type of matching", INT_OPT(regex::setGlob) },
        };

        APPEND_OPTIONS(options, args);
    }

    void setGlob(const option_t* option, int value) { _glob = true; }

    virtual int run(const vector<string>& remainings_args)
    {
        if (remainings_args.size() < 2) {
            cerr << "Missing arguments: <re> <test>" << endl;
            exit(1);
        }

#if 0

    RegEx re(remainings_args[0]) ;
    cout << RegEx::search_replace(re, remainings_args[1], RegEx::SR_GLOBAL_REPLACE, remainings_args[2]) << endl ;
    return 0 ;

#else

        if (remainings_args[0][0] == 's') {
            cout << RegEx::search_replace(remainings_args[0], remainings_args[1]) << endl;

            return 0;
        } else {
            RegEx re1;

            if (_glob) {
                string pattern = RegEx::glob2regex(remainings_args[0]);
                cout << "Using pattern: " << pattern << endl;
                re1 = pattern;
            } else {
                RegEx re(remainings_args[0]);

                // test operator=
                re1 = re;
                re1 = remainings_args[0];
                re1 = remainings_args[0].c_str();
            }

            int rc = 0;

            for (unsigned long i = 1; i < remainings_args.size(); i++) {
                vector<string> res;

                SPCDBG(L_INFO,
                       "Trying to match '" << re1 << "' with '" << remainings_args[i] << "'", "x");

                if (re1.match(remainings_args[i], res)) {
                    for (unsigned long j = 0; j < res.size(); j++) {
                        cout << " ++ " << res[j] << endl;
                    }
                } else {
                    rc++;
                }
            }

            return rc;
        }

#endif
    }
};

MAIN_APP(regex);
