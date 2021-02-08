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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_TYPE
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_TYPE

#include <SPL/Runtime/Common/RuntimeException.h>

namespace SPL {

/// Class to hold transport type information
class PETransportType
{
  public:
    enum Value
    {
        TCP,
        TCP_SECURE
    };

  public:
    /// Constructor
    /// default to TCP transport
    PETransportType()
      : _value(TCP)
    {}

    /// Constructor
    /// @param value Desired transport type
    PETransportType(Value const& value)
      : _value(value)
    {}

    /// Constructor
    /// @param name String representation of transport type
    PETransportType(std::string const& name) { this->operator=(name); }

    /// Copy Constructor
    /// @param trp transport type to copy
    PETransportType(PETransportType const& trp)
      : _value(trp._value)
    {}

    /// Return transport type
    /// @return transport type
    PETransportType::Value getValue() const { return _value; }

    /// Set the transport type
    /// @param value new transport type
    PETransportType& operator=(Value const& value)
    {
        _value = value;
        return *this;
    }

    /// Set the transport type
    /// @param name new transport type
    PETransportType& operator=(std::string const& name)
    {
        if (name == "TCP") {
            _value = TCP;
        } else if (name == "TCP_SECURE") {
            _value = TCP_SECURE;
        } else {
            THROW(SPLRuntime, "Unknown transport type '" << name << "' - internal error");
        }
        return *this;
    }

    /// Return the transport type
    /// @return transport type
    operator Value() { return _value; }

    /// Return the name of the transport type
    /// @return transport type name
    operator std::string()
    {
        switch (_value) {
            case TCP:
                return "TCP";
            case TCP_SECURE:
                return "TCP_SECURE";
            default:
                assert(!"cannot happen!");
        }
        return "";
    }

  private:
    Value _value;
};

};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_TRANSPORT_TYPE */
