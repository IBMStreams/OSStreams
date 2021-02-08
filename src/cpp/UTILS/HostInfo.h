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

// Class Description:
//
// Implements the hostinfo class which contains hostname and IP addr of a node.

#ifndef HOSTINFO_H
#define HOSTINFO_H

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <cstring>
#include <exception>
#include <iostream>
#include <netdb.h>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

// forward declaration
class SerializationBuffer;

/// This class stores information about a host
class HostInfo
{
  public:
    enum HostInfoType
    {
        /// INVALID_CONSTANT
        INVALID_CONSTANT = -1,
        /// UNDEFINED denotes a HostInfo object that will be initialized later
        UNDEFINED = 0,
        /// LOCALHOST denotes "localhost", i.e., the loopback address
        LOCAL_HOST = 1,
        /// THIS_HOST denotes whatever this host is
        THIS_HOST = 2
    };

    enum InterfaceHardwareType
    {
        /// NOTLOOPBACK_HARDWARE_TYPE
        NOTLOOPBACK_HARDWARE_TYPE = -3,
        /// ANY_HARDWARE_TYPE
        ANY_HARDWARE_TYPE = -2,
        /// INVALID_HARDWARE_TYPE
        INVALID_HARDWARE_TYPE = -1,
        /// UNDEFINED_HARDWARE_TYPE
        UNDEFINED_HARDWARE_TYPE = 0,
        /// ETHERNET_HARDWARE_TYPE
        ETHERNET_HARDWARE_TYPE = 1,
        /// INFINIBAND_HARDWARE_TYPE
        INFINIBAND_HARDWARE_TYPE = 2,
        /// LOOPBACK_HARDWARE_TYPE
        LOOPBACK_HARDWARE_TYPE = 3
    };

    /// Obtain the name of the host in which this process is running
    /// @return the host name
    static std::string getMyShortHostName();

    /// Obtain the long name (i.e., including the domain) of the host in
    /// which this process is running
    /// @return the host name
    static std::string getMyHostName();

    /// Convert a string into a HostInfoType
    /// @param s host string
    /// @return a HostInfoType corresponding to the host string
    static HostInfoType string2HostInfoType(const std::string& s);

    /// Create a HostInfo object from a string
    /// @param s host string
    /// @return a HostInfo object corresponding to the host string
    static HostInfo string2HostInfo(const std::string& s);

    static std::string host2name(const HostInfo& h, bool short_form = false);

    /// Obtain the IP address for an interface
    /// @param name interface name
    /// @return the the IP address (as a string) for an interface
    static std::string getInterfaceAddr(const std::string& name);

    /// Obtain the IP address for an interface that matches the input CIDR selection
    /// as specified by the value in the STREAMS_APPLICATION_NETWORK
    /// environment variable.
    /// @param ipaddress string reference to contain the matched ip address
    /// @param networkaddress string reference to contain the matched ip address of the network
    /// @param exclusively controls if the matching should be exclusive of the environment
    ///        variable. default is false
    /// @param hdwtype controls which type of interface to return; see InterfaceAddressType enum.
    /// @return the IP address for an interface that
    ///  matches the CIDR selection or INADDR_ANY
    static in_addr_t getNetworkAddr(std::string& ipaddress,
                                    std::string& networkaddress,
                                    bool exclusively = false,
                                    InterfaceHardwareType hdwtype = NOTLOOPBACK_HARDWARE_TYPE);

    /// Constructor
    /// @param hostname the host name
    HostInfo(const std::string& hostname);

    /// Constructor
    /// @param sct special case choice type (see HostInfoType)
    HostInfo(const HostInfoType sc);

    /// Constructor
    /// @param sct special case choice type (see HostInfoType)
    /// @param verify verify network interface for validation or not.
    HostInfo(const HostInfoType sc, bool verify);

    /// Constructor
    /// @param ipAddr the host IP address
    HostInfo(in_addr_t ipAddr);

    /// Constructor
    /// @param ipAddr the host IP address
    HostInfo(const struct in_addr& addr);

    /// Copy constructor
    /// @param h an existing HostInfo object
    HostInfo(const HostInfo& h);

    /// Serialization constructor
    /// @param s an existing serialization buffer
    HostInfo(SerializationBuffer& s);

    /// Assign the contents of another HostInfo object to this
    /// @param hi the input HostInfo object
    HostInfo& assign(const HostInfo& hi);

    /// Set the host name
    /// @param hname the host name
    HostInfo& assign(const std::string& hname);

    /// Serialize the HostInfo object
    /// @param s the serialization buffer
    virtual void serialize(SerializationBuffer& s) const;

    /// Get the host name
    /// @return the host name
    virtual std::string getHostName(void) const;

    /// Get the host name (without domain)
    /// @return the host name
    virtual std::string getShortHostName(void) const;

    /// Get the host IP address
    /// @return the host IP address as an in_addr_t reference
    virtual const in_addr_t& getIPAddress(void) const;

    /// Get the host IP address
    /// @return the host IP address in a struct in_addr
    virtual const struct in_addr getIPAddress_in(void) const;

    /// Get the host IP address
    /// @return the host IP address as a string
    virtual std::string getStringIPAddress(void) const;

    /// Determine if this is local host
    /// @return true if HostInfo is "localhost", i.e., the loopback interface
    virtual bool isLocalHost() const;

    /// Determine if this HostInfo object refers to the host where this
    // calls is being made
    /// @return true if any of the IP addresses of the host where the code
    /// is being invoked matches what is defined in this HostInfo object
    virtual bool isThisHost() const;

    /// Determine if this is a defined host
    /// @return true if HostInfo is defined
    virtual bool isDefined() const;

    /// Test if two HostInfo objects refer to the same host
    /// @param hi the other host
    /// @return true if this HostInfo object and hi refer to the same host
    virtual bool operator==(const HostInfo& hi) const;

    /// Test if two HostInfo objects refer to different hosts
    /// @param hi the other host
    /// @return true if this HostInfo object and hi refer to different hosts
    virtual bool operator!=(const HostInfo& hi) const;

    // Destructor
    virtual ~HostInfo();

  protected:
    /// Print internal state (i.e., the index contents) to an output stream
    /// @param o output stream
    virtual void print(std::ostream& o) const;

    /// host IP address (in network byte order)
    in_addr_t ipAddr;

    friend std::ostream& operator<<(std::ostream& o, const HostInfo& hi);

    static in_addr_t thisIP; // static variable to keep THIS_HOST IP
    static Mutex ipMutex;    // Mutex to guard updating/retrieving thisIP.

  private:
    /// Run the real constructor code
    void initialize(const HostInfoType sc, bool verify);
};

DECL_EXCEPTION(UTILS_NAMESPACE, HostInfo, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, UndefinedHostInfo, HostInfo);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidInterface, HostInfo);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
