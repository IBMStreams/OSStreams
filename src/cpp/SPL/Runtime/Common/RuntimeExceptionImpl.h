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

#ifndef SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION_IMPL
#define SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION_IMPL

#include <SPL/Runtime/Common/RuntimeException.h>

namespace SPL {

PUSH_DLL_PUBLIC
DECL_EXCEPTION(SPL, SPLRuntimeInvalidOperator, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeAssertionFailed, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeUnableToLoadADL, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeUnableToParseAADL, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeUnableToCreateAADL, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeMissingSubmissionValue, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeStreamPropertyNotFound, SPLRuntimeStreamProperty)
DECL_EXCEPTION(SPL, SPLRuntimeMissingArgument, SPLRuntimeSubscription)
DECL_EXCEPTION(SPL, SPLRuntimePayload, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeInternalError, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeDebuggerError, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeFailedOperators, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeXMLParsing, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeJNI, SPLRuntime)
DECL_EXCEPTION(SPL, SPLRuntimeShutdown, SPLRuntime)
POP_DLL_PUBLIC

};

#endif /* SPL_RUNTIME_COMMON_RUNTIME_EXCEPTION_IMPL */
