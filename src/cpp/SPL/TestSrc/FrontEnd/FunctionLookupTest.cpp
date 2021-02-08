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

#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/GenericsUnifier.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <UTILS/DistilleryApplication.h>

using namespace Distillery;

namespace SPL {
class FunctionLookupTest : public DistilleryApplication
{
    int _nFailures;
    int _nTests;

    void testAnalyzePrintRoundtrip(std::string const& input)
    {
        _nTests++;
        std::ostringstream namespaceName, fileName;
        namespaceName << "namespace" << _nTests;
        fileName << "file" << _nTests << ".xml";
        FunctionHeadSym& fh = FunctionHeadSym::analyzeFunctionFromToolkitModel(
          namespaceName.str(), input, fileName.str(), "", 0, 0, "", "", true, NULL, -1, 0, "", "");
        fh.resolvePrototype();
        std::ostringstream os;
        fh.printBare(os);
        std::string output = os.str();
        if (input != output) {
            std::cout << "failure in testAnalyzePrintRoundtrip" << std::endl;
            std::cout << "   input == " << input << std::endl;
            std::cout << "  output == " << output << std::endl;
            _nFailures++;
        }
    }

    void testOverlappingOverload(bool expected,
                                 std::string const& input1,
                                 std::string const& input2)
    {
        _nTests++;
        std::ostringstream namespaceName, fileName;
        namespaceName << "namespace" << _nTests;
        fileName << "file" << _nTests << ".xml";
        FunctionHeadSym& fh1 = FunctionHeadSym::analyzeFunctionFromToolkitModel(
          namespaceName.str(), input1, fileName.str(), "", 0, 0, "", "", true, NULL, -1, 0, "", "");
        fh1.resolvePrototype();
        FunctionHeadSym& fh2 = FunctionHeadSym::analyzeFunctionFromToolkitModel(
          namespaceName.str(), input2, fileName.str(), "", 0, 0, "", "", true, NULL, -1, 0, "", "");
        fh2.resolvePrototype();
        std::auto_ptr<GenericsUnifier const> gu(&GenericsUnifier::unify(fh1, fh2));
        bool actual = !gu->isFailure();
        if (expected != actual) {
            std::cout << "failure in testOverlappingOverload" << std::endl;
            std::cout << "    input1 == " << input1 << std::endl;
            std::cout << "    input2 == " << input2 << std::endl;
            std::cout << "  expected == " << (expected ? "overlap" : "no overlap") << std::endl;
            std::cout << "    actual == " << (actual ? "overlap" : "no overlap") << std::endl;
            _nFailures++;
        }
    }

  public:
    FunctionLookupTest()
      : _nFailures(0)
      , _nTests(0)
    {}

    virtual int run(std::vector<std::string> const& /*remains*/)
    {
        CompilerConfiguration config;
        FrontEndDriver frontEndDriver;
        testAnalyzePrintRoundtrip("stateful int32 f(list<int32> x, mutable int64)");
        testAnalyzePrintRoundtrip("<any S, any T> void f(S x, T y)");
        testAnalyzePrintRoundtrip("[M, N] void f(list<rstring>[M] x, list<rstring>[N] y)");
        testAnalyzePrintRoundtrip("[M, N] stateful void foo(set<int8>[M], set<int8>[N])");
        testOverlappingOverload(true, "void f()", "void f()");
        testOverlappingOverload(false, "void f()", "void g()");
        testOverlappingOverload(true, "<any T> void f(T)", "void f(int32)");
        testOverlappingOverload(true, "<any T> void f(T)", "<any U> void f(U)");
        testOverlappingOverload(false, "<any T> void f(T, T)", "void f(rstring, int32)");
        testOverlappingOverload(true, "<any T> void f(T, T)", "void f(rstring, rstring)");
        testOverlappingOverload(false, "<floatingpoint T> void f(T)", "<integral T> void f(T)");
        testOverlappingOverload(false, "<float T> void f(T)", "<decimal T> void f(T)");
        testOverlappingOverload(false, "void f(float64)", "<decimal T> void f(T)");
        testOverlappingOverload(true, "void f(float64)", "<float T> void f(T)");
        testOverlappingOverload(false, "<float S, decimal T> void f(S, T)", "<any U> void f(U, U)");
        testOverlappingOverload(false, "<float S> void f(S, decimal64)", "<any U> void f(U, U)");
        testOverlappingOverload(false, "void f(list<int32>)", "[N] void f(list<int32>[N])");
        testOverlappingOverload(false, "void f(rstring)", "[N] void f(rstring[N])");
        if (0 < _nFailures) {
            std::cout << std::endl << "There were " << _nFailures << " failures." << std::endl;
        }
        return _nFailures;
    }
};
};

MAIN_APP(SPL::FunctionLookupTest)
