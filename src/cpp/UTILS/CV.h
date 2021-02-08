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

//
// Class Description:
//
// Implements a wrapper class for a pthread conditional variable

#ifndef CV_H
#define CV_H

#include <UTILS/UTILSTypes.h>
#include <ctime>
#include <pthread.h>

UTILS_NAMESPACE_BEGIN

// forward declarations
class Mutex;

/// This class implements a wrapper for a pthread conditional variable
class CV
{
  public:
    /// Constructor
    /// @param myattr the attributes used by pthread_cond_init
    CV(pthread_condattr_t* myattr = NULL);

    /// Signal a condition variable
    ///
    void signal(void);

    /// Broadcast-signal a condition variable
    ///
    void broadcast(void);

    /// Wait on a conditional variable
    /// @param m the mutex used to suspend and start waiting
    void wait(Mutex& m);

    /// Wait on a conditional variable, subjected to a deadline
    /// @param m the mutex used to suspend and start waiting
    /// @param abstimeout the deadline (absolute time -- i.e., the actual time
    /// by which the timeout should occur)
    bool wait(Mutex& m, const struct timespec& abstimeout);

    /// Wait on a conditional variable, subjected to a deadline (equivalent
    /// to wait -- "waitUntil" explict denotes to wait until some time in the
    /// future, i.e., in terms of absolute time)
    /// @param m the mutex used to suspend and start waiting
    /// @param abstimeout the deadline (absolute time -- i.e., the actual time
    /// by which the timeout should occur -- e.g., by next tuesday at 3pm)
    inline bool waitUntil(Mutex& m, const struct timespec& abstimeout)
    {
        return wait(m, abstimeout);
    }
    /// Wait on a conditional variable, subjected to a deadline, defined in
    /// terms of how much to wait (relative time)
    /// @param m the mutex used to suspend and start waiting
    /// @param reltimeout the deadline (relative time -- i.e., how much time
    /// in the future the timeout should occur -- e.g., in 5 seconds)
    bool waitFor(Mutex& m, const struct timespec& reltimeout);

    /// Destructor
    ///
    ~CV(void);

  protected:
    /// Test if time denoted by abstime is more than a second in the past
    /// @param abstime the deadline (absolute time)
    /// @return true if abstime has already passed
    bool isTimeMoreThanASecondInThePast(const struct timespec& abstime);

    /// the pthread conditional variable
    pthread_cond_t cv;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
