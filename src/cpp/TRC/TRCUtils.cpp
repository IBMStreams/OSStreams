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

#include <TRC/ConsoleLogger.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/TRCUtils.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>
#include <stdio.h>
#include <time.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
UTILS_RUNTIME_NS_USE;
using namespace std;
using namespace UTILS_RUNTIME_NS::mr;

const string DEBUG_NAMESPACE::getTrcFormat()
{
    string format(get_environment_variable("STREAMS_DEBUG_FORMAT", streams_default_trace_format));

    if (format == "default" || format == "") {
        format = FORMAT_DEFAULT;
    } else if (format == "long") {
        format = FORMAT_LONG;
    } else if (format == "short") {
        format = FORMAT_SHORT;
    } else if (format == "spc") {
        format = FORMAT_SPC;
    }

    return format;
}

const string DEBUG_NAMESPACE::getLogFormat(string logType)
{
    string format = FORMAT_LOG;
    if (logType == "syslog") {
        format = FORMAT_SYSLOG;
    }
    return format;
}

const string DEBUG_NAMESPACE::formatMessage(int level,
                                            const string& aspect,
                                            const string& args,
                                            unsigned long long timestamp,
                                            int pid,
                                            int tid,
                                            const string& file,
                                            const string& function,
                                            int line,
                                            const string& format,
                                            const string& message,
                                            bool forTrace)
{
    char time[1024];
    char trctime[1024];
    if (forTrace) {
        // Get local time structure based on timestamp
        time_t sec = timestamp / 1000;
        struct tm local_tm;
        struct tm* local_tm_ptr = &local_tm;
        local_tm_ptr = localtime_r(&sec, local_tm_ptr);
        // struct tm * ptm;
        // time(&sec);
        // ptm = gmtime(&sec);
        // Create date format
        char date_fmt[1024];
        snprintf(date_fmt, 1024, "%s.%3.3d%%z", "%a %b %d %H:%M:%S", (int)(timestamp % 1000));
        char trcdate_fmt[1024];
        // format as example of 12 May 2017 13:04:51.331
        snprintf(trcdate_fmt, 1024, "%s.%3.3d%%z", "%d %b %Y %H:%M:%S", (int)(timestamp % 1000));
        // format as example of 2017-05-12T17:04:21.010+0000
        // snprintf(trcdate_fmt, 1024, "%s.%3.3d", "%Y-%m-%dT%H:%M:%S%z", (int)(timestamp % 1000)) ;
        // Generate date string based on format
        strftime(time, 1024, date_fmt, local_tm_ptr);
        strftime(trctime, 1024, trcdate_fmt, local_tm_ptr);
    }

    char trclevel[32];
    switch (level) {
        case iL_ERROR:
            snprintf(trclevel, 32, "ERROR");
            break;
        case iL_WARN:
            snprintf(trclevel, 32, "WARN");
            break;
        case iL_INFO:
            snprintf(trclevel, 32, "INFO");
            break;
        case iL_DEBUG:
            snprintf(trclevel, 32, "DEBUG");
            break;
        case iL_TRACE:
            snprintf(trclevel, 32, "TRACE");
            break;
        default:
            strcpy(trclevel, "UNKNOWN");
    }

    char component[32];
    snprintf(component, 32, "%s", debug::cmd.c_str());

    char host[1024];
    snprintf(host, 1024, "%s", debug::hostname.c_str());

    string formatted_msg;
    formatted_msg = format;

#define REPLACE(name)                                                                              \
    do {                                                                                           \
        stringstream strm;                                                                         \
        strm << name;                                                                              \
        RegEx re_##name("@" #name "@");                                                            \
        formatted_msg = RegEx::search_replace(                                                     \
          re_##name, strm.str(), RegEx::SR_GLOBAL_REPLACE | RegEx::SR_FIXED, formatted_msg);       \
    } while (0)

    REPLACE(aspect);
    // Do not needed following commented ones based on current log/trace format.
    // But please keep for future reference.
    // REPLACE(pid) ;
    // REPLACE(component) ;
    if (forTrace) {
        REPLACE(level);
        REPLACE(tid);
        REPLACE(time);
        REPLACE(trctime);
        REPLACE(trclevel);
        REPLACE(args);
        REPLACE(file);
        REPLACE(function);
        REPLACE(line);
    } else {
        REPLACE(host);
        REPLACE(timestamp);
        string instance = getInstanceId();
        REPLACE(instance);
    }
    REPLACE(message);

    return formatted_msg;
}

const int DEBUG_NAMESPACE::levelFromStr2Int(const string& levelStr)
{
    int level = L_OFF;

    if (levelStr == "off" || levelStr == "OFF") {
        level = iL_OFF;
    } else if (levelStr == "error" || levelStr == "ERROR") {
        level = iL_ERROR;
    } else if (levelStr == "warn" || levelStr == "WARN") {
        level = iL_WARN;
    } else if (levelStr == "info" || levelStr == "INFO") {
        level = iL_INFO;
    } else if (levelStr == "debug" || levelStr == "DEBUG") {
        level = iL_DEBUG;
    } else if (levelStr == "trace" || levelStr == "TRACE") {
        level = iL_TRACE;
    }

    return level;
}

const string DEBUG_NAMESPACE::levelFromInt2Str(int level)
{
    string lvl;
    switch (level) {
        case iL_OFF:
            lvl = "OFF";
            break;
        case iL_ERROR:
            lvl = "ERROR";
            break;
        case iL_WARN:
            lvl = "WARN";
            break;
        case iL_INFO:
            lvl = "INFO";
            break;
        case iL_DEBUG:
            lvl = "DEBUG";
            break;
        case iL_TRACE:
            lvl = "TRACE";
            break;
        default:
            lvl = "UNKNOWN";
    }
    return lvl;
}

const int DEBUG_NAMESPACE::auditLevelFromStr2Int(const string& levelStr)
{
    int level = iL_INFO;

    if (levelStr == "off" || levelStr == "OFF") {
        level = iL_OFF;
    } else if (levelStr == "standard" || levelStr == "STANDARD") {
        level = iL_INFO;
    } else if (levelStr == "advanced" || levelStr == "ADVANCED") {
        level = iL_TRACE;
    }

    return level;
}

const string DEBUG_NAMESPACE::levelFromInt2dashD(int level)
{
    string dashD = "-D";
    switch (level) {
        case iL_OFF:
            dashD = "";
            break;
        case iL_ERROR:
            dashD = "-D";
            break;
        case iL_WARN:
            dashD = "-DD";
            break;
        case iL_INFO:
            dashD = "-DDD";
            break;
        case iL_DEBUG:
            dashD = "-DDDD";
            break;
        case iL_TRACE:
            dashD = "-DDDDD";
            break;
        default:
            SPCDBG(L_INFO, "Unsupported debug level '" << level << "'. Using default", "TRC");
            break;
    }
    return dashD;
}

const int DEBUG_NAMESPACE::MessageRecordingConfigTraceLevelToInt(const tracingLevelType& lvl)
{
    int level = iL_ERROR;
    switch (lvl) {
        case tracingLevelType::off:
            level = iL_OFF;
            break;
        case tracingLevelType::error:
            level = iL_ERROR;
            break;
        case tracingLevelType::warn:
            level = iL_WARN;
            break;
        case tracingLevelType::info:
            level = iL_INFO;
            break;
        case tracingLevelType::debug:
            level = iL_DEBUG;
            break;
        case tracingLevelType::trace:
            level = iL_TRACE;
            break;
        default:
            SPCDBG(L_WARN, "Unsupported tracing level '" << lvl << "'. Returning default", "TRC");
            break;
    }
    return level;
}

const tracingLevelType DEBUG_NAMESPACE::MessageRecordingConfigTraceLevelFromInt(const int lvl)
{
    tracingLevelType level = tracingLevelType::error;
    switch (lvl) {
        case iL_OFF:
            level = tracingLevelType::off;
            break;
        case iL_ERROR:
            level = tracingLevelType::error;
            break;
        case iL_WARN:
            level = tracingLevelType::warn;
            break;
        case iL_INFO:
            level = tracingLevelType::info;
            break;
        case iL_DEBUG:
            level = tracingLevelType::debug;
            break;
        case iL_TRACE:
            level = tracingLevelType::trace;
            break;
        default:
            SPCDBG(L_WARN, "Unsupported tracing level '" << lvl << "'. Returning default", "TRC");
            break;
    }
    return level;
}

const int DEBUG_NAMESPACE::MessageRecordingConfigLogLevelToInt(const loggingLevelType& lvl)
{
    int level = iL_WARN;
    switch (lvl) {
        case loggingLevelType::error:
            level = iL_ERROR;
            break;
        case loggingLevelType::warn:
            level = iL_WARN;
            break;
        case loggingLevelType::info:
            level = iL_INFO;
            break;
        default:
            SPCDBG(L_WARN, "Unsupported logging level '" << lvl << "'. Returning default", "TRC");
            break;
    }
    return level;
}

const loggingLevelType DEBUG_NAMESPACE::MessageRecordingConfigLogLevelFromInt(const int lvl)
{
    loggingLevelType level = loggingLevelType::warn;
    switch (lvl) {
        case iL_ERROR:
            level = loggingLevelType::error;
            break;
        case iL_WARN:
            level = loggingLevelType::warn;
            break;
        case iL_INFO:
            level = loggingLevelType::info;
            break;
        default:
            SPCDBG(L_WARN, "Unsupported logging level '" << lvl << "'. Returning default", "TRC");
            break;
    }
    return level;
}

const string DEBUG_NAMESPACE::getTRCFormattedDate()
{
    char trctime[1024];
    // Get local time structure based on timestamp
    int64_t timestamp = debug::TimeStamp::TOD_ms();
    time_t sec = timestamp / 1000;
    struct tm local_tm;
    struct tm* local_tm_ptr = &local_tm;
    local_tm_ptr = localtime_r(&sec, local_tm_ptr);
    // Create date format
    char trcdate_fmt[1024];
    snprintf(trcdate_fmt, 1024, "%s.%3.3d", "%d %b %Y %H:%M:%S", (int)(timestamp % 1000));
    // Generate date string based on format
    strftime(trctime, 1024, trcdate_fmt, local_tm_ptr);
    return string(trctime);
}
