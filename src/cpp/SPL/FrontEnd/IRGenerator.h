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

#ifndef SPL_IR_GENERATOR_H
#define SPL_IR_GENERATOR_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

#include <SPL/CodeGen/SPLAnnotation.h>

namespace SPL {
class AstPool;
class Config;
class ParserContext;

namespace IRGeneratorHelper {
/// Convert the AST for a 'config' into a Config IR
/// @param context Parser scope for the AST
/// @param ast Parser AST for the  config
/// @return NULL if a 'threadedPort' config, or a Config IR
Config* createConfig(ParserContext const& context, AstNode& ast);
}

/// Visitor that generates intermediate representation (IR) for the
/// compiler stages that come after the front-end.
class IRGenerator : public AstVisitorPushPop
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    static void run(ParserContext&, AstNode&);

    // separate opaque class to avoid header file pollution
    class State;
    State& _state;

    /// Create an SPLAnnotation object
    static const SPLAnnotation* createSPLAnnotation(ParserContext& pCtx,
                                                    AstNode& annot,
                                                    const std::string& compositeName);

  private:
    static SPLSimpleAnnotation::KeyValuePair& createKeyValuePair(ParserContext& pCtx,
                                                                 const std::string& tag,
                                                                 const std::string& keyText,
                                                                 AstNode& value,
                                                                 const std::string& compositeName);
    AstPool& _pool;

    IRGenerator(ParserContext&);

  protected:
    /// If this stage isn't yet done for 'ast', mark it as done, and then
    /// call the superclass generic visit() method to actually do this stage.
    virtual void visit(AstNode& ast);

    // ----- The compilation unit is the start symbol of the SPL grammar.
    virtual void visitNamespace(AstNode&);
    virtual void visitNamespaceName(AstNode&);

    // ----- Composite operators are defined at the top-level in a namespace.
    virtual void visitCompositeDef(AstNode&);
    virtual void visitCompositeHead(AstNode&);
    virtual void visitConfigItem(AstNode&);

    // ----- Streams are defined in a composite operator's graph clause.
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpOutput(AstNode&);
    virtual void visitPortInputs(AstNode&);
    virtual void visitLogicState(AstNode&);
    virtual void visitOnProcessLogic(AstNode&);
    virtual void visitOnTupleLogic(AstNode&);
    virtual void visitOnPunctLogic(AstNode&);
#if 0
        // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
        virtual void visitOnWatermarkLogic(AstNode &);
#endif
    virtual void visitOpInvokeWindow(AstNode&);
    virtual void visitOpInvokeActual(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);

    // ----- Functions are defined at the top-level in a namespace.
    virtual void visitFunctionDef(AstNode&);

    // ----- Imperative statements can appear in function bodies or the logic clause of operator
    // invocations.
    virtual void visitLocalDecl(AstNode&);
    virtual void visitBlockStmt(AstNode&);
    virtual void visitBlockStmtNoScope(AstNode&);
    virtual void visitExprStmt(AstNode&);
    virtual void visitIfStmt(AstNode&);
    virtual void visitForStmt(AstNode&);
    virtual void visitWhileStmt(AstNode&);
    virtual void visitBreakStmt(AstNode&);
    virtual void visitContinueStmt(AstNode&);
    virtual void visitReturnStmt(AstNode&);

    // ----- Expressions can appear in many places in the grammar.
    virtual void visitAttributeExpr(AstNode&);
    virtual void visitCastExpr(AstNode&);
    virtual void visitCallExpr(AstNode&);
    virtual void visitConditionalExpr(AstNode&);
    virtual void visitInfixExpr(AstNode&);
    virtual void visitParenthesizedExpr(AstNode&);
    virtual void visitPostfixExpr(AstNode&);
    virtual void visitPrefixExpr(AstNode&);
    virtual void visitQualifierExpr(AstNode&);
    virtual void visitSubscriptExpr(AstNode&);
    virtual void visitId(AstNode&);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

    // ----- Literals are the highest-precedence expressions denoting values.
    virtual void visitListLiteral(AstNode&);
    virtual void visitCurlyLiteral(AstNode&);
    virtual void visitTupleLiteral(AstNode&);
    virtual void visitMapLiteral(AstNode&);
    virtual void visitSetLiteral(AstNode&);
    virtual void visitBooleanLiteral(AstNode&);
    virtual void visitStringLiteral(AstNode&);
    virtual void visitXMLLiteral(AstNode&);
    virtual void visitNullLiteral(AstNode&);
    virtual void visitNumericLiteral(AstNode&);

    // ----- Types are defined in a composite operator's type clause, or in a block, or at the
    // top-level.
    virtual void visitTypeName(AstNode&);
};
}

#endif /* SPL_IR_GENERATOR_H */
