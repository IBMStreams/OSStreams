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

#ifndef SPL_DSA_DATA_STORE_EOF_EXCEPTION
#define SPL_DSA_DATA_STORE_EOF_EXCEPTION

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL {
PUSH_DLL_PUBLIC
DECL_EXCEPTION(SPL, DataStoreEOF, DataStore)
POP_DLL_PUBLIC
} // namespace SPL

#endif // SPL_DSA_DATA_STORE_EOF_EXCEPTION
