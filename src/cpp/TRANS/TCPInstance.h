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

#ifndef TRANS_TCPINSTANCE_H_
#define TRANS_TCPINSTANCE_H_

#include <TRANS/DataSender.h>
#include <TRANS/TCPMonitorThread.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/LinkedList.h>
#include <UTILS/Mutex.h>
#include <UTILS/Security.h>

#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>

#include <limits>
#include <vector>

namespace UTILS_NAMESPACE {
class TCPSender;

/**
 * @internal
 *
 * Manages a list of DataSender instances.
 *
 * This implementation provides a common thread which sends heartbeat messages
 * on all connections of all the registered senders.
 */
class TCPInstance : private boost::noncopyable
{
  public:
    typedef boost::shared_ptr<TCPInstance> shared_ptr;

    /**
     * The instance Id
     */
    struct Id
    {
        uint64_t jobId;         ///< Job id of the sender end
        uint64_t peId;          ///< PE id of the sender end
        uint32_t peLaunchCount; ///< PE launch count

        /** The default constructor. */
        Id()
          : jobId(0)
          , peId(std::numeric_limits<uint64_t>::max())
          , peLaunchCount(0)
        {}

        /** Constructs an Id from the given objects. */
        Id(uint64_t a, uint64_t b, uint32_t c)
          : jobId(a)
          , peId(b)
          , peLaunchCount(c)
        {}
    };

    /**
     * Sender types.
     */
    enum SenderType
    {
        Dynamic,
        Static
    };

    /**
     *  A convenience wrapper for creating a TCPInstance::Id.
     *  @param job_id the Job id of the sender
     *  @param pe_id the PE id of the sender
     *  @param count the PE launch count
     *  @return A newly-constructed TCPInstance::Id.
     */
    static inline Id makeId(uint64_t job_id, uint64_t pe_id, uint32_t count)
    {
        return Id(job_id, pe_id, count);
    }

    /**
     * Factory method returning shared pointer to singleton.
     * The instance is created if it does not exist.
     */
    static shared_ptr instance();

    /**
     * Set the instance id.
     * @param the instance id
     */
    void setId(const TCPInstance::Id& theId) { id_ = theId; }

    /**
     * Instance id accessor.
     */
    const TCPInstance::Id& id() const { return id_; }

    /**
     * TLS/SSL Enablement
     */
    bool isSecure() const { return secure_; };
    bool hasSecurityDirectory() const { return securityDirectorySet_; };

    /**
     * Set & Get the SSL/TLS preference
     */
    void setSecureMethod(const std::string& method)
    {
        securityType_ = Security::parseType(method);
        secure_ = securityType_ != Security::None;
    }

    Security::Type getSecureMethod() { return securityType_; }

    void setSecurityDirectory(const std::string& dir)
    {
        securityDirectory_ = dir;
        securityDirectorySet_ = true;
    }

    std::string getSecurityDirectory() { return securityDirectory_; }

    /**
     * Returns a new DataSender instance.
     * @param portId
     * @param pds  vector of descriptions of remote ports the sender will
     * connect to.
     * @param type the sender's type
     */
    DataSender* newSender(uint64_t portId,
                          const std::vector<DataSender::PortDescription>& pds,
                          const SenderType type);

    /**
     * Returns a new DataSender instance.
     *
     * This implementation is used for testing.
     * @param pds  vector of descriptions of remote ports the sender will
     * connect to.
     * @param type the sender's type
     */
    DataSender* newSender(const std::vector<DataSender::PortDescription>& pds,
                          const SenderType type);

    /**
     * Returns a new DataSender instance.
     *
     * This implementation is used for testing.
     * @param pd  description of the remote port the sender will connect to.
     * @param type the sender's type
     */
    DataSender* newSender(const DataSender::PortDescription& pd, const SenderType type);

    /**
     * Returns a new DataSender instance which connects to the specified address and port.
     *
     * This implementation is used for testing.
     * @param address
     * @param port
     * @param type the sender's type
     */
    DataSender* newSender(const std::string& address, uint16_t port, const SenderType type);

    /**
     * Return a new InetSocket dependening on the security settings.
     *
     * @param server the socket is a server socket
     * @return a new socket
     */
    InetSocket* newSocket(const bool server);

    /**
     * Registers the specified sender to this instance.
     *
     * The instance will send heartbeat to the connections of all the
     * registered senders.
     */
    void add(TCPSender* const sender);

    /**
     * Removes the specified sender from this instance.
     */
    void remove(TCPSender* const sender);

    /**
     * Stops and joins with heartbeat thread.
     *
     * This must be called after all of the TCPSenders have been shutdown,
     * but before they have been destroyed.
     */
    void stop();

  private:
    TCPInstance::Id id_;

    // TCP senders, iterator types, and guarding mutex
    typedef LinkedList<TCPSender*> TCPSenderList;
    mutable Mutex sendersMutex_;
    TCPSenderList senders_;
    bool secure_;
    Security::Type securityType_;
    bool securityDirectorySet_;
    std::string securityDirectory_;

    /*
     * Controlling the heartbeat mechanism.  Turning heartbeat off
     * disables TCP senders' ability to detect when a connection is broken
     * until a write() returns failure.  The TCP sender will not notify
     * listeners if the connection breaks but no data are written.
     */
    const bool heartBeatOn_;

    // Heartbeat thread
    TCPMonitorThread monitor_;
    friend class TCPMonitorThread; // invokes doHeartbeat()

    // Task invoked by a TCPMonitorThread to send heartbeats to all senders of this instance
    // TODO add task to the monitor thread initialization and remove the need for friend class
    // TCPMonitorThread
    void doHeartbeat(volatile bool* interruptFlag,
                     streams_time_t beforeTime,
                     streams_time_t curTime);

    TCPInstance();
    void init();
    void start();

    // Prevent client code from deleting a TCPInstance pointer:
    //  - make destructor private
    //  - pass the deleter structure to the creation of the shared pointer
    ~TCPInstance();
    struct deleter
    {
        void operator()(TCPInstance* p) { delete p; }
    };

    static Mutex& getMutex();
    static boost::shared_ptr<TCPInstance>& getInstancePtr();
};

std::ostream& operator<<(std::ostream& s, const TCPInstance::Id& id);
}
#endif /* TRANS_TCPINSTANCE_H_ */
