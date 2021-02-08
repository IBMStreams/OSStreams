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

#ifndef SPL_DEBUGGER_ValueDefinition_H
#define SPL_DEBUGGER_ValueDefinition_H

#include <boost/shared_ptr.hpp>
#include <string>
#include <vector>

#include <SPL/Runtime/ProcessingElement/PEImpl.h>

namespace SPL {

class ProbePointServices;
class ValueDefinition;
namespace Meta {
class BaseType;
}
typedef boost::shared_ptr<ValueDefinition> ValueDefinitionPtr;
typedef std::vector<ValueDefinitionPtr> ValueDefinitionVector;

/*!
 * Each instance of this class represents a probe point
 * on the SPL application's data path.  A probe point
 * is associated with an operator's port and is distinguished
 * by the operator's name, port type (input or output), and
 * port index.
 *
 * This class represents the base function shared by the
 * various probe point types - e.g. breakpoints, tracepoints, etc.
 */
class ValueDefinition
{
    std::string _name;
    const Meta::BaseType* _type;
    std::string _opName;
    PEImpl::PortType _portType;
    uint32_t _portIndex;
    static void matchValueDefinition(ValueDefinitionVector& result,
                                     const std::string& matchName,
                                     const std::string& matchType,
                                     const std::string& opName,
                                     PEImpl::PortType portType,
                                     uint32_t portIndex,
                                     const Meta::BaseType& baseType);
    static bool isMatchingString(const std::string& StringVal, const std::string& compareVal);
    static bool useRegex(const std::string& compareVal);

  public:
    /// Constructor - populate one record
    /// @param name Attribute Name
    /// @param type Attribute type
    /// @param opName Operator Name
    /// @param portType Input or output
    /// @param portIndex Port Number
    ValueDefinition(const std::string& name,
                    const Meta::BaseType* type,
                    const std::string& opName,
                    PEImpl::PortType portType,
                    uint32_t portIndex)
      : _name(name)
      , _type(type)
      , _opName(opName)
      , _portType(portType)
      , _portIndex(portIndex)
    {}

    /// Return attribute name
    /// @return attribute name
    inline std::string getName() const { return _name; }

    /// Return attribute type
    /// @return attribute type
    inline const Meta::BaseType* getType() const { return _type; }

    /// Return operator name
    /// @return operator name
    inline std::string getOpName() const { return _opName; }

    /// Return port type
    /// @return port type
    inline PEImpl::PortType getPortType() const { return _portType; }

    /// Return port index
    /// @return port index
    inline uint32_t getportIndex() const { return _portIndex; }

    /// Return attribute name and type as a string
    /// @ return field : type
    const std::string toString() const;

    /// Return port information as a string
    /// @return opName i/o portIndex
    const std::string portString() const;

    /// Search for matching attributes
    /// @param result vector of matching attributes
    /// @param services Probe Point Services
    /// @param matchName regular expression to match name
    /// @param matchType regular expression to match type
    /// @return number of results
    static uint32_t find(ValueDefinitionVector& result,
                         ProbePointServices& services,
                         const std::string& matchName,
                         const std::string& matchType)
    {
        return find(result, services, matchName, matchType, "*");
    }

    /// Search for matching attributes
    /// @param result vector of matching attributes
    /// @param services Probe Point Services
    /// @param matchName regular expression to match name
    /// @param matchType regular expression to match type
    /// @param matchOpName regular expression to match operator name
    /// @return number of results
    static uint32_t find(ValueDefinitionVector& result,
                         ProbePointServices& services,
                         const std::string& matchName,
                         const std::string& matchType,
                         const std::string& matchOpName)
    {
        return find(result, services, matchName, matchType, matchOpName, "*", "*");
    }

    /// Search for matching attributes
    /// @param result vector of matching attributes
    /// @param services Probe Point Services
    /// @param matchName regular expression to match name
    /// @param matchType regular expression to match type
    /// @param matchOpName regular expression to match operator name
    /// @param matchPortType regular expression to match port type
    /// @param matchPortIndex regular expression to match port index
    /// @return number of results
    static uint32_t find(ValueDefinitionVector& result,
                         ProbePointServices& services,
                         const std::string& matchName,
                         const std::string& matchType,
                         const std::string& matchOpName,
                         const std::string& matchPortType,
                         const std::string& matchPortIndex);
};
}

#endif /* SPL_DEBUGGER_ValueDefinition_H */
