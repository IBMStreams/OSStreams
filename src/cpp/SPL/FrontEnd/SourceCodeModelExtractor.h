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

#ifndef SPL_SOURCE_CODE_MODEL_EXTRACTOR_H
#define SPL_SOURCE_CODE_MODEL_EXTRACTOR_H

#include <SPL/Core/sourceCodeModel.h>
#include <SPL/FrontEnd/AstVisitor.h>

namespace SPL {

class MakeToolkit;
class ParserContext;

class SourceCodeModelExtractor : public AstVisitor
{
  public:
    /// Build source code model from the given ast and parser context, then
    /// register the tree with the toolkit maker
    /// @param mtk the toolkit maker
    /// @param pc the parser context
    /// @param ast the ast node
    static void run(MakeToolkit& mtk, ParserContext& pc, AstNode& ast);

  protected:
    virtual void visitCompilationUnit(AstNode& ast);
    virtual void visitUseDirectives(AstNode& ast);
    virtual void visitUseDirective(AstNode& ast);
    virtual void visitDefType(AstNode& ast);
    virtual void visitFunctionDef(AstNode& ast);
    virtual void visitFunctionHead(AstNode& ast);
    virtual void visitModifiers(AstNode& ast);
    virtual void visitFunctionFormal(AstNode& ast);
    virtual void visitBlockStmtNoScope(AstNode& ast);
    virtual void visitCompositeDef(AstNode& ast);
    virtual void visitCompositeHead(AstNode& ast);
    virtual void visitCompositeIn(AstNode& ast);
    virtual void visitCompositeOut(AstNode& ast);
    virtual void visitCompositePort(AstNode& ast);
    virtual void visitCompositeBody(AstNode& ast);
    virtual void visitTypeClause(AstNode& ast);
    virtual void visitFormalClause(AstNode& ast);
    virtual void visitCompositeFormal(AstNode& ast);
    virtual void visitGraphClause(AstNode& ast);
    virtual void visitOpInvoke(AstNode& ast);
    virtual void visitOpInvokeHead(AstNode& ast);
    virtual void visitOpOutput(AstNode& ast);
    virtual void visitPortInputs(AstNode& ast);
    virtual void visitOpInvokeBody(AstNode& ast);
    virtual void visitLogicClause(AstNode& ast);
    virtual void visitLogicState(AstNode& ast);
    virtual void visitOnProcessLogic(AstNode& ast);
    virtual void visitOnTupleLogic(AstNode& ast);
    virtual void visitOnPunctLogic(AstNode& ast);
#if 0
    // STREAMS_SPL_EVENTTIME_CUSTOM_SUPPORT
    virtual void visitOnWatermarkLogic(AstNode & ast);
#endif
    virtual void visitWindowClause(AstNode& ast);
    virtual void visitOpInvokeWindow(AstNode& ast);
    virtual void visitActualClause(AstNode& ast);
    virtual void visitOutputClause(AstNode& ast);
    virtual void visitOpInvokeActual(AstNode& ast);
    virtual void visitOpInvokeOutput(AstNode& ast);
    virtual void visitConfigClause(AstNode& ast);
    virtual void visitConfigItem(AstNode& ast);

  private:
    // The below are used to keep a stack of model types as we visit the
    // tree. By looking at the stack one can tell the path from the root to
    // the current node we are at.
    void push(::xml_schema::type& node) { stack_.push_back(&node); }
    ::xml_schema::type& pop()
    {
        ::xml_schema::type* node = *(stack_.rbegin());
        stack_.pop_back();
        return *node;
    }
    ::xml_schema::type& top() { return **(stack_.rbegin()); }

    /// RAII class for pushing into and popping from the stack automatically
    /// at scope entrance and exit.
    class AutoStack
    {
      public:
        AutoStack(SourceCodeModelExtractor& me, ::xml_schema::type& tp)
          : me_(me)
        {
            me_.push(tp);
        }
        ~AutoStack() { me_.pop(); }

      private:
        SourceCodeModelExtractor& me_;
    };

    /// Constructor
    /// @param pc the parser context
    SourceCodeModelExtractor(ParserContext& pc);

    /// Release the built xml tree
    /// @return the xml tree built
    xmlns::prod::streams::spl::sourceCode::compilationUnitType& releaseResult();

    ParserContext& pCtx_;
    xmlns::prod::streams::spl::sourceCode::compilationUnitType* compUnit_; //< the xml model
    std::vector< ::xml_schema::type*> stack_;
};

} // namespace SPL

#endif // SPL_SOURCE_CODE_MODEL_EXTRACTOR_H
