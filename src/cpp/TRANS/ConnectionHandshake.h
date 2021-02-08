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

#ifndef TRANS_CONNECTION_HANDSHAKE_H_
#define TRANS_CONNECTION_HANDSHAKE_H_

#include <TRANS/DataSender.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>

#include <utility>

UTILS_NAMESPACE_BEGIN

// forward declaration
class SerializationBuffer;

/// This class represents and verifies a connection handshake
class ConnectionHandshake
{
  public:
    /**
     * Get the connection handshake timeout value.
     *
     * The default (1000000, 1 second) can be changed using an
     * undocumented env var STREAMS_CONN_HANDSHAKE_TIMEOUT_USEC
     *
     * @param secure The connection is secure
     * @return The connection handshake timeout value in microseconds
     */
    static long getTimeoutUsec(const bool secure);

    /**
     * Constructor.
     * @param senderId Uniquely identifies the sender which initiated this
     *      handshake
     * @param label The name service label of the receiver's port for this
     *      connection
     * @param required Is this a required Connection (false=optional)
     */
    ConnectionHandshake(const DataSender::Id& senderId, const std::string& label, bool required);

    /**
     * Serialization constructor with exception if the connection handshake
     *  is for the incorrect receiver.
     * @param s an existing serialization buffer
     * @param label The expected name service label of the receiver's port for
     *     this connection
     * @throws ConnectionHandshakeException
     */
    ConnectionHandshake(SerializationBuffer& s, const std::string& label);

    /**
     * Serialize the ConnectionHandshake object.
     * @param s the serialization buffer
     */
    void serialize(SerializationBuffer& s) const;

    /**
     * Get the Name Service Label of the receiver's port for this connection.
     * @return Name Service Label of the receiver's port for this connection
     */
    std::string getNSLabel() const { return nsLabel_; }

    void setNSLabel(const std::string& ns_label) { nsLabel_ = ns_label; }

    /**
     * Get the instance id where the connection handshake was created.
     * @return the instance id where the connection handshake was created
     */
    std::string getInstanceId() const { return instanceId_; }

    /**
     * Get the required connection flag for this connection.
     * @return the required connection flag for this connection
     */
    bool isRequiredConnection() const { return requiredConnection_; }

    /**
     * Get the Id of the sender.  This Id uniquely identifies the data
     * sender which attempts to establish a connection.
     * @return the stream Id
     */
    const DataSender::Id& getSenderId() const { return senderId_; }

    /** Destructor */
    ~ConnectionHandshake();

  private:
    // Magic number to verify its a transport message
    const static uint32_t handshakeMagic = 0x03FCFEE;
    // The instance id where the connection handshake was created
    std::string instanceId_;
    // Name Service Label of the receiver's port for this connection
    std::string nsLabel_;
    // Flag indicating if this is a required connection (false=optional)
    bool requiredConnection_;
    // Sender Id
    DataSender::Id senderId_;
};

/// This class represents and verifies a connection handshake reply.
class ConnectionHandshakeReply
{
  public:
    enum connHandshakeCode
    {
        /// Connection Handshake OK
        CONNECTED,
        /// Unexpected Instance
        BAD_INSTANCE,
        /// Unexpected Port Label
        BAD_LABEL
    };

    /// Constructor
    /// @param reply The reply code for this connection handshake
    ConnectionHandshakeReply(const connHandshakeCode reply);

    /// Serialization constructor with exception if the connection handshake
    /// reply indicated an incorrect receiver.
    /// @param s an existing serialization buffer
    /// @throws ConnectionHandshakeException
    ConnectionHandshakeReply(SerializationBuffer& s);

    /// Serialize the ConnectionHandshake object
    /// @param s the serialization buffer
    void serialize(SerializationBuffer& s) const;

    /// Get the reply code
    /// @return the connection handshake reply code
    connHandshakeCode getReplyCode() const;

    /// Destructor
    ~ConnectionHandshakeReply();

  private:
    /// The connection handshake reply code
    uint32_t replyCode;
    /// Magic number to verify its a transport message
    const static uint32_t handshakeMagic = 0x03FCFEE;
};

DECL_EXCEPTION(UTILS_NAMESPACE, ConnectionHandshake, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, UnexpectedInstance, ConnectionHandshake);
DECL_EXCEPTION(UTILS_NAMESPACE, UnexpectedLabel, ConnectionHandshake);

UTILS_NAMESPACE_END

#endif // !TRANS_CONNECTION_HANDSHAKE_H_
