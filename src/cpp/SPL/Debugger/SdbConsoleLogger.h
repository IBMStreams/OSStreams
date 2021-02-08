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

#ifndef SPL_DEBUGGER_CONSOLE_LOGGER_H
#define SPL_DEBUGGER_CONSOLE_LOGGER_H

#include <TRC/ConsoleTracer.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE

namespace SPL {
class StandaloneClient;

/// Debugging Logger for the console in the SPL debugger environment
class SdbConsoleLogger : public ConsoleTracer
{
  public:
    /// Constructor
    /// @param level logging level
    SdbConsoleLogger(int level = 0);

    /// Output a line
    /// @param msg Message to output
    virtual void output(const std::string& msg);

    /// Set the logging format
    /// @param format short/long
    void setFormat(std::string format);

    /// Return the logging format
    /// @return format short/long
    std::string getFormat() const { return _formatName; }

    /// Set the level
    /// @param level new level
    void setLogLevel(int level);

    /// Set the standalone client to be used to print messages
    /// @param client Standalone client
    void setStandaloneClient(StandaloneClient* client) { _standaloneClient = client; }

  protected:
    StandaloneClient* _standaloneClient;
    std::string _formatName;
};

class SdbConsoleTracer : public SdbConsoleLogger
{
  public:
    /// Constructor
    /// @param level tracing level
    SdbConsoleTracer(int level = 0);

    /// Set the tracing format
    /// @param format short/long
    void setFormat(std::string format);

    /// Set the level
    /// @param level new level
    void setTraceLevel(int level);
};
}

#endif /* SPL_DEBUGGER_CONSOLE_LOGGER_H */
