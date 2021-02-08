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

#ifndef SPL_RUNTIME_FUNCTION_BUILTIN_UTILS_FUNCTIONS_H
#define SPL_RUNTIME_FUNCTION_BUILTIN_UTILS_FUNCTIONS_H

/*!
 * \file UtilFunctions.h \brief Definitions of C++ counterparts of %SPL native functions that
 * provide general utilities.
 */

#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Type/SPLType.h>
#include <errno.h>

/*
 * Builtin SPL Utility functions
 */
/// @splcppns SPL::Functions::Utility
/// @spldir utility
/// @splname SPL-Utility-Functions

/*!
 * \namespace SPL::Functions::Utility \brief  C++ counterparts of %SPL native
 * functions that provide general utilities
 */

namespace SPL {
namespace Functions {
namespace Utility {

/// @spldesc Return a named compiler invocation argument value (name=value).
/// @spldesc It is an error if the name is not specified on the compiler invocation.
/// @splparam name Desired argument name.
/// @splreturn String value of the named compiler argument.
/// @splnative public rstring getCompileTimeValue(rstring name)

/// @spldesc Return a named compiler invocation argument value (name=value).
/// @spldesc If the named argument is not found, use the default value.
/// @splparam name Desired argument name.
/// @splparam defaultValue Value to be used if the named argument is not found.
/// @splreturn String value of the named compiler argument if present, or default value.
/// @splnative public rstring getCompileTimeValue(rstring name, rstring defaultValue)

/// @spldesc Return the named compiler invocation argument list value (name="value0,value1").
/// @spldesc It is an error if the name is not specified on the compiler invocation.
/// @splparam name The name of the argument time value.
/// @splreturn The value as a list of rstrings.
/// @splnative public list<rstring> getCompileTimeListValue(rstring name)

/// @spldesc Return the current SPL file name as an `rstring`.
/// @splreturn File name of the current SPL file.
/// @splnative public rstring getThisFileName()

/// @spldesc Return the full pathname of the runtime location of the current SPL file as an `rstring`.
/// @splreturn Path name for the current SPL file.
/// @splnative public rstring getThisFilePath()

/// @spldesc Return the full pathname of the runtime location of the directory containing the current SPL file as an `rstring`.
/// @splreturn Path name of the directory containing the current SPL file.
/// @splnative public rstring getThisFileDir()

/// @spldesc Return the full path to the runtime location of the root directory of the toolkit containing the current SPL file.
/// @splreturn Path to the toolkit root directory.
/// @splnative public rstring getThisToolkitDir()

/// @spldesc Return the current line number as an `int32`.
/// @splreturn Line number for the current line in the SPL file.
/// @splnative public int32 getThisLine()

/// @spldesc Return the runtime location of the full path to the SPL application directory.
/// @splreturn Path of the application directory.
/// @splnative public rstring getApplicationDir()

/// @spldesc Return the name of the current composite instance. For example, this may be used to assure
/// @spldesc a unique name generation for parameters and files.
/// @splnote This function may not be used in the config clause of a composite or within a static type definition.
/// @splreturn A string containing the expanded composite instance name.
/// @splnative public rstring getThisCompositeInstanceName()

/// @spldesc Return the name of the main composite.
/// @splreturn A string containing the expanded main composite name.
/// @splnative public rstring getMainCompositeName()

/// @spldesc Return the named submission time value (name="value").
/// @spldesc This function can appear only in a composite operator scope.
/// @splparam name The name of the submission time value.
/// @splreturn The submission time value (submission fails if the argument is not present).
/// @splnative public rstring getSubmissionTimeValue(rstring name)

/// @spldesc Return the named submission time value (name="value").
/// @spldesc This function can appear only in a composite operator scope.
/// @splparam name The name of the submission time value.
/// @splparam default Value to be returned if the submission time value is not present.
/// @splreturn The submission time value, or the default value if it is not present.
/// @splnative public rstring getSubmissionTimeValue(rstring name, rstring default)

/// @spldesc Return the named submission time list value (name="value0,value1").
/// @spldesc This function can appear only in a composite operator scope.
/// @splparam name The name of the submission time value.
/// @splreturn The submission time value (submission fails if the argument is not present).
/// @splnative public list<rstring> getSubmissionTimeListValue(rstring name)

/// @spldesc Return the named submission time list value (name="value0,value1").
/// @spldesc This function can appear only in a composite operator scope.
/// @splparam name The name of the submission time value.
/// @splparam default Value to be returned if the submission time value is not present.
/// @splreturn The submission time value, or the default value if it is not present.
/// @splnative public list<rstring> getSubmissionTimeListValue(rstring name, list<rstring> default)

/// @spldesc Return the index corresponding to an input port.
/// @splparam inputStream Stream name, port name or alias.
/// @splreturn Index of the input port.
/// @splnative <tuple T> public int32 inputPort(T inputStream)

/// @spldesc Return the index corresponding to an output port.
/// @splparam outputStream Stream name or alias.
/// @splreturn Index of the output port.
/// @splnative <tuple T> public int32 outputPort(T outputStream)

#ifndef DOXYGEN_SKIP_FOR_USERS
/*
     * Assert/Abort/Log routines defined in the SPL Language Reference
     */
void realAssert(const std::string& msg, const std::string& file, uint32_t line);

#ifdef NDEBUG

#define Assert(...) NonAssert()
inline void NonAssert() {}

#else

/// Check assertion at runtime in non-optimized code.
/// @param condition  If `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @splcppfcn Assert
/// @splnative public void assert (boolean condition)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
inline void Assert(bool condition, const std::string& file, uint32_t line)
{
    if (!condition) {
        realAssert("Assertion failure", file, line);
    }
}

/// Check assertion at runtime in non-optimized code.
/// @param condition List of conditions. If any are `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @splcppfcn Assert
/// @splnative public void assert (list<boolean> condition)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
inline void Assert(const SPL::list<SPL::boolean>& condition, const std::string& file, uint32_t line)
{
    uint32_t i, n = condition.size();
    for (i = 0; i < n; i++) {
        if (!condition[i]) {
            std::ostringstream s;
            s.imbue(std::locale::classic());
            s << "Assertion failure at index " << i;
            realAssert(s.str(), file, line);
        }
    }
}

/// Check assertion at runtime in non-optimized code.
/// @param condition List of conditions. If any are `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @splcppfcn Assert
/// @splnative [N] public void assert (list<boolean>[N] condition)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
template<int32_t N>
inline void Assert(const SPL::blist<SPL::boolean, N>& condition,
                   const std::string& file,
                   uint32_t line)
{
    uint32_t i, n = condition.size();
    for (i = 0; i < n; i++) {
        if (!condition[i]) {
            std::ostringstream s;
            s.imbue(std::locale::classic());
            s << "Assertion failure at index " << i;
            realAssert(s.str(), file, line);
        }
    }
}

/// Check assertion at runtime in non-optimized code.
/// @param condition Map of conditions.  If any are `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @splcppfcn Assert
/// @splnative <any T> public void assert (map<T, boolean> condition)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
template<class K>
inline void Assert(const SPL::map<K, SPL::boolean>& condition,
                   const std::string& file,
                   uint32_t line)
{
    typename SPL::map<K, SPL::boolean>::const_iterator it = condition.begin();
    for (; it != condition.end(); it++) {
        if (!it->second) {
            std::ostringstream s;
            s.imbue(std::locale::classic());
            s << "Assertion failure at index " << it->first;
            realAssert(s.str(), file, line);
        }
    }
}

/// Check assertion at runtime in non-optimized code.
/// @param condition Map of conditions  if any are `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @splcppfcn Assert
/// @splnative <any T>[N] public void assert (map<T, boolean>[N] condition)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
template<class K, int32_t N>
inline void Assert(const SPL::bmap<K, SPL::boolean, N>& condition,
                   const std::string& file,
                   uint32_t line)
{
    typename SPL::bmap<K, SPL::boolean, N>::const_iterator it = condition.begin();
    for (; it != condition.end(); it++) {
        if (!it->second) {
            std::ostringstream s;
            s.imbue(std::locale::classic());
            s << "Assertion failure at index " << it->first;
            realAssert(s.str(), file, line);
        }
    }
}

/// Check assertion at runtime in non-optimized code (with a specified message).
/// @param condition  If `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @param msg Message to be printed.
/// @splcppfcn Assert
/// @splnative <string T> public void assert (boolean condition, T msg)
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
inline void Assert(bool condition, const std::string& msg, const std::string& file, uint32_t line)
{
    if (!condition) {
        realAssert(msg, file, line);
    }
}

/// Check assertion at runtime in non-optimized code (with a specified message).
/// @param condition  If `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @param msg Message to be printed.
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
inline void Assert(bool condition, const SPL::ustring& msg, const std::string& file, uint32_t line)
{
    if (!condition) {
        realAssert(SPL::ustring::toUTF8(msg), file, line);
    }
}

/// Check assertion at runtime in non-optimized code (with a specified message).
/// @param condition  If `false`, print a message to the standard output, `stdout`, log a message, and abort.
/// @param msg Message to be printed.
/// @param file SPL filename at `assert` call.
/// @param line SPL line number at `assert` call.
inline void Assert(bool condition, const SPL::BString& msg, const std::string& file, uint32_t line)
{
    if (!condition) {
        realAssert(msg.getCString(), file, line);
    }
}
#endif

SPL::rstring getToolkitDirectory(const SPL::rstring& toolkitName);

#endif /* DOXYGEN_SKIP_FOR_USERS */

/// Abort program execution.
/// @splnative public stateful void abort()
/// @param file SPL filename at `abort` call.
/// @param line SPL line number at `abort` call.
inline void abort(const std::string& file, uint32_t line)
{
    realAssert("Abort", file, line);
}

/// Determine whether a message at the given level would be traced.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @return The value `true` if a message at the given traceLevel would be traced.
/// @splnative public boolean isTraceable(enum {error, warn, info, debug, trace} traceLevel)
SPL::boolean isTraceable(const Enum& traceLevel);

/// Log a message in the tracing system.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @splnative <string T> public void appTrc(enum {error, warn, info, debug, trace} traceLevel, T message, T aspect)
/// @param file SPL filename at `appTrc` call.
/// @param line SPL line number at `appTrc` call.
void appTrc(const Enum& traceLevel,
            const std::string& message,
            const std::string& aspect,
            const std::string& file,
            uint32_t line);

/// Log a message in the tracing system.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `appTrc` call.
/// @param line SPL line number at `appTrc` call.
inline void appTrc(const Enum& traceLevel,
                   const SPL::ustring& message,
                   const SPL::ustring& aspect,
                   const std::string& file,
                   uint32_t line)
{
    appTrc(traceLevel, SPL::ustring::toUTF8(message), SPL::ustring::toUTF8(aspect), file, line);
}

/// Log a message in the tracing system.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `appTrc` call.
/// @param line SPL line number at `appTrc` call.
inline void appTrc(const Enum& traceLevel,
                   const SPL::BString& message,
                   const SPL::BString& aspect,
                   const std::string& file,
                   uint32_t line)
{
    appTrc(traceLevel, std::string(message.getCString()), std::string(aspect.getCString()), file,
           line);
}

/// Log a message in the tracing system.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @param message Message to be logged.
/// @splnative <string T> public void appTrc(enum {error, warn, info, debug, trace} traceLevel, T message)
/// @param file SPL filename at `appTrc` call.
/// @param line SPL line number at `appTrc` call.
inline void appTrc(const Enum& traceLevel,
                   const SPL::rstring& message,
                   const std::string& file,
                   uint32_t line)
{
    appTrc(traceLevel, message, "", file, line);
}

/// Log a message in the tracing system.
/// @param traceLevel Classification of the trace level (as `error`, `warn`, `info`, `debug` or `trace`).  Use the Level enumeration in the `spl::Trace` to specify the trace level.
/// @param message Message to be logged.
/// @param file SPL filename at `appTrc` call.
/// @param line SPL line number at `appTrc` call.
inline void appTrc(const Enum& traceLevel,
                   const SPL::ustring& message,
                   const std::string& file,
                   uint32_t line)
{
    appTrc(traceLevel, SPL::ustring::toUTF8(message), "", file, line);
}

/// Determine whether a message at the given level would be logged.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @return The value `true` if a message at the given logLevel would be logged.
/// @splnative public boolean isLoggable(enum {error, warn, info} logLevel)
SPL::boolean isLoggable(const Enum& logLevel);

/// Log a message in the logging system.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @splnative <string T> public void appLog(enum {error, warn, info} logLevel, T message, T aspect)
/// @param file SPL filename at `appLog` call.
/// @param line SPL line number at `appLog` call.
void appLog(const Enum& logLevel,
            const std::string& message,
            const std::string& aspect,
            const std::string& file,
            uint32_t line);

/// Log a message in the logging system.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `appLog` call.
/// @param line SPL line number at `appLog` call.
inline void appLog(const Enum& logLevel,
                   const SPL::ustring& message,
                   const SPL::ustring& aspect,
                   const std::string& file,
                   uint32_t line)
{
    appLog(logLevel, SPL::ustring::toUTF8(message), SPL::ustring::toUTF8(aspect), file, line);
}

/// Log a message in the logging system.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `appLog` call.
/// @param line SPL line number at `appLog` call.
inline void appLog(const Enum& logLevel,
                   const SPL::BString& message,
                   const SPL::BString& aspect,
                   const std::string& file,
                   uint32_t line)
{
    appLog(logLevel, std::string(message.getCString()), std::string(aspect.getCString()), file,
           line);
}

/// Log a message in the logging system.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @param message Message to be logged.
/// @splnative <string T> public void appLog(enum {error, warn, info} logLevel, T message)
/// @param file SPL filename at `appLog` call.
/// @param line SPL line number at `appLog` call.
inline void appLog(const Enum& logLevel,
                   const SPL::rstring& message,
                   const std::string& file,
                   uint32_t line)
{
    appLog(logLevel, message, "", file, line);
}

/// Log a message in the logging system.
/// @param logLevel Classification of the log level (as `error`, `warn`, or `info`).  Use the Level enumeration in the `spl::Log` to specify the log level.
/// @param message Message to be logged.
/// @param file SPL filename at `appLog` call.
/// @param line SPL line number at `appLog` call.
inline void appLog(const Enum& logLevel,
                   const SPL::ustring& message,
                   const std::string& file,
                   uint32_t line)
{
    appLog(logLevel, SPL::ustring::toUTF8(message), "", file, line);
}

/// Log a message in the tracing system.
/// @note Deprecated - use `appTrc` instead.
/// @param logLevel Classification of the log level (as `error`, `info`, `debug`, or `trace`).
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @splnative <string T> public void log(enum {error, info, debug, trace} logLevel, T message, T aspect)
/// @param file SPL filename at `log` call.
/// @param line SPL line number at `log` call.
void log(const Enum& logLevel,
         const std::string& message,
         const std::string& aspect,
         const std::string& file,
         uint32_t line);

/// Log a message in the tracing system.
/// @note Deprecated - use `appTrc` instead.
/// @param logLevel Classification of the log level (as `error`, `info`, `debug`, or `trace`).
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `log` call.
/// @param line SPL line number at `log` call.
inline void log(const Enum& logLevel,
                const SPL::ustring& message,
                const SPL::ustring& aspect,
                const std::string& file,
                uint32_t line)
{
    log(logLevel, SPL::ustring::toUTF8(message), SPL::ustring::toUTF8(aspect), file, line);
}

/// Log a message in the tracing system.
/// @note Deprecated - use `appTrc` instead.
/// @param logLevel Classification of the log level (as `error`, `info`, `debug`, or `trace`).
/// @param message Message to be logged.
/// @param aspect Message category (can be used for filtering when inspecting messages).
/// @param file SPL filename at `log` call.
/// @param line SPL line number at `log` call.
inline void log(const Enum& logLevel,
                const SPL::BString& message,
                const SPL::BString& aspect,
                const std::string& file,
                uint32_t line)
{
    log(logLevel, std::string(message.getCString()), std::string(aspect.getCString()), file, line);
}

/// Log a message in the tracing system.
/// @note Deprecated - use `appTrc` instead.
/// @param logLevel Classification of the log level (as `error`, `info`, `debug`, or `trace`).
/// @param message Message to be logged.
/// @splnative <string T> public void log(enum {error, info, debug, trace} logLevel, T message)
/// @param file SPL filename at `log` call.
/// @param line SPL line number at `log` call.
inline void log(const Enum& logLevel,
                const SPL::rstring& message,
                const std::string& file,
                uint32_t line)
{
    log(logLevel, message, "", file, line);
}

/// Log a message in the tracing system.
/// @note Deprecated - use `appTrc` instead.
/// @param logLevel Classification of the log level (as `error`, `info`, `debug`, or `trace`).
/// @param message Message to be logged.
/// @param file SPL filename at `log` call.
/// @param line SPL line number at `log` call.
inline void log(const Enum& logLevel,
                const SPL::ustring& message,
                const std::string& file,
                uint32_t line)
{
    log(logLevel, SPL::ustring::toUTF8(message), "", file, line);
}

/// Shutdown the current processing element (PE).
/// @splnative public stateful void shutdownPE ()
void shutdownPE();

/// Check if shutdown is requested.
/// @return The value `true` if the system is shutting down.
/// @splnative public stateful boolean isShutdown()
SPL::boolean isShutdown();

/// Block until shutdown is requested, or the specified amount of time has
/// elapsed, whichever comes first.
/// @param time Maximum wait time in seconds (fractional values are allowed).
/// @splnative public stateful void block(float64 time)
void block(double const time);

/// Get the parallel channel index. Note that in the context of a nested parallel region the range of indexes considers
/// replicated nested regions.
/// @return The index of the parallel channel.
/// @splnative public int32 getChannel()
SPL::int32 getChannel();

/// Get the parallel channel index. Note that in the context of nested parallel regions the returned index does not consider
/// replicated nested regions.
/// @return The index of the parallel channel.
/// @splnative public int32 getLocalChannel()
SPL::int32 getLocalChannel();

/// Get the parallel channel indexes for all nested parallel regions.
/// @return The indexes of the nested parallel channels.
/// @splnative public list<int32> getAllChannels()
SPL::list<SPL::int32> getAllChannels();

/// Get the maximum number of parallel channels. Note that in the context of a nested parallel region the range of indexes considers
/// replicated nested regions.
/// @return The maximum number of parallel channels.
/// @splnative public int32 getMaxChannels()
SPL::int32 getMaxChannels();

/// Get the maximum number of parallel channels. Note that in the context of nested parallel regions the returned index does not consider
/// replicated nested regions.
/// @return The maximum number of parallel channels.
/// @splnative public int32 getLocalMaxChannels()
SPL::int32 getLocalMaxChannels();

/// Get the maximum number of parallel channels for all nested parallel regions.
/// @return The maximum number of parallel channels for all nested parallel regions.
/// @splnative public list<int32> getAllMaxChannels()
SPL::list<SPL::int32> getAllMaxChannels();

/// Get the physical name of the operator.
/// @return The physical name of the operator.
/// @splnative public rstring getThisOperatorName()
SPL::rstring getThisOperatorName();

/// Get the logical name of the operator.
/// @return The logical name of the operator.
/// @splnative public rstring getThisOperatorLogicalName()
SPL::rstring getThisOperatorLogicalName();

/// Get the value of an environment variable.
/// @param name Environment variable name.
/// @return The value for the environment variable, or an empty string if the environment variable is not set.
/// @splnative public stateful rstring getEnvironmentVariable(rstring name)
SPL::rstring getEnvironmentVariable(SPL::rstring const& name);

/// Get the value of an environment variable.
/// @param name Environment variable name.
/// @param defaultValue Default value for the environment variable.
/// @return The value for the environment variable if set, otherwise the default value.
/// @splnative public stateful rstring getEnvironmentVariable(rstring name, rstring defaultValue)
SPL::rstring getEnvironmentVariable(SPL::rstring const& name, SPL::rstring const& defaultValue);

/// Get the value of an environment variable.
/// @param name Environment variable name.
/// @param isSet This parameter is set to `true` if the environment variable
/// is found, otherwise to `false`. This parameter can be used to differentiate between an empty
/// value and an unset variable.
/// @return The value for the environment variable, or an empty string if the environment variable is not set.
/// @splnative public stateful rstring getEnvironmentVariable(rstring name, mutable boolean isSet)
SPL::rstring getEnvironmentVariable(SPL::rstring const& name, SPL::boolean& isSet);

/*
     * Metrics access
     */

/// Get the specified input port metric value for the current operator.
/// @param port Input port index.
/// @param name The name of the metric.
/// @param value Value of the specified input port metric for the current operator.
/// @throws SPLRuntimeInvalidIndexException If the port index is out of bounds.
/// @splnative public stateful void getInputPortMetricValue(uint32 port, enum {nTuplesProcessed, nTuplesDropped, nTuplesQueued, nWindowPunctsProcessed, nFinalPunctsProcessed, nWindowPunctsQueued, nFinalPunctsQueued, queueSize, maxItemsQueued, recentMaxItemsQueued, recentMaxItemsQueuedInterval, nEnqueueWaits} name, mutable int64 value)
void getInputPortMetricValue(SPL::uint32 port, int name, SPL::int64& value);
inline void getInputPortMetricValue(SPL::uint32 port, const SPL::Enum& name, SPL::int64& value)
{
    getInputPortMetricValue(port, (int)name.getIndex(), value);
}

/// Get the specified  output port metric value for the current operator.
/// @param port Input port index.
/// @param name The name of the metric.
/// @param value Value of the specified input port metric for the current operator.
/// @throws SPLRuntimeInvalidIndexException If the port index is out of bounds.
/// @splnative public stateful void getOutputPortMetricValue(uint32 port, enum {nTuplesSubmitted, nWindowPunctsSubmitted, nFinalPunctsSubmitted} name, mutable int64 value)
void getOutputPortMetricValue(SPL::uint32 port, int name, SPL::int64& value);
inline void getOutputPortMetricValue(SPL::uint32 port, const SPL::Enum& name, SPL::int64& value)
{
    getOutputPortMetricValue(port, (int)name.getIndex(), value);
}

/// Get the specified system  metric value for the current operator.
/// @param name The name of the metric.
/// @param value Value of the specified system metric for the current operator.
/// @splnative public stateful void getOperatorMetricValue(enum {relativeOperatorCost} name, mutable int64 value)
void getOperatorMetricValue(int name, SPL::int64& value);
inline void getOperatorMetricValue(const SPL::Enum& name, SPL::int64& value)
{
    getOperatorMetricValue((int)name.getIndex(), value);
}

/// Get the specified custom metric value for the current operator.
/// @param name The name of the metric.
/// @param value Value of the specified custom metric for the current operator.
/// @throws SPLRuntimeInvalidMetricException If the metric does not exist.
/// @splnative public stateful void getCustomMetricValue(rstring name, mutable int64 value)
void getCustomMetricValue(SPL::rstring const& name, SPL::int64& value);

/// Get the specified custom metric value for the current operator.
/// @param name The name of the metric.
/// @param value Value of the specified custom metric for the current operator.
/// @throws SPLRuntimeInvalidMetricException If the metric does not exist.
/// @splnative public stateful void getCustomMetricValue(ustring name, mutable int64 value)
inline void getCustomMetricValue(SPL::ustring const& name, SPL::int64& value)
{
    return getCustomMetricValue(SPL::ustring::toUTF8(name), value);
}

/// Set the specified custom metric value for the current operator.
/// @param name The name of the metric.
/// @param value Value to use.
/// @throws SPLRuntimeInvalidMetricException If the metric does not exist.
/// @splnative public stateful void setCustomMetricValue(rstring name, int64 value)
void setCustomMetricValue(SPL::rstring const& name, SPL::int64 value);

/// Set the specified custom metric value for the current operator.
/// @param name The name of the metric.
/// @param value Value to use.
/// @throws SPLRuntimeInvalidMetricException If the metric does not exist.
/// @splnative public stateful void setCustomMetricValue(ustring name, int64 value)
inline void setCustomMetricValue(SPL::ustring const& name, SPL::int64 value)
{
    setCustomMetricValue(SPL::ustring::toUTF8(name), value);
}

/// Create the specified custom metric for the current operator and set its initial value.
/// If Metric name already exists, does nothing starting with IBM Streams v4.3.0 and throws
/// SPLRuntimeInvalidMetricException for versions prior to 4.3.0.
/// @param name The name of the metric.
/// @param value The initial value for the metric.
/// @param description A description of the metric.
/// @splparam kind The kind of the metric (value of `enum{Gauge,Counter,Time}`).
/// @cppparam kind The kind of the metric (Metric::Gauge, Metric::Counter, or Metric::Time).
/// @throws SPLRuntimeInvalidMetricException If the metric already exists (prior to IBM Streams v4.3.0).
/// @splnative public stateful void createCustomMetric(rstring name, rstring description, enum{Gauge,Counter,Time} kind, int64 value)
void createCustomMetric(SPL::rstring const& name,
                        SPL::rstring const& description,
                        const Enum& kind,
                        SPL::int64 value);

/// Create the specified custom metric for the current operator and set its initial value.
/// If Metric name already exists, does nothing starting with IBM Streams v4.3.0 and throws
/// SPLRuntimeInvalidMetricException for versions prior to 4.3.0.
/// @param name The name of the metric.
/// @param value The initial value for the metric.
/// @param description A description of the metric.
/// @splparam kind The kind of the metric (value of `enum{Gauge,Counter,Time}`).
/// @cppparam kind The kind of the metric (Metric::Gauge, Metric::Counter, or Metric::Time).
/// @throws SPLRuntimeInvalidMetricException If the metric already exists (prior to IBM Streams v4.3.0).
/// @splnative public stateful void createCustomMetric(ustring name, ustring description, enum{Gauge,Counter,Time} kind, int64 value)
inline void createCustomMetric(SPL::ustring const& name,
                               SPL::ustring const& description,
                               const Enum& kind,
                               SPL::int64 value)
{
    createCustomMetric(SPL::ustring::toUTF8(name), SPL::ustring::toUTF8(description), kind, value);
}

/// Check if the specified custom metric exists.
/// @param name The name of the metric.
/// @return The value `true` if the custom metric exists, otherwise `false`.
/// @splnative public stateful boolean hasCustomMetric(rstring name)
bool hasCustomMetric(SPL::rstring const& name);

/// Check if the specified custom metric exists.
/// @param name The name of the metric.
/// @return The value `true` if the custom metric exists, otherwise `false`.
/// @splnative public stateful boolean hasCustomMetric(ustring name)
inline bool hasCustomMetric(SPL::ustring const& name)
{
    return hasCustomMetric(SPL::ustring::toUTF8(name));
}

/// Return the names of the custom metrics for the current operator.
/// @return List of names of custom metrics.
/// @splnative public stateful list<rstring> getCustomMetricNames()
SPL::list<SPL::rstring> getCustomMetricNames();

/*
     * General Utility Functions
     */

/// Format an integer using locale specific format.
/// The number is returned as a string with the locale specific grouping.
/// @param value Value to be formatted.
/// @return The input value as a string using locale specific number format.
/// @splnative public rstring formatNumber(int64 value)
SPL::rstring formatNumber(const SPL::int64 value);

/// Format an integer using locale specific format.
/// The number is returned as a string with the locale specific grouping.
/// @param value Value to be formatted.
/// @param width Minimal field width.
/// @return The input value as a string using locale specific number format.
/// @splnative public rstring formatNumber(int64 value, uint32 width)
SPL::rstring formatNumber(const SPL::int64 value, SPL::uint32 width);

/// Format a float using locale specific format.
/// The number is returned as a string with the locale specific grouping and
/// decimal point.
/// @param value Value to be formatted.
/// @return The input value as a string using locale specific number format.
/// @splnative public rstring formatNumber(float64 value)
SPL::rstring formatNumber(const SPL::float64 value);

/// Format a float using locale specific format.
/// The number is returned as a string with the locale specific grouping and
/// decimal point.
/// If scientific form is specified, there is one digit before the decimal point
/// and a two-digit exponent following the letter 'e'.
/// @param value Value to be formatted.
/// @param width Minimal field width.
/// @param precision Number of digits of precision.
/// @param scientific Set to `true` if scientific form should be used.
/// @return The input value as a string using locale specific number format.
/// @splnative public rstring formatNumber(float64 value, uint32 width, uint32 precision, boolean scientific)
SPL::rstring formatNumber(const SPL::float64 value,
                          SPL::uint32 width,
                          SPL::uint32 precision,
                          bool scientific);

/// Parse a string that represents a number of the specified base, using the current locale.
/// @param text String to parse.
/// @param number Variable to store the result.
/// @param base Base to use for conversion.
/// Set to 0, or a number between 2 and 36 inclusive.
/// If set to 0, the leading characters will determine the base.
/// See the POSIX `strtoll` routine for more details.
/// @return The number of bytes read on success, -1 otherwise.
/// @splnative public int32 parseNumber(mutable int64 number, rstring text, int32 base)
SPL::int32 parseNumber(SPL::int64& number, SPL::rstring const& text, SPL::int32 base);

/// Parse a string that represents a base 10 number, using the current locale.
/// @param text String to parse.
/// @param number Variable to store the result.
/// @return The number of bytes read on success, -1 otherwise.
/// @splnative public int32 parseNumber(mutable int64 number, rstring text)
SPL::int32 parseNumber(SPL::int64& number, SPL::rstring const& text);

/// Parse a string that represents a number, using the current locale.
/// @param text String to parse.
/// @param number Variable to store the result.
/// @return The number of bytes read on success, -1 otherwise.
/// @splnative public int32 parseNumber(mutable int64 number, ustring text)
SPL::int32 parseNumber(SPL::int64& number, SPL::ustring const& text);

/// Parse a string that represents a number, using the current locale.
/// @note This function is implemented using ICU and does not accept 'e' as the exponent character.  Use 'E' instead (e.g. 1.05E+3).
/// @param text String to parse.
/// @param number Variable to store the result.
/// @return The number of bytes read on success, -1 otherwise.
/// @splnative public int32 parseNumber(mutable float64 number, rstring text)
SPL::int32 parseNumber(SPL::float64& number, SPL::rstring const& text);

/// Parse a string that represents a number, using the current locale.
/// @note This function is implemented using ICU and does not accept 'e' as the exponent character.  Use 'E' instead (e.g. 1.05E+3).
/// @param text String to parse.
/// @param number Variable to store the result.
/// @return The number of bytes read on success, -1 otherwise.
/// @splnative public int32 parseNumber(mutable float64 number, ustring text)
SPL::int32 parseNumber(SPL::float64& number, SPL::ustring const& text);

/// Convert an `errno` value to a string for intelligent printing.
/// @param errnum Error number to convert to a string description.
/// @return String description of errnum.
/// @splnative public rstring strerror(int32 errnum)
SPL::rstring strerror(const SPL::int32 errnum);

/*
     * Conversion from string to [u]int64 for various bases
     */

/// Convert a string in base 2 to 36 to an unsigned number.
/// @param value String to convert.
/// @param base Base to use for conversion.
/// Set to 0, or a number between 2 and 36 inclusive.
/// If set to 0, the leading characters will determine the base.
/// See the POSIX `strtoll` routine for more details.
/// @return String converted to a uint64.
/// @splnative public uint64 strtoull (rstring value, int32 base)
/// @spleval $name = "strtoull"; $ret = 'uint64'; $numArgs = 2; $arg[0] = 'rstring'; $arg[1] = 'int32';
inline SPL::uint64 strtoull(const SPL::rstring& value, SPL::int32 base)
{
    return ::strtoull(value.c_str(), NULL, base);
}

/// Convert a string in base 2 to 36 to an unsigned number.
/// @param value String to convert.
/// @param base Base to use for conversion.
/// Set to 0, or a number between 2 and 36 inclusive.
/// If set to 0, the leading characters will determine the base.
/// See the POSIX `strtoll` routine for more details.
/// @param numCharsProcessed Number of characters consumed from `value` during the conversion, or -1 if the conversion failed (invalid `base` or result out of range).
/// @return String converted to a uint64.
/// @splnative public uint64 strtoull (rstring value, int32 base, mutable int32 numCharsProcessed)
inline SPL::uint64 strtoull(const SPL::rstring& value,
                            SPL::int32 base,
                            SPL::int32& numCharsProcessed)
{
    char* endPtr;
    errno = 0;
    SPL::uint64 r = ::strtoull(value.c_str(), &endPtr, base);
    numCharsProcessed = errno ? -1 : endPtr - const_cast<char*>(value.c_str());
    return r;
}

/// Convert a string in base 2 to 36 to an signed number.
/// @param value String to convert.
/// @param base Base to use for conversion.
/// Set to 0, or a number between 2 and 36 inclusive.
/// If set to 0, the leading characters will determine the base.
/// See the POSIX `strtoll` routine for more details.
/// @return String converted to an `int64`.
/// @splnative public int64 strtoll (rstring value, int32 base)
/// @spleval $name = "strtoll"; $ret = 'int64'; $numArgs = 2; $arg[0] = 'rstring'; $arg[1] = 'int32';
inline SPL::int64 strtoll(const SPL::rstring& value, SPL::int32 base)
{
    return ::strtoll(value.c_str(), NULL, base);
}

/// Convert a string in base 2 to 36 to an signed number.
/// @param value String to convert.
/// @param base Base to use for conversion.
/// Set to 0, or a number between 2 and 36 inclusive.
/// If set to 0, the leading characters will determine the base.
/// See the POSIX `strtoll` routine for more details.
/// @param numCharsProcessed Number of characters consumed from `value` during the conversion, or -1 if the conversion failed (invalid `base` or result out of range).
/// @return String converted to a int64.
/// @splnative public int64 strtoll (rstring value, int32 base, mutable int32 numCharsProcessed)
inline SPL::int64 strtoll(const SPL::rstring& value, SPL::int32 base, SPL::int32& numCharsProcessed)
{
    char* endPtr;
    errno = 0;
    SPL::int64 r = ::strtoll(value.c_str(), &endPtr, base);
    numCharsProcessed = errno ? -1 : endPtr - const_cast<char*>(value.c_str());
    return r;
}

/// Convert a string to a floating point number.  See the POSIX `strtod` routine for details.
/// @param value String to convert.
/// @return String converted to a float64.
/// @splnative public float64 strtod (rstring value)
/// @spleval $name = "strtod"; $ret = 'float64'; $numArgs = 1; $arg[0] = 'rstring';
inline SPL::float64 strtod(const SPL::rstring& value)
{
    return ::strtod(value.c_str(), NULL);
}

/// Convert a string to a floating point number.  See the POSIX `strtod` routine for details.
/// @param value String to convert.
/// @param numCharsProcessed Number of characters consumed from `value` during the conversion, or -1 if the conversion failed (result out of range).
/// @return String converted to a float64.
/// @splnative public float64 strtod (rstring value, mutable int32 numCharsProcessed)
inline SPL::float64 strtod(const SPL::rstring& value, SPL::int32& numCharsProcessed)
{
    char* endPtr;
    errno = 0;
    SPL::float64 r = ::strtod(value.c_str(), &endPtr);
    numCharsProcessed = errno ? -1 : endPtr - const_cast<char*>(value.c_str());
    return r;
}

/*
 * Some printing functions (for debugging)
 */

/// Write any SPL type to the standard output `stdout` as an SPL literal. For example, strings are
/// quoted, and special characters are escaped. If you want to print strings
/// without using the string literal format, use the `printString` function
/// instead. The output is not flushed.
/// @param t Value to print.
/// @splnative <any T> public void print (T t)
template<class T>
inline void print(const T& t)
{
    std::ostringstream ostr;
    ostr.imbue(std::locale::classic());
    SPL::serializeWithPrecision(ostr, t);
    std::cout << ostr.str();
}

/// Write a string to the standard output, `stdout`, followed by a newline, then flush the output.
/// @param s String to print.
/// @splnative <string T> public void printStringLn (T s)
inline void printStringLn(const SPL::rstring& s)
{
    std::cout << static_cast<const std::string&>(s) << std::endl;
}

/// Write a string to the standard output, `stdout`, followed by a newline, then flush the output.
/// @param s String to print.
inline void printStringLn(const SPL::BString& s)
{
    std::cout << s.getCString() << std::endl;
}

/// Write a string to the standard output, `stdout`, followed by a newline, then flush the output.
/// @param s String to print.
inline void printStringLn(const SPL::ustring& s)
{
    printStringLn(SPL::ustring::toUTF8(s));
}

/// Write a string to the standard output, `stdout`.  The output is not flushed.
/// @param s String to print.
/// @splnative <string T> public void printString (T s)
inline void printString(const SPL::rstring& s)
{
    std::cout << static_cast<const std::string&>(s);
}

/// Write a string to the standard output, `stdout`. The output is not flushed.
/// @param s String to print.
inline void printString(const SPL::BString& s)
{
    print(s.getCString());
}

/// Write a string to the standard output, `stdout`. The output is not flushed.
/// @param s String to print.
inline void printString(const SPL::ustring& s)
{
    printString(SPL::ustring::toUTF8(s));
}

/// Write any SPL type to the standard output, `stdout`, in serialized form, followed by a newline.
/// For example, strings are quoted, and special characters are escaped.
/// The output is then flushed.
/// @param t Value to print.
/// @splnative <any T> public void println (T t)
template<class T>
inline void println(const T& t)
{
    print(t);
    std::cout << std::endl;
}

/// Flush all `stdio` output streams. This will ensure that `printString` and other print output is flushed and written to
/// the standard output, `stdout`.
/// @splnative public void flushStdio()
inline void flushStdio()
{
    fflush(NULL);
}

/// Assign matching attributes of a tuple from another one.
/// An attribute matches if the name is the same,
/// and the type is either the same,
/// or the type to be copied from is `T`
/// and the type to be copied to is `optional<T>`.
/// @param lhs Destination tuple.
/// @param rhs Source tuple.
/// @splnative <tuple T1, tuple T2> public void assignFrom (mutable T1 lhs, T2 rhs)
inline void assignFrom(SPL::Tuple& lhs, SPL::Tuple const& rhs)
{
    lhs.assignFrom(rhs, false);
}

// Hashing functions

/// Compute the hash of a value.
///
/// The hash function is designed to return a fixed output value for a given input value,
/// and to reduce the likelihood of returning the same output value for different input values.
/// The hash function can be used to map a set of input values to a smaller range of output values,
/// for example, by performing a modulo operation on the returned hash value and the size of the desired range.
/// In cases where there may be patterns in the input values (such as all even numbers),
/// a more even distribution of output values may be possible by using a prime number as the modulus.
///
/// The current hash function uses the C++ standard library `std::tr1::hash` function.
///
/// @param v Value.
/// @return Hash of the value.
/// @splnative <any T> public uint64 hashCode (T v)
/// @spleval $name = "hashCode"; $ret = "uint64"; $numArgs = 1; $arg[0] = '@primitive@'
template<class T>
inline SPL::uint64 hashCode(const T& v)
{
    return static_cast<SPL::uint64>(std::tr1::hash<T>()(v));
}

/// Compute hashes of a list of values, using the same hashing method as hashCode().
/// @param l List of values.
/// @return List of the hashes of the values.
/// @splnative <list L> public list<uint64> hashCodeForEach (L l)
/// @spleval $name = "hashCodeForEach"; $ret = 'list<uint64>'; $numArgs = 1; $arg[0] = 'list<@primitive@>'
template<class T>
inline SPL::list<SPL::uint64> hashCodeForEach(const SPL::list<T>& l)
{
    SPL::list<SPL::uint64> res;
    res.reserve(l.size());
    std::tr1::hash<T> hasher;
    for (size_t i = 0, iu = l.size(); i < iu; ++i) {
        res.push_back(static_cast<SPL::uint64>(hasher(l[i])));
    }
    return res;
}

/// Compute hashes of a list of values, using the same hashing method as hashCode().
/// @param l List of values.
/// @return List of the hashes of the values.
template<class T, int32_t N>
inline SPL::list<SPL::uint64> hashCodeForEach(const SPL::blist<T, N>& l)
{
    SPL::list<SPL::uint64> res;
    res.reserve(l.size());
    std::tr1::hash<T> hasher;
    for (size_t i = 0, iu = l.size(); i < iu; ++i) {
        res.push_back(static_cast<SPL::uint64>(hasher(l[i])));
    }
    return res;
}

/// Miscellaneous

/// Return the absolute pathname to the runtime location of the data directory for the SPL application.
/// To use this function you must define a data directory for the application.
/// @return Data directory (an absolute path).
/// @splnative public rstring dataDirectory()
inline SPL::rstring dataDirectory()
{
    return SPL::ProcessingElement::pe().getDataDirectory();
}

/// Swap any two SPL values
/// @param s1 Value to swap.
/// @param s2 Value to swap.
/// @splnative <any T> public void swap (mutable T s1, mutable T s2)
template<class T>
void swap(T& s1, T& s2)
{
    std::swap(s1, s2);
}

/// Return the name of a temporary file created using a template.  The POSIX `mkstemp` function is used
/// to create the file, and the file is closed.
/// @param templat A pathname for the temporary file.  The last six characters of `templat` must be "XXXXXX",
/// and those characters will be replaced by a string to make the name unique.
/// @return The generated temporary file name, or the empty string if there were errors.   Note that since the
/// file is closed before returning, there is a brief window that another process might use to create the
/// same file.
/// @splnative public stateful rstring mktemp(rstring templat)
SPL::rstring mktemp(const SPL::rstring& templat);

/// Return the Job ID for the current job that this operator is executing in.
/// @return The job ID for the current job.
/// @splnative public uint64 jobID()
inline SPL::uint64 jobID()
{
    return SPL::ProcessingElement::pe().getJobId();
}

/// Return the name of the current job that this operator is executing in.
/// @return The name of the current job.
/// @splnative public rstring getJobName()
inline SPL::rstring getJobName()
{
    return SPL::ProcessingElement::pe().getJobName();
}

/// Return the name of the job group for the current job that this operator is executing in.
/// @return The name of the job group.
/// @splnative public rstring getJobGroup()
inline SPL::rstring getJobGroup()
{
    return SPL::ProcessingElement::pe().getJobGroup();
}

/// Return the name of the application that is executing this operator.
/// @return The name of the application.
/// @splnative public rstring getApplicationName()
inline SPL::rstring getApplicationName()
{
    return SPL::ProcessingElement::pe().getApplicationName();
}

/// Return the PE ID for the current processing element that this operator is executing in.
/// @return The PE ID for the current processing element.
/// @splnative public uint64 PEID()
inline SPL::uint64 PEID()
{
    return SPL::ProcessingElement::pe().getPEId();
}

/// Return the Streams instance ID that the PE is running in.
/// @return The Streams instance ID for the current processing element (or the username if running in standalone mode).
/// @splnative public rstring instanceID()
inline SPL::rstring instanceID()
{
    return SPL::ProcessingElement::pe().getInstanceID();
}

/// Return the domain ID for the domain that the PE is running in.
/// @return The domain ID for the current processing element, or, if running in standalone mode, the name of the user running the PE.
/// @splnative public rstring domainID()
inline SPL::rstring domainID()
{
    return SPL::ProcessingElement::pe().getDomainID();
}

/// Return the number of times the PE that the operator is executing in has been relaunched.
/// @return The relaunch count of the PE.
/// @splnative public uint32 getRelaunchCount()
inline SPL::uint32 getRelaunchCount()
{
    return SPL::ProcessingElement::pe().getRelaunchCount();
}

/// Check if the input port is a control port.
/// @param port Input port index.
/// @return The value `true` if the port is a control port, `false` otherwise.
/// @splnative public boolean isControlInputPort(uint32 port)
SPL::boolean isControlInputPort(uint32 port);

/// Check if the input port is consistent. An input port is consistent if
/// all its input streams are produced by operators that belong to the same
/// consistent region.
/// @param port Input port index.
/// @return The value `true` if the input port is consistent, `false` otherwise.
/// @splnative public boolean isConsistentInputPort(uint32 port)
SPL::boolean isConsistentInputPort(uint32 port);

/// Check if the operator is in a consistent region.
/// @return The value `true` if the operator is in a consistent region, `false` otherwise.
/// @splnative public boolean isInConsistentRegion()
SPL::boolean isInConsistentRegion();

/// Check if the operator is a start operator of a consistent region.
/// @return The value `true` if the operator is a start operator of a consistent region, `false` otherwise.
/// @splnative public boolean isStartOfConsistentRegion()
SPL::boolean isStartOfConsistentRegion();

/// Check if the operator is an end operator of a consistent region.
/// @return The value `true` if the operator is an end operator in a consistent region, `false` otherwise.
/// @splnative public boolean isEndOfConsistentRegion()
SPL::boolean isEndOfConsistentRegion();

/// Check if the operator is the trigger operator of an operator-driven consistent region.
/// @return The value `true` if the operator is the trigger operator of a consistent region, `false` otherwise.
/// @splnative public boolean isTriggerOfConsistentRegion()
SPL::boolean isTriggerOfConsistentRegion();

/// Gets the index of the consistent region.
/// @return Index of the consistent region, or -1 if the operator is not in a
/// consistent region.
/// @splnative public int32 getConsistentRegionIndex()
SPL::int32 getConsistentRegionIndex();

/// Gets the current sequence identifier of a consistent region.
/// This is the identifier that a consistent region drain or reset is
/// associated with. Immediately after operator startup, this function returns 1.
/// On operator restart, this function returns -1 until the operator has reset.
/// @return The current sequence identifier for a drain or reset, or -1 if the operator
/// is not in a consistent region or has not yet reset after an operator
/// restart.
/// @splnative public int64 getSequenceId()
SPL::int64 getSequenceId();

/// Gets the drain timeout of a consistent region.
/// @return The drain timeout in seconds, or -1 if the operator is not in a consistent
/// region.
/// @splnative public float64 getDrainTimeout()
SPL::float64 getDrainTimeout();

/// Gets the reset timeout of a consistent region.
/// @return The reset timeout in seconds, or -1 if the operator is not in a consistent
/// region.
/// @splnative public float64 getResetTimeout()
SPL::float64 getResetTimeout();

/// If the operator is in a consistent region, this function returns the
/// number of attempts made to reset the consistent region
/// to a consistent state.
/// This function returns the current number of reset attempts if the region is resetting,
/// or the number of reset attempts made for the last reset.
/// @return The number of reset attempts, or -1 if the operator is not in a consistent
/// region, or has not yet reset after an operator restart.
/// @splnative public int32 getResetAttempt()
SPL::int32 getResetAttempt();

/// This function requests the ConsistentRegionControllerMXBean to restore the consistent region
/// to the last established consistent state. This function can be used by operators when
/// detecting an operator-specific transient failure. If the region is already being reset,
/// a new reset attempt is triggered. If the operator is not in a consistent region,
/// this function has no effect.
/// @splnative public void resetConsistentRegion()
void resetConsistentRegion();

/// Evaluates an SPL predicate expression, represented as an `rstring`, using the given tuple. The predicate expression has the
/// same syntax and limitations as the `filter` parameter to the `Import` operator.
/// @return The result of evaluating the predicate expression.
/// @param splExpr The SPL predicate expression to evaluate, in `rstring` form. For example:
/// `(a == "hi") && ("x" in b || g[4] > 6.7 || id % 8 == 3)`.
/// @param tup The tuple to use in the evaluation.
/// @param error 0 for success, or greater than 0 if there is an error parsing the predicate expression.
/// @splnative <tuple T> public boolean evalPredicate (rstring splExpr, T tup, mutable int32 error)
SPL::boolean evalPredicate(const SPL::rstring& splExpr, const Tuple& tuple, SPL::int32& error);

// network functions

/// Get the host name.
/// @return Full host name (with the domain) of the node running this operator as returned
/// by the system call `gethostname()`.
/// @throws SPLRuntimeException If unsuccessful.
/// @splnative public stateful rstring getHostName()
SPL::rstring getHostName();

/// Get the short host name.
/// @return Short host name (without the domain) of the node running this operator.
/// @throws SPLRuntimeException If unsuccessful.
/// @splnative public stateful rstring getShortHostName()
inline SPL::rstring getShortHostName()
{
    SPL::rstring name = getHostName();
    SPL::rstring::size_type pos = name.find_first_of(".");
    if (pos != SPL::rstring::npos) {
        return name.substr(0, pos);
    } else {
        return name;
    }
}

/// Get the hostname used by the streams instance for this host.   This returns
/// the full hostname (with domain) for the interface configured for
/// use by the streams instance for this host.  This may differ from the
/// IP address for the name returned from getHostName() if there are
/// multiple network interfaces on this host.
/// @return The hostname for the configured streams interface for this host
/// representation.
/// @throws SPLRuntimeException If unsuccessful.
/// @splnative public stateful rstring getConfiguredHostName()
SPL::rstring getConfiguredHostName();

/// Get the IP address for the current host.
/// @return The IP address as a string.
/// @splnative public stateful rstring getIPAddress()
SPL::rstring getIPAddress();

/// Get the application configuration with the given name.
/// The application configuration is a set of properties where each key and
/// its value in the property set is a string. This method copies all the
/// properties from the configuration into the properties output map.
/// @cppparam [out] properties Output map where the configuration properties are to be inserted.
/// @splparam properties Output map where the configuration properties are to be inserted.
/// @param name Application configuration name.
/// @return 0 if the configuration specified by name was found; otherwise -1.
/// @splnative public stateful int32 getApplicationConfiguration(mutable map<rstring, rstring> properties, rstring name)
SPL::int32 getApplicationConfiguration(SPL::map<SPL::rstring, SPL::rstring>& properties,
                                       SPL::rstring const& name);

/// Get the value associated with the given configuration property
/// `propertyName` from the configuration identified by `configName`.
/// The `defaultValue` is returned if there is no value associated with
/// `propertyName` in the configuration, or the configuration
/// does not exist.
/// @param configName Application configuration name.
/// @param propertyName Configuration property name.
/// @param defaultValue Default value for the configuration property.
/// @return The value associated with the property name or `defaultValue` if
/// the associated value or the configuration does not exist.
/// @splnative public stateful rstring getApplicationConfigurationProperty(rstring configName, rstring propertyName, rstring defaultValue)
SPL::rstring getApplicationConfigurationProperty(SPL::rstring const& configName,
                                                 SPL::rstring const& propertyName,
                                                 SPL::rstring const& defaultValue);

/// Get the value associated with the given configuration property
/// `propertyName` from the configuration identified by `configName`.
/// An empty string is returned if there is no value associated with
/// `propertyName` in the configuration, or the configuration
/// does not exist.
/// @param configName Application configuration name.
/// @param propertyName Configuration property name.
/// @cppparam [out] errorCode Output parameter, set to 0 for success, -1 if the
/// configuration specified by `configName` could not be found, -2 if the
/// configuration property specified by `propertyName` could not be found.
/// @splparam errorCode Output parameter, set to 0 for success, -1 if the
/// configuration specified by `configName` could not be found, -2 if the
/// configuration property specified by `propertyName` could not be found.
/// @return The value associated with the property name or an empty string
/// if the configuration or the property does not exist.
/// @splnative public stateful rstring getApplicationConfigurationProperty(rstring configName, rstring propertyName, mutable int32 errorCode)
inline SPL::rstring getApplicationConfigurationProperty(SPL::rstring const& configName,
                                                        SPL::rstring const& propertyName,
                                                        SPL::int32& errorCode)
{
    return SPL::ProcessingElement::pe().getApplicationConfigurationProperty(
      configName, propertyName, &errorCode);
}

/*
 * Import Subscriptions/Export Properties
 */

/// Get the export properties for an output port that produces a stream that is exported with properties.
/// @param properties Output list of tuples of type `tuple<rstring name, rstring value, rstring typ>`
/// representing the current export properties for the stream output by this port, where
/// `name` is the name of the property. `value` is the value of the property as a literal value of the type for the property,
/// and `typ` is the type of the property (one of: `int64`, `float64`, `rstring`, `list<int64>`, `list<float64>`, `list<rstring>`.
/// @param port Output port index.
/// @return Error return code: 0 for success, or 1 for an invalid port or a port that does not produce a stream that is exported with properties.
/// @splnative public int32 getOutputPortExportProperties(mutable list<tuple<rstring name, rstring value, rstring typ>> properties, uint32 port)
SPL::int32 getOutputPortExportProperties(List& properties, uint32 port);

/// Set the export properties for an output port that produces a stream that is exported with properties.
/// See the `Export` operator for details of valid export properties.
///
/// For example, in SPL: `setOutputPortExportProperties([{name = "a", value = "\"string value\"", typ = "rstring"}], 0u);`
/// @param properties A list of tuples of type `tuple<rstring name, rstring value, rstring typ>`
/// representing the new export properties for the stream output by this port, where
/// `name` is the name of the property. `value` is the value of the property as a literal value of the type for the property,
/// and `typ` is the type of the property (one of: `int64`, `float64`, `rstring`, `list<int64>`, `list<float64>`, `list<rstring>`.
/// For string literals, use double quotes to specify the value.
/// @param port Output port index.
/// @return Error return code: 0 for success, 1 for an invalid port or a port that does not produce a stream that is exported with properties,
/// 2 for invalid properties, or 3 for a failure to set the properties.
/// @splnative public stateful int32 setOutputPortExportProperties(list<tuple<rstring name, rstring value, rstring typ>> properties, uint32 port)
SPL::int32 setOutputPortExportProperties(const List& properties, uint32 port);

/// Set the export properties for an output port that produces a stream that is exported with properties.
/// See the `Export` operator for details of valid export properties.
///
/// For example, in SPL: `setOutputPortExportProperties({a = "hi", b = [ 4l, -8l]}, 0u);`
/// @param properties A tuple representing the new export properties for the stream output by this port.
/// @param port Output port index.
/// @return Error return code: 0 for success, 1 for an invalid port or a port that does not produce a stream that is exported with properties,
/// 2 for an invalid type used in the properties tuple, or 3 for a failure to set the properties.
/// @splnative <tuple T> public stateful int32 setOutputPortExportProperties(T properties, uint32 port)
SPL::int32 setOutputPortExportProperties(const Tuple& properties, uint32 port);

/// Get the import subscription for an input port that receives a stream that is imported by subscription.
/// @param port Input port index.
/// @param error Return code: 0 for success, 1 for an invalid port or a port that does not receive a stream that is imported by subscription.
/// @return The subscription for the port, or an empty string on error.
/// @splnative public rstring getInputPortImportSubscription(uint32 port, mutable int32 error)
SPL::rstring getInputPortImportSubscription(uint32 port, SPL::int32& error);

/// Set the import subscription for an input port that receives a stream that is imported by subscription.
/// See the `Import` operator for details of valid subscription expressions.
/// Note: Setting a import subscription removes any existing filter expression in the same input port.
/// @param subscription A string representing the import subscription expression for this port.
/// @param port Input port index.
/// @return Error return code:  0 for success, 1 for an invalid port or a port that does not receive a stream that is imported by subscription,
/// 2 for an invalid subscription expression, or 3 for a failure to set the subscription.
/// @splnative public stateful int32 setInputPortImportSubscription(rstring subscription, uint32 port)
SPL::int32 setInputPortImportSubscription(const SPL::rstring& subscription, uint32 port);

/*
 * Import Filter Expressions
 */

/// Get the filter expression for an input port that receives a stream that is imported.
/// @param port Input port index.
/// @param error Return code: 0 for success, 1 for an invalid port or a port that does not receive a stream that is imported.
/// @return The filter expression for the port, or an empty string if the filter expression is empty or there is an error.
/// @splnative public rstring getInputPortImportFilterExpression(uint32 port, mutable int32 error)
SPL::rstring getInputPortImportFilterExpression(uint32 port, SPL::int32& error);

/// Set the filter expression for an input port that receives a stream that is imported.
/// See the `Import` operator for details of valid filter expressions.
/// Note: Setting a import subscription removes any existing filter expression in the same input port.
/// @param filter A string representing the filter expression for this port. An empty
/// string removes the filter.
/// @param port Input port index.
/// @return Error return code:  0 for success, 1 for an invalid port or a port that does not receive a stream that is imported,
/// 2 for an invalid filter expression, or 3 for a failure to set the filter.
/// @splnative public stateful int32 setInputPortImportFilterExpression(rstring filter, uint32 port)
SPL::int32 setInputPortImportFilterExpression(const SPL::rstring& filter, uint32 port);

/*
 * Resource loading Functions
 */

/// Loads and formats a string from a toolkit resource using the execution locale.
/// @param toolkitName The name of the toolkit that owns the resource.
/// @param bundleName The name of the resource bundle within the toolkit.
/// @param resourceID The ID of the resource to load.
/// @param substitutionValues A tuple containing substitution values.  Note that the types and values of the
/// attributes in the tuple are important.  The attribute names are used only for error reporting.
/// @return The formatted string.
/// @throws SPLRuntimeUnsupportedSubstitutionTypeException If the type of a substitution variable is not supported.
/// @splnative <tuple T> public rstring loadAndFormatResource(rstring toolkitName, rstring bundleName, rstring resourceID, T substitutionValues)
SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const Tuple& substitutionValues);

/// Loads and formats a string from a toolkit resource using the specified locale.
/// @param toolkitName The name of the toolkit that owns the resource.
/// @param bundleName The name of the resource bundle within the toolkit.
/// @param resourceID The ID of the resource to load.
/// @param substitutionValues A tuple containing substitution values.  Note that the types and values of the
/// attributes in the tuple are important.  The attribute names are used only for error reporting.
/// @param locale The locale the message should be formatted in.  For example, "en_US.UTF-8".
/// @return The formatted string.
/// @throws SPLRuntimeUnsupportedSubstitutionTypeException If the type of a substitution variable is not supported.
/// @splnative <tuple T> public rstring loadAndFormatResource(rstring toolkitName, rstring bundleName, rstring resourceID, T substitutionValues, rstring locale)
SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const Tuple& substitutionValues,
                                   const SPL::rstring& locale);

/// Loads a string from a toolkit resource using the execution locale.
/// @param toolkitName The name of the toolkit that owns the resource.
/// @param bundleName The name of the resource bundle within the toolkit.
/// @param resourceID The ID of the resource to load.
/// @return The requested string.
/// @splnative public rstring loadAndFormatResource(rstring toolkitName, rstring bundleName, rstring resourceID)
SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID);

/// Loads a string from a toolkit resource using the specified locale.
/// @param toolkitName The name of the toolkit that owns the resource.
/// @param bundleName The name of the resource bundle within the toolkit.
/// @param resourceID The ID of the resource to load.
/// @param locale The locale the message should be formatted in.  For example, "en_US.UTF-8".
/// @return The requested string.
/// @splnative public rstring loadAndFormatResource(rstring toolkitName, rstring bundleName, rstring resourceID, rstring locale)
SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const SPL::rstring& locale);

/*
 * Export connection and congestion information
 */

/// Return the connection information for an output port that connects to another PE.
///
/// For example, in SPL: `mutable Sys.PEConnections conns; int32 ret = getPEOutputPortConnections(0, conns);`
/// @param port Output port index.
/// @param connections Output list of tuples containing information for each output connection.
/// @return Error return code: 0 for success, -1 for an invalid port index, -2 for an invalid list.
/// @splnative public int32 getPEOutputPortConnections (uint32 port, mutable list<tuple<int64 pe, int64 port, uint8 congestionFactor, uint64 nTuplesFilteredOut>> connections)
SPL::int32 getPEOutputPortConnections(uint32 port, SPL::List& connections);

/// Return the maximum congestion factor for all the connections from an output port that
/// connect to another PE.
/// @param port Output port index.
/// @return -1 for an invalid port, or the congestion factor (0 to 100).
/// @splnative public int32 getOutputPortCongestionFactor(uint32 port)
SPL::int32 getOutputPortCongestionFactor(uint32 port);

}
}
}

#endif /* SPL_RUNTIME_FUNCTION_BUILTIN_UTILS_FUNCTIONS_H */
