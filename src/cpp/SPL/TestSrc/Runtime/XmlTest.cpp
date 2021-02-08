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

#define SPL_TMP_TUPLE
#include <SPL/FrontEnd/Typ.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Type/Meta/BaseType.h>
#include <SPL/Runtime/Type/Xml.h>
#include <UTILS/DistilleryApplication.h>

using namespace std;
using namespace Distillery;

namespace SPL {

class XmlTest : public DistilleryApplication
{
  public:
    XmlTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        testAll();
        return EXIT_SUCCESS;
    }

    void good(const char* value, const char* schema = NULL)
    {
        cerr << "good: '" << value << "'";
        if (schema) {
            cerr << ", schema: '" << schema << "'";
        }
        cerr << endl;
        try {
            if (schema) {
                xml test(schema, value);
            } else {
                xml test("", value);
            }
        } catch (const SPLRuntimeInvalidXMLException& e) {
            cerr << "Unexpected exception:\n" << e.getExplanation() << endl;
            throw;
        } catch (...) {
            cerr << "Unexpected unknown exception\n";
            throw;
        }
    }

    void bad(const char* value, const char* schema = NULL)
    {
        cerr << "bad: '" << value << "'";
        if (schema) {
            cerr << ", schema: '" << schema << "'";
        }
        cerr << endl;
        try {
            if (schema) {
                xml test(schema, value);
            } else {
                xml test("", value);
            }
        } catch (const SPLRuntimeInvalidXMLException& e) {
            cerr << "Got expected exception:\n" << e.getExplanation() << endl;
            return;
        } catch (...) {
            cerr << "Unknown exception\n";
        }
        // Shouldn't get here
        FASSERT(false);
    }

    void testAll()
    {
        // XML only, no schema
        good("<a/>");
        good("<a>hi</a>");
        bad("a");
        bad("</a>");
        bad("<a>hi</b>");
        bad("<a>hi</a>x");

        // XML with schema
        good("<a>-55</a>", "simple.xsd");
        bad("<a>hi</a>", "simple.xsd");
        bad("<b>hi</b>", "simple.xsd");
        bad("<a><b>hi</b></a>", "simple.xsd");

        bad("<a>55</a>", "schemaNotFound.xsd");
        bad("<a>55</a>", "badSchema.xsd");
    }
};

};

MAIN_APP(SPL::XmlTest)
