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

#include <SPL/Core/CompilerException.h>

using namespace SPL;

IMPL_EXCEPTION(SPL, SPLCompiler, Distillery::Distillery)

IMPL_EXCEPTION(SPL, SPLCompilerAssertionFailed, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerDivModByZero, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerInvalidArgument, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerInvalidExpression, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerInvalidIndex, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerNoValidCanonicalHostNames, SPLCompiler)
IMPL_EXCEPTION(SPL, SPLCompilerScriptFailed, SPLCompiler)
