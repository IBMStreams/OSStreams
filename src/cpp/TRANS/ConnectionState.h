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

#ifndef TRANS_CONNECTION_STATE_EVENT_H_
#define TRANS_CONNECTION_STATE_EVENT_H_

#include <TRANS/OperatorPortLabel.h>
#include <TRANS/PortLabel.h>
#include <UTILS/Notification.h>
#include <UTILS/UTILSTypes.h>

#include <cassert>

UTILS_NAMESPACE_BEGIN

class SerializationBuffer;

/**
 * @file ConnectionState.h
 *
 * Definition of connection state changes, emitter and listener interfaces.
 */

class ConnectionState;

std::ostream& operator<<(std::ostream& o, const ConnectionState& ref);

/**
 * Provides definitions of connection state changes.
 */
class ConnectionState
{
  public:
    /**
     * Connection type values enumeration.
     */
    enum Direction
    {
        NONE = 0,
        INCOMING = 1,
        OUTGOING = 2
    };
    /**
     * Connection state values enumeration.
     */
    enum State
    {
        UNKNOWN = 0,
        INITIAL = 1,
        CONNECTING = 2,
        CONNECTED = 3,
        DISCONNECTED = 4,
        CLOSED = 5
    };

    /**
     * Does bounds checking before casting int to ConnectionState::State.
     * Assumes that the ConnectionState::State enum has contiguous values
     * starting with 0.
     * @throws InvalidArgumentException if value out of enum bounds
     */
    static State toState(int value);

    /**
     * Returns a string representation.
     */
    static const char* directionToString(ConnectionState::Direction);
    static const char* stateToString(ConnectionState::State);

    /**
     * Default constructor.
     */
    ConnectionState();

    /**
     * Constructs instance from port id, label and state.
     */
    ConnectionState(Direction dir,
                    unsigned long long id,
                    const PortLabel& pl,
                    State s,
                    bool isOptional);

    /**
     * Constructs instance from serialized form.
     *
     * @throws InvalidArgumentException if PeId from serialization buffer is
     *  different from local peId
     */
    ConnectionState(SerializationBuffer& sb);

    ~ConnectionState() {}

    /**
     * Serialize the object into the given buffer.
     * @param sb the serialization buffer
     */
    void serialize(UTILS_NAMESPACE::SerializationBuffer& sb) const;

    /** Get the string representation. */
    std::string toString() const;

    /** Accessors. */
    static void peId(unsigned long long id) { peId_ = id; }
    static unsigned long long peId() { return peId_; }

    void portId(unsigned long long id) { portId_ = id; }
    unsigned long long portId() const { return portId_; }

    void label(const PortLabel& pl) { label_ = pl; }
    PortLabel const& label() const { return label_; }

    void direction(ConnectionState::Direction v) { direction_ = v; }
    ConnectionState::Direction direction() const { return direction_; }

    void value(ConnectionState::State v) { value_ = v; }
    ConnectionState::State value() const { return value_; }

    void isOptional(bool v) { optional_ = v; }
    bool isOptional() const { return optional_; }

    void setTimestamp(uint64_t timestamp) { timestamp_ = timestamp; }
    uint64_t getTimestamp() const { return timestamp_; }

    std::string stringTuple() const;

    friend std::ostream& UTILS_NAMESPACE::operator<<(std::ostream& o, const ConnectionState& ref);

  private:
    /**
     * Note: instances are within the context of one PE, therefore peId is a static member
     */
    static unsigned long long peId_; ///< output PE Id of the connection

    Direction direction_;       ///< connection direction value
    unsigned long long portId_; ///< output port Id of the connection
    PortLabel label_;           ///< destination port label
    State value_;               ///< connection state value
    bool optional_;             ///< optional connection
    uint64_t timestamp_;        /// timestamp of this state change

    // print to stream
    std::ostream& print(std::ostream& s) const;
};

/// Connection state event emitter and listener interfaces.
typedef UTILS_NAMESPACE::NotificationEmitter<ConnectionState> ConnectionStateEmitter;
typedef UTILS_NAMESPACE::NotificationListener<ConnectionState> ConnectionStateListener;

///////////////////////////////////////////////////////////////////////////
inline std::string ConnectionState::toString() const
{
    std::ostringstream os;
    print(os);
    return os.str();
}

inline std::ostream& ConnectionState::print(std::ostream& os) const
{
    os << *this;
    return os;
}

UTILS_NAMESPACE_END

#endif /* !TRANS_CONNECTION_STATE_EVENT_H_ */
