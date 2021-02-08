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

#ifndef RUNTIME_TCP_SERVER_H
#define RUNTIME_TCP_SERVER_H
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
    class ProcessingElement;
    class Metric;
    class WireFormatIO;

    /// Class to establish a server TCP connection
    /// Initial connection will retry until some connects or PE Shutdown
    class DLL_PUBLIC TCPServer {
      public:
        /// Setup a TCP port on the current machine and wait for a connection
        /// @param pe Processing Element
        TCPServer (ProcessingElement& pe);

        /// Destructor
        ~TCPServer();

        /// Set the send buffer size
        /// @param size buffer size for socket
        void setSendBufferSize (uint32_t size) { _sendBufferSize = size; }

        /// Set the receive buffer size on the socket
        /// @param size Size to set the socket's receive buffer
        void setReceiveBufferSize (uint32_t size) { _receiveBufferSize = size; }

        /// Save a user interface parameter
        /// @param interface user specified interface
        void setInterface (const std::string& interface)
            { _interface = interface; }

        /// Setup a TCP port on the current machine and wait for a connection
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& port);

        /// Setup a TCP port on the current machine and wait for a connection
        /// @param port Port number
        void setup (uint32_t port);

        /// Setup a TCP port on the current machine.  Register the port number using NAM
        /// with the given label, so that it can be found by clients
        /// @param label Name of object in NAM to store host/port information
        /// @param port Port to be used or service ("0" to pick any free port)
        void setup (const std::string& label, const std::string& port);

        /// Setup a TCP port on the current machine.  Register the port number using NAM
        /// with the given label, so that it can be found by clients
        /// @param label Name of object in NAM to store host/port information
        /// @param port Port to be used (0 to pick any free port)
        void setup (const std::string& label, uint32_t port);

        /// Return the connection state
        /// @return 'true' if a connection has been established
        bool connected() const { return _connected;}

        /// Return the file descriptor for the current connection
        /// @return file descriptor
        int32_t fileDescriptor() const { return _fd;}

         /// Wait for a new a connection.  Retry until someone connects or PE shutdown
         /// @return new file descriptor for the new connection
        int32_t reconnect();

        // Shutdown everything
        void terminate() ;

        /// Has the connection been disconnected
        /// @param anotherConnectionAvailable set to true/false if another connection is
        /// available to reconnect immediately
        bool isDisconnected (bool& anotherConnectionAvailable);

        /// Sets a tuple literal containing values to use for the keepalive settings
        /// when running in server mode.  The string is empty if keepalive not to be used.
        /// @param time The time in seconds that the connection is idle before performing a probe.
        /// @param probes The number of probes to perform.
        /// @param interval The time in seconds between intervals.
        void setKeepAlive(int32_t time, int32_t probes, int32_t interval) ;

      protected:
        ProcessingElement&      _pe;
        int32_t                 _fd;
        pollfd                  _baseFds[2];
        uint32_t                _numBaseFds;
        bool                    _connected;
        std::string             _label;
        std::string             _hostInfo;
        std::string             _interface;
        uint32_t                _sendBufferSize;
        uint32_t                _receiveBufferSize;
        int32_t                 _keepAliveTime;
        int32_t                 _keepAliveProbes;
        int32_t                 _keepAliveInterval;

        void connect (const std::string& port);
        void connect ();
        void setDebugString (const std::string& port);
        void setDebugLabelString (const std::string& label, const std::string& port);
        void setDebugLabelString (const std::string& label, int32_t port);
        void setBufferSizes (int32_t fd);
        void unregister();
    };

    /// Class to establish a server connection to a TCP client, remembering more information
    class DLL_PUBLIC TCPServerExtended : public TCPServer {
      public:
        /// Setup a TCP port on the current machine and wait for a connection
        /// @param pe Processing Element
        TCPServerExtended (ProcessingElement& pe) : TCPServer(pe), _metric(NULL) {}

        /// Set the wire format expected by server
        /// @param format wire format expected by server
        /// @param metric metric to be bumped on wire format handshake failures
        void setWireFormat (const std::string& format,
                            Metric& metric) { _wireFormat = format; _metric = &metric; }

        /// Return the IP address of the client
        /// @return the hostname or dotted IP address of the client
        std::string getRemoteIP() const { return _remoteName; }

        /// Return the port number of the client side of the connection
        /// @return the port number of the client
        uint32_t getRemotePort() const { return _remotePort; }

        /// Return the port number of the server side of the connection
        /// @return the port number of the server connection
        uint32_t getLocalPort() const { return _localPort; }

        /// Return the port number that the server is listening on
        /// @return the port number of the server listening port
        uint32_t getServerPort() const { return _serverPort; }

        /// Setup a TCP port on the current machine and wait for a connection
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& port);

        /// Setup a TCP port on the current machine and wait for a connection
        /// @param port Port number
        void setup (uint32_t port);

        /// Setup a TCP port on the current machine.  Register the port number using NAM
        /// with the given label, so that it can be found by clients
        /// @param label Name of object in NAM to store host/port information
        /// @param port Port to be used or service ("0" to pick any free port)
        void setup (const std::string& label, const std::string& port);

        /// Setup a TCP port on the current machine.  Register the port number using NAM
        /// with the given label, so that it can be found by clients
        /// @param label Name of object in NAM to store host/port information
        /// @param port Port to be used (0 to pick any free port)
        void setup (const std::string& label, uint32_t port);

        /// Wait for a new a connection.  Retry until someone connects or PE shutdown
        /// @return new file descriptor for the new connection
        int32_t reconnect();

      protected:
        std::string _remoteName;
        std::string _wireFormat;
        Metric *_metric;
        uint32_t _remotePort;
        uint32_t _localPort;
        uint32_t _serverPort;
        void setServerPort();
        void setInformation();
        bool confirmWireFormat();
        bool failHandshake();
        bool succeedHandshakeAndDropConnection();

        bool confirmWireFormat (WireFormatIO&);
        bool failHandshake (WireFormatIO&);
    };
}

#endif // RUNTIME_TCP_SERVER_H
