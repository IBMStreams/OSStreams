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

#ifndef SPL_RUNTIME_OPERATOR_OPTIONAL_CONTEXT_H
#define SPL_RUNTIME_OPERATOR_OPTIONAL_CONTEXT_H

/*!
 * \file OptionalContext.h \brief Definition of the SPL::OptionalContext class.
 */

#include <string>

#define CHECKPOINT        "Checkpoint"
#define CONSISTENT_REGION "ConsistentRegion"

namespace SPL
{
    /// Class that serves as a base for all optional operator contexts.
    class OptionalContext
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        virtual ~OptionalContext() {}
#endif

        /// Get the optional context name.
        /// @return context name
        virtual std::string const & getName() const = 0;
    };
}

#endif // SPL_RUNTIME_OPERATOR_OPTIONAL_CONTEXT_H
