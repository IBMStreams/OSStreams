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

#ifndef SPL_RUNTIME_UTILITY_SPIN_LOCK_H
#define SPL_RUNTIME_UTILITY_SPIN_LOCK_H

/*!
 * \file SpinLock.h \brief Definition of the SPL::SpinLock class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/SpinLock.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

namespace SPL
{
    /// Class that provides a wrapper for a pthread mutex
    class SpinLock : public Distillery::SpinLock
    {
    public:
        /// Constructor
        ///
        SpinLock()
            : Distillery::SpinLock() {}

        /// Lock
        ///
        void lock()
        {
            Distillery::SpinLock::lock();
        }

        /// Unlock
        ///
        void unlock()
        {
            Distillery::SpinLock::unlock();
        }
    };

    /*! \brief Class that provides an RAII handler for critical sections.

        Protect a critical section of code using a passed in spin lock. E.g.:

        \code
        {
          AutoSpinLock myAutoSpinLock(mySpinLockObject); // Obtain the lock
          // Code that manipulates state
          ...
        } // Release the lock
        \endcode
    */
    class AutoSpinLock
    {
    public:
        /// Constructor
        /// @param s SpinLock object
        AutoSpinLock(SpinLock & s)
            : lock_(s)
        {
            lock_.lock();
        }

        /// Destructor
        ///
        ~AutoSpinLock()
        {
            lock_.unlock();
        }

    private:
        SpinLock & lock_;
    };
}

#endif /* SPL_RUNTIME_UTILITY_SPIN_LOCK_H */
