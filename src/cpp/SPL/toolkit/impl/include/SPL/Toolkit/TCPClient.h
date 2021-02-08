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

#ifndef RUNTIME_TCP_CLIENT_H
#define RUNTIME_TCP_CLIENT_H
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <vector>
#include <SPL/Runtime/Utility/Visibility.h>

namespace xmlns { namespace prod { namespace streams { namespace spl { namespace wireFormatType {
    class wireFormatType;
}}}}};

namespace SPL {
    class ProcessingElement;
    class Metric;
    class WireFormatIO;
    struct iface;

    /// Class to establish a client connection to a TCP server
    /// Initial connection will retry until connected or PE Shutdown
    class DLL_PUBLIC TCPClient {
      public:
        /// Connect to a port on a given host
        /// @param pe Processing Element
        TCPClient (ProcessingElement& pe);

        /// Destructor
        virtual ~TCPClient();

        /// Set the send buffer size
        /// @param size buffer size for socket
        void setSendBufferSize (uint32_t size) { _sendBufferSize = size; }

        /// Set the receive buffer size on the socket
        /// @param size Size to set the socket's receive buffer
        void setReceiveBufferSize (uint32_t size) { _receiveBufferSize = size; }

        /// @param host Hostname
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& host, const std::string& port);

        /// Connect to a port on a given host
        /// @param host Hostname
        /// @param port Port number
        void setup (const std::string& host, uint32_t port);

        /// Connect to a port on a given host using NAM to map from label to host/port
        /// @param label Name of object in NAM containing host/port information
        void setup (const std::string& label);

        /// Return the connection state
        /// @return 'true' if a connection has been established
        bool connected() const { return _connected;}

        /// Return the file descriptor for the current connection
        /// @return file descriptor
        int32_t fileDescriptor() const { return _fd;}

        /// Re-establish a connection to same server.  Retry until connected or PE shutdown
        /// @return new file descriptor for the new connection
        int32_t reconnect();

        /// Shutdown everything
        void terminate();

        /// Has the connection been disconnected
        /// @param anotherConnectionAvailable set to true/false if another connection is
        /// available to reconnect immediately
        bool isDisconnected (bool& anotherConnectionAvailable);

      protected:
        ProcessingElement&      _pe;
        int32_t                 _fd;
        addrinfo               *_ai;
        bool                    _connected;
        std::string             _hostInfo;
        uint32_t                _sendBufferSize;
        uint32_t                _receiveBufferSize;

        void connect (const std::string& host, const std::string& port, std::string* saveHost,
                     uint32_t* savePort);
        void connect (std::string* saveHost, uint32_t* savePort);
        void setLabelDebugString (const std::string& label, const std::string& host,
                                  const std::string& portNo);
        void setDebugString (const std::string& host, const std::string& portNo);
        void labelLookup (const std::string & name, std::string& host, std::string& port);

      private:
        void finishConnection (int32_t i, std::vector<iface>& interfaces,
                               char *host, char *serv, std::string* saveHost,
                               uint32_t* savePort);

    };

    /// Class to establish a client connection to a TCP server, remembering more information
    class DLL_PUBLIC TCPClientExtended : public TCPClient {
      public:
        /// Connect to a port on a given host
        /// @param pe Processing Element
        TCPClientExtended (ProcessingElement& pe)
            : TCPClient(pe), _metric(NULL), _internalWireFormat(NULL) {}

        /// Destructor
        virtual ~TCPClientExtended();

        /// Set the wire format expected by client
        /// @param format wire format expected by client
        /// @param metric metric to be bumped on wire format handshake failures
        void setWireFormat (const std::string& format,
                            Metric& metric) { _wireFormat = format; _metric = &metric; }

        /// Connect to a port on a given host
        /// @param pe Processing Element
        /// @param format wire format expected by server
        TCPClientExtended (ProcessingElement& pe, const std::string& format)
            : TCPClient(pe), _wireFormat(format) {}

        /// Return the IP address of the server
        /// @return the hostname or dotted IP address of the server
        std::string getRemoteIP() const { return _remoteName; }

        /// Return the port number of the server side of the connection
        /// @return the port number of the server
        uint32_t getRemotePort() const { return _remotePort; }

        /// Return the port number of the client side of the connection
        /// @return the port number of the client connection
        uint32_t getLocalPort() const { return _localPort; }

        /// @param host Hostname
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& host, const std::string& port);

        /// Connect to a port on a given host
        /// @param host Hostname
        /// @param port Port number
        void setup (const std::string& host, uint32_t port);

        /// Connect to a port on a given host using NAM to map from label to host/port
        /// @param label Name of object in NAM containing host/port information
        void setup (const std::string& label);

        /// Re-establish a connection to same server.  Retry until connected or PE shutdown
        /// @return new file descriptor for the new connection
        int32_t reconnect();

      protected:
        std::string _remoteName;
        std::string _wireFormat;
        Metric *_metric;
        typedef xmlns::prod::streams::spl::wireFormatType::wireFormatType WireFormatType;
        WireFormatType *_internalWireFormat;
        uint32_t _remotePort;
        uint32_t _localPort;
        void setLocalPort();
        bool confirmWireFormat();
        bool formatMatches (const std::string&, std::string&);
        bool failHandshake(const std::string&);

        bool confirmWireFormat (WireFormatIO&);
        bool failHandshake (WireFormatIO&, const std::string&);
    };
}

#endif // RUNTIME_TCP_CLIENT_H
