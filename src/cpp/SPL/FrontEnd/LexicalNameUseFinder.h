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

#ifndef SPL_LEXICAL_NAME_USE_FINDER_H
#define SPL_LEXICAL_NAME_USE_FINDER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {
/// Visitor that resolves identifiers based on the current lexical scope
/// only, as opposed to resolving names that are in scope indirectly
/// since they are attributes of connected streams or enumeration
/// constants of primitive operator parameters.
class LexicalNameUseFinder : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

  private:
    LexicalNameUseFinder(ParserContext&);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitOpActualName(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvokeHead(AstNode&);
    virtual void visitPortInputs(AstNode&);
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitCallExpr(AstNode&);
    virtual void visitQualifierExpr(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    void visitTypeName(AstNode&);
};
}

#endif /* SPL_LEXICAL_NAME_USE_FINDER_H */
