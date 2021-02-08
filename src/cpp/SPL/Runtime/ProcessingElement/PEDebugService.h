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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_DEBUG_PE_SERVICE_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_DEBUG_PE_SERVICE_H

#include <SPL/Runtime/Utility/Thread.h>

namespace SPL {

class PEImpl;

/// Class that represents a debug service
class PEDebugService : public Thread
{
  public:
    /// Constructor
    /// @param pe perocessing element
    PEDebugService(PEImpl& pe)
      : pe_(pe)
    {}

    /// Destructor
    virtual ~PEDebugService(){};

    virtual void* run(void* args);

    /// Called asynchronously when the pe is ready to process tuples
    virtual void prepareToProcess();

    /// Called asynchronously when the pe is about to terminate
    virtual void prepareToShutdown();

  protected:
    PEImpl& pe_;
};

};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_DEBUG_PE_SERVICE_H */
