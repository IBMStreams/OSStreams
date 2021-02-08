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
// An abstract semaphore class

#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <sys/time.h>

#include <UTILS/UTILSTypes.h>

UTILS_NAMESPACE_BEGIN

/// This abstract class describes Dijkstra's counting semaphores
class Semaphore
{
  public:
    virtual ~Semaphore() {} // To silence compiler warning

    /// Implement P (Proberen te Verlangen - wait) operation. When a semaphore
    /// is requested, if its value is 0 the caller is blocked. If it is greater
    /// than zero, than it is decremented
    virtual int wait(struct timeval* tv = 0) = 0;

    /// Implement V (Verhogen) operation. When the
    /// semaphore is released, the counter is incremented
    virtual void post(void) = 0;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
