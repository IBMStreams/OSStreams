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

#include <SPL/Toolkit/RuntimeException.h>

using namespace SPL;

IMPL_EXCEPTION(SPL, SPLRuntimeDecompressOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeFileSourceOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeFileSinkOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeTCPOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeTCPSourceOperator, SPLRuntimeTCPOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeTCPSinkOperator, SPLRuntimeTCPOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeV1TCPSinkOperator, SPLRuntimeTCPSinkOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeUDPSourceOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeUDPSinkOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeSplitOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeDirectoryScanOperator, SPLRuntimeOperator)
IMPL_EXCEPTION(SPL, SPLRuntimeJavaOperator, SPLRuntimeOperator)
