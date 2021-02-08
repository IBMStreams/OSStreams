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

#ifndef SPL_RUNTIME_UTILITY_CV_H
#define SPL_RUNTIME_UTILITY_CV_H

/*!
 * \file CV.h \brief Definition of the SPL::CV class.
 */

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <UTILS/CV.h>
#endif /*DOXYGEN_SKIP_FOR_USERS*/

#include <SPL/Runtime/Utility/Mutex.h>

namespace SPL
{
    /// Class that implements a wrapper for a pthread conditional variable
    class CV : protected Distillery::CV
    {
    public:
        /// Constructor
        /// @param attr the attributes used by \c pthread_cond_init
        CV(pthread_condattr_t * attr = NULL)
            : Distillery::CV(attr) {}

        /// Signal the condition variable
        ///
        void signal()
        {
            Distillery::CV::signal();
        }

        /// Broadcast-signal the condition variable
        ///
        void broadcast()
        {
            Distillery::CV::broadcast();
        }

        /// Wait on the conditional variable
        /// @param m the mutex used to suspend and start waiting
        void wait(Mutex & m)
        {
            Distillery::CV::wait(m);
        }

        /// Wait on a conditional variable, subjected to a deadline
        /// @param m the mutex used to suspend and start waiting
        /// @param absTimeout the deadline (absolute time -- i.e., the actual time
        /// by which the timeout should occur -- e.g., by next tuesday at 3pm)
        /// @return true if timed out during wait
        bool waitUntil(Mutex & m, const struct timespec & absTimeout)
        {
            return Distillery::CV::waitUntil(m, absTimeout);
        }

        /// Wait on a conditional variable, subjected to a deadline
        /// @param m the mutex used to suspend and start waiting
        /// @param relTimeout the deadline (relative time -- i.e., how much time
        /// in the future the timeout should occur -- e.g., in 5 seconds)
        /// @return true if timed out during wait
        bool waitFor(Mutex & m, const timespec & relTimeout)
        {
            return Distillery::CV::waitFor(m, relTimeout);
        }
    };
}

#endif /* SPL_RUNTIME_UTILITY_CV_H */
