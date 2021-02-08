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

#ifndef SPL_SCOPE_EXPANDER_H
#define SPL_SCOPE_EXPANDER_H

#include <SPL/FrontEnd/AstVisitorPushPop.h>

namespace SPL {

class ParserContext;

class ScopeExpander : public AstVisitorPushPop
{
  public:
    ScopeExpander(ParserContext& pContext);
    static void run(ParserContext& pContext, AstNode& ast);

  protected:
    virtual void visitCompositeDef(AstNode& ast);
    virtual void visitConfigItem(AstNode& ast);
    virtual void visitOpInvoke(AstNode& ast);
    virtual void visitOnProcessLogic(AstNode& ast);
    virtual void visitOnTupleLogic(AstNode& ast);
    virtual void visitOnPunctLogic(AstNode& ast);
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    virtual void visitOnWatermarkLogic(AstNode & ast);
#endif
    virtual void visitOpInvokeWindow(AstNode& ast);
    virtual void visitOpInvokeActual(AstNode& ast);
    virtual void visitOpInvokeOutput(AstNode& ast);
    virtual void visitTupleAttrib(AstNode& ast);
    virtual void visitTupleExtend(AstNode& ast);

  private:
    AstNode* _opInvoke;
};

} // namespace SPL

#endif // SPL_SCOPE_EXPANDER_H
