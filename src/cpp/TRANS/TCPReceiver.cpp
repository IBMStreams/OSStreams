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

#include <NAM/NAM_NameService.h>
#include <TRANS/ConnectionHandshake.h>
#include <TRANS/TCPCommon.h>
#include <TRANS/TCPInstance.h>
#include <TRANS/TCPReceiver.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Formatter.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/SBuffer.h>
#include <UTILS/auto_array.h>

#include <sys/epoll.h>

UTILS_NAMESPACE_USE;
NAM_NAMESPACE_USE;
using namespace std;
DEBUG_NAMESPACE_USE

#define CTOR_INIT                                                                                  \
    _wait_called(false), _epoll_fd(-1), _shutdown_fd(-1), _shutdown_other_fd(-1),                  \
      _shutdown_requested(false)

#define PREFETCH_SIZE (32 * 1024)

uint16_t TCPReceiver::PORT_BASE = 10000;

ostream& Distillery::operator<<(ostream& strm, const TCPReceiver::internal_port_data_t& p)
{
    strm << "{socket: " << **p.sock << " ns:" << p.ns_label << "}";

    return strm;
}

TCPReceiver::TCPReceiver(const vector<port_data_t>& ports)
  : CTOR_INIT
{
    string hostinfo;
    string networkaddress;
    in_addr_t addr = INADDR_ANY;

    try {
        addr = HostInfo::getNetworkAddr(hostinfo, networkaddress, false);
    } catch (HostInfoException& e) {
        const char* msg = (ports.size() > 0) ? ports[0].ns_label : "";
        THROW_CHAR(TCPReceiver, msg, TRANSHostInfoNotAvailable);
    }

    NAM_NameService* ns = NAM_NameService::getInstance();

    // build temporary port list
    list<internal_port_data_t*> new_sockets;
    for (vector<port_data_t>::const_iterator it = ports.begin(); it != ports.end(); it++) {
        std::vector<std::string> parts;
        tokenize(it->ns_label, parts, ".@", false);
        uint16_t tcp_port = PORT_BASE + atoi(parts[1].c_str());
        SPCDBG(L_DEBUG, "Assigning port " << tcp_port << " to label " << it->ns_label,
               CORE_TRANS_TCP);
        internal_port_data_t* pd = createAcceptSocket(tcp_port, &(*it), addr);
        new_sockets.push_back(pd);
    }

    // register port labels and add socket entries to _port_data list
    registerPortLabels(ns, hostinfo.c_str(), new_sockets);
}

TCPReceiver::~TCPReceiver()
{
    SPCDBG(L_TRACE, "enter", CORE_TRANS_TCP);

    // TODO move shutdown in separate call before destructor
    doShutdown();

    PortList::iterator it = _port_data.begin();
    while (it != _port_data.end()) {
        internal_port_data_t* pd = *it;
        if (!pd->accept) {
            pd->sock->close();
        }
        deletePort(pd);
        it++;
    }

    _port_data.clear();

    close(_shutdown_fd);
    close(_shutdown_other_fd);
    close(_epoll_fd);
    SPCDBG(L_TRACE, "exit", CORE_TRANS_TCP);
}

TCPReceiver::internal_port_data_t* TCPReceiver::createAcceptSocket(uint16_t tcp_port,
                                                                   const port_data_t* p,
                                                                   in_addr_t addr)
{
    if (_epoll_fd == -1) {
        _epoll_fd = epoll_create(2);
        if (_epoll_fd == -1) {
            ErrnoString s(errno);
            THROW_CHAR(TCPReceiver, "epoll_create() failed", TRANSSystemCallFailed, "epoll_create",
                       s.c_str());
        }

        SPCDBG(L_INFO, "epoll fd=" << _epoll_fd, CORE_TRANS_TCP);

        int shutdown_fd[2];

        if (pipe(shutdown_fd)) {
            ErrnoString s(errno);
            THROW_CHAR(TCPReceiver, "pipe() failed", TRANSSystemCallFailed, "pipe", s.c_str());
        }

        SPCDBG(L_INFO, "shutdown fd=" << shutdown_fd[1] << " (+" << shutdown_fd[0] << ")",
               CORE_TRANS_TCP);

        _shutdown_other_fd = shutdown_fd[0];
        _shutdown_fd = shutdown_fd[1];

        struct epoll_event evt;
        memset(&evt, 0, sizeof(evt));
        evt.events = EPOLLIN;
        evt.data.fd = _shutdown_fd;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, shutdown_fd[0], &evt)) {
            ErrnoString s(errno);
            THROW_CHAR(TCPReceiver, "epoll_ctl_add failed", TRANSSystemCallFailed, "epoll_ctl_add",
                       s.c_str());
        }
    }

    // TODO make internal_port_data_t accept port constructor
    internal_port_data_t* pd = new internal_port_data_t;
    pd->user_data = p->user_data;
    pd->callback = p->callback;
    pd->accept = true;
    pd->sock = new InetSocket;
    pd->recipients = p->recipients;
    pd->connections = p->recipients.size();
    if (p->ns_label) {
        pd->ns_label = p->ns_label;
    }
    if (p->operator_ns_label) {
        pd->operator_ns_label = p->operator_ns_label;
    }
    pd->prefetch_buffer = NULL;
    pd->prefetch_size = 0;
    pd->prefetch_offset = 0;
    DataSender::Id id;
    pd->senderId = id;
    pd->epollRegistered = false;

    try {
        pd->sock->bind(tcp_port, addr);
    } catch (SocketException& ex) {
        SPCDBG(L_ERROR,
               "Caught exception " << ex.getType() << " thrown from: " << ex.getLocation()
                                   << " with explanation: " << ex.getExplanation(),
               CORE_TRANS_TCP);
        THROW_NESTED(BindFailed, "Unable to bind to " << tcp_port, ex, TRANSSystemCallFailed,
                     "bind", "");
    }

    pd->sock->listen();

    if (!pd->epollRegistered) {
        struct epoll_event evt;
        memset(&evt, 0, sizeof(evt));
        evt.events = EPOLLIN;
        evt.data.ptr = pd;
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, **pd->sock, &evt)) {
            ErrnoString s(errno);
            THROW_CHAR(TCPReceiver, "epoll_ctl(... EPOLL_CTL_ADD) failed", TRANSSystemCallFailed,
                       "epoll_ctl(... EPOLL_CTL_ADD)", s.c_str());
        }
        pd->epollRegistered = true;
    } else {
        // Socket already registered with epoll
        std::stringstream ss;
        ss << "Socket fd=" << **pd->sock << " from sender " << pd->senderId
           << " already registered with the epoll instance";
        THROW_STRING(LogicError, pd->ns_label.str(), TRANSLogicError, ss.str().c_str());
    }
    return pd;
}

TCPReceiver::internal_port_data_t* TCPReceiver::acceptNewConnection(
  internal_port_data_t* accept_port)
{
    if (!accept_port->accept) {
        THROW_STRING(LogicError, accept_port->ns_label.str(), TRANSLogicError,
                     "accept_port->accept != true");
    }

    InetSocket* socket = NULL;
    try {
        int fd = accept_port->sock->accept();
        if (fd == -1) {
            ErrnoString s(errno);
            THROW_CHAR(AcceptError, "accept failed", TRANSSystemCallFailed, "accept", s.c_str());
        }

        SPCDBG(L_INFO, "New connection accepted on " << QT(accept_port->ns_label) << ": " << fd,
               CORE_TRANS_TCP);

        socket = TCPInstance::instance()->newSocket(true);
        socket->attach(fd);
    }
    /**
     * Both accept() and attach() can throw a SocketException, so make sure to
     * clean up the created socket if necessary and rethrow.
     */
    catch (SocketException& ex) {
        if (socket != NULL) {
            delete socket;
        }
        throw;
    }

    // Check the connection handshake to make sure it is not erroneous
    bool required = true;
    DataSender::Id senderId;
    doConnectionHandshake(socket, accept_port->ns_label.str(), required, senderId);

    // TODO make internal_port_data_t data port constructor
    internal_port_data_t* new_pd = new internal_port_data_t;
    new_pd->user_data = accept_port->user_data;
    new_pd->callback = accept_port->callback;
    new_pd->accept = false;
    new_pd->sock = socket;
    new_pd->ns_label = accept_port->ns_label;
    new_pd->operator_ns_label = accept_port->operator_ns_label;
    new_pd->connections = 0;
    new_pd->reqConnFlag = required;
#ifdef PREFETCH_SIZE
    new_pd->prefetch_buffer = new unsigned char[PREFETCH_SIZE];
#else
    new_pd->prefetch_buffer = NULL;
#endif
    new_pd->prefetch_offset = 0;
    new_pd->prefetch_size = 0;
    new_pd->epollRegistered = false;
    new_pd->senderId = senderId;
    return new_pd;
}

void TCPReceiver::doConnectionHandshake(InetSocket* socket,
                                        const string& ns_label,
                                        bool& required,
                                        DataSender::Id& senderId)
{
    // Skip connection handshake if there is no label
    if (ns_label.empty()) {
        return;
    }

    SPCDBG(L_DEBUG, "Doing Connection Handshake for fd=" << **socket << ", ns_label=" << ns_label,
           CORE_TRANS_TCP);

    try {
        // Wait for connection handshake
        TCPCommon::waitOnSocket(
          socket, 0, ConnectionHandshake::getTimeoutUsec(TCPInstance::instance()->isSecure()));

        // Read the connection Handshake
        size_t size;
        auto_array<unsigned char> data(
          (unsigned char*)TCPCommon::readSocketWithHeader(socket, &size));

        // Check the handshake by creating the object
        // Get and verify the Connection Handshake
        SBuffer sbuf(data.get(), size);
        // The constructor throws an exception if erroneous connection.
        ConnectionHandshake handshake(sbuf, ns_label);
        required = handshake.isRequiredConnection();
        senderId = handshake.getSenderId();
    } catch (UnexpectedInstanceException const& ex) {
        SPCDBG(L_ERROR,
               "Rejecting Connection for input port "
                 << QT(ns_label) << ", connection handshake exception : " << ex.getExplanation(),
               CORE_TRANS_TCP);
        socket->close();
        delete socket;
        THROW_NESTED(HandshakeError, "Unexpected instance exception", ex);
    } catch (UnexpectedLabelException const& ex) {
        SPCDBG(L_ERROR,
               "Rejecting Connection for input port "
                 << QT(ns_label) << ", connection handshake exception : " << ex.getExplanation(),
               CORE_TRANS_TCP);
        socket->close();
        delete socket;
        THROW_NESTED(HandshakeError, "Unexpected label exception", ex);
    } catch (DistilleryException const& ex) {
        SPCDBG(L_ERROR,
               "Rejecting Connection for input port "
                 << QT(ns_label) << ", connection handshake exception : " << ex.getExplanation(),
               CORE_TRANS_TCP);
        socket->close();
        delete socket;
        THROW_NESTED(HandshakeError, "Bad Connection Handshake", ex);
    }
}

void TCPReceiver::closeDataPort(internal_port_data_t* pd)
{
    if (pd->epollRegistered) {
        if (epoll_ctl(_epoll_fd, EPOLL_CTL_DEL, **pd->sock, NULL)) {
            ErrnoString s(errno);
            THROW(TCPReceiver, "Unable to remove fd=" << **pd->sock << " from epoll",
                  TRANSSystemCallFailed, "epoll_ctl(...EPOLL_CTL_DEL)", s.c_str());
        }
        pd->epollRegistered = false;
        pd->callback->onDisconnected(pd->ns_label.getPortId(), pd->senderId.toPortLabel(),
                                     !pd->reqConnFlag);
    } else {
        // Trying to unregister a socket which is not registered with epoll
        std::stringstream ss;
        ss << " Trying to remove fd=" << **pd->sock << " from sender " << pd->senderId
           << " from epoll instance when socket not registered";
        // TODO remove        THROW_STRING(LogicError, pd->ns_label.str(), TRANSLogicError,
        // ss.str().c_str());
        SPCDBG(L_DEBUG, "Receiver " << pd->ns_label.str() << ss.str().c_str(), CORE_TRANS_TCP);
    }
    if (pd->sock != NULL) {
        pd->sock->close();
    }
    pd->prefetch_size = 0;
}

void TCPReceiver::deletePort(internal_port_data_t* pd)
{
    if (pd->sock != NULL) {
        delete pd->sock;
    }
#ifdef PREFETCH_SIZE
    delete[] pd->prefetch_buffer;
#endif
    delete pd;
}

void TCPReceiver::rejectSameSender(internal_port_data_t** ppd, PortList& ports)
{
    internal_port_data_t* newPd = *ppd;

    // Check only for data ports which do not have a test senderId
    if (isAcceptPort(newPd) || newPd->senderId.isForTesting()) {
        return;
    }

    SPCDBG(L_DEBUG, "Checking accepted connection from senderId: " << QT(newPd->senderId),
           CORE_TRANS_TCP);

    PortList::iterator it = ports.begin();
    while (it != ports.end()) {
        internal_port_data_t* pd = *it;
        if (isAcceptPort(pd) || pd->senderId.isForTesting()) {
            it++;
            continue;
        }

        SPCDBG(L_TRACE, "Compare with pd senderId: " << QT(pd->senderId), CORE_TRANS_TCP);

        if (pd->senderId.jobId == newPd->senderId.jobId &&
            pd->senderId.peId == newPd->senderId.peId &&
            pd->senderId.outPortId == newPd->senderId.outPortId) {

            if (pd->senderId.peRestartCount <= newPd->senderId.peRestartCount) {
                SPCDBG(L_INFO,
                       "Accepting new connection from sender id "
                         << QT(newPd->senderId) << " with PE restart count "
                         << newPd->senderId.peRestartCount
                         << ", closing existing connection from PE with restart count "
                         << pd->senderId.peRestartCount,
                       CORE_TRANS_TCP);

                // newPd has higher or same PE launch count, close old pd and
                // remove from list
                closeDataPort(pd);
                deletePort(pd);
                PortList::iterator tmp(it);
                it++;
                ports.erase(tmp);

                // TODO for equal launch counts, investigate draining the old
                // connection before reading the new one
            } else if (pd->senderId.peRestartCount > newPd->senderId.peRestartCount) {
                SPCDBG(
                  L_INFO,
                  "Rejecting connection from sender id "
                    << QT(newPd->senderId) << " with PE restart count "
                    << newPd->senderId.peRestartCount
                    << " because a connection exists for same sender id from PE with restart count "
                    << pd->senderId.peRestartCount,
                  CORE_TRANS_TCP);

                // newPd has lower PE launch count, close and delete
                closeDataPort(newPd);
                deletePort(newPd);
                *ppd = NULL; // return NULL pointer
                break;
            }
        } else {
            it++;
        }
    }
}

void TCPReceiver::rejectUnexpectedSender(internal_port_data_t** ppd, internal_port_data_t* pd)
{
    std::string newLabel = (*ppd)->senderId.toPortLabel().str();
    if ((*ppd)->reqConnFlag && pd->recipients.count(newLabel) != 1) {
        SPCDBG(L_WARN, "Rejecting unexpected connection " << newLabel << " on " << QT(pd->ns_label),
               CORE_TRANS_TCP);
        closeDataPort(*ppd);
        deletePort(*ppd);
        *ppd = NULL;
    }
}

void TCPReceiver::wait()
{
    assert(_wait_called == false);
    _wait_called = true;

    PortList new_clients;
    uint32_t total_recipients = 0;

    // Calculate count of expected connections on all input ports
    PortList::iterator it1 = _port_data.begin();
    while (it1 != _port_data.end()) {
        if (!(*it1)->accept) {
            THROW_STRING(LogicError, (*it1)->ns_label.str(), TRANSLogicError,
                         "(*it1)->accept != true");
        }

        total_recipients += (*it1)->recipients.size();
        it1++;
    }

    // Accept all expected connections
    while (total_recipients) {
        struct epoll_event evt;
        memset(&evt, 0, sizeof(evt));

        SPCDBG(L_INFO, "Waiting for " << total_recipients << " recipient(s)", CORE_TRANS_TCP);

        int ret = epoll_wait(_epoll_fd, &evt, 1, -1);

        if (ret == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                ErrnoString s(errno);
                THROW(AcceptError, "epoll_wait(" << _epoll_fd << "...)", TRANSSystemCallFailed,
                      "epoll_wait", s.c_str());
            }
        }

        if (ret == 1) {
            if (evt.events == EPOLLIN) {
                if (evt.data.fd == _shutdown_fd) {
                    SPCDBG(L_INFO, "Shutdown signal received", CORE_TRANS_TCP);
                    doShutdown();
                    THROW(ShutdownRequested, "");
                }

                internal_port_data_t* pd = (internal_port_data_t*)evt.data.ptr;

                SPCDBG(L_DEBUG, "Accepting new connection on " << QT(pd->ns_label), CORE_TRANS_TCP);

                if (!pd->accept) {
                    THROW_STRING(LogicError, pd->ns_label.str(), TRANSLogicError,
                                 "pd->accept != true");
                }

                // Add newly accepted connection to new_clients
                try {
                    internal_port_data_t* new_pd = acceptNewConnection(pd);
                    rejectSameSender(&new_pd, new_clients);
                    rejectUnexpectedSender(&new_pd, pd);

                    if (new_pd == NULL) {
                        continue;
                    }

                    if ((pd->connections > 0) && new_pd->reqConnFlag) {
                        pd->connections--;
                        total_recipients--;

                        if (pd->connections > 0) {
                            SPCDBG(L_INFO,
                                   "Still waiting for " << pd->connections << " recipients on "
                                                        << QT(pd->ns_label),
                                   CORE_TRANS_TCP);
                        }
                    } else {
                        SPCDBG(L_INFO,
                               "Not waiting for additional connection on " << QT(pd->ns_label),
                               CORE_TRANS_TCP);
                    }

                    new_clients.push_back(new_pd);
                } catch (const HandshakeErrorException& ex) {
                    SPCDBG(L_INFO,
                           "Rejecting Connection for input port "
                             << QT(pd->ns_label)
                             << ", connection handshake exception : " << ex.getExplanation(),
                           CORE_TRANS_TCP);
                } catch (const SocketException& ex) {
                    SPCDBG(L_INFO,
                           "Skipping Connection for input port "
                             << QT(pd->ns_label) << ", socket exception : " << ex.getExplanation(),
                           CORE_TRANS_TCP);
                }
            } else {
                EPollEventString str(evt.events);
                THROW(AcceptError, "Invalid event on epoll_wait(" << _epoll_fd << "...)",
                      TRANSSystemCallFailed, "epoll_wait", str.c_str());
            }
        } else {
            THROW(AcceptError, "Expecting 1 from epoll_wait(" << _epoll_fd << "...)",
                  TRANSSystemCallFailed, "epoll_wait", ret);
        }
    }

    // For each new connection, register its file descriptor and event EPOLLIN with the current
    // epoll instance _epoll_fd Add new connection to _port_data
    list<internal_port_data_t*>::iterator it2 = new_clients.begin();
    while (it2 != new_clients.end()) {
        internal_port_data_t* pd = *it2;
        _port_data.push_back(pd);

        if (!pd->epollRegistered) {
            struct epoll_event evt;
            memset(&evt, 0, sizeof(evt));
            evt.events = EPOLLIN;
            evt.data.ptr = pd;
            if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, **pd->sock, &evt)) {
                ErrnoString s(errno);
                THROW(TCPReceiver, "epoll_ctl(" << _epoll_fd << ", EPOLL_CTL_ADD...)",
                      TRANSSystemCallFailed, "epoll_ctl", s.c_str());
            }
            pd->epollRegistered = true;
            pd->callback->onConnected(pd->ns_label.getPortId(), pd->senderId.toPortLabel(),
                                      !pd->reqConnFlag);
        } else {
            // Socket already registered with epoll
            std::stringstream ss;
            ss << "Socket fd=" << **pd->sock << " from " << pd->senderId
               << " already registered with the epoll instance";
            THROW_STRING(LogicError, pd->ns_label.str(), TRANSLogicError, ss.str().c_str());
        }
        it2++;
    }
}

uint16_t TCPReceiver::port() const
{
    if (_port_data.size() != 1) {
        THROW_CHAR(InvalidArgument, "_port_data.size() != 1", TRANSInvalidArgument,
                   "_port_data.size()", _port_data.size());
    }

    PortList::const_iterator it = _port_data.begin();
    return (*it)->sock->getPort();
}

void TCPReceiver::getAllConnectionLabels(std::vector<std::string>& labels) const
{
    PortList::const_iterator it = _port_data.begin();
    for (; it != _port_data.end(); it++) {
        std::set<std::string>::const_iterator jt = (*it)->recipients.begin();
        for (; jt != (*it)->recipients.end(); jt++) {
            unsigned long long portId = (*it)->ns_label.getPortId();
            std::string prefix = boost::lexical_cast<std::string>(portId) + ":";
            labels.push_back(prefix + *jt);
        }
    }
}

void TCPReceiver::shutdown()
{
    _shutdown_requested = true;
    char c = 'x';
    write(_shutdown_fd, &c, 1);

    SPCDBG(L_INFO, "Posting shutdown request", CORE_TRANS_TCP);
}

uint32_t TCPReceiver::readOrDie(internal_port_data_t* pd,
                                void* data,
                                size_t min_size,
                                size_t max_size)
{
    size_t readBytes = 0;
    SPCDBG(L_TRACE,
           "readOrDie(" << *pd << ", " << data << ", " << min_size << ", " << max_size << ")",
           CORE_TRANS_TCP);

    if (max_size == 0 || min_size == 0) {
        return 0; // read nothing
    }
    do {
        ssize_t rc = pd->sock->read(data, max_size);

        if (rc < 0) {
            ErrnoString s(errno);
            SPCDBG(L_ERROR, "Read error: " << strerror(errno), CORE_TRANS_TCP);
            THROW(ReadError, "read(" << *pd << "...)", TRANSSystemCallFailed, "read", s.c_str());
        }

        if (rc == 0) {
            SPCDBG(L_INFO, "Connection lost: " << *pd, CORE_TRANS_TCP);
            closeDataPort(pd);
            return 0;
        }

        readBytes += rc;
        data = (char*)data + rc;

        if ((uint32_t)rc > min_size) {
            min_size = 0;
        } else {
            min_size -= rc;
        }

        max_size -= rc;

        SPCDBG(L_TRACE, "readOrDie iteration: min=" << min_size << " max=" << max_size,
               CORE_TRANS_TCP);
    } while (min_size);

    return readBytes;
}

uint32_t TCPReceiver::prefetchOrRead(internal_port_data_t* pd,
                                     unsigned char** data_ptr,
                                     unsigned char* small_buffer)
{
    // This function is called only if there is data in the prefetch buffer

    // prefetch_offset = current prefetch buffer read position
    // prefetch_size = count of bytes available in the prefetch buffer

    SPCDBG(L_TRACE,
           "prefetchOrRead(" << *pd << ", " << (void*)*data_ptr << ", " << (void*)small_buffer
                             << ")",
           CORE_TRANS_TCP);

    if (pd->prefetch_size < sizeof(socket_header_t)) {
        // The previous read has left the system in an inconsistent state
        THROW(ReadError,
              "pd->prefetch_size=" << pd->prefetch_size
                                   << " must be greater or equal to sizeof(socket_header_t)="
                                   << sizeof(socket_header_t),
              TRANSDataCorrupted);
    }

    // validate the next message header and get message size
    socket_header_t* hdr =
      reinterpret_cast<socket_header_t*>(pd->prefetch_buffer + pd->prefetch_offset);
    if (!hdr->isValid()) {
        string s = hdr->toString();
        THROW_CHAR(ReadError, "Bad MAGIC", TRANSInvalidMessageHeader, s.c_str());
    }
    uint32_t msgSize = hdr->messageSize();
    uint32_t hdrSize = sizeof(*hdr);

    // TODO limit the amount allocated - what is the MAX_TUPLE_SIZE?
    unsigned char* data = msgSize > TCP_SMALL_BUFFER ? new unsigned char[msgSize] : small_buffer;

    pd->prefetch_offset += hdrSize;
    pd->prefetch_size -= hdrSize;

    // if (message size > 0)
    //   assert(prefetch_offset points at the message payload)
    // else
    //   assert(prefetch_offset points at the next message buffer)

    if (pd->prefetch_size >= msgSize) {
        SPCDBG(L_TRACE,
               "prefetch buffer contains what we need or more (" << msgSize << " / "
                                                                 << pd->prefetch_size << ")",
               CORE_TRANS_TCP);

        // copy message payload into data
        memcpy(data, pd->prefetch_buffer + pd->prefetch_offset, msgSize);

        pd->prefetch_offset += msgSize;
        pd->prefetch_size -= msgSize;

        // assert(prefetch_offset points at the beginning of the next message header)

        if (pd->prefetch_size > 0 && pd->prefetch_size < hdrSize) {
            // make sure that there is at least a full message header in the prefetech
            // buffer and move it to the front of the buffer
            memcpy(pd->prefetch_buffer, pd->prefetch_buffer + pd->prefetch_offset,
                   pd->prefetch_size);
            pd->prefetch_offset = 0;
            if (readOrDie(pd, pd->prefetch_buffer + pd->prefetch_size, hdrSize - pd->prefetch_size,
                          hdrSize - pd->prefetch_size) == 0) {
                // We got EOF. the socket has already been removed from the
                // epoll list. We'll treat the current buffer as the last one
                pd->prefetch_size = 0;
            } else {
                pd->prefetch_size = hdrSize;
            }
        }
    } else {
        SPCDBG(L_TRACE,
               "prefetch buffer too small. Need at least " << msgSize - pd->prefetch_size
                                                           << " bytes",
               CORE_TRANS_TCP);
        // make sure that there is at least a full message payload in the 'data' buffer;
        // copy the beginning from the prefetch_buffer and the rest from readOrDie
        memcpy(data, pd->prefetch_buffer + pd->prefetch_offset, pd->prefetch_size);
        if (readOrDie(pd, data + pd->prefetch_size, msgSize - pd->prefetch_size,
                      msgSize - pd->prefetch_size) == 0) {
            // We got EOF. the socket has already been removed from the
            // epoll list. Unable to get a complete buffer. skipping socket
            pd->prefetch_size = 0;
            return 0;
        }

        pd->prefetch_offset = 0;
        pd->prefetch_size = 0;
    }

    // assert(prefetch_size == 0 or prefetch_size >= sizeof message header)
    // assert(prefetch_offset points at the beginning of the next message header)

    *data_ptr = data;
    return msgSize;
}

void TCPReceiver::run()
{
    unsigned char small_buffer[TCP_SMALL_BUFFER];

    while (!_shutdown_requested) {

        /*
         * Use prefetched data if we have any
         */

#if PREFETCH_SIZE
        PortList::iterator it = _port_data.begin();
        while (it != _port_data.end() && !_shutdown_requested) {
            internal_port_data_t* pd = *it;
            while (pd->prefetch_size > 0 && !_shutdown_requested) {
                unsigned char* data_ptr = NULL;
                uint32_t size = prefetchOrRead(pd, &data_ptr, small_buffer);
                if (size == 0) {
                    // We either: (a) received a zero-length message, or
                    // (b) got EOF in read(), the socket has already been removed from
                    // the epoll list, skipping it now
                    continue;
                }

                pd->callback->onMessage(data_ptr, size, pd->user_data);

                if (data_ptr != small_buffer) {
                    delete[] data_ptr;
                }
            }

            it++;
        }
#endif

        if (_shutdown_requested) {
            break;
        }

        /*
         * No prefetched data available, waiting for a socket to be active
         */

        struct epoll_event evt;
        memset(&evt, 0, sizeof(evt));

        SPCDBG(L_TRACE, "Entering epoll_wait()", CORE_TRANS_TCP);

        int rc = epoll_wait(_epoll_fd, &evt, 1, -1);

        SPCDBG(L_TRACE, "epoll_wait() returned with code " << rc, CORE_TRANS_TCP);

        if (rc == -1) {
            if (errno == EINTR) {
                continue;
            } else {
                ErrnoString s(errno);
                THROW_CHAR(TCPReceiver, "epoll_wait failed", TRANSSystemCallFailed, "epoll_wait",
                           s.c_str());
            }
        }

        if (rc != 1) {
            THROW_CHAR(AcceptError, "Invalid return code from epoll_wait", TRANSSystemCallFailed,
                       "epoll_wait", rc);
        }

        if (evt.events == EPOLLIN) {
            if (evt.data.fd == _shutdown_fd) {
                SPCDBG(L_INFO, "Shutdown signal received", CORE_TRANS_TCP);
                doShutdown();
                THROW(ShutdownRequested, "");
            }

            internal_port_data_t* pd = (internal_port_data_t*)evt.data.ptr;

            /*
             * Process accept requests.
             */

            if (pd->accept) {
                SPCDBG(L_DEBUG,
                       "Activity on an accept socket"
                         << " (port " << pd->sock->getPort() << ")",
                       CORE_TRANS_TCP);

                try {
                    internal_port_data_t* new_pd = acceptNewConnection(pd);
                    rejectSameSender(&new_pd, _port_data);
                    rejectUnexpectedSender(&new_pd, pd);

                    if (new_pd != NULL) {
                        SPCDBG(L_DEBUG,
                               "Accept port " << pd->ns_label.str()
                                              << ": registering connection from sender id "
                                              << QT(new_pd->senderId) << " with PE restart count "
                                              << new_pd->senderId.peRestartCount,
                               CORE_TRANS_TCP);

                        _port_data.push_back(new_pd);

                        if (!new_pd->epollRegistered) {

                            memset(&evt, 0, sizeof(evt));
                            evt.events = EPOLLIN;
                            evt.data.ptr = new_pd;

                            if (epoll_ctl(_epoll_fd, EPOLL_CTL_ADD, **new_pd->sock, &evt)) {
                                ErrnoString s(errno);
                                THROW_CHAR(TCPReceiver, "epoll_ctl_add failed",
                                           TRANSSystemCallFailed, "epoll_ctl(...EPOLL_CTL_ADD)",
                                           s.c_str());
                            }
                            new_pd->epollRegistered = true;
                            pd->callback->onConnected(new_pd->ns_label.getPortId(),
                                                      new_pd->senderId.toPortLabel(),
                                                      !new_pd->reqConnFlag);
                        } else {
                            // Socket already registered with epoll
                            std::stringstream ss;
                            ss << "Socket fd=" << **new_pd->sock << " from " << new_pd->senderId
                               << " already registered with the epoll instance";
                            THROW_STRING(LogicError, new_pd->ns_label.str(), TRANSLogicError,
                                         ss.str().c_str());
                        }
                    }
                } catch (const HandshakeErrorException& ex) {
                    SPCDBG(L_INFO,
                           "Rejecting Connection for input port "
                             << QT(pd->ns_label)
                             << ", connection handshake exception : " << ex.getExplanation(),
                           CORE_TRANS_TCP);
                } catch (const SocketException& ex) {
                    SPCDBG(L_INFO,
                           "Skipping Connection for input port "
                             << QT(pd->ns_label) << ", socket exception : " << ex.getExplanation(),
                           CORE_TRANS_TCP);
                }
                continue;
            }

            /*
             * Process read requests.
             */

            SPCDBG(L_TRACE,
                   "Reading data from fd=" << **pd->sock << ", " << evt.data.fd << ", ptr=0x" << pd,
                   CORE_TRANS_TCP);

            unsigned char* data_ptr = NULL;
            uint32_t size = 0;

#ifdef PREFETCH_SIZE
            pd->prefetch_size =
              readOrDie(pd, pd->prefetch_buffer, sizeof(socket_header_t), PREFETCH_SIZE);
            pd->prefetch_offset = 0;
            if (pd->prefetch_size == 0) {
                // We got EOF. the socket has already been removed from
                // the epoll list. skipping it now
                continue;
            }

            size = prefetchOrRead(pd, &data_ptr, small_buffer);
            if (size == 0) {
                // We either: (a) received a zero-length message, or
                // (b) got EOF in read(), the socket has already been removed from
                // the epoll list, skipping it now
                continue;
            }
#else
            // no prefetch, read only as much as needed
            socket_header_t hdr;

            if (readOrDie(pd, &hdr, sizeof(hdr), sizeof(hdr)) == 0) {
                // We got EOF. the socket has already been removed from
                // the epoll list. skipping it now
                continue;
            }

            if (!hdr.isValid()) {
                string s = hdr.toString();
                THROW_CHAR(ReadError, "Bad MAGIC", TRANSInvalidMessageHeader, s.c_str());
            }

            uint32_t msgSize = hdr.messageSize();

            unsigned char* data = new unsigned char[msgSize];

            if (readOrDie(pd, data, msgSize, msgSize) == 0) {
                // We either: (a) received a zero-length message, or
                // (b) got EOF in read(), the socket has already been removed from
                // the epoll list, skipping it now
                continue;
            }

            data_ptr = data;
            size = msgSize;
#endif
            pd->callback->onMessage(data_ptr, size, pd->user_data);

            if (data_ptr != small_buffer) {
                delete[] data_ptr;
            }
        } else if (evt.events & EPOLLHUP) {
            // Close data port (sender closed connection)
            internal_port_data_t* pd = (internal_port_data_t*)evt.data.ptr;
            closeDataPort(pd);
            SPCDBG(L_INFO, "Connection lost: " << *pd, CORE_TRANS_TCP);
            continue;
        } else {
            EPollEventString str(evt.events);
            THROW_CHAR(AcceptError, "Invalid event on epoll_wait", TRANSSystemCallFailed,
                       "epoll_wait", str.c_str());
        }
    }

    SPCDBG(L_INFO, "Shutdown signal received", CORE_TRANS_TCP);
    doShutdown();
}

void TCPReceiver::doShutdown()
{
    SPCDBG(L_TRACE, "Enter", CORE_TRANS_TCP);
    unregisterPortLabels();
    SPCDBG(L_TRACE, "Exit", CORE_TRANS_TCP);
}

void TCPReceiver::registerPortLabels(NAM_NameService* ns,
                                     const char* hostName,
                                     list<internal_port_data_t*>& ports)
{
    SPCDBG(L_TRACE, "Enter registerPortLabels", CORE_TRANS_TCP);

    for (list<internal_port_data_t*>::iterator it = ports.begin(); it != ports.end(); it++) {
        internal_port_data_t* pd = *it;
        pd->ns_label.rebind(ns, hostName, pd->sock->getPort(), 1000000 /* 1 second */);
        if (!pd->operator_ns_label.empty()) {
            pd->operator_ns_label.rebind(ns, pd->ns_label.getPEId(), pd->ns_label.getPortId(),
                                         1000000 /* 1 second */);
        }

        // internal port data record fully initialized, append it to the ports list
        _port_data.push_back(pd);
    }
    SPCDBG(L_TRACE, "Exit registerPortLabels", CORE_TRANS_TCP);
}

void TCPReceiver::unregisterPortLabels()
{
    SPCDBG(L_TRACE, "Enter unregisterPortLabels", CORE_TRANS_TCP);
    NAM_NameService* ns = NAM_NameService::getInstance();

    for (PortList::iterator it = _port_data.begin(); it != _port_data.end(); it++) {
        internal_port_data_t* pd = *it;
        pd->ns_label.unbind(ns);
        pd->operator_ns_label.unbind(ns);
    }
    SPCDBG(L_TRACE, "Exit unregisterPortLabels", CORE_TRANS_TCP);
}

IMPL_EXCEPTION(Distillery, TCPReceiver, DataReceiver);
IMPL_EXCEPTION(Distillery, BindFailed, TCPReceiver);
IMPL_EXCEPTION(Distillery, AcceptError, TCPReceiver);
