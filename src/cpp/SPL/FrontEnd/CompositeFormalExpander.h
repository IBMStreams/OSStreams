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

#ifndef SPL_COMPOSITE_FORMAL_EXPANDER_H
#define SPL_COMPOSITE_FORMAL_EXPANDER_H

#include <SPL/FrontEnd/TreeHelper.h>

namespace SPL {
/// Expand uses of composite formals to the default value, if any, from
/// the definition. This function destructively modifies the AST.
/// @param parentAst compilation unit AST or recursive descendent
/// @return true if an immediate child of parentAst changed
bool expandCompositeFormals(ParserContext& pCtx, AstNode& parentAst);
}; /*namespace SPL*/

#endif /*SPL_COMPOSITE_FORMAL_EXPANDER_H*/
