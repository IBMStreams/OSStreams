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

#ifndef _SPL_PARSER_EXCEPTION_
#define _SPL_PARSER_EXCEPTION_

#include <SPL/Core/CompilerException.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {

PUSH_DLL_PUBLIC
// SPL, <exception-base-name>, <exception-parent-base-name>
DECL_EXCEPTION(SPL, SPLParser, SPLCompiler)
DECL_EXCEPTION(SPL, SPLParserInternalError, SPLParser)
POP_DLL_PUBLIC

};

#endif /* _SPL_PARSER_EXCEPTION_ */
