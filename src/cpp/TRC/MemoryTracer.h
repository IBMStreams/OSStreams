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

//----------------------------------------------------------------------------
// Title-
//     MemoryTracer.h
//
// Purpose-
//     Logging engine for saving trace messages before all other tracers
// (ConsoleTracer, RollingFileTracer, etc) available. This logging engine will save
// trace message to a memory buffer, so later tracer can grab and save into a
// file repository or console.
//     When Distillery applications (all daemon services) start up, the logging
// engine setting does not happen immediately, instead, it will up to some user
// input options, such as --Xdebug-file, etc. In this case, some logging messages
// before tracer set up may get lost. This is not essential before supporting
// RollingFileTracer, as we used to dependent on Linux routing engine for
// stdout/stderr, but becoming essential as we support RollingFileTracer, since
// it is up to DistilleryApplication for logging set up.
//     Note: As MemoryTracer using memory buffer to save log messages, this
// logging engine shall NOT be used as a final logging engine, instead, just be used
// as a temporary log saver.
//
//----------------------------------------------------------------------------

#ifndef MEMORY_TRACER_H__
#define MEMORY_TRACER_H__

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>
#include <sstream>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class MemoryTracer : public Tracer
{
  private:
    /// The buffer to keep log messages.
    std::stringstream _buf;

  public:
    MemoryTracer();

    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);

    virtual std::string getMessages(void);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif
