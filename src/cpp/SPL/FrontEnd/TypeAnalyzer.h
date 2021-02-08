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

#ifndef SPL_TYPE_ANALYZER_H
#define SPL_TYPE_ANALYZER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {

class ParserContext;
class SourceLocation;
class TypeFactory;

class TypeAnalyzer : public AstVisitorPushPop
{
  public:
    static void run(ParserContext&, AstNode&);

  protected:
    TypeAnalyzer(ParserContext& pCtx);

    virtual void visit(AstNode& ast);
    virtual void visitNullLiteral(AstNode& ast);
    virtual void visitXMLLiteral(AstNode& ast);
    virtual void visitConditionalExpr(AstNode& ast);
    virtual void visitParenthesizedExpr(AstNode& ast);
    virtual void visitPostfixExpr(AstNode& ast);
    virtual void visitCastExpr(AstNode& ast);
    virtual void visitNumericLiteral(AstNode& ast);
    virtual void visitStringLiteral(AstNode& ast);
    virtual void visitBooleanLiteral(AstNode& ast);
    virtual void visitPrefixExpr(AstNode& ast);
    virtual void visitCurlyLiteral(AstNode& ast);
    virtual void visitCallExpr(AstNode& ast);
    virtual void visitTupleLiteral(AstNode& ast);
    virtual void visitSubscriptExpr(AstNode& ast);
    virtual void visitListLiteral(AstNode& ast);
    virtual void visitMapLiteral(AstNode& ast);
    virtual void visitSetLiteral(AstNode& ast);
    virtual void visitInfixExpr(AstNode& ast);
    virtual void visitAnnotationT(AstNode& ast);
    virtual void visitOptionalType(AstNode& ast);
    virtual void visitUnwrapExpr(AstNode& ast);
    virtual void visitUnwrapOrElseExpr(AstNode& ast);
    virtual void visitIsPresentExpr(AstNode& ast);

  private:
    void setValueExprType(AstNode& ast, RootTyp const& type);
    SourceLocation const& newLoc(AstNode& ast);
    TypeFactory& typeFactory() const { return _typeFactory; }

    TypeFactory& _typeFactory;
};

} // namespace SPL

#endif // SPL_TYPE_ANALYZER_H
