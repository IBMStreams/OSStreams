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
#include <SPL/Core/OperatorModelImpl.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <UTILS/DistilleryApplication.h>

#include <fstream>
#include <string>

using namespace std;
using namespace std::tr1;
using namespace SPL;

class PathSearchTest : public CompilerApp
{
  public:
    PathSearchTest() {}

    virtual int run(const std::vector<std::string>& remains)
    {
        // Compile the input program
        CompilerApp::run(remains);
        // lookups for known symbols
        PathSearch& ps = PathSearch::instance();

        const PathSearch::NonFunctionResult* r;
        r = ps.findNonFunction("spl.adapter", "GarBage");
        FASSERT(r == NULL);
        r = ps.findNonFunction("spl.relational", "Functor");
        FASSERT(r != NULL);
        cout << "spl.relational.Functor\n";
        cout << "path = " << r->getPath() << endl;
        cout << "is primitive = " << r->isPrimitiveOperator() << endl;

        // Now lookup functions
        if (NULL != ps.findFunctionGroup("this.is", "garbage")) {
            cout << "ERROR: garbage found\n";
            return EXIT_FAILURE;
        }

        OverloadedFunctionsSym* naan = ps.findFunctionGroup("spl.math", "isNaN");
        if (NULL == naan) {
            return EXIT_FAILURE;
        } else {
            cout << "Lookup isNaN:\n";
            cout << naan << endl;
        }

        OverloadedFunctionsSym* maax = ps.findFunctionGroup("spl.math", "max");
        if (NULL == maax) {
            return EXIT_FAILURE;
        } else {
            cout << "Lookup max:\n";
            cout << maax << endl;
        }

        return EXIT_SUCCESS;
    }

  private:
    string fileName_;
};

MAIN_APP(PathSearchTest)
