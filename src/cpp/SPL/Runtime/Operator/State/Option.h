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
 * \file Option.h \brief Definition of the SPL::Option class.
 */
#ifndef SPL_DSA_OPTIONS_H
#define SPL_DSA_OPTIONS_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
/// \brief Class that is used to specify various options for creating or opening a Data Store Entry.
class DLL_PUBLIC Option
{
  public:
    bool create_if_missing; // create the Data Store Entry if it does not exist
    bool error_if_exist;    // return error if the Data Store Entry already exists
    void* lowLevelOptions; // an opaque handle to be interpreted by the low level Data Store Adapter
                           // implementation
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_OPTIONS_H
