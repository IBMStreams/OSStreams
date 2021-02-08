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

#include <iostream>
#include <stdio.h>
#include <string.h>
#include <vector>

#include <NAM/FS_NameService.h>
#include <NAM/K8S_NameService.h>
#include <NAM/NAMMessages.h>
#include <NAM/NAM_NameService.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/DistilleryCallback.h>
#include <UTILS/HostInfo.h>
#include <UTILS/SupportFunctions.h>

#include <boost/property_tree/json_parser.hpp>

using namespace std;
UTILS_NAMESPACE_USE
NAM_NAMESPACE_USE
DEBUG_NAMESPACE_USE

volatile bool NAM::NAM_NameService::shutdown_request = false;
volatile int NAM::NAM_NameService::max_retry = 60;
NAM::NAM_NameService* NAM_NameService::nsInstance = NULL;
Mutex NAM::NAM_NameService::_mutex;
std::string NAM::NAM_NameService::_distNS;
std::string NAM::NAM_NameService::_distID;
std::string NAM::NAM_NameService::_domainID;

NameRecord::~NameRecord() {}

NAM_NameService::NAM_NameService()
{
    // NAM does not register a quiesce function because NAM must still be
    // available while a component quiesces (to complete requests)
    SPCDBG(L_INFO, "Register setShutdown", NAM_GET_INSTANCE);
    _shutdownKey = DistilleryApplication::getThisApp().registerShutdown(
      mem_cb(this, &NAM_NameService::setShutdown));
}

NAM_NameService::~NAM_NameService()
{
    DistilleryApplication::getThisApp().unregisterShutdown(_shutdownKey);
}

NAM_NameService* NAM_NameService::getInstance(const string& nsreg, const string& domainID)
{
    SPCDBG(L_DEBUG, "getInstance with ns: " << nsreg << " and domainID: " << domainID,
           NAM_GET_INSTANCE);
    // keep same behavior as in UTILS
    // Fix the behavior by bugzilla 4080. After fix, throw exception if Distillery NS is empty
    if (nsreg.length() == 0) {
        THROW(NAM, "Distillery Name Server is empty", NAMServerEmpty);
    }
    if (domainID.length() == 0) {
        THROW(NAM, "Distillery Domain ID is empty");
    }

    AutoMutex lck(_mutex);
    if (nsInstance == NULL || (nsreg.compare(_distNS) != 0) || (domainID.compare(_domainID) != 0)) {
        _distNS = nsreg;
        _domainID = domainID;
        try {
            nsInstance = actualNSObject(_distNS, _domainID);
        } catch (UTILS_NAMESPACE::DistilleryException& e) {
            nsInstance = NULL;
            throw;
        }
    }

    SPCDBG(L_DEBUG, "Exit", NAM_GET_INSTANCE);
    return nsInstance;
}

NAM_NameService* NAM_NameService::getInstance(const std::string& nsreg,
                                              const std::string& domainID,
                                              const std::string& instID)
{
    SPCDBG(L_DEBUG,
           "getInstance with ns: " << nsreg << ", domainId: " << domainID
                                   << ", and iid: " << instID,
           NAM_GET_INSTANCE);
    if (nsreg.length() == 0) {
        THROW(NAM, "Distillery Name Server is empty", NAMServerEmpty);
    }
    if (domainID.length() == 0) {
        THROW(NAM, "Distillery Domain ID is empty");
    }
    if (instID.length() == 0) {
        THROW(NAM, "Distillery Instance ID is empty", NAMInstanceIdEmpty);
    }

    AutoMutex lck(_mutex);
    if (nsInstance == NULL || (nsreg.compare(_distNS) != 0) || domainID.compare(_domainID) != 0 ||
        (instID.compare(_distID) != 0)) {
        _distNS = nsreg;
        _distID = instID;
        _domainID = domainID;
        try {
            nsInstance = actualNSObject(_distNS, domainID, instID);
        } catch (UTILS_NAMESPACE::DistilleryException& e) {
            nsInstance = NULL;
            throw;
        }
    }

    SPCDBG(L_DEBUG, "Exit", NAM_GET_INSTANCE);
    return nsInstance;
}

NAM_NameService* NAM_NameService::getInstance(bool refresh)
{
    SPCDBG(L_TRACE, "Enter", NAM_GET_INSTANCE);
    std::string nsreg = UTILS_NAMESPACE::getNS();
    string domainID = get_environment_variable("STREAMS_DOMAIN_ID", "");
    string instID = get_environment_variable("STREAMS_INSTANCE_ID", "");
    AutoMutex lck(_mutex);
    if (nsInstance == NULL || (nsreg.compare(_distNS) != 0) || (domainID.compare(_domainID) != 0) ||
        (!instID.empty() && instID.compare(_distID) != 0) || refresh) {
        _distNS = nsreg;
        _domainID = domainID;
        _distID = instID;

        SPCDBG(L_TRACE, "getInstance with ns: " << nsreg << " and domain id: " << domainID,
               NAM_GET_INSTANCE);
        // Can not delete the pointer, as some usage may still there.
        // if(nsInstance != NULL) {
        // delete nsInstance;
        //}
        try {
            nsInstance = actualNSObject(_distNS, domainID, instID);
        } catch (DistilleryException& e) {
            nsInstance = NULL;
            throw;
        }
    }
    SPCDBG(L_TRACE, "Exit", NAM_GET_INSTANCE);
    return nsInstance;
}

NAM_NameService* NAM_NameService::actualNSObject(const string& distNS, const string& domainID)
{
    SPCDBG(L_TRACE, "Enter actualNSObject with ns: " << distNS << " and domainID: " << domainID,
           NAM_GET_INSTANCE);
    if (distNS.substr(0, 3) == "FS:") {
        SPCDBG(L_TRACE, "NAM_NameService::creating FS instance", NAM_GET_INSTANCE);
        return new FS_NameService(distNS, domainID);
    } else if (distNS.substr(0, 4) == "K8S:") {
        SPCDBG(L_TRACE, "NAM_NameService::creating K8S instance", NAM_GET_INSTANCE);
        return new K8S_NameService(distNS, domainID);
    } else {
        if (distNS.empty()) {
            THROW(NAM, "Name Service not configured", NAMNotConfigured);
        } else {
            THROW(NAM, "Invalid Name Service: " << QT(distNS), NAMInvalidService, distNS.c_str());
        }
    }
    SPCDBG(L_TRACE, "Exit", NAM_GET_INSTANCE);
}

NAM_NameService* NAM_NameService::actualNSObject(const string& distNS,
                                                 const std::string& domainID,
                                                 const string& distilleryID)
{
    SPCDBG(L_DEBUG,
           "getInstance with ns: " << distNS << ", domainId: " << domainID
                                   << ", and iid: " << distilleryID,
           NAM_GET_INSTANCE);
    if (distNS.substr(0, 3) == "FS:") {
        SPCDBG(L_TRACE, "NAM_NameService::creating FS instance", NAM_GET_INSTANCE);
        return new FS_NameService(distNS, domainID, distilleryID);
    } else if (distNS.substr(0, 4) == "K8S:") {
        SPCDBG(L_TRACE, "NAM_NameService::creating K8S instance", NAM_GET_INSTANCE);
        return new K8S_NameService(distNS, domainID, distilleryID);
    } else {
        if (distNS.empty()) {
            THROW(NAM, "Name Service not configured", NAMNotConfigured);
        } else {
            THROW(NAM, "Invalid Name Service: " << QT(distNS), NAMInvalidService, distNS.c_str());
        }
    }
    SPCDBG(L_DEBUG, "Exit", NAM_GET_INSTANCE);
}

void NAM_NameService::setNumRetries(int retries)
{
    max_retry = retries;
}

int NAM_NameService::getNumRetries()
{
    return max_retry;
}

void NAM_NameService::setShutdown()
{
    shutdown_request = true;
}

void NAM_NameService::registerObject(const string& name,
                                     const NameRecord& nr,
                                     const string& iid,
                                     int numRetries)
{}

void NAM_NameService::unregisterObject(const string& name, const string& iid, int numRetries) {}

void NAM_NameService::updateObject(const string& name,
                                   const NameRecord& nr,
                                   const string& iid,
                                   int numRetries)
{}

void NAM_NameService::lookupObject(const string& name,
                                   NameRecord& nr,
                                   const string& iid,
                                   int numRetries,
                                   const bool force)
{}

void NAM_NameService::createSubdir(const string& name, const string& iid, int numRetries) {}

void NAM_NameService::destroySubdir(const string& name, const string& iid, int numRetries) {}

vector<string> NAM_NameService::listObjects(const string& filter, const string& iid, int numRetries)
{
    return vector<string>();
}

PortRecord::PortRecord()
  : port(0)
{}

PortRecord::PortRecord(unsigned short myport)
  : port(myport)
{}

PortRecord::~PortRecord() {}

unsigned short PortRecord::getPort()
{
    return port;
}

string PortRecord::toString() const
{
    stringstream ssport;
    ssport << port;
    return ssport.str();
}

void PortRecord::setObject(const string& serializedData)
{
    port = atoi(serializedData.c_str());
}

StringRecord::StringRecord() {}

StringRecord::StringRecord(const string& mystring)
  : value(mystring)
{}

StringRecord::~StringRecord() {}

string StringRecord::toString() const
{
    return value;
}

void StringRecord::setObject(const string& serializedData)
{
    value = serializedData;
}

HostRecord::HostRecord()
  : port(0)
{}

HostRecord::HostRecord(const string& hostIP, unsigned short myport)
  : hostAddr(hostIP)
  , port(myport)
{}

unsigned short HostRecord::getPort()
{
    return port;
}

string HostRecord::getHostAddr()
{
    return hostAddr;
}

string HostRecord::toString() const
{
    stringstream ssport;
    ssport << hostAddr << ":" << port;
    return ssport.str();
}

void HostRecord::setObject(const string& serializedData)
{
    std::vector<std::string> tok;
    tokenize(serializedData, tok, ":", false);
    // Note: HostRecord is used with transport layer for setting up connection.
    // In some cases, receiver side registers a HostRecordWithIB object, but
    // sender side tries to get it as a HostRecord object. Need to treat both
    // when setObject. This will not affect HostRecord contents and functions.
    // This is just a limited verification.
    if (tok.size() < 2) {
        THROW(Distillery, "Invalid HostRecord format: " << QT(serializedData), NAMInvalidRecord,
              serializedData.c_str());
    }
    hostAddr = tok[0];
    port = atoi(tok[1].c_str());
}

HostRecord::~HostRecord() {}

string PePortRecord::toString() const
{
    stringstream ss;
    ss << peId_ << "." << portId_ << "@" << jobId_;
    return ss.str();
}

void PePortRecord::setObject(const string& serializedData)
{
    std::vector<std::string> tok;
    tokenize(serializedData, tok, ".@", false);

    if (tok.size() < 3) {
        THROW(Distillery, "Invalid PePortRecord format: " << QT(serializedData), NAMInvalidRecord,
              serializedData.c_str());
    }
    peId_ = strtoull(tok[0].c_str(), NULL, 10);
    portId_ = strtoull(tok[1].c_str(), NULL, 10);
    jobId_ = strtoull(tok[2].c_str(), NULL, 10);
}

HostRecordWithIB::HostRecordWithIB()
  : HostRecord()
  , _ib(false)
{}

HostRecordWithIB::HostRecordWithIB(const string& hostIP, unsigned short myport, bool ib)
  : HostRecord(hostIP, myport)
  , _ib(ib)
{}

bool HostRecordWithIB::getIb() const
{
    return _ib;
}

string HostRecordWithIB::toString() const
{
    stringstream ssport;
    string boolStr = "false";
    if (_ib) {
        boolStr = "true";
    }
    ssport << hostAddr << ":" << port << ":" << boolStr;
    return ssport.str();
}

void HostRecordWithIB::setObject(const std::string& serializedData)
{
    std::vector<std::string> tok;
    tokenize(serializedData, tok, ":", false);
    if (tok.size() != 3) {
        THROW(Distillery, "Invalid HostRecordWithIB format: " << QT(serializedData),
              NAMInvalidRecord, serializedData.c_str());
    }
    hostAddr = tok[0];
    port = atoi(tok[1].c_str());
    _ib = false;
    if (0 == tolowercase(tok[2]).compare("true")) {
        _ib = true;
    }
}

HostRecordWithIB::~HostRecordWithIB() {}

EndpointRecord::EndpointRecord() {}

EndpointRecord::EndpointRecord(std::string const& label,
                               unsigned short port,
                               std::string const& proto)
  : m_label(label)
  , m_canonicalName()
  , m_port(port)
  , m_protocol(proto)
{}

std::string const& EndpointRecord::getLabel() const
{
    return m_label;
}

std::string const& EndpointRecord::getCanonicalName() const
{
    return m_canonicalName;
}

unsigned short EndpointRecord::getPort() const
{
    return m_port;
}

std::string const& EndpointRecord::getProtocol() const
{
    return m_protocol;
}

std::string EndpointRecord::toString() const
{
    std::ostringstream oss;
    oss << "{";
    oss << "\"canonicalName\": \"" << HostInfo::getMyHostName() << "\",";
    oss << "\"port\": " << m_port << ",";
    oss << "\"protocol\": \"" << m_protocol << "\"";
    oss << "}";
    return oss.str();
}

void EndpointRecord::setObject(const std::string& serializedData)
{
    /*
     * Parse the JSON content.
     */
    std::istringstream ss(serializedData);
    boost::property_tree::ptree pt;
    boost::property_tree::read_json(ss, pt);
    /*
     * Grab the values.
     */
    m_canonicalName = pt.get<std::string>("canonicalName");
    m_port = pt.get<unsigned short>("port");
    m_protocol = pt.get<std::string>("protocol");
}

IMPL_EXCEPTION(NAM, NAM, UTILS_NAMESPACE::Distillery);
IMPL_EXCEPTION(NAM, NameExists, NAM);
IMPL_EXCEPTION(NAM, NameNotFound, NAM);
IMPL_EXCEPTION(NAM, NSLocation, NAM);
