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

#ifndef SPL_FRONT_END_SIDE_EFFECT_DETECTOR_H
#define SPL_FRONT_END_SIDE_EFFECT_DETECTOR_H

#include <antlr3interfaces.h>
#include <string>

namespace SPL {
typedef ANTLR3_BASE_TREE_struct AstNode;
class SourceLocation;
class ParserContext;

class SideEffectDetector
{
  public:
    /// Virtual destructr to make g++ happy
    virtual ~SideEffectDetector() {}

    /// Find out if the AST has destructive side effects
    /// @param pCtx parser context for source location detection
    /// @param ast AST to check for side effects
    void findSideEffects(ParserContext const& pCtx, AstNode& ast);

    /// Report an error involving use/mod
    /// @param v1 name of variable that modifies data used by v2
    /// @param loc1 location of v1
    /// @param v2 name of other variable using/modifying data
    /// @param location of v1
    virtual void useModError(const std::string& v1,
                             const SourceLocation& loc1,
                             const std::string& v2,
                             const SourceLocation& loc2) = 0;

    /// Report an error with multiple stateful calls
    /// @param fcn1 name of first state function
    /// @param fcn2 name of other stateful function
    /// @param loc1 location of v1
    virtual void multipleStatefulCall(const std::string& fcn1,
                                      const std::string& fcn2,
                                      const SourceLocation& loc1) = 0;
};
}

#endif /* SPL_FRONT_END_SIDE_EFFECT_DETECTOR_H */
