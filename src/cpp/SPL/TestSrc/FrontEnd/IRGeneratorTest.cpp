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

#include <SPL/CodeGen/Expression.h>
#include <SPL/Core/CompilerApp.h>
#include <SPL/FrontEnd/EmptyContainerTypeFinder.h>
#include <SPL/FrontEnd/ExpressionTypeFinder.h>
#include <SPL/FrontEnd/FunctionSymbolTableEntries.h>
#include <SPL/FrontEnd/Generated/ScopeExpander.h>
#include <SPL/FrontEnd/IRGenerator.h>
#include <SPL/FrontEnd/LexicalNameUseFinder.h>
#include <SPL/FrontEnd/LogicSubmitChecker.h>
#include <SPL/FrontEnd/NameDefFinder.h>
#include <SPL/FrontEnd/RelativeNameUseFinder.h>
#include <SPL/FrontEnd/SymbolTable.h>
#include <SPL/FrontEnd/SymbolTableEntries.h>
#include <SPL/FrontEnd/TreeHelper.h>
#include <SPL/FrontEnd/TypeFactory.h>
#include <SPL/FrontEnd/UsedSPLFunctions.h>
#include <SPL/FrontEnd/VariableUseChecker.h>

using namespace Distillery;

namespace SPL {
class IRGeneratorTest : public CompilerApp
{
    int _nFailures;

    void assertSame(std::string const& source, std::string& expected, std::string const& actual)
    {
        if ("" == expected)
            expected = source;
        if (expected == actual)
            return;
        std::cout << "IR generation failure" << std::endl;
        std::cout << "   source == '" << source << "'" << std::endl;
        std::cout << " expected == '" << expected << "'" << std::endl;
        std::cout << "   actual == '" << actual << "'" << std::endl;
        _nFailures++;
    }

    void testExpr(std::string const& source, std::string expected = "")
    {
        SPLAst& splAst = SPLAst::expressionFromString("foo.spl", source);
        analyzeExpressionAst(splAst);
        Expression& ir = astExpression(splAst.ast());
        std::ostringstream os;
        ir.print(os);
        assertSame(source, expected, os.str());
    }

    void testStmt(std::string const& source, std::string expected = "")
    {
        SPLAst& splAst = SPLAst::statementFromString("foo.spl", source);
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
        std::ostringstream os;
        ir->print(os, 0);
        assertSame(source, expected, os.str());
        delete &splAst;
    }

    void testFunction(std::string const& source, std::string expected = "")
    {
        SPLAst& splAst = SPLAst::functionDefFromString("foo.spl", source);
        AstNode* ast = &splAst.ast();
        NameDefFinder::run(splAst.parserContext(), *ast);
        LexicalNameUseFinder::run(splAst.parserContext(), *ast);
        runAntlrAstParser(ScopeExpander, functionDef, ast, splAst.parserContext());
        RelativeNameUseFinder::run(splAst.parserContext(), *ast);
        ExpressionTypeFinder::run(splAst.parserContext(), *ast);
        EmptyContainerTypeFinder::run(splAst.parserContext(), *ast);
        VariableUseChecker::run(splAst.parserContext(), *ast);
        LogicSubmitChecker::run(splAst.parserContext(), *ast);
        IRGenerator::run(splAst.parserContext(), *ast);
        SymbolTableEntry& e = astSymbol(*ast);
        delete &splAst;
        FunctionHeadSym* ir = dynamic_cast<FunctionHeadSym*>(&e);
        std::ostringstream os;
        ir->printBare(os);
        os << ' ';
        Statement* body = ir->functionBodyIR();
        body->print(os, 0);
        assertSame(source, expected, os.str());
    }

  public:
    IRGeneratorTest() {}

    virtual int run(std::vector<std::string> const& /*remains*/)
    {
        FunctionHeadSym::analyzeFunctionFromToolkitModel("some.name.space", "void foo()",
                                                         "some/file.xml", "some/file.spl",
                                                         "some/file.h", "", false, NULL, -1);
        FunctionHeadSym::analyzeFunctionFromToolkitModel(
          "some.name.space", "<any T> int32 bar(T, T)", "some/file.xml", "some/file.spl",
          "some/file.h", "", false, NULL, -1);
        SymbolTable* symTab = &SymbolTable::instance();
        symTab->pushNamespace("some.name.space");                  // same namespace ...
        ParserContext prsCtx("some/otherFileInSameNamespace.spl"); // ... but different file
        symTab->pushCompilationUnit(prsCtx);
        // primitive literals
        testExpr("true");
        testExpr("false");
        testExpr("123uw", "123");
        testExpr("12.34");
        testExpr("12.34d", "12.34");
        testExpr("\"hi world\"");
        // composite literals
        testExpr("[1, 2, 3]");
        testExpr("[1,2,3]", "[1, 2, 3]");
        testExpr("{1, 2, 3}");
        testExpr("{\"a\" : 1, \"b\" : 2}");
        // expressions
        testExpr("!false");
        testExpr("(123)", "123");
        testExpr("(1.2 + 3.4)");
        testExpr("2+3", "(2 + 3)");
        testExpr("(true ? 1 : 0)");
        testExpr("int32(123u)", "int32(123)");
        //       testExpr("printString(\"hello, world!\")");
        //       testExpr("irange(1, 2)");
        testExpr("1 - 2 - 3", "((1 - 2) - 3)");
        // statements
        testStmt("break;\n");
        testStmt("continue;\n");
        testStmt("return;\n");
        testStmt("return 1;\n");
        std::string indent(4, ' ');
        testStmt("while (true)\n" + indent + "continue;\n");
        testStmt("if (true)\n" + indent + "break;\n");
        testStmt("if (false)\n" + indent + "break;\nelse\n" + indent + "continue;\n");
        testStmt("{\n" + indent + "break;\n" + indent + "continue;\n" + indent + "break;\n}\n");
        testStmt("for (int32 i in [1, 2, 3])\n" + indent + "{\n" + indent + "}\n");
        testStmt("{ int32 i=1, j=2; }",
                 "{\n" + indent + "int32 i = 1;\n" + indent + "int32 j = 2;\n}\n");
        // function definitions
        testFunction("void foo() {\n}\n");
        if (_nFailures > 0)
            std::cout << std::endl << "There were " << _nFailures << " failures." << std::endl;
        return _nFailures;
    }
};
};

MAIN_APP(SPL::IRGeneratorTest)
