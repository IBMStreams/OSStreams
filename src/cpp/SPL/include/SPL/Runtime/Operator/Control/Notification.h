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

#ifndef SPL_RUNTIME_OPERATOR_CTRL_NOTIFICATION_H
#define SPL_RUNTIME_OPERATOR_CTRL_NOTIFICATION_H

/**
 * @file Notification.h
 * @brief Definition of the SPL::Notification class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Function/TimeFunctions.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <stdint.h>
#include <string>
#include <iostream>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL {

/// @ingroup Control
/// @{
/**
 * @brief Represents a notification emitted by an MBean.
 *
 * A Notification contains a type, sequence number, timestamp and,
 * optionally, specific user data represented as @c std::string, and a message.
 * The notification source is the object name of the MBean which emitted the
 * notification.
 *
 * @since InfoSphere&reg; Streams Version 4.0.0.
 */
class DLL_PUBLIC Notification
{
public:
    /**
     * Copy constructor.
     */
    Notification(Notification const& n) : source_(n.source_), type_(n.type_),
        sequenceNumber_(n.sequenceNumber_), ts_(n.ts_),
        userData_(n.userData_), message_(n.message_)
    {}

    /**
     * Creates a Notification object.
     *
     * The notification timeStamp is set to the current time.
     *
     * @param type The notification type.
     * @param source The notification source.
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     */
    Notification(std::string const& type, std::string const& source, uint64_t sequenceNumber) :
            source_(source), type_(type), sequenceNumber_(sequenceNumber),
            ts_(SPL::Functions::Time::getTimestamp()), userData_(""),
            message_("")
    {}

    /**
     * Creates a Notification object with message.
     *
     * The notification timeStamp is set to the current time.
     *
     * @param type The notification type.
     * @param source The notification source.
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     * @param message The detailed message.
     */
    Notification(std::string const& type, std::string const& source, uint64_t sequenceNumber, std::string const& message) :
            source_(source), type_(type), sequenceNumber_(sequenceNumber),
            ts_(SPL::Functions::Time::getTimestamp()), userData_(""),
            message_(message)
    {}

    /**
     * Creates a Notification object.
     *
     * @param type The notification type.
     * @param source The notification source.
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     * @param timeStamp It indicates when the notification was generated.
     *
     */
    Notification(std::string const& type, std::string const& source,
            uint64_t sequenceNumber, timestamp const& timeStamp) :
            source_(source), type_(type), sequenceNumber_(sequenceNumber),
            ts_(timeStamp), userData_(""), message_("")
    {}

    /**
     * Creates a Notification object.
     *
     * @param type The notification type.
     * @param source The notification source.
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     * @param timeStamp It indicates when the notification was generated.
     * @param message The detailed message.
     */
    Notification(std::string const& type, std::string const& source,
            uint64_t sequenceNumber, timestamp const& timeStamp, std::string const& message) :
            source_(source), type_(type), sequenceNumber_(sequenceNumber),
            ts_(timeStamp), userData_(""), message_(message)
    {}

    /**
     * Creates a Notification object.
     *
     * @param type The notification type.
     * @param source The notification source.
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     * @param timeStamp It indicates when the notification was generated.
     * @param message The detailed message.
     * @param userData The user data object.
     */
    Notification(std::string const& type, std::string const& source,
            uint64_t sequenceNumber, timestamp const& timeStamp, std::string const& message,
            std::string const& userData) :
            source_(source), type_(type), sequenceNumber_(sequenceNumber),
            ts_(timeStamp), userData_(userData), message_(message)
    {}

    Notification(std::string const& type, std::string const& userData) :
            type_(type), userData_(userData)
    {}

    const Notification& operator=(const Notification& n)
    {
        source_ = n.source_;
        type_ = n.type_;
        sequenceNumber_ = n.sequenceNumber_;
        ts_ = n.ts_;
        userData_ = n.userData_;
        message_ = n.message_;
        return *this;
    }

    /**
     * Set the source.
     *
     * @param source The source for this object.
     *
     * @see #getSource
     */
    void setSource(std::string const& source)
    {
        source_ = source;
    }

    /**
     * Get the notification sequence number.
     *
     * The sequence number is a serial number identifying a particular
     * instance of notification in the context of the notification source.
     * The notification model does not assume that notifications will be
     * received in the same order that they are sent. The sequence number
     * helps listeners to sort received notifications.
     *
     * @return The notification sequence number within the source object.
     * @see #setSequenceNumber
     */
    uint64_t getSequenceNumber() const
    {
        return sequenceNumber_;
    }

    /**
     * Set the notification sequence number.
     *
     * The sequence number is a serial number identifying a particular
     * instance of notification in the context of the notification source.
     *
     * @param sequenceNumber The notification sequence number within the
     *      source object.
     * @see #getSequenceNumber
     */
    void setSequenceNumber(uint64_t sequenceNumber)
    {
        sequenceNumber_ = sequenceNumber;
    }

    /**
     * Get the notification type.
     *
     * This is a string expressed in a dot notation similar to Java
     * properties.  It is recommended that the notification type should
     * follow the reverse-domain-name convention used by Java package
     * names.  An example of a notification type is
     * <tt>com.acme.streams.router.alarm</tt>.
     *
     * @return The notification type.
     */
    std::string getType() const
    {
        return type_;
    }

    /**
     * Get the notification timestamp.
     *
     * When converted from a JMX Notification, the Java timestamp value is
     * interpreted as the number of milliseconds since the Unix Epoch,
     * January 1, 1970, 00:00:00 GMT (as returned by <tt>java.util.Date.getTime()</tt>),
     * and the machineId field is set to 0.
     *
     * @return The notification timestamp as an SPL @c timestamp type.
     *
     * @see #setTimeStamp
     */
    timestamp getTimeStamp() const
    {
        return ts_;
    }

    /**
     * Set the notification timestamp.
     *
     * The timestamp indicates when the notification was generated.
     *
     * @param timeStamp The notification timestamp.
     *
     * @see #getTimeStamp
     */
    void setTimeStamp(timestamp const& timeStamp)
    {
        ts_ = timeStamp;
    }

    /**
     * Get the notification message.
     *
     * @return The message string of this notification object.
     */
    std::string getMessage() const
    {
        return message_;
    }

    /**
     * Get the user data.
     *
     * This is used for whatever data the notification source wishes to
     * communicate to its consumers.  For JMX Notifications, this function
     * returns the <tt>toString()</tt> value of the Java Notification's user
     * data.  An empty C++ string is returned if the Java Notification user
     * data is null.
     *
     * @return A string representation of the user data.
     *
     * @see #setUserData
     */
    std::string getUserData() const
    {
        return userData_;
    }

    /**
     * Get the notification source.
     *
     * If the source is an MXBean then this function returns the
     * canonical form of the source's @c ObjectName
     * (see <tt>javax.management.ObjectName.getCanonicalName()</tt>).
     *
     * @return The notification source.
     */
    std::string getSource() const
    {
        return source_;
    }

    /**
     * Set the user data.
     *
     * @param userData The user data object. It is used for whatever data
     * the notification source wishes to communicate to its consumers.
     *
     * @see #getUserData
     */
    void setUserData(std::string const& userData)
    {
        userData_ = userData;
    }

    /**
     * Returns a string representation of this notification.
     *
     * @return A string representation of this notification.
     */
    std::string toString() const;

    /**
     * Print internal state through the << operator.
     *
     * @param o output stream.
     * @param n The Notification object.
     */
    friend std::ostream& operator<< (std::ostream& o, Notification const& n);

#ifndef DOXYGEN_SKIP_FOR_USERS
    /** Destructor */
    ~Notification()
    {}
#endif /* DOXYGEN_SKIP_FOR_USERS */

private:
    void print(std::ostream& o) const;

    std::string source_;
    std::string type_;
    uint64_t sequenceNumber_;
    timestamp ts_;
    std::string userData_;
    std::string message_;
};

/////////////////////////////////////////////////////////////////////////////
inline std::ostream& operator<<(std::ostream& o, Notification const& n)
{
    n.print(o); return o;
}

inline std::string Notification::toString() const
{
    std::ostringstream s;
    print(s);
    return s.str();
}
/// @}
}

#endif /* SPL_RUNTIME_OPERATOR_CTRL_NOTIFICATION_LISTENER_H */
