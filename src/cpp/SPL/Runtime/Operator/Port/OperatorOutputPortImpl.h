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

#ifndef SPL_RUNTIME_OPERATOR_OUTPUT_PORT_IMPL_H
#define SPL_RUNTIME_OPERATOR_OUTPUT_PORT_IMPL_H

#include <SPL/Runtime/Operator/Port/OperatorOutputPort.h>
#include <SPL/Runtime/Operator/Port/OperatorPortImpl.h>

namespace SPL {
class StreamProperty;
class StreamPropertyCollection;
class SubscriptionExpression;

/// \brief Class representing an operator output port
class OperatorOutputPortImpl
  : public OperatorPortImpl
  , public OperatorOutputPort
{
  public:
    OperatorOutputPortImpl(OperatorImpl& oper, uint32_t index)
      : OperatorPortImpl(oper, OperatorPortImpl::OUTPUT, index)
    {}

    /// Check if this is an exported port (produces a dynamic stream)
    /// @return true if the port is an exported port, false otherwise
    bool isExportedPort() const;

    /// Get the export type for the port
    /// @return export type for the port
    ExportType getExportType() const;

    /// Get all the properties of this output port's stream
    /// @param spc the set of properties retrieved ('out' parameter)
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties
    void getStreamProperties(StreamPropertyCollection& spc) const;

    /// Get the specified properties of this output port's stream
    /// @param spc the set of properties retrieved ('out' parameter)
    /// @param names the names of the properties to be retrieved.
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or one of the names is not present
    void getStreamProperties(StreamPropertyCollection& spc,
                             std::vector<std::string> const& names) const;

    /// Set the specified properties of this output port's stream
    /// @param spc the set of properties to be set
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or one of the properties does not exist
    void setStreamProperties(StreamPropertyCollection const& spc) const;

    /// Add the specified properties to this output port's stream
    /// @param spc the set of properties to be added
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or one of the properties already exists
    void addStreamProperties(StreamPropertyCollection const& spc) const;

    /// Remove the specified properties from this output port's stream
    /// @param spc the set of properties to be removed
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or one of the properties does not exist
    void removeStreamProperties(std::vector<std::string> const& names) const;

    /// Check if a property of this output port's stream exists
    /// @param sp the property to be checked for existence
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties
    bool hasStreamProperty(std::string const& sp) const;

    /// Get a property of this output port's stream
    /// @param sp the property to be retreived ('out' parameter)
    /// @param name the name of the stream property to be retrieved
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or the property does not exist
    void getStreamProperty(StreamProperty& sp, std::string const& name) const;

    /// Set a property of this output port's stream
    /// @param sp the property to be set
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or the property does not exist
    void setStreamProperty(StreamProperty const& sp) const;

    /// Add a property to this output port's stream
    /// @param sp the property to be added
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or the property already exists
    void addStreamProperty(StreamProperty const& sp) const;

    /// Remove a property of an operator's output port's stream
    /// @param sp the property to be removed
    /// @throws SPLRuntimeStreamPropertyException if the stream is not
    /// exported by properties or the property does not exist
    void removeStreamProperty(std::string const& sp) const;

    /// Check if this output port is connected to a PE output port
    /// @return true if connected to a PE output port, false otherwise
    bool isConnectedToAPEOutputPort() const;

    /// Get the PE output port index for this operator output port
    /// @return the PE output port index for the given operator output port index
    /// @throw SPLRuntimeInvalidIndex when the operator port does not have an associated PE port
    uint32_t getPEOutputPortIndex() const;

    /// Get the number of static connections on this port
    /// @return number of static connections on this port
    uint32_t getNumberOfStaticConnections() const;

    /// Get the number of intra-PE static connections
    /// @return number of intra-PE static connections
    uint32_t getNumberOfIntraPEStaticConnections() const;

    /// Get the number of inter-PE static connections
    /// @return number of inter-PE static connections
    uint32_t getNumberOfInterPEStaticConnections() const;

  private:
    friend class OperatorImpl;
    friend class SubmitSignal;
};
};

#endif /*SPL_RUNTIME_OPERATOR_OUTPUT_PORT_IMPL_H*/
