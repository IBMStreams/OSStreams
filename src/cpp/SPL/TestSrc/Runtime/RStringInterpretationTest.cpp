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

#include <SPL/Runtime/Function/SPLFunctions.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class RStringInterpretationTest : public DistilleryApplication
{
  public:
    RStringInterpretationTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        try {
            runTests();
        } catch (DistilleryException const& e) {
            cerr << e << endl;
            throw e;
        }
        return EXIT_SUCCESS;
    }

    void checkInvalidExceptionCaught(const string& s)
    {
        bool caught = false;
        try {
            cout << "Convert: '" << s << '\'' << endl;
            rstring res = interpretRStringLiteral(s);
        } catch (const SPLRuntimeInvalidArgumentException& e) {
            caught = true;
            cout << "Caught expected exception: " << e.getExplanation() << endl;
        }
        FASSERT(caught);
    }

    void checkValidString(const string& s, const string& expected, const string& alternateBack)
    {
        rstring res;
        try {
            cout << "Convert: '" << s << '\'' << endl;
            res = interpretRStringLiteral(s);
        } catch (const DistilleryException& e) {
            cout << "Unexpected exception: " << e.getExplanation() << endl;
            FASSERT(false);
        } catch (...) {
            cout << "Unexpected exception caught\n";
            FASSERT(false);
        }
        cout << "Successful conversion\n";
        std::string sres(res.c_str(), res.size());
        FASSERT(sres == expected);
        string back = makeRStringLiteral(res);
        cout << "Converted back to '" << back << "'\n";
        FASSERT(s == back || back == alternateBack);
    }

    void runTests()
    {
        rstring s;
        // check invalid string input
        checkInvalidExceptionCaught("a");           // not quoted
        checkInvalidExceptionCaught("\"a");         // not quoted
        checkInvalidExceptionCaught("a\"");         // not quoted
        checkInvalidExceptionCaught("\"a\\\"");     // trailing '/'
        checkInvalidExceptionCaught("\"\\u\"");     // bad \u
        checkInvalidExceptionCaught("\"\\u0\"");    // bad \u
        checkInvalidExceptionCaught("\"\\u00\"");   // bad \u
        checkInvalidExceptionCaught("\"\\u000\"");  // bad \u
        checkInvalidExceptionCaught("\"\\ux000\""); // bad \u
        checkInvalidExceptionCaught("\"\\u0x00\""); // bad \u
        checkInvalidExceptionCaught("\"\\u00x0\""); // bad \u
        checkInvalidExceptionCaught("\"\\u000x\""); // bad \u
        checkValidString("\"\\w\"", "w", "\"w\"");  // unknown \X -> X
        checkValidString("\"\\u0000\"", string("\0", 1), "\"\\0\"");
        checkValidString("\"\\uaAbB\"",
                         "\xea"
                         "\xaa"
                         "\xbb",
                         "\"\\uaabb\"");
        checkValidString("\"\\\\ \\0 \\t \\n \\r \\a \\b \\f \\v xyz\"",
                         string("\\ \0 \t \n \r \a \b \f \v xyz", 21), "");
        checkInvalidExceptionCaught("\"\xea\""); // incorrect number of bytes (UTF-8)
        checkInvalidExceptionCaught("\"\xea"
                                    "\xa4\""); // incorrect number of bytes (UTF-8)
        checkInvalidExceptionCaught("\"\xea"
                                    "\x0a\""); // bad trailing char (UTF-8)
    }
};

};

MAIN_APP(SPL::RStringInterpretationTest)
