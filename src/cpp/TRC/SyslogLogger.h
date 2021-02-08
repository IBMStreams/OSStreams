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
//       SyslogLogger.h
//
// Purpose-
//       Debugging Logger for the syslog
//
//----------------------------------------------------------------------------

#ifndef __SYSLOG_LOGGER_H__
#define __SYSLOG_LOGGER_H__

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class SyslogLogger : public Logger
{

  public:
    SyslogLogger(int level = 0);

    /* ----------------- logger interface ----------------- */

    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);

    virtual void writeMessage(const std::string& aspect,
                              const std::string& key,
                              const std::vector<std::string>& substitutionTexts);

    /* ---------------------------------------------------- */

  private:
    /// Method to get syslog level based on Streams log level
    /// @param level Streams log level
    /// @return syslog level
    int getSyslogLevelFromStreamsLevel(int level);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
