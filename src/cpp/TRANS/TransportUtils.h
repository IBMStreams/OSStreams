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

#ifndef TRANS_TRANSPORTUTILS_H_
#define TRANS_TRANSPORTUTILS_H_

#include <UTILS/DistilleryException.h>
#include <UTILS/Mutex.h>
#include <UTILS/RWMutex.h>
#include <UTILS/UTILSTypes.h>

#include <cstdlib>

/**
 * @file TransportUtils.h
 *
 * Common Transport utility functions and definitions.
 */

UTILS_NAMESPACE_BEGIN

class TransportUtils
{
  public:
    /**
     * Determine by reading the STREAMS_TCP_HEARTBEAT environment variable
     * whether TCP Transport is configured such that heartbeat packets are
     * sent to determine whether a connection is still alive.
     *
     * @return true if STREAMS_TCP_HEARTBEAT is not defined, or is defined
     * and equals the string "true" (case sensitive); otherwise false.
     */
    static bool configureHeartbeatOn();

    /**
     * Determine by reading the STREAMS_TCP_SOCKET_BLOCKING_WRITE environment
     * variable whether TCP Transport should use blocking socket writes or
     * not.
     *
     * @return true if STREAMS_TCP_SOCKET_BLOCKING_WRITE is defined and equals
     * the string "true" (case sensitive); otherwise false.
     */
    static bool configureBlockingWrites();

    /**
     * Return the path of the directory dedicated to transport security.
     */
    static std::string getTransportSecurityDirectory(void);

    /**
     * Return the path of the transport certrificate.
     */
    static std::string getTransportCertificatePath(void);

    /**
     * Return the path of the transport key.
     */
    static std::string getTransportKeyPath(void);

    static const uint32_t MIN_WAIT_TIME_US = 10000;
    static const uint32_t MAX_WAIT_TIME_US = 10000000;

    static inline uint32_t getReconnectWaitTimeBackoff(uint16_t retryCount)
    {
        ::srand(time(NULL));
        uint32_t backoffTime = (::rand() % 2 + 1) * (retryCount + 1) * MIN_WAIT_TIME_US;
        return backoffTime < MAX_WAIT_TIME_US ? backoffTime : MAX_WAIT_TIME_US;
    }
};

// RAII Mutex wrapper which allows the caller to lock/trylock/release.
class AutoMutex2
{
  public:
    AutoMutex2(Mutex& m, bool doLock = true)
      : mutex_(m)
      , released_(true)
    {
        if (doLock) {
            mutex_.lock();
            released_ = false;
        }
    }

    ~AutoMutex2()
    {
        if (!released_) {
            mutex_.unlock();
        }
    }

    void lock()
    {
        if (released_) {
            mutex_.lock();
            released_ = false;
        }
    }

    bool tryLock()
    {
        if (released_) {
            released_ = !mutex_.tryLock();
        }
        return !released_;
    }

    void release()
    {
        if (!released_) {
            mutex_.unlock();
            released_ = true;
        }
    }

  private:
    Mutex& mutex_;
    bool released_;
};

// RAII RWMutex wrapper variant which allows the caller to lock/trylock/release
class AutoReadMutex2
{
  public:
    AutoReadMutex2(RWMutex& m, bool doLock = true)
      : mutex_(m)
      , released_(true)
    {
        if (doLock) {
            mutex_.readLock();
            released_ = false;
        }
    }

    ~AutoReadMutex2(void)
    {
        if (!released_) {
            mutex_.unlock();
        }
    }

    void lock()
    {
        if (released_) {
            mutex_.readLock();
            released_ = false;
        }
    }

    bool tryLock()
    {
        if (released_) {
            released_ = !mutex_.tryReadLock();
        }
        return !released_;
    }

    void release()
    {
        if (!released_) {
            mutex_.unlock();
            released_ = true;
        }
    }

  private:
    RWMutex& mutex_;
    bool released_;
};

// RAII RWMutex wrapper variant which allows the caller to lock/trylock/release
class AutoWriteMutex2
{
  public:
    AutoWriteMutex2(RWMutex& m, bool doLock = true)
      : mutex_(m)
      , released_(true)
    {
        if (doLock) {
            mutex_.writeLock();
            released_ = false;
        }
    }

    ~AutoWriteMutex2(void)
    {
        if (!released_) {
            mutex_.unlock();
        }
    }

    void lock()
    {
        if (released_) {
            mutex_.writeLock();
            released_ = false;
        }
    }

    bool tryLock()
    {
        if (released_) {
            released_ = !mutex_.tryWriteLock();
        }
        return !released_;
    }

    void release()
    {
        if (!released_) {
            mutex_.unlock();
            released_ = true;
        }
    }

  private:
    RWMutex& mutex_;
    bool released_;
};

DECL_EXCEPTION(UTILS_NAMESPACE, Transport, Distillery);
DECL_EXCEPTION(UTILS_NAMESPACE, ShutdownRequested, Transport);

UTILS_NAMESPACE_END

#endif /* TRANS_TRANSPORTUTILS_H_ */
