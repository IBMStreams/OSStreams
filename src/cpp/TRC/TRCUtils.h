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

#ifndef __TRCUTILS_H__
#define __TRCUTILS_H__

#include <TRC/TRCTypes.h>
#include <UTILS/MessageRecordingTypes.h>
#include <UTILS/UTILSXmlHelper.h>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

static std::string streams_default_trace_format = "default";

/// Get trace format
const std::string getTrcFormat();

/// Get log format
const std::string getLogFormat(std::string logType = "");

/// Format message based on trace/log format
/// @param level trace/log level
/// @param aspect trace/log aspect
/// @param args some special arguments for PE, JOb, or Port
/// @param timestamp timestamp
/// @param pid process id
/// @param tid thread id
/// @param file file/class name
/// @param function function name
/// @param line line number
/// @param format log/trace format
/// @param message log/trace message
/// @param forTrace mark if this formatting is for log or trace.
const std::string formatMessage(int level,
                                const std::string& aspect,
                                const std::string& args,
                                unsigned long long timestamp,
                                int pid,
                                int tid,
                                const std::string& file,
                                const std::string& function,
                                int line,
                                const std::string& format,
                                const std::string& message,
                                const bool forTrace = true);

/// Convert log/trace level from string to predefined integer
/// @param levelStr log/trace level in string type.
const int levelFromStr2Int(const std::string& levelStr);

/// Convert log/trace level from predefined integer to string
/// @param level trace/log level.
const std::string levelFromInt2Str(int level);

/// Convert audit level from string to predefined integer
/// @param levelStr audit level in string type.
const int auditLevelFromStr2Int(const std::string& levelStr);

/// Convert trace level from predefined integer to -D...D style.
/// @param level trace level
const std::string levelFromInt2dashD(int level);

const int MessageRecordingConfigTraceLevelToInt(const UTILS_RUNTIME_NS::mr::tracingLevelType& lvl);

const UTILS_RUNTIME_NS::mr::tracingLevelType MessageRecordingConfigTraceLevelFromInt(const int lvl);

const int MessageRecordingConfigLogLevelToInt(const UTILS_RUNTIME_NS::mr::loggingLevelType& lvl);

const UTILS_RUNTIME_NS::mr::loggingLevelType MessageRecordingConfigLogLevelFromInt(const int lvl);

const std::string getTRCFormattedDate();

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif
