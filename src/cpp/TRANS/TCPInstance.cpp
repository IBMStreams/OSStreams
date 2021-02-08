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

#include <TRANS/TCPInstance.h>
#include <TRANS/TCPSender.h>
#include <TRANS/TCPSenderDynamic.h>
#include <TRANS/TransportUtils.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/RuntimeMessages.h>

#include <openssl/engine.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

using namespace Distillery;
using namespace Distillery::debug;
using namespace std;

Mutex& TCPInstance::getMutex()
{
    // Function-local static variables force correct initialization
    static Mutex mtx_;
    return mtx_;
}

boost::shared_ptr<TCPInstance>& TCPInstance::getInstancePtr()
{
    // Function-local static variables force correct initialization
    static shared_ptr theInstance;
    return theInstance;
}

boost::shared_ptr<TCPInstance> TCPInstance::instance()
{
    shared_ptr& theInstance = TCPInstance::getInstancePtr();
    AutoMutex m(TCPInstance::getMutex());
    if (theInstance.use_count() == 0) {
        theInstance.reset(new TCPInstance(), TCPInstance::deleter());
    }
    return theInstance;
}

TCPInstance::TCPInstance()
  : secure_(false)
  , securityType_(Security::None)
  , securityDirectorySet_(false)
  , securityDirectory_()
  , heartBeatOn_(TransportUtils::configureHeartbeatOn())
  , monitor_(this)
{
    init();
    start();
}

void TCPInstance::init()
{
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    ENGINE_load_builtin_engines();
    ENGINE_register_all_complete();

    SPCDBG(L_INFO, "Successfully initialized", CORE_TRANS_TCP);
}

TCPInstance::~TCPInstance()
{
    FIPS_mode_set(0);
    ENGINE_cleanup();
    EVP_cleanup();
    CRYPTO_cleanup_all_ex_data();
    ERR_free_strings();
    SPCDBG(L_INFO, "TCPInstance destroyed", CORE_TRANS_TCP);
}

void TCPInstance::start()
{
    // Create TCP monitor thread if the instance configuration has heartbeat on
    if (heartBeatOn_) {
        monitor_.create();
        SPCDBG(L_INFO, "TCPMonitorThread started", CORE_TRANS_TCP);
    } else {
        SPCDBG(L_INFO, "TCPMonitorThread configured to not start", CORE_TRANS_TCP);
    }
    SPCDBG(L_INFO, "Successfully started", CORE_TRANS_TCP);
}

void TCPInstance::stop()
{
    SPCDBG(L_INFO, "Stopping the monitor thread", CORE_TRANS_TCP);
    monitor_.shutdown();
    SPCDBG(L_INFO, "Successfully stopped", CORE_TRANS_TCP);
}

DataSender* TCPInstance::newSender(uint64_t portId,
                                   const std::vector<DataSender::PortDescription>& pds,
                                   const SenderType type)
{
    switch (type) {
        case Dynamic: {
            return new TCPSenderDynamic(portId, pds, TCPInstance::instance());
        }
        case Static: {
            return new TCPSender(portId, pds, TCPInstance::instance());
        }
    }
    return NULL;
}

DataSender* TCPInstance::newSender(const std::vector<DataSender::PortDescription>& pds,
                                   const SenderType type)
{
    switch (type) {
        case Dynamic: {
            return new TCPSenderDynamic(0, pds, TCPInstance::instance());
        }
        case Static: {
            return new TCPSender(0, pds, TCPInstance::instance());
        }
    }
    return NULL;
}

DataSender* TCPInstance::newSender(const DataSender::PortDescription& pd, const SenderType type)
{
    std::vector<DataSender::PortDescription> pds;
    pds.push_back(pd);

    return newSender(pds, type);
}

DataSender* TCPInstance::newSender(const std::string& address, uint16_t port, const SenderType type)
{
    switch (type) {
        case Dynamic: {
            return new TCPSenderDynamic(address, port, TCPInstance::instance());
        }
        case Static: {
            return new TCPSender(address, port, TCPInstance::instance());
        }
    }
    return NULL;
}

InetSocket* TCPInstance::newSocket(const bool server)
{
    if (secure_) {
        if (server) {
            return new SecureInetSocket(securityType_,
                                        TransportUtils::getTransportCertificatePath(),
                                        TransportUtils::getTransportKeyPath());
        } else {
            return new SecureInetSocket(securityType_,
                                        TransportUtils::getTransportCertificatePath());
        }
    }
    return new InetSocket;
}

void TCPInstance::add(TCPSender* const sender)
{
    AutoMutex am(sendersMutex_);
    senders_.append(sender);
    SPCDBG(L_DEBUG,
           "Successfully added sender " << std::hex << std::showbase << sender
                                        << " the TCPInstance now has " << std::dec
                                        << std::noshowbase << senders_.size() << " senders",
           CORE_TRANS_TCP);
}

void TCPInstance::remove(TCPSender* const sender)
{
    AutoMutex am(sendersMutex_);
    for (TCPSenderList::list_iterator it(senders_.begin()); !it.end(); it++) {
        if (*it == sender) {
            senders_.erase(it);
            SPCDBG(L_DEBUG,
                   "Successfully removed sender id "
                     << sender->getId() << " the TCPInstance now has " << std::dec
                     << std::noshowbase << senders_.size() << " senders",
                   CORE_TRANS_TCP);
            break;
        }
    }
}

void TCPInstance::doHeartbeat(volatile bool* interruptFlag,
                              streams_time_t beforeTime,
                              streams_time_t curTime)
{
    AutoMutex am(sendersMutex_);
    if (senders_.size() > 0) {
        SPCDBG(L_TRACE,
               "Sending heartbeat to "
                 << senders_.size()
                 << " senders on connections with latest write time earlier than " << beforeTime,
               CORE_TRANS_TCP);
    }

    for (TCPSenderList::list_iterator it(senders_.begin()); !it.end() && !(*interruptFlag); it++) {
        TCPSender* sender = *it;
        if (sender->completedShutdown()) {
            //
            // Skip this sender, it will be removed by TCPSender
            //
        } else {
            try {
                sender->doHeartbeat(interruptFlag, beforeTime, curTime);
            } catch (DistilleryException const& e) {
                SPCDBG(L_ERROR, "Unexpected exception " << e, CORE_TRANS_TCP);
            } catch (std::exception const& e) {
                SPCDBG(L_ERROR, "Unexpected exception " << e.what(), CORE_TRANS_TCP);
            } catch (...) {
                SPCDBG(L_ERROR, "Unexpected exception", CORE_TRANS_TCP);
            }
        }
    }
}

std::ostream& Distillery::operator<<(std::ostream& s, const Distillery::TCPInstance::Id& id)
{
    s << "(PE=" << id.peId << ", launchCount=" << id.peLaunchCount << ')';
    return s;
}
