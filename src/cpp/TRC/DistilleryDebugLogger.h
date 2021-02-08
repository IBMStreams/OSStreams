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
//
// Title-
//       DistilleryDebugLogger.h
//
// Purpose-
//       Base Debugging Logger, Tracer interfaces
//
//----------------------------------------------------------------------------

#ifndef __STREAMS_DEBUG_LOGGER_H__
#define __STREAMS_DEBUG_LOGGER_H__

#include <TRC/TRCTypes.h>
#include <list>
#include <map>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

/// Streams base logger class
class Logger
{
  protected:
    int _level;          // log level
    std::string _format; // log message format
    // Flag for using internal mutex with the writeMessage.
    // If set false, end user need to use mutex manually for logging or tracing.
    bool _useAutoMutex;
    std::string _sslOption;

  public:
    Logger(int level);
    virtual ~Logger();

    void setUseAutoMutex(bool useAutoMutex);

    virtual void setSSLOption(const std::string& sslOption);

    /// Increase log level by 1
    virtual int increaseLevel(void);

    /// Get log level
    virtual int getLevel(void);

    /// Set log level
    /// Must hold mutex before call this method
    /// @param new_level new log level
    virtual int setLevel(int new_level);

    /// write log/trace message
    /// @param level log level
    /// @param aspect aspect related to this log message
    /// @param args arguments related to this message.
    ///        Currently, Streams support to put PEID, JobID, PortID as arguments.
    /// @param function function name
    /// @param file class file name
    /// @param line line number
    /// @param message message body.
    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message) = 0;

    /// write log message
    /// @param aspect aspect related to this log message
    /// @param key message key
    /// @param message message body.
    virtual void writeMessage(const std::string& aspect,
                              const std::string& key,
                              const std::vector<std::string>& substitutionTexts);
};

/// Streams base trace class.
class Tracer : public Logger
{
  public:
    Tracer(int level);
    virtual ~Tracer();

    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);

    /// write trace message
    /// @param message message body.
    virtual void writeMessage(const std::string& message);

    /// get trace message. Used with some special tracer.
    /// @return trace messages.
    virtual std::string getMessages(void);

    /// Remove trace file
    virtual void removeTraceFile();
};

// audit type.
static const std::string STREAMS_AUDIT_TYPE_STANDARD = "standard";
static const std::string STREAMS_AUDIT_TYPE_ADVANCED = "advanced";

/// Streams base audit logger class.
class AuditLogger : public Logger
{
  public:
    AuditLogger();
    virtual ~AuditLogger();

    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);

    /// write trace message
    /// @param message message body.
    virtual void writeMessage(const std::string& type,
                              bool success,
                              const std::string& instance,
                              const std::string& source,
                              const std::string& user,
                              const std::string& object,
                              const std::string& operation,
                              const std::string& msgId,
                              const std::vector<std::string>& msgvars,
                              const std::map<std::string, std::string>& opParams);
};

// Do not change following variables as they are used with SPCDBG macros.
// Change them will break backward compatibility.
// Keep logger_max_level as it is used with SPCDBG macro with prior releases.
extern int logger_max_level;
// for Streams trace.
extern int spc_trace_level;
// for application trace
extern int app_trace_level;
// for Streams and application log
extern int logger_level;

// For streams trace support
void set_streams_tracer(Tracer* l);
void set_streams_trclevel(int level);

// Following are for LOG support
void set_log_logger(Logger* l);
void set_log_level(int level);

// Following are for application trace support
void set_app_tracer(Tracer* l);
void set_app_trclevel(int level);

// Auditing support
extern int audit_level;
void set_audit_logger(AuditLogger* l);
void set_audit_level(int level);

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
