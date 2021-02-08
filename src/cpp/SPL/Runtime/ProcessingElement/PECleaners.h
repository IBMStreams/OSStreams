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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_CLEANERS_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_CLEANERS_H

#include <UTILS/Mutex.h>

namespace SPL {
class PEImpl;

class AutoPeCleaner
{
  public:
    /// Constructor
    /// @param pe Processing Element that needs to be cleaned up
    AutoPeCleaner(PEImpl& pe)
      : pe_(pe)
      , status_(NONE)
    {}

    /// Destructor
    virtual ~AutoPeCleaner();

    /// Call initializeImpl() in derived class
    /// Change State to 'Initialized'
    void initialize();

    /// Call prepareToFinalizeImpl() in derived class
    /// Change state to 'Preparing to Finalize'
    void prepareToFinalize();

    /// Call finalizeImpl() in derived class
    /// Change state to 'Finalized'
    void finalize();

    /// Call finalize, ignoring exceptions thrown by finalizeImpl
    void finalize_nothrow();

  protected:
    virtual void initializeImpl() {}
    virtual void prepareToFinalizeImpl() {}
    virtual void finalizeImpl() {}

  protected:
    PEImpl& pe_;
    enum Status
    {
        NONE = 0,
        INIT,
        PREP,
        FINL
    };
    Status status_;
    Distillery::Mutex mutex_;
};

class AutoDebugServices : public AutoPeCleaner
{
  public:
    AutoDebugServices(PEImpl& pe)
      : AutoPeCleaner(pe)
    {}

  protected:
    virtual void initializeImpl();
    virtual void prepareToFinalizeImpl();
    virtual void finalizeImpl();
};

class AutoCheckpointServices : public AutoPeCleaner
{
  public:
    AutoCheckpointServices(PEImpl& pe)
      : AutoPeCleaner(pe)
    {}

  protected:
    virtual void initializeImpl();
    virtual void finalizeImpl();
};

class AutoMetricsIntervalTimer : public AutoPeCleaner
{
  public:
    AutoMetricsIntervalTimer(PEImpl& pe)
      : AutoPeCleaner(pe)
    {}

  protected:
    virtual void initializeImpl();
    virtual void finalizeImpl();
};

class AutoCreateOperators : public AutoPeCleaner
{
  public:
    AutoCreateOperators(PEImpl& pe)
      : AutoPeCleaner(pe)
    {}

  protected:
    virtual void initializeImpl();
    virtual void prepareToFinalizeImpl();
    virtual void finalizeImpl();
};

class AutoPePorts : public AutoPeCleaner
{
  public:
    AutoPePorts(PEImpl& pe)
      : AutoPeCleaner(pe)
    {}

    /// Call prepareToFinalizeImpl() in derived class
    /// Change state to 'Preparing to Finalize'.
    /// Overridden to handle shutdown during startup
    void prepareToFinalize();

    /// Call finalize, ignoring exceptions thrown by finalizeImpl
    /// Has to handle case where state is NONE
    void finalize_nothrow();

  protected:
    virtual void initializeImpl();
    virtual void prepareToFinalizeImpl();
    virtual void finalizeImpl();
};
};

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_PROCESSING_ELEMENT_CLEANERS_H */
