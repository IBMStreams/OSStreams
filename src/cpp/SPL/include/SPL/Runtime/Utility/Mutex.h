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

#ifndef SPL_RUNTIME_UTILITY_MUTEX_H
#define SPL_RUNTIME_UTILITY_MUTEX_H

/*!
 * \file Mutex.h \brief Definition of the SPL::Mutex class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/Mutex.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /// Class that provides a wrapper for a pthread mutex
    class Mutex : public Distillery::Mutex
    {
    public:
        /// Constructor
        /// @param attr the attributes used by \c pthread_mutex_init
        Mutex(pthread_mutexattr_t * attr = NULL)
            : Distillery::Mutex(attr) {}

        /// Constructor
        /// @param name name for this mutex
        /// @param attr the attributes used by \c pthread_mutex_init
        Mutex(const char * name, pthread_mutexattr_t * attr = NULL)
            : Distillery::Mutex(name, attr) {}

        /// Lock the mutex
        ///
        void lock()
        {
            Distillery::Mutex::lock();
        }

        /// Unlock the mutex
        ///
        void unlock()
        {
            Distillery::Mutex::unlock();
        }

        /// Lock the mutex. If the mutex is already locked this call does not
        /// block.
        /// @return true if the mutex was locked by this call, false otherwise
        bool tryLock()
        {
            return Distillery::Mutex::tryLock();
        }
    };

    /*! \brief Class that provides an RAII handler for critical sections.

        Protect a critical section of code using a passed in mutex. E.g.:

        \code
        {
          AutoMutex myAutoMutex(myMutexObject); // Obtain the lock
          // Code that manipulates state
          ...
        } // Release the lock
        \endcode
    */
    class AutoMutex
    {
    public:
        /// Constructor
        /// @param m Mutex object
        AutoMutex(Mutex & m)
            : mutex_(m)
        {
            mutex_.lock();
        }

        /// Destructor
        ///
        ~AutoMutex()
        {
            mutex_.unlock();
        }

    private:
        Mutex & mutex_;
    };

}

#endif /* SPL_RUNTIME_UTILITY_MUTEX_H */
