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

#ifndef SPL_OUTPUT_SIDE_EFFECTS_CHECKER_H
#define SPL_OUTPUT_SIDE_EFFECTS_CHECKER_H

#include <SPL/FrontEnd/AstVisitor.h>
#include <SPL/FrontEnd/SideEffectDetector.h>

namespace SPL {

class ParserContext;

/// Check for read/write or write/write uses in output clause
class OutputSideEffectsChecker
  : public SideEffectDetector
  , public AstVisitor
{
  public:
    /// Create a temporary instance of this class and visit the AST with it.
    /// @param pCtx The parser context
    /// @param ast The root node of the ast we are going to walk
    static void run(ParserContext& pCtx, AstNode& ast);

  private:
    /// Constructor for the OutputSideEffectsChecker class
    /// @param pCtx The parser context
    OutputSideEffectsChecker(ParserContext& pCtx, bool emitError)
      : AstVisitor(Debug::None)
      , _pCtx(pCtx)
      , _emitError(emitError)
    {}
    ParserContext& _pCtx;
    bool _emitError;

    virtual void useModError(const std::string& v1,
                             const SourceLocation& loc1,
                             const std::string& v2,
                             const SourceLocation& loc2);
    virtual void multipleStatefulCall(const std::string& fcn1,
                                      const std::string& fcn2,
                                      const SourceLocation& loc1);
    virtual void visitOpInvoke(AstNode&);
    virtual void visitOpInvokeOutput(AstNode&);
};

}

#endif // SPL_OUTPUT_SIDE_EFFECTS_CHECKER_H
