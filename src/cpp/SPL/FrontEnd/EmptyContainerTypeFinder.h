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

#ifndef SPL_NEW_EMPTY_CONTAINER_TYPE_FINDER_H
#define SPL_NEW_EMPTY_CONTAINER_TYPE_FINDER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {
class AstPool;

/// Visitor that "pushes" types top-down from a context that provides a
/// type (like a cast or assignment) into an empty container that cannot
/// be typed bottom-up (like empty curly braces {}, which might be a map
/// pr set or tuple literal).
class EmptyContainerTypeFinder : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

  private:
    AstPool& _pool;

    EmptyContainerTypeFinder(ParserContext& pCtx);

    void pushType(RootTyp const& type, AstNode& parentAst, AstNode& oldChildAst);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeFormal(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvokeActual(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitCastExpr(AstNode&);
    virtual void visitInfixExpr(AstNode&);
};
}

#endif /* SPL_NEW_EMPTY_CONTAINER_TYPE_FINDER_H */
