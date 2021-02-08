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

#ifndef RUNTIME_UDP_SOURCE_H
#define RUNTIME_UDP_SOURCE_H
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <SPL/Runtime/Utility/Visibility.h>

namespace SPL {
    class ProcessingElement;

    /// Class to establish a UDP Source
    class DLL_PUBLIC UDPSource {
      public:
        /// Construct a UDP Source
        /// @param pe Processing Element
        UDPSource (ProcessingElement& pe) : _pe(pe), _fd(-1), _receiveBufferSize(0) {}

        /// Destruct a UDP Source
        ~UDPSource ();

        /// Set the receive buffer size on the socket
        /// @param size Size to set the socket's receive buffer
        void setReceiveBufferSize (uint32_t size) { _receiveBufferSize = size; }

        /// Set up a UDP connection, on port 'port'
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& port);

        /// Set up a UDP connection, on port 'port'
        /// @param port Port number, or service (e.g. "smtp")
        void setup (uint32_t port);

        /// Set up a UDP connection, on port 'port', accepting packets from host or
        /// multicast address.
        /// @param addr host address or multicast address
        /// @param interface Interface name, or empty string
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& addr, const std::string& interface,
                    const std::string& port);

        /// Set up a UDP connection, on port 'port', accepting packets from host or
        /// multicast address.
        /// @param addr host address or multicast address
        /// @param interface Interface name, or empty string
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& addr, const std::string& interface, uint32_t port);

        /// Setup a UDP connection, and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        void setupLabel (const std::string& label);

        /// Setup a UDP connection on port 'port', and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        /// @param port Port number
        void setupLabel (const std::string& label, const std::string& port);

        /// Setup a UDP connection on port 'port', and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        /// @param port Port number
        void setupLabel (const std::string& label, uint32_t port);

        /// Save a user interface parameter
        /// @param interface user specified interface
        void setInterface (const std::string& interface)
            { _interface = interface; }

        /// Return the file descriptor for the connection
        /// @return file descriptor
        int32_t fileDescriptor() const { return _fd;}

        /// Shutdown everything
        void terminate();

      protected:
        ProcessingElement&      _pe;
        int32_t                 _fd;
        std::string             _label;
        std::string             _interface;
        uint32_t                _receiveBufferSize;

        void connect (const std::string& addr, const std::string& interface,
                      const std::string& port);
        void connect (const std::string& port);
        void createSocket (const addrinfo& addr, const std::string& port);
        void setMulticastInterface (ipv6_mreq& mcastaddr, const std::string& interface);
        void setMulticastInterface (ip_mreqn& mcastaddr, const std::string& interface);
    };

    /// Class to establish a UDP Source, remembering more information
    class DLL_PUBLIC UDPSourceExtended : public UDPSource {
      public:
        /// Construct a UDP Source
        /// @param pe Processing Element
        UDPSourceExtended (ProcessingElement& pe) : UDPSource(pe) {}

        /// Return the IP address of the client
        /// @return the hostname or dotted IP address of the client
        static std::string getRemoteIP(const sockaddr_storage& sa);

        /// Return the port number of the client side
        /// @return the port number of the client
        static uint32_t getRemotePort(const sockaddr_storage& sa);

        /// Return the port number that the server is listening on
        /// @return the port number of the server listening port
        uint32_t getServerPort() const { return _serverPort; }

        /// Set up a UDP connection, on port 'port'
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& port);

        /// Set up a UDP connection, on port 'port'
        /// @param port Port number, or service (e.g. "smtp")
        void setup (uint32_t port);

        /// Set up a UDP connection, on port 'port', accepting packets from host or
        /// multicast address.
        /// @param addr host address or multicast address
        /// @param interface Interface name, or empty string
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& addr, const std::string& interface,
                    const std::string& port);

        /// Set up a UDP connection, on port 'port', accepting packets from host or
        /// multicast address.
        /// @param addr host address or multicast address
        /// @param interface Interface name, or empty string
        /// @param port Port number, or service (e.g. "smtp")
        void setup (const std::string& addr, const std::string& interface, uint32_t port);

        /// Setup a UDP connection, and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        void setupLabel (const std::string& label);

        /// Setup a UDP connection on port 'port', and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        /// @param port Port number
        void setupLabel (const std::string& label, const std::string& port);

        /// Setup a UDP connection on port 'port', and register this host/port using NAM
        /// @param label Name of object in NAM to contain host/port information
        /// @param port Port number
        void setupLabel (const std::string& label, uint32_t port);

        //

      protected:
        uint32_t _serverPort;
        void setServerPort();
    };
}

#endif // RUNTIME_UDP_SOURCE_H
