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

#ifndef SPL_SIGNATURE_EXTRACTOR_H
#define SPL_SIGNATURE_EXTRACTOR_H

#include <SPL/FrontEnd/AstVisitor.h>

namespace SPL {
class MakeToolkit;
class ParserContext;

/// Visitor that decorates the AST with new SymbolTableEntry instances
/// that also go into the symbol table scopes for later lookup.
class SignatureExtractor : public AstVisitor
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(MakeToolkit&, ParserContext&, AstNode&);

  private:
    MakeToolkit& _toolkitMaker;
    ParserContext& _pCtx;
    SignatureExtractor(MakeToolkit&, ParserContext&);

  protected:
    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitCompilationUnit(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeDef(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitDefType(AstNode&);
    virtual void visitEnumType(AstNode&);
};
}

#endif /* SPL_SIGNATURE_EXTRACTOR_H */
