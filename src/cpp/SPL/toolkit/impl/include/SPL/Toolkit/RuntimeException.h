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

#ifndef SPL_TOOLKIT_RUNTIME_EXCEPTION
#define SPL_TOOLKIT_RUNTIME_EXCEPTION

#include <SPL/Runtime/Common/RuntimeException.h>

namespace SPL {

PUSH_DLL_PUBLIC
DECL_EXCEPTION(SPL, SPLRuntimeDecompressOperator,     SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeFileSourceOperator,     SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeFileSinkOperator,       SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeTCPOperator,            SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeTCPSourceOperator,      SPLRuntimeTCPOperator)
DECL_EXCEPTION(SPL, SPLRuntimeTCPSinkOperator,        SPLRuntimeTCPOperator)
DECL_EXCEPTION(SPL, SPLRuntimeV1TCPSinkOperator,      SPLRuntimeTCPSinkOperator)
DECL_EXCEPTION(SPL, SPLRuntimeUDPSourceOperator,      SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeUDPSinkOperator,        SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeSplitOperator,          SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeDirectoryScanOperator,  SPLRuntimeOperator)
DECL_EXCEPTION(SPL, SPLRuntimeJavaOperator,           SPLRuntimeOperator)
POP_DLL_PUBLIC

};

#endif /* SPL_TOOLKIT_RUNTIME_EXCEPTION */
