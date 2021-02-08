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

#ifndef SPL_RUNTIME_OPERATOR_PORT_H
#define SPL_RUNTIME_OPERATOR_PORT_H

/*!
 * \file OperatorPort.h \brief Definition of the SPL::OperatorPort class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <inttypes.h>
#include <memory>
#include <string>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    namespace Meta {
        class BaseType;
    }
    class Tuple;

    /// \brief Class that forms a common base for operator ports
    class OperatorPort
    {
    public:
#ifndef DOXYGEN_SKIP_FOR_USERS
        /// Destructor
        ///
        virtual ~OperatorPort() {}
#endif /*DOXYGEN_SKIP_FOR_USERS*/

        /// Get the port index
        /// @return the port index
        virtual uint32_t getIndex() const = 0;

        /// Get the port name. This is the port alias name, if specified in the
        /// SPL source for this operator instance. If a port alias is not
        /// specified, then it is the local name of the first stream attached to
        /// the port. It is an unqualifed name in both cases.
        /// @return the port name
        virtual std::string const & getName() const = 0;

        /// Create a tuple from this port
        /// @return tuple that is created, memory is owned by the caller
        virtual std::auto_ptr<Tuple> createTuple() const = 0;

        /// Get the tuple type for the port
        /// @return tuple type for the port
        virtual Meta::BaseType const & getTupleType() const = 0;
    };
};

#endif /*SPL_RUNTIME_OPERATOR_PORT_H*/
