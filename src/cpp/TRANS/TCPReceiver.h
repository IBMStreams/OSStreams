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

#ifndef TRANS_TCPRECEIVER_H_
#define TRANS_TCPRECEIVER_H_

#include <TRANS/DataReceiver.h>
#include <TRANS/DataSender.h>
#include <TRANS/OperatorPortLabelImpl.h>
#include <TRANS/PortLabelImpl.h>
#include <TRANS/TCPCommon.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/LinkedList.h>
#include <UTILS/Socket.h>
#include <UTILS/UTILSTypes.h>
#include <netinet/in.h>

#include <string>
#include <vector>

// forward declarations
namespace NAM {
class NAM_NameService;
}

UTILS_NAMESPACE_BEGIN

/// \internal
/// This class implements a TCP receiver to receive data from several
/// TCPSender.
class TCPReceiver : public DataReceiver
{
  public:
    static uint16_t PORT_BASE;

  protected:
    typedef struct _internal_port_data_t
    {
        DataReceiver::Callback* callback; // PE Input Port message callback
        user_data_t user_data;            // PE Input Port user data
        PortLabelImpl ns_label;           // PE Input port Label
        OperatorPortLabelImpl operator_ns_label;
        bool accept; // Flag indicating if this is a socket
                     // listening for new connections
        InetSocket* sock;
        unsigned char* prefetch_buffer;   // Pre-fetch buffer
        uint32_t prefetch_offset;         // Current offset into pre-fetch buffer
        uint32_t prefetch_size;           // Size of data in pre-fetch buffer
        std::set<std::string> recipients; // Required senders that will connect
        uint32_t connections;             // Number of senders remaining to be connected
        bool reqConnFlag;                 // Flag indicating if this is a required connection
        DataSender::Id senderId;          // Id of sender connected to this port
        bool epollRegistered; // true if port registered with the epoll instance, otherwise false
    } internal_port_data_t;

    bool _wait_called;
    int _epoll_fd;
    int _shutdown_fd;
    int _shutdown_other_fd;
    volatile bool _shutdown_requested;
    typedef std::list<internal_port_data_t*> PortList;
    PortList _port_data;

    /// Create a socket to listen for new connections
    /// @param tcp_port port to listen on
    /// @param p PE input port
    /// @param addr IPv4 address to bind to
    /// @return A TCPReceiver port
    internal_port_data_t* createAcceptSocket(uint16_t tcp_port,
                                             const port_data_t* p,
                                             in_addr_t addr = INADDR_ANY);

    /// Accept expected connections
    /// @param recipients The required number of senders that will connect
    void acceptConnections(uint32_t recipients);

    /// Accept a new connection
    /// @param accept_port The listener port that received the new connection
    /// @return A TCPReceiver port
    internal_port_data_t* acceptNewConnection(internal_port_data_t* accept_port);

    /// Do the connection handshake.
    /// @param fd Socket fd to do connection handshake on
    /// @param ns_label Label of the input port
    /// @param required (Out) Flag indicating if this is a required connection
    /// @param senderId (Out) The sender Id read from the handshake buffer
    void doConnectionHandshake(InetSocket* socket,
                               const std::string& ns_label,
                               bool& required,
                               DataSender::Id& senderId);

    /// Read a message from a port
    /// @param pd TCPReceiver port with data to be read
    /// @param data Pointer to buffer to read message into
    /// @param min_size Minimum amount of data to be read
    /// @param max_size Maximum amount of data to be read
    /// @return amount of data read from port
    uint32_t readOrDie(internal_port_data_t* pd, void* data, size_t min_size, size_t max_size);

    /// Pre-fetch or read a message from a port
    /// @param pd TCPReceiver port with data to be read
    /// @param data Pointer to buffer to read message into
    /// @param small_buffer
    /// @return amount of data pre-fetched or read from port
    uint32_t prefetchOrRead(internal_port_data_t* pd,
                            unsigned char** data,
                            unsigned char* small_buffer);

  public:
    /// Constructor
    /// @param ports vector of input port details, including callbacks and labels
    TCPReceiver(const std::vector<port_data_t>& ports);

    /// Destructor
    /// Close all the ports and unregister from the name service
    virtual ~TCPReceiver();

    /// Wait for all the required senders to connect
    virtual void wait();

    /// Run the receiver and process messages
    virtual void run();

    /// Ask the Receiver to shutdown and stop processing messages
    virtual void shutdown();

    /// Get the port that the Receiver is listening on (used for testing)
    /// @return port that the Receiver is listening on
    virtual uint16_t port() const;

    // Get the connection labels of all connections.
    // This function appends all the connection labels to the specified vector.
    // @param[out] labels vector which gets populated with connection labels
    virtual void getAllConnectionLabels(std::vector<std::string>& labels) const;

    friend std::ostream& operator<<(std::ostream& strm, const TCPReceiver::internal_port_data_t& p);

  private:
    /// Register PE Input port labels with the current name service, and add
    /// successfully registered port records to the _port_data list.
    /// @param ns The name service
    /// @param hostName The host name
    /// @param ports List of ports holding labels to register
    void registerPortLabels(NAM::NAM_NameService* ns,
                            const char* hostName,
                            std::list<internal_port_data_t*>& ports);

    /// Unregister PE Input port labels from the name service.
    void unregisterPortLabels();

    /// Cleanup resources at shutdown time.  This function is getting called
    /// from the main thread after a shutdown state transition.
    void doShutdown();

    /**
     * Unregister data port from the epoll instance and close it.
     * No action if the port is accept port.
     * @param pd data port
     */
    void closeDataPort(internal_port_data_t* pd);

    /**
     * Delete the memory allocated for this port.
     * On return, pd points to free memory.
     * @param pd
     */
    void deletePort(internal_port_data_t* pd);

    /**
     * Search the specified @c ports list for ports with the same SenderId as
     * the port pointed by @c ppd.  If one is found, then close the port
     * which is connected to the sender having a lower PE launch count.
     *
     * Do the search only for data ports which do not have a testing sender Id.
     *
     * @param ppd points to a port descriptor. On return it is set to NULL
     *    if the port was closed and deleted.
     * @param ports list of ports to search for ports with the same SenderId
     *    as @c pd.
     * @see DataSender::Id.isFortesting
     */
    void rejectSameSender(internal_port_data_t** ppd, PortList& ports);

    /**
     * Reject unexpected sender.
     */
    void rejectUnexpectedSender(internal_port_data_t** ppd, internal_port_data_t* pd);

    bool isAcceptPort(internal_port_data_t* pd) const { return pd->accept; }
};

DECL_EXCEPTION(UTILS_NAMESPACE, TCPReceiver, DataReceiver);
DECL_EXCEPTION(UTILS_NAMESPACE, BindFailed, TCPReceiver);
DECL_EXCEPTION(UTILS_NAMESPACE, AcceptError, TCPReceiver);

UTILS_NAMESPACE_END

#endif // !TRANS_TCPRECEIVER_H_
