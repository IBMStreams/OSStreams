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

#ifndef NAM_NAMESERVICE_H
#define NAM_NAMESERVICE_H

#include <NAM/NAM_Exceptions.h>
#include <NAM/NAM_Namespace.h>
#include <UTILS/MessageRecordingTypes.h>
#include <UTILS/Mutex.h>
#include <limits.h>
#include <sstream>
#include <vector>

NAM_NAMESPACE_BEGIN

/// \class NameRecord
/// Components derive their name records from this base type
class NameRecord
{
  public:
    /// must provide this to serialize object to a std::string which will be
    /// stored in the name server.
    virtual std::string toString() const = 0;
    /// must provide a method to reconstruct the object from the serialized
    /// buffer when looked up by the name service.
    virtual void setObject(const std::string& serializedData) = 0;

    virtual ~NameRecord();
};

class PortRecord : public NameRecord
{
  public:
    PortRecord();
    PortRecord(unsigned short myport);
    unsigned short getPort();
    std::string toString() const;
    void setObject(const std::string& serializedData);
    virtual ~PortRecord();

  private:
    unsigned short port;
};

class StringRecord : public NameRecord
{
  public:
    StringRecord();
    StringRecord(const std::string& mystring);
    std::string toString() const;
    void setObject(const std::string& serializedData);
    virtual ~StringRecord();

  private:
    std::string value;
};

class HostRecord : public NameRecord
{
  public:
    HostRecord();
    HostRecord(const std::string& hostIP, unsigned short myport);
    unsigned short getPort();
    std::string getHostAddr();
    virtual std::string toString() const;
    virtual void setObject(const std::string& serializedData);
    virtual ~HostRecord();

  protected:
    std::string hostAddr;
    unsigned short port;
};

class PePortRecord : public NameRecord
{
  public:
    PePortRecord()
      : peId_(ULLONG_MAX)
      , portId_(ULLONG_MAX)
    {}
    PePortRecord(unsigned long long peId, unsigned long long portId, unsigned long long jobId)
      : peId_(peId)
      , portId_(portId)
      , jobId_(jobId)
    {}
    unsigned long long getPeId() { return peId_; }
    unsigned long long getPortId() { return portId_; }
    unsigned long long getJobId() { return jobId_; }
    virtual std::string toString() const;
    virtual void setObject(const std::string& serializedData);
    virtual ~PePortRecord() {}

  protected:
    unsigned long long peId_;
    unsigned long long portId_;
    unsigned long long jobId_;
};

class HostRecordWithIB : public HostRecord
{
  public:
    HostRecordWithIB();
    HostRecordWithIB(const std::string& hostIP, unsigned short myport, bool ib);
    bool getIb() const;
    virtual std::string toString() const;
    virtual void setObject(const std::string& serializedData);
    virtual ~HostRecordWithIB();

  private:
    bool _ib; // flag for using IB or not.
};

class EndpointRecord : public NameRecord
{
  public:
    EndpointRecord();
    EndpointRecord(std::string const& label, unsigned short port, std::string const& proto);
    virtual ~EndpointRecord() {}

    std::string const& getLabel() const;
    std::string const& getCanonicalName() const;
    unsigned short getPort() const;
    std::string const& getProtocol() const;

    virtual std::string toString() const;
    virtual void setObject(const std::string& serializedData);

  protected:
    std::string m_label;
    std::string m_canonicalName;
    unsigned short m_port;
    std::string m_protocol;
};

/// class NAM_NameService
/// This is the NAM component interface. Various other components use the API
/// to register named records.
class NAM_NameService
{
  public:
    /// construct a name service client object
    /// @param nsreg -- provide the appropriate Name Service location information
    /// @param domainID -- Streams domain ID
    /// Return a reference to the Name Service implementation
    static NAM_NameService* getInstance(const std::string& nsreg, const std::string& domainID);
    /// construct a name service client object
    /// @param nsreg -- provide the appropriate Name Service location information
    /// @param domainID -- Streams domain ID
    /// @param instID -- Streams instance ID
    /// Return a reference to the Name Service implementation
    static NAM_NameService* getInstance(const std::string& nsreg,
                                        const std::string& domainID,
                                        const std::string& instID);

    /// construct a name service client object
    /// Return a reference to the Name Service implementation
    static NAM_NameService* getInstance(bool refresh = false);

    /// register a name record against a name
    /// @param name -- the name of the object
    /// @param nr -- the name record that has to be stored in the server.
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    /// @throws NameExistsException if the name already exists in the server
    virtual void registerObject(const std::string& name,
                                const NameRecord& nr,
                                int numRetries = -1) = 0;
    virtual void registerObject(const std::string& name,
                                const NameRecord& nr,
                                const std::string& iid,
                                int numRetries = -1);

    /// unregister a name
    /// @param name -- name to be unregistered
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    virtual void unregisterObject(const std::string& name, int numRetries = -1) = 0;
    virtual void unregisterObject(const std::string& name,
                                  const std::string& iid,
                                  int numRetries = -1);

    /// update the record associated with a name
    /// @param name -- name whose record has to be updated
    /// @param nr -- name record to be updated
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    /// @throws NameNotFoundException if the supplied name is not found.
    virtual void updateObject(const std::string& name,
                              const NameRecord& nr,
                              int numRetries = -1) = 0;
    virtual void updateObject(const std::string& name,
                              const NameRecord& nr,
                              const std::string& iid,
                              int numRetries = -1);

    /// Lookup a given name in the NS. The behavior of LDAP and FS name servers is different on
    /// lookup. In FS, due to NFS stale handle issues, lookup is retried on any failure. Eventually
    /// NameNotFoundxception is thrown. In LDAP, retries occur only if server is busy and a
    /// NAMException is returned eventually. If name is not found, NameNotFoundException is thrown
    /// on first try in LDAP. In all cases, the default retry is 60 times. Can be overridden with
    /// nRetries.
    /// @param name -- name to be looked up
    /// @param nr -- name record to be associated with teh name
    /// @param numRetries -- number of times the server must retry during name lookup.
    /// @param force -- bypass any caching mechanism implemented by the service.
    virtual void lookupObject(const std::string& name,
                              NameRecord& nr,
                              int numRetries = -1,
                              const bool force = false) = 0;
    virtual void lookupObject(const std::string& name,
                              NameRecord& nr,
                              const std::string& iid,
                              int numRetries = -1,
                              const bool force = false);

    /// create a directory in the name tree. Returns success if subdir already exists.
    /// @param name -- full path name of the sub-directory to be created
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    virtual void createSubdir(const std::string& name, int numRetries = -1) = 0;
    virtual void createSubdir(const std::string& name, const std::string& iid, int numRetries = -1);

    /// remove a sub directory from the name tree. All elements in it should
    /// already have been removed.
    /// @param name -- full path name of the directory to be removed
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    virtual void destroySubdir(const std::string& name, int numRetries = -1) = 0;
    virtual void destroySubdir(const std::string& name,
                               const std::string& iid,
                               int numRetries = -1);

    /// return a list of names that matched a given filter
    /// @param filter -- filter using the LDAP filter syntax (RFC 1823)
    /// @param numRetries -- number of times to retry the call.
    ///               If -1, the default setting of max_retry for the NS instance is used.
    virtual std::vector<std::string> listObjects(const std::string& filter,
                                                 int numRetries = -1) = 0;
    virtual std::vector<std::string> listObjects(const std::string& filter,
                                                 const std::string& iid,
                                                 int numRetries = -1);

    /// Set message recording configurations.
    /// @param mrct MessageRecordingConfigurationType object
    /// @param numRetries -- number of times to retry the call.
    ///        If -1, the default setting of max_retry for the NS instance is used.
    virtual void setMessageRecordingConfiguration(
      const UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
      int numRetries = -1) = 0;

    /// Get message recording configurations.
    /// @return MessageRecordingConfigurationType object
    /// @param numRetries -- number of times to retry the call.
    ///        If -1, the default setting of max_retry for the NS instance is used.
    virtual void getMessageRecordingConfiguration(
      UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
      int numRetries = -1) = 0;

    /// Set the retry time limit.
    /// @param numRetries -- number of attempts to retry looking up a name.
    static void setNumRetries(int retries);

    /// Return the current the retry time limit.
    /// @Return the current the retry time limit.
    static int getNumRetries();

    /// Indicate that a shutdown has been requested.
    /// exit in case it is in a retry loop.
    void setShutdown();

    virtual ~NAM_NameService();
    // max_retry must be volatile as it is updated without mutex protection
    static volatile int max_retry;

    static volatile bool shutdown_request;

  protected:
    NAM_NameService();
    static NAM_NameService* actualNSObject(const std::string& distNS,
                                           const std::string& distilleryID);
    static NAM_NameService* actualNSObject(const std::string& distNS,
                                           const std::string& domainID,
                                           const std::string& distilleryID);

    static NAM_NameService* nsInstance;
    static UTILS_NAMESPACE::Mutex _mutex;
    static std::string _distNS;
    static std::string _distID;
    static std::string _domainID;

    int _shutdownKey;
};

NAM_NAMESPACE_END

#endif
