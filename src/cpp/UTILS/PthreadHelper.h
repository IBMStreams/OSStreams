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
// Implements helper functionality for pthread calls

#ifndef PTHREAD_HELPER_H
#define PTHREAD_HELPER_H

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/BacktraceDumper.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

DECL_EXCEPTION(UTILS_NAMESPACE, PthreadFailure, Utils);

const unsigned PTHREAD_MAX_TRIES = 10;
const unsigned PTHREAD_INTER_RETRY_DELAY = 100;

// attempts the pthread call and fails with a return code
#define PTHREAD_CALL(call, pthreadCallRetValue)                                                    \
    {                                                                                              \
        pthreadCallRetValue = call;                                                                \
        if (pthreadCallRetValue) {                                                                 \
            char const* errstr;                                                                    \
            char err[256];                                                                         \
            char* rc = strerror_r(pthreadCallRetValue, err, 256);                                  \
            if (rc == NULL)                                                                        \
                errstr = "could not retrieve the message corresponding to the error number";       \
            else                                                                                   \
                errstr = rc;                                                                       \
            SPCDBG(L_ERROR,                                                                        \
                   "Calling '" << #call << "' failed with error code " << pthreadCallRetValue      \
                               << " (which translates to '" << errstr << "') --> returning",       \
                   DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                          \
        }                                                                                          \
    }

// attempts the pthread call and fails after retries with an exception
#define PTHREAD_CALL_NORET(call)                                                                   \
    {                                                                                              \
        unsigned thistryn = 0;                                                                     \
        while (1) {                                                                                \
            int pthreadCallRetValue = call;                                                        \
            if (pthreadCallRetValue == 0)                                                          \
                break;                                                                             \
            char const* errstr;                                                                    \
            char err[256];                                                                         \
            char* rc = strerror_r(pthreadCallRetValue, err, 256);                                  \
            if (rc == NULL)                                                                        \
                errstr = "could not retrieve the message corresponding to the error number";       \
            else                                                                                   \
                errstr = rc;                                                                       \
            if (pthreadCallRetValue == EINVAL) {                                                   \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> no point in retrying as an invalid argument was "    \
                                      "passed to the call",                                        \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue == EPERM) {                                             \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #"                                                                 \
                         << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '" << #call   \
                         << "' failed with error code " << pthreadCallRetValue                     \
                         << " (which translates to '" << errstr                                    \
                         << "') --> no point in retrying since thread does not own the mutex",     \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue && thistryn < PTHREAD_MAX_TRIES - 1) {                  \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr << "') --> retrying",    \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                sleep_random(PTHREAD_INTER_RETRY_DELAY, 50);                                       \
            } else if (pthreadCallRetValue && thistryn >= PTHREAD_MAX_TRIES - 1) {                 \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> abandoning with an exception",                       \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                BacktraceDumper::dump(2, true, 0);                                                 \
                THROW(PthreadFailure, "failure executing '"                                        \
                                        << #call                                                   \
                                        << "' -- additional details in the component log file");   \
            }                                                                                      \
            thistryn++;                                                                            \
        }                                                                                          \
    }

// attempts the pthread call, retries on failures, stops on sucess or timeout
#define PTHREAD_CALL_WITH_TIMEOUT(call, pthreadCallRetValue)                                       \
    {                                                                                              \
        unsigned thistryn = 0;                                                                     \
        while (1) {                                                                                \
            pthreadCallRetValue = call;                                                            \
            if (pthreadCallRetValue == 0 || pthreadCallRetValue == ETIMEDOUT)                      \
                break;                                                                             \
            char const* errstr;                                                                    \
            char err[256];                                                                         \
            char* rc = strerror_r(pthreadCallRetValue, err, 256);                                  \
            if (rc == NULL)                                                                        \
                errstr = "could not retrieve the message corresponding to the error number";       \
            else                                                                                   \
                errstr = rc;                                                                       \
            if (pthreadCallRetValue == EPERM) {                                                    \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #"                                                                 \
                         << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '" << #call   \
                         << "' failed with error code " << pthreadCallRetValue                     \
                         << " (which translates to '" << errstr                                    \
                         << "') --> no point in retrying since thread does not own the mutex",     \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue == EINVAL) {                                            \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> no point in retrying as an invalid argument was "    \
                                      "passed to the call",                                        \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue && thistryn < PTHREAD_MAX_TRIES - 1) {                  \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr << "') --> retrying",    \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                sleep_random(PTHREAD_INTER_RETRY_DELAY, 50);                                       \
            } else if (pthreadCallRetValue && thistryn >= PTHREAD_MAX_TRIES - 1) {                 \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> abandoning with an exception",                       \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                THROW(PthreadFailure, "failure executing '"                                        \
                                        << #call                                                   \
                                        << "' -- additional details in the component log file");   \
            }                                                                                      \
            thistryn++;                                                                            \
        }                                                                                          \
    }

// attempts the pthread call and fails with a return code if the return code
// is EBUSY, otherwise retries are attempted
#define PTHREAD_CALL_WITH_BUSY(call, pthreadCallRetValue)                                          \
    {                                                                                              \
        unsigned thistryn = 0;                                                                     \
        while (1) {                                                                                \
            pthreadCallRetValue = call;                                                            \
            if (pthreadCallRetValue == 0)                                                          \
                break;                                                                             \
            char const* errstr;                                                                    \
            char err[256];                                                                         \
            char* rc = strerror_r(pthreadCallRetValue, err, 256);                                  \
            if (rc == NULL)                                                                        \
                errstr = "could not retrieve the message corresponding to the error number";       \
            else                                                                                   \
                errstr = rc;                                                                       \
            if (pthreadCallRetValue == EBUSY) {                                                    \
                SPCDBG(L_TRACE,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' is busy",                                        \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue == EPERM) {                                             \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #"                                                                 \
                         << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '" << #call   \
                         << "' failed with error code " << pthreadCallRetValue                     \
                         << " (which translates to '" << errstr                                    \
                         << "') --> no point in retrying since thread does not own the mutex",     \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue == EINVAL) {                                            \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> no point in retrying as an invalid argument was "    \
                                      "passed to the call",                                        \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                break;                                                                             \
            } else if (pthreadCallRetValue && thistryn < PTHREAD_MAX_TRIES - 1) {                  \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr << "') --> retrying",    \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                sleep_random(PTHREAD_INTER_RETRY_DELAY, 50);                                       \
            } else if (pthreadCallRetValue && thistryn >= PTHREAD_MAX_TRIES - 1) {                 \
                SPCDBG(L_ERROR,                                                                    \
                       "Attempt #" << thistryn + 1 << " of " << PTHREAD_MAX_TRIES << " calling '"  \
                                   << #call << "' failed with error code " << pthreadCallRetValue  \
                                   << " (which translates to '" << errstr                          \
                                   << "') --> abandoning with an exception",                       \
                       DEBUG_NAMESPACE_QUALIFIER CORE_UTILS);                                      \
                THROW(PthreadFailure, "failure executing '"                                        \
                                        << #call                                                   \
                                        << "' -- additional details in the component log file");   \
            }                                                                                      \
            thistryn++;                                                                            \
        }                                                                                          \
    }

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
