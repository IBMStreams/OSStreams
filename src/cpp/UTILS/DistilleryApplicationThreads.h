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

#ifndef DISTILLERYAPPLICATION_THREADS_H_INCLUDED
#define DISTILLERYAPPLICATION_THREADS_H_INCLUDED

#include <UTILS/Thread.h>
#include <UTILS/UTILSTypes.h>
#include <UTILS/WorkerThread.h>

UTILS_NAMESPACE_BEGIN

class DistilleryApplication;

/// Run a thread for collecting and managing stdout/stderr messages.
class StdouterrLoggingThread : public UTILS_NAMESPACE::Thread
{
  public:
    virtual void* run(void* arg);
};

/// Run a thread for collecting and managing stdout/stderr messages.
class StateMonitorThread : public UTILS_NAMESPACE::Thread
{
  public:
    StateMonitorThread(UTILS_NAMESPACE::DistilleryApplication* s);
    virtual void* run(void* arg);

  private:
    UTILS_NAMESPACE::DistilleryApplication* _server;
};

UTILS_NAMESPACE_END

#endif
