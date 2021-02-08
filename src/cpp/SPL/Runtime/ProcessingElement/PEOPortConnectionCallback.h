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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PE_OPORT_CONNECTION_CALLBACK_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PE_OPORT_CONNECTION_CALLBACK_H

#include <TRANS/DataSender.h>

namespace SPL {
// forward declarations
class PETransportOPort;
class PEImpl;

/// Class that represents a PE output port's connection callback handler
class PEOPortConnectionCallback : public UTILS_NAMESPACE::DataSender::ConnectionCallback
{
  public:
    /// destructor
    ~PEOPortConnectionCallback() {}

    /// Create a new object
    /// @param pe PEImpl object
    /// @param portNo output port index for this port
    /// @param portId output port ID for this port
    /// @param label connection endpoint port label
    PEOPortConnectionCallback(PEImpl& pe,
                              uint32_t portNo,
                              const unsigned long long portId,
                              const std::string& label)
      : _pe(pe)
      , _portNo(portNo)
      , _portId(portId)
      , _nsLabel(label)
      , _isOptional(false)
    {}

    /// The sender actively establishing this connection
    /// @param isReconnect true if this is a reconnect (following a previously established
    /// connection); false otherwise
    /// @param isOptional true if this is an optional connection
    void onConnecting(bool isReconnect, bool isOptional);

    /// The sender has successfully established this connection
    void onConnected(const std::string& ns_label);

    /// The sender has disconnected
    void onDisconnected();

    /// The sender has closed connection
    void onClosed();

  private:
    PEImpl& _pe;
    const uint32_t _portNo;           ///< port index (PE-wide unique)
    const unsigned long long _portId; ///< port ID (instance-wide unique)
    std::string _nsLabel;             ///< destination port label
    bool _isOptional;
};

};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PE_OPORT_CONNECTION_CALLBACK_H */
