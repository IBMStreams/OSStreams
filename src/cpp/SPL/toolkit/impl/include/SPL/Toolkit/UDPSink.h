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

#ifndef RUNTIME_UDP_SINK_H
#define RUNTIME_UDP_SINK_H
#include <string>
#include <stdint.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
    class ProcessingElement;

    /// Class to establish a UDP Sink
    class DLL_PUBLIC UDPSink {
      public:
        /// Construct a UDP Sink
        /// @param pe Processing Element
        UDPSink (ProcessingElement& pe) : _pe(pe), _fd(-1), _sendBufferSize(0), _ttl(0), _hasTTL(false) {}

        /// Destruct the UDP Sink, closing the socket if it is open
        virtual ~UDPSink() { terminate(); }

        /// Set the send buffer size
        /// @param size buffer size for socket
        void setSendBufferSize (uint32_t size) { _sendBufferSize = size; }

        /// Set the time-to-live (TTL) for the generated datagrams.
        /// @param ttl time-to-live value
        void setTTL (uint8_t ttl) { _ttl = ttl; _hasTTL = true;}

        /// Set up a UDP connection, to host 'host' on port 'port'
        /// @param host Host to send to
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& host, const std::string& port);

        /// Set up a UDP connection, to host 'host' on port 'port'
        /// @param host Host to send to
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& host, uint32_t port);

        /// Setup a UDP connection, using host/port from NAM
        /// @param label Name of object in NAM to contain host/port information
        void setup (const std::string& label);

        /// Return the file descriptor for the connection
        /// @return file descriptor
        int32_t fileDescriptor() const { return _fd;}

        /// Shutdown everything
        void terminate();

      private:
        ProcessingElement&      _pe;
        int32_t                 _fd;
        uint32_t                _sendBufferSize;
        uint8_t                 _ttl; // time-to-live
        bool                    _hasTTL;

        void connect (const std::string& host, const std::string& port);
        void setupSendBufferSize();
        void setupMulticast();
        void setupUnicast();
    };
}

#endif // RUNTIME_UDP_SINK_H
