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

#include <TRC/DistilleryDebug.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/Formatter.h>
#include <UTILS/SupportFunctions.h>
#include <iostream>
#include <stdio.h>

using namespace std;
UTILS_NAMESPACE_USE;

/**
   Program used to test the runCommand function. It uses any argument
   left on the command line after standard argument processing as the
   parameter of the runCommand function. The first value will be the
   executable name, the remaining will be the options of the command.
*/

class cmd_tst : public DistilleryApplication
{
  protected:
    /// value of stdin to send to the command
    stringstream _stdin;

    /// Print stdout and stderr in hexdecimal, like hexdunp(1)
    bool _hex_dump;

    /// Time in second to wait before killing the command and raising an
    /// exception (10s default)
    int _timeout;

    /// How many time to execute the same command. This is only to test
    /// if there is any resource leaks.
    unsigned int _loop;

  public:
    cmd_tst()
      : _hex_dump(false)
      , _timeout(10)
      , _loop(1)
    {
        setKeepRemainingArguments();
    }

    void setStdin(const option_t* option, const char* value)
    {
        if (_stdin.str().length()) {
            _stdin << endl;
        }

        _stdin << value;
    }

    void setHexDump(const option_t* option, int value) { _hex_dump = true; }

    void setTimeout(const option_t* option, int value) { _timeout = value; }

    void setLoop(const option_t* option, int value) { _loop = value; }

    void getArguments(option_vector_t& options)
    {
        option_t args[] = {
            { 's', "stdin", ARG, "", "Send argument as stdin", STR_OPT(cmd_tst::setStdin) },
            { 'x', "hex", 0, "", "Print the output in hexa", INT_OPT(cmd_tst::setHexDump) },
            { 't', "timeout", ARG, "", "Send argument as stdin", INT_OPT(cmd_tst::setTimeout) },
            { 'l', "loop", ARG, "", "Rerun the command n time", INT_OPT(cmd_tst::setLoop) },
        };

        APPEND_OPTIONS(options, args);
    }

    virtual ostream& printDescription(ostream& o) const
    {
        o << "This is the test application for UTILS::runCommand";

        return o;
    }

    virtual int run(const arg_vector_t& args)
    {
        for (unsigned int l = 1;; l++) {
            try {
                int e = 0;

                if (_hex_dump) {
                    stringstream out;
                    e = runCommand(args, _stdin.str(), out, out, _timeout);

                    string s = out.str();

                    int offset = 0;
                    for (unsigned int i = 0; i < s.length(); i++) {
                        if (!(offset % 16)) {
                            if (i) {
                                printf("\n");
                            }

                            printf("%4.4x: ", i);
                            offset = 0;
                        }

                        printf("%2.2x ", s[i]);
                        offset++;
                    }
                    if (offset) {
                        printf("\n");
                    }
                } else {
                    int64_t ts_begin = debug::TimeStamp::TOD_ms();
                    e = runCommand(args, _stdin.str(), cout, cerr, _timeout);
                    int64_t ts_end = debug::TimeStamp::TOD_ms();
                    SPCDBG(L_INFO, "Command ran in " << (ts_end - ts_begin) << "ms", "xx");
                }

                if (l == _loop) {
                    return WEXITSTATUS(e);
                }
            } catch (DistilleryException& e) {
                cerr << "Got an " << e.getType() << " exception: " << e.getExplanation() << endl;
                if (l == _loop) {
                    return 255;
                }
            }
        }
    }
};

MAIN_APP(cmd_tst);
