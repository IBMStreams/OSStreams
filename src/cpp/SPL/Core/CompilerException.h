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

#ifndef SPL_COMPILER_EXCEPTION_H
#define SPL_COMPILER_EXCEPTION_H

#include <SPL/Runtime/Utility/Visibility.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>

namespace SPL {
PUSH_DLL_PUBLIC

DECL_EXCEPTION(SPL, SPLCompiler, Distillery::Distillery)

DECL_EXCEPTION(SPL, SPLCompilerAssertionFailed, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerDivModByZero, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerInvalidArgument, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerInvalidExpression, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerInvalidIndex, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerNoValidCanonicalHostNames, SPLCompiler)
DECL_EXCEPTION(SPL, SPLCompilerScriptFailed, SPLCompiler)

POP_DLL_PUBLIC
};

#endif /* SPL_COMPILER_EXCEPTION_H */
