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

#ifndef TRANS_DATA_RECEIVER_H_
#define TRANS_DATA_RECEIVER_H_

#include <TRANS/PortLabel.h>
#include <TRANS/TransportUtils.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <set>

UTILS_NAMESPACE_BEGIN

/**
 * \internal
 * This class is the interface for a transport receiver which receives data
 * from several Data Senders.
 */
class DataReceiver
{

  public:
    typedef union _user_data_t
    {
        void* ptr;
        uint32_t u32;
        uint64_t u64;
    } user_data_t;

    /**
     * This class defines the callback used to process messages received by a Data receiver.
     */
    class Callback
    {
      public:
        /**
         * Destructor
         */
        virtual ~Callback();

        /**
         * The sender has successfully established this connection.
         * @param portId the port ID of the receiver
         * @param label the label of the closed connection
         * @param opt whether the connection is optional or not
         */
        virtual void onConnected(const unsigned long long portId,
                                 PortLabel const& label,
                                 const bool opt) = 0;

        /**
         * The sender has been disconnected because the connection broke unexpectedly.
         * @param portId the port ID of the receiver
         * @param label the label of the closed connection
         * @param opt whether the connection is optional or not
         */
        virtual void onDisconnected(const unsigned long long portId,
                                    PortLabel const& label,
                                    const bool opt) = 0;

        /**
         * Process a message received by a Data Receiver.
         * @param msg Message data pointer
         * @param size Message data size
         * @param user_data User data to indicate the origin of the message
         */
        virtual void onMessage(void* msg, uint32_t size, const user_data_t& user_data) = 0;
    };

    /// Description of a DATA port
    typedef struct _port_data_t
    {
        _port_data_t()
          : callback(NULL)
          , ns_label(NULL)
          , recipients()
          , operator_ns_label(NULL)
        {}
        /// Function for processing input
        Callback* callback;
        /// Name to use to register the port with the name server
        const char* ns_label;
        /// Flag to return to indicate the origin of incoming data
        user_data_t user_data;
        /// Required number of senders that will connect
        std::set<std::string> recipients;
        const char* operator_ns_label;
    } port_data_t;

  public:
    /// Destructor
    /// Close all the ports and unregister from the name service
    virtual ~DataReceiver();

    /// Wait for all the required senders to connect
    virtual void wait() = 0;

    /// Ask the Receiver to shutdown and stop processing messages
    virtual void shutdown() = 0;

    /// Run the receiver and process messages
    virtual void run() = 0;

    /// Get the port that the Receiver is listening on (used for testing)
    /// @return port that the Receiver is listening on
    virtual uint16_t port() const = 0;

    // Get the connection labels of all connections.
    // This function appends all the connection labels to the specified vector.
    // @param[out] labels vector which gets populated with connection labels
    virtual void getAllConnectionLabels(std::vector<std::string>& labels) const = 0;
};

DECL_EXCEPTION(UTILS_NAMESPACE, DataReceiver, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, ReadError, DataReceiver);
DECL_EXCEPTION(UTILS_NAMESPACE, EOFError, ReadError);

UTILS_NAMESPACE_END

#endif // !TRANS_DATA_RECEIVER_H_
