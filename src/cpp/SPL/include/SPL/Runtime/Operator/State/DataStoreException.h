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

/*
 * \file DataStoreException.h \brief Definition of the SPL::DataStoreException class.
 */

#ifndef SPL_RUNTIME_OPERATOR_STATE_DATA_STORE_EXCEPTION
#define SPL_RUNTIME_OPERATOR_STATE_DATA_STORE_EXCEPTION

#include <SPL/Runtime/Common/RuntimeException.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
PUSH_DLL_PUBLIC

DECL_EXCEPTION(SPL, DataStore, SPLRuntime)

POP_DLL_PUBLIC

#ifdef DOXYGEN_SKIP_FOR_USERS
    // Note: this is not real code, it is for documentation purposes

    /// \brief Class that represents runtime exceptions thrown by Checkpointing APIs
    class DataStoreException : public SPLRuntimeException { };
#endif

} // namespace SPL

#endif // SPL_RUNTIME_OPERATOR_STATE_DATA_STORE_EXCEPTION
