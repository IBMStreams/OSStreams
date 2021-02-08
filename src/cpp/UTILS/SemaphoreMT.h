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
// Implements a semaphore class

#ifndef SEMAPHOREMT_H
#define SEMAPHOREMT_H

#include <UTILS/Semaphore.h>
#include <UTILS/UTILSTypes.h>
#include <cassert>

UTILS_NAMESPACE_BEGIN

// forward declarations
class CV;
class Mutex;

/// This class implements Dijkstra's counting semaphores for multithreaded (MT)
/// programs
class SemaphoreMT : public Semaphore
{
  public:
    /// Default constructor
    ///
    SemaphoreMT(const int myN = 0);

    /// Copy constructor (not implemented)
    /// @param smt a pre-existing SemaphoreMT object
    SemaphoreMT(const SemaphoreMT& smt);

    /// Test if a wait operation will cause the semaphore to block
    ///
    virtual bool testBlock(void);

    // DON'T USE THIS VERSION OF WAIT
    int wait(struct timeval* tv)
    {
        assert(0);
        return -1;
    };
    virtual void wait(void);

    virtual void post(void);

    /// Destructor
    ///
    virtual ~SemaphoreMT(void);

  protected:
    /// The number of resources to be protected
    int n;
    /// A mutex to protect the semaphore from race conditions
    Mutex* m;
    /// A conditional variable to signal when resources have become available
    CV* cv;
};

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
