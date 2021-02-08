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

#ifndef SPL_RUNTIME_UTILITY_RWMUTEX_H
#define SPL_RUNTIME_UTILITY_RWMUTEX_H

/*!
 * \file RWMutex.h \brief Definition of the SPL::RWMutex class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/RWMutex.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /// Class that provides a wrapper for a pthread rwlock
    class RWMutex : public Distillery::RWMutex
    {
    public:
        /// Constructor
        /// @param attr the attributes used by \c pthread_rwlock_init
        RWMutex(pthread_rwlockattr_t * attr = NULL)
            : Distillery::RWMutex(attr) {}

        /// Constructor
        /// @param name name for this mutex
        /// @param attr the attributes used by \c pthread_rwlock_init
        RWMutex(const char * name, pthread_rwlockattr_t * attr = NULL)
            : Distillery::RWMutex(name, attr) {}

        /// Read lock the mutex
        ///
        void readLock()
        {
            Distillery::RWMutex::readLock();
        }

        /// Write lock the mutex
        ///
        void writeLock()
        {
            Distillery::RWMutex::writeLock();
        }

        /// Unlock the mutex
        ///
        void unlock()
        {
            Distillery::RWMutex::unlock();
        }

        /// Lock the mutex. If the mutex is already locked this call does not
        /// block.
        /// @return true if the mutex was locked by this call, false otherwise
        bool tryReadLock()
        {
            return Distillery::RWMutex::tryReadLock();
        }

        /// Lock the mutex. If the mutex is already locked this call does not
        /// block.
        /// @return true if the mutex was locked by this call, false otherwise
        bool tryWriteLock()
        {
            return Distillery::RWMutex::tryWriteLock();
        }
    };
}

#endif /* SPL_RUNTIME_UTILITY_RWMUTEX_H */
