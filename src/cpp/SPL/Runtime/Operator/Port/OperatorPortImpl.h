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

#ifndef SPL_RUNTIME_OPERATOR_PORT_IMPL_H
#define SPL_RUNTIME_OPERATOR_PORT_IMPL_H

#include <SPL/Runtime/Operator/Port/OperatorPort.h>
#include <memory>
#include <typeinfo>

namespace SPL {
namespace Meta {
class BaseType;
}
class OperatorImpl;
class Tuple;

/// \brief Common base class for operator ports
class OperatorPortImpl : public virtual OperatorPort
{
  public:
    /// Port type
    enum PortType
    {
        INPUT, ///< Input port
        OUTPUT ///< Output port
    };

    /// Constructor
    /// @param oper operator holding this port
    /// @param type port type, input or output
    /// @param index port index
    OperatorPortImpl(OperatorImpl& oper, PortType type, uint32_t index);

    /// Destructor
    virtual ~OperatorPortImpl() {}

    /// Get the port index
    /// @return the port index
    uint32_t getIndex() const { return index_; }

    /// Get the port name. This is the port alias name, if specified in the
    /// SPL source for this operator instance. If a port alias is not
    /// specified, then it is the local name of the first stream attached to
    /// the port. It is an unqualifed name in both cases.
    /// @return the port name
    std::string const& getName() const { return name_; }

    /// Create a tuple from this port
    /// @return tuple that is created, memory is owned by the caller
    std::auto_ptr<Tuple> createTuple() const;

    /// Get the tuple type for the port
    /// @return tuple type for the port
    Meta::BaseType const& getTupleType() const;

    /// Get the type id for the tuple type
    /// @return type id for the tuple type
    std::type_info const& getTypeId() const;

    /// Get the number of static connections on this port
    /// @return number of static connections on this port
    virtual uint32_t getNumberOfStaticConnections() const = 0;

    /// Get the number of intra-PE static connections
    /// @return number of intra-PE static connections
    virtual uint32_t getNumberOfIntraPEStaticConnections() const = 0;

    /// Get the number of inter-PE static connections
    /// @return number of inter-PE static connections
    virtual uint32_t getNumberOfInterPEStaticConnections() const = 0;

    /// Check if the port carries facade tuples
    /// @return true if the port carries facade tuples, false otherwise
    bool isFacade() const;

  protected:
    PortType type_;
    uint32_t index_;
    std::string name_;
    OperatorImpl* oper_;
};
};

#endif /*SPL_RUNTIME_OPERATOR_PORT_IMPL_H*/
