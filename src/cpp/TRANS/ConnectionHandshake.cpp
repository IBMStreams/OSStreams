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

#include <TRANS/ConnectionHandshake.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Formatter.h>
#include <UTILS/RuntimeMessages.h>
#include <UTILS/SerializationBuffer.h>
#include <UTILS/SupportFunctions.h>

#include <string>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

#define TIMEOUT_USEC_DEFAULT 1000000 // 1 second
#define TIMEOUT_USEC_SECURED 5000000 // 5 second
static const std::string TIMEOUT_USEC_ENV_VAR("STREAMS_CONN_HANDSHAKE_TIMEOUT_USEC");

// Get the connection handshake timeout value
long ConnectionHandshake::getTimeoutUsec(const bool secure)
{
    static long timeoutUsec = 0;
    double defv = secure ? TIMEOUT_USEC_SECURED : TIMEOUT_USEC_DEFAULT;
    if (!timeoutUsec) {
        timeoutUsec = get_environment_variable(TIMEOUT_USEC_ENV_VAR, defv);
    }
    return timeoutUsec;
}

// Constructor
ConnectionHandshake::ConnectionHandshake(const DataSender::Id& senderId,
                                         const std::string& label,
                                         const bool required)
  : instanceId_(Distillery::getInstanceId())
  , nsLabel_(label)
  , requiredConnection_(required)
  , senderId_(senderId)
{
    SPCDBG(L_DEBUG,
           "Created Connection Handshake (" << instanceId_ << ":" << nsLabel_
                                            << " requiredConnection=" << requiredConnection_
                                            << " senderId=" << senderId_ << ")",
           CORE_TRANS_HS);
}

// Serialization constructor with exception if the connection handshake
// is for the incorrect receiver.
ConnectionHandshake::ConnectionHandshake(SerializationBuffer& s, const std::string& label)
{
    uint32_t recvdMagic = s.getUInt32();
    if (recvdMagic != handshakeMagic) {
        HexString hexs(recvdMagic);
        // Unexpected Magic code {0} received while establishing connection
        THROW_CHAR(ConnectionHandshake, "Bad Connection Handshake Magic",
                   TRANSConnectionHandshakeMagic, hexs.c_str());
    }
    instanceId_ = s.getSTLString();
    if (instanceId_ != Distillery::getInstanceId()) {
        // This trace statement used by test handshake3.exp
        SPCDBG(L_INFO,
               "Throwing Exception: Distillery"
                 << " Msg: "
                 << "Unexpected Instance ID (" << instanceId_
                 << " != " << Distillery::getInstanceId() << ")",
               CORE_UTILS);

        // Unexpected instance ID received while establishing connection:
        // expected {0}, received {1}
        THROW_CHAR(UnexpectedInstance, "Unexpected Instance ID", TRANSConnectionHandshakeInstance,
                   Distillery::getInstanceId().c_str(), instanceId_.c_str());
    }
    nsLabel_ = s.getSTLString();
    if (nsLabel_ != label) {
        // This trace statement used by tests handshake2.exp, receiverImposter.pl
        SPCDBG(L_INFO,
               "Throwing Exception: Distillery"
                 << " Msg: "
                 << "Unexpected input port label (" << nsLabel_ << " != " << label << ")",
               CORE_UTILS);

        // Unexpected input port label {0} received while establishing connection; expected {1}
        THROW_CHAR(UnexpectedLabel, "Unexpected input port label",
                   TRANSConnectionHandshakePortLabel, nsLabel_.c_str(), label.c_str());
    }
    requiredConnection_ = s.getBool();
    senderId_.jobId = s.getUInt64();
    senderId_.peId = s.getUInt64();
    senderId_.outPortId = s.getUInt64();
    senderId_.peRestartCount = s.getUInt64();
}

// Serialize the ConnectionHandshake object
void ConnectionHandshake::serialize(SerializationBuffer& s) const
{
    SPCDBG(L_DEBUG, "serializing Connection Handshake (" << instanceId_ << ":" << nsLabel_ << ")",
           CORE_TRANS_HS);
    s.addUInt32(handshakeMagic);
    s.addSTLString(instanceId_);
    s.addSTLString(nsLabel_);
    s.addBool(requiredConnection_);
    s.addUInt64(senderId_.jobId);
    s.addUInt64(senderId_.peId);
    s.addUInt64(senderId_.outPortId);
    s.addUInt64(senderId_.peRestartCount);
}

// Destructor
ConnectionHandshake::~ConnectionHandshake() {}

IMPL_EXCEPTION(Distillery, ConnectionHandshake, Utils);
IMPL_EXCEPTION(Distillery, UnexpectedInstance, ConnectionHandshake);
IMPL_EXCEPTION(Distillery, UnexpectedLabel, ConnectionHandshake);
