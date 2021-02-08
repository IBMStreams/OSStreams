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

#include <SPL/FrontEnd/SignatureExtractor.h>
#include <SPL/FrontEnd/SourceLocationFactory.h>
#include <SPL/FrontEnd/TreeHelper.h>

#include <SPL/Core/MakeToolkit.h>
#include <sstream>

namespace SPL {
void SignatureExtractor::run(MakeToolkit& toolkitMaker, ParserContext& pCtx, AstNode& ast)
{
    SignatureExtractor visitor(toolkitMaker, pCtx);
    visitor.visit(ast);
}

SignatureExtractor::SignatureExtractor(MakeToolkit& toolkitMaker, ParserContext& pCtx)
  : AstVisitor(Debug::None)
  , _toolkitMaker(toolkitMaker)
  , _pCtx(pCtx)
{}

// ----- The compilation unit is the start symbol of the SPL grammar.
void SignatureExtractor::visitCompilationUnit(AstNode& ast)
{
    AstNode& splNamespaceAst = AST::compilationUnit::splNamespace(ast);
    if (AST::splNamespace::has_name(splNamespaceAst)) {
        std::ostringstream os;
        AstNode& n = AST::splNamespace::name(splNamespaceAst);
        prettyPrint(os, n);
        const SourceLocation& loc = SourceLocationFactory::instance().create(_pCtx, n);
        _toolkitMaker.setNamespace(os.str(), loc);
    } else {
        const SourceLocation& loc =
          SourceLocationFactory::instance().create(_pCtx, *astNodeWithFirstLocation(ast));
        _toolkitMaker.setNamespace("", loc);
    }
    AstVisitor::visitCompilationUnit(ast);
}

// ----- Composite operators are defined at the top-level in a namespace.
void SignatureExtractor::visitCompositeDef(AstNode& ast)
{
    AstNode& headAst = AST::compositeDef::compositeHead(ast);
    const std::string& id = astText(AST::compositeHead::id(headAst));
    _toolkitMaker.addComposite(id, ast);
    AstVisitor::visitCompositeDef(ast);
    _toolkitMaker.finishComposite(id, ast);
}

// ----- Functions are defined at the top-level in a namespace.
void SignatureExtractor::visitFunctionDef(AstNode& ast)
{
    AstNode& headAst = AST::functionDef::head(ast);
    _toolkitMaker.addFunction(astText(AST::functionHead::id(headAst)), headAst);
    // don't visit the innards, nothing to be extracted in there
}

// ----- Types are defined in a composite operator's type clause, or in a block, or at the
// top-level.
void SignatureExtractor::visitDefType(AstNode& ast)
{
    const std::string& id = astText(AST::typeDef::id(ast));
    _toolkitMaker.addTypedef(id, ast);
    AstVisitor::visitDefType(ast); // visit children, in case there is an enum
    _toolkitMaker.finishTypedef(id, ast);
}

void SignatureExtractor::visitEnumType(AstNode& ast)
{
    _toolkitMaker.addEnum(ast);
}
}
