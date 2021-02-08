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

#ifndef SPL_RUNTIME_OPERATOR_OUTPUT_PORT_H
#define SPL_RUNTIME_OPERATOR_OUTPUT_PORT_H

/*!
 * \file OperatorOutputPort.h \brief Definition of the SPL::OperatorOutputPort class.
 */

#include <SPL/Runtime/Operator/Port/OperatorPort.h>

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <vector>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    class StreamProperty;
    class StreamPropertyCollection;
    class SubscriptionExpression;

    /// \brief Class that represents an operator output port
    class OperatorOutputPort : public virtual OperatorPort
    {
    public:
        /// Export type enumeration
        enum ExportType
        {
            ByName,       //!< Export by name
            ByProperties, //!< Export by properties
            NotExported   //!< Invalid (not exported)
        };

        /// Check if this is an exported port
        /// @return true if the port is an exported port, false otherwise
        virtual bool isExportedPort() const = 0;

        /// Get the export type for the port
        /// @return export type for the port
        virtual ExportType getExportType() const = 0;

        /// Get all the properties of this output port's stream
        /// @param spc the set of properties retrieved ('out' parameter)
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties
        virtual void getStreamProperties(StreamPropertyCollection & spc) const = 0;

        /// Get the specified properties of this output port's stream
        /// @param spc the set of properties retrieved ('out' parameter)
        /// @param names the names of the properties to be retrieved
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or one of the names is not present
        virtual void getStreamProperties(StreamPropertyCollection & spc,
                                         std::vector<std::string> const & names) const = 0;

        /// %Set the specified properties of this output port's stream
        /// @param spc the set of properties to be set
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or one of the properties does not exist
        virtual void setStreamProperties(StreamPropertyCollection const & spc) const = 0;

        /// Add the specified properties to this output port's stream
        /// @param spc the set of properties to be added
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or one of the properties already exists
        virtual void addStreamProperties(StreamPropertyCollection const & spc) const = 0;

        /// Remove the specified properties from this output port's stream
        /// @param spc the set of properties to be removed
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or one of the properties does not exist
        virtual void removeStreamProperties(std::vector<std::string> const & spc) const = 0;

        /// Check if a property of this output port's stream exists
        /// @param sp the property to be checked for existence
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties
        virtual bool hasStreamProperty(std::string const & sp) const = 0;

        /// Get a property of this output port's stream
        /// @param sp the property to be retreived ('out' parameter)
        /// @param name the name of the stream property to be retrieved
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or the property does not exist
        virtual void getStreamProperty(StreamProperty & sp, std::string const & name) const = 0;

        /// %Set a property of this output port's stream
        /// @param sp the property to be set
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or the property does not exist
        virtual void setStreamProperty(StreamProperty const & sp) const = 0;

        /// Add a property to this output port's stream
        /// @param sp the property to be added
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or the property already exists
        virtual void addStreamProperty(StreamProperty const & sp) const = 0;

        /// Remove a property of an operator's output port's stream
        /// @param sp the property to be removed
        /// @throws SPLRuntimeStreamPropertyException if the stream is not
        /// exported by properties or the property does not exist
        virtual void removeStreamProperty(std::string const & sp) const = 0;

        /// Check if this output port is connected to a PE output port
        /// @return true if connected to a PE output port, false otherwise
        virtual bool isConnectedToAPEOutputPort() const = 0;

        /// Get the PE output port index for this operator output port
        /// @pre isConnectedToAPEOutputPort() == true
        /// @return the PE output port index for the given operator output port index
        /// @throw SPLRuntimeInvalidIndexException when the operator port does not have an associated PE port
        virtual uint32_t getPEOutputPortIndex() const = 0;
    };
};

#endif /*SPL_RUNTIME_OPERATOR_OUTPUT_PORT_H*/
