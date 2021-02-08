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

#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Typ.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#undef FALSE
#undef TRUE
#include <SPL/Core/CompilerConfiguration.h>
#include <SPL/Core/PEGraph.h>
#include <SPL/Core/PathSearch.h>
#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>
#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FrontEndDriver.h>
#include <SPL/FrontEnd/Generated/ScopeExpander.h>
#include <SPL/FrontEnd/IRGenerator.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/LogicSubmitChecker.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/VariableUseChecker.h>
#undef LIST
#undef TUPLE
#undef INT
#undef FLOAT
#undef STRING
#undef PACKAGE
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#undef VERSION
#include <SPL/CodeGen/Expression.h>
#include <SPL/CodeGen/FunctionGenerator.h>
#include <SPL/CodeGen/Literal.h>
#include <SPL/CodeGen/MakefileGenerator.h>
#include <SPL/CodeGen/Statement.h>
#include <SPL/CodeGen/TypeGenerator.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/Core/ExpressionEvaluator.h>
#include <SPL/Core/ExpressionValue.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

namespace SPL {

class FunctionGenerationtest : public CompilerApp
{
  public:
    FunctionGenerationtest() {}

    void addFunction(const std::string& ns, const std::string& hdr, const std::string& body)
    {
        FunctionHeadSym& fcn = FunctionHeadSym::analyzeFunctionFromToolkitModel(
          ns, hdr, "somefile.xml", "somefile.spl", "somefile.h", "", false, NULL, -1);

        SPLAst& splAst = SPLAst::statementFromString("foo.spl", body);
        AstNode* ast = &splAst.ast();
        NameDefFinder::run(splAst.parserContext(), *ast);
        LexicalNameUseFinder::run(splAst.parserContext(), *ast);
        runAntlrAstParser(ScopeExpander, stmt, ast, splAst.parserContext());
        RelativeNameUseFinder::run(splAst.parserContext(), *ast);
        ExpressionTypeFinder::run(splAst.parserContext(), *ast);
        EmptyContainerTypeFinder::run(splAst.parserContext(), *ast);
        VariableUseChecker::run(splAst.parserContext(), *ast);
        LogicSubmitChecker::run(splAst.parserContext(), *ast);
        IRGenerator::run(splAst.parserContext(), *ast);
        Statement* ir = getAstStatement(*ast);
        assert(ir);
        BlockStatement* bod = dynamic_cast<BlockStatement*>(ir);
        assert(bod);
        UsedSPLFunctions::instance().addFunction(fcn, *bod);
        delete &splAst;
    }

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

        addFunction("com.foo.bar", "void f (tuple< int32 a, int32 b> a, mutable tuple<int32 b> b)",
                    // "{b = tuple<int32 b>(a); }"
                    " { return; }");
        addFunction("com.foo.bar", "stateful int32 f (list<int32> x, mutable int64 y)",
                    // "{ y = 0; y = x[4];}"
                    "{ return -1;}");
        addFunction("com.snafu.bar", "int32 g ()", "{ return -1;}");
        UsedSPLFunctions& uft = UsedSPLFunctions::instance();
        MakefileGenerator makefileGen;
        _frontEnd->collectUsedTypes();
        typeGenerator.generateCPPCode();
        FunctionGenerator functionGenerator;
        functionGenerator.generateCPPCode();
        makefileGen.generate();

        return EXIT_SUCCESS;
    }
};

};

MAIN_APP(SPL::FunctionGenerationtest)
