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

#include <unicode/udata.h>
#include <unicode/ustream.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/SyslogLogger.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/ResourceBundleHelper.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

SyslogLogger::SyslogLogger(int level)
  : Logger(level)
{
    _format = getLogFormat("syslog");
}

void SyslogLogger::writeMessage(int level,
                                const string& aspect,
                                const string& args,
                                const string& function,
                                const string& file,
                                int line,
                                const string& message)
{
    // Mapping log level to syslog level
    int syslogLevel = getSyslogLevelFromStreamsLevel(level);

    // Getting formatted message
    int64_t timestamp = getTimeInMicrosecs();
    string formatedMsg =
      formatMessage(level, aspect, args, timestamp, getpid(), distillery_gettid(),
                    Directory::sbasename(file), function, line, _format, message, false);

    // Doing syslog logging.
    openlog(Distillery::debug::cmd.c_str(), LOG_ODELAY, LOG_USER);
    syslog(syslogLevel, formatedMsg.c_str());
    closelog();
}

void SyslogLogger::writeMessage(const string& aspect,
                                const string& key,
                                const vector<string>& substitutionTexts)
{
    ResourceBundleHelper rb;
    stringstream strm;
    strm << PrintMessageToString(rb, key, substitutionTexts);
    writeMessage(0 /*level not matter here*/, aspect, "", "", "", 0, strm.str());
}

int SyslogLogger::getSyslogLevelFromStreamsLevel(int level)
{
    int syslogLevel = LOG_ERR;
    switch (level) {
        case iL_ERROR:
            break;
        case iL_WARN:
            syslogLevel = LOG_WARNING;
            break;
        case iL_INFO:
            syslogLevel = LOG_INFO;
            break;
        case iL_DEBUG:
            syslogLevel = LOG_DEBUG;
            break;
        case iL_TRACE:
            syslogLevel = LOG_DEBUG;
            break;
        default:
            break;
    }
    return syslogLevel;
}
