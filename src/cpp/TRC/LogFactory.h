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

#ifndef __LOG_FACTORY_H__
#define __LOG_FACTORY_H__

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class LogFactory
{
  public:
    /// Static function to get a Logger object for logging.
    /// @param logType type of log: file or syslog
    /// @param logLevel level of log
    /// @param fileSettings log file settings if using file.
    ///        format as: filedir[:maxfilesize:maxfilenum] (to support rolling)
    static Logger* getLogger(const std::string& logType,
                             int logLevel,
                             const std::string& fileSettings);

  private:
    LogFactory(void);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif
