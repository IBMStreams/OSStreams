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

#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/FunctionGenerator.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/MakefileGenerator.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>
#include <SPL/Core/PEGraph.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/Utility/SourceLocation.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class UsedFunctionTest : public CompilerApp
{
  public:
    UsedFunctionTest() {}

    virtual int run(const std::vector<std::string>& /*remains*/)
    {
        PEGraph peGraph; // need one for makefile gen

        // read the toolkit.xml
        computeCompilerPaths();
        _pathSearch->loadToolkits();
        _config->setStringValue(CompilerConfiguration::ApplicationDir, ".");
        setApplicationDirectories();
        createApplicationDirectories();

        CodeGenHelper codeGenHelper;
        // TypeGenerator needs a CodeGenHelper
        TypeGenerator typeGenerator;

        // Some basic types
        uint32 a;
        const RootTyp& btu32 = RootTyp::makeType(a);

        FunctionHeadSym* fcn = &FunctionHeadSym::analyzeFunctionFromToolkitModel(
          "com.foo.bar", "stateful int32 f (list<int32> x, mutable int64)", "somefile.xml",
          "somefile.spl", "somefile.h", "", false, NULL, -1);
        Expression* e;
        Expression* e1 =
          new LiteralExpression(btu32, loc_, *new Literal(btu32, new ExpressionValue(uint32(4))));
        Expression* sam = new SymbolExpression(btu32, loc_, "sam");

        BlockStatement* b = new BlockStatement(loc_, NULL);
        b->append(*new ExpressionStatement(
          loc_, *new BinaryExpression(btu32, loc_, Expression::eq, sam->clone(), e1->clone())));

        UsedSPLFunctions& uft = UsedSPLFunctions::instance();
        uft.addFunction(*fcn, *b);
        fcn = &FunctionHeadSym::analyzeFunctionFromToolkitModel(
          "com.foo.bar", "int32 f (list<set<uint8>> snafu)", "somefile.xml", "somefile.spl",
          "somefile.h", "", false, NULL, -1);
        b = new BlockStatement(loc_, NULL);
        b->append(
          *new ExpressionStatement(loc_, *new BinaryExpression(btu32, loc_, Expression::plusEq,
                                                               sam->clone(), sam->clone())));

        uft.addFunction(*fcn, *b);
        fcn = &FunctionHeadSym::analyzeFunctionFromToolkitModel(
          "com.ibm.fred", "void f (mutable ustring snafu)", "somefile.xml", "somefile.spl",
          "somefile.h", "", false, NULL, -1);
        b = new BlockStatement(loc_, NULL);
        b->append(
          *new ExpressionStatement(loc_, *new BinaryExpression(btu32, loc_, Expression::minusEq,
                                                               sam->clone(), sam->clone())));
        vector<Expression*> args;
        args.push_back(&e1->clone());
        b->append(*new ExpressionStatement(
          loc_,
          *new CallExpression(btu32, loc_,
                              FunctionHeadSym::analyzeFunctionFromToolkitModel(
                                "name.space", "uint32 f (uint32)", "dummy_xmlfilename",
                                "dummy_splfilename", "dummy_headerfilename", "", false, NULL, -1),
                              args, "")));

        uft.addFunction(*fcn, *b);
        MakefileGenerator makefileGen;
        typeGenerator.generateCPPCode();
        FunctionGenerator functionGenerator;
        functionGenerator.generateCPPCode();
        makefileGen.generate();

        return EXIT_SUCCESS;
    }

  private:
    SourceLocation loc_;
};

};

MAIN_APP(SPL::UsedFunctionTest)
