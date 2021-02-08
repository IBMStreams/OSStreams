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
// RollingFileLogger: support swapping log files based on file size and the
//                    maximum number of log files we can keep.
//
// Two properties that control how to roll:
//     1. Maximum file size -- maxFileSize
//     2. Maximum number of (log) files system can keep -- maxFileNum
//
// Rolling policies:
//     1. If maxFileSize==0 or maxFileNum==0, no log going to write;
//     2. If SELinux enabled and no one can delete log files, maxFileNum will
//        not effect, the number of log files will keep growing, with each file
//        maximum size around maxFileSize;
//     3. Otherwise, system will keep maxFileNum of log files with each file
//        size as maxFileSize.
//     4. As RollingFileLogger always remove the oldest files, the basic rules
//        of setting maxFileSize and maxFileNum is: if the total size is set,
//        a. In order to avoid losing too much log information, trying to set
//           maxFileNum high and maxFileSize low.
//        b. In order to avoid too many rollings and affect performance, trying
//           to set maxFileNum low and maxFileSize high.
//        Such as: Suppose we only want log file take maximum 100MB space:
//           If we set maxFileNum to 100000, and maxFileSize to 1(KB), rolling
//           will happen too often to affect performance.
//           If we set maxFileNum to 1, and maxFileSize to 100000(KB), every
//           time rolling will lost all log history.
//----------------------------------------------------------------------------

#ifndef ROLLINGFILELOGGER_H_
#define ROLLINGFILELOGGER_H_

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>
#include <UTILS/DistilleryException.h>
#include <fstream>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class RollingFileLogger : public Logger
{
  public:
    RollingFileLogger(const std::string& filename);
    RollingFileLogger(const std::string& filename, int level);

    virtual ~RollingFileLogger();

    /// Writing message
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

  private:
    std::string _filename;
    /// Keep track of how many archived log files with the service.
    int _archivedNum;
    /// Maximum log files that the service can have.
    int _maxFileNum;
    /// Maximum log file size.
    int _maxFileSize;

    /// Flag to mark if we log or not based on log file size or number above.
    bool _doLog;

    /// output file stream to write log messages.
    std::ofstream _strm;

    /// Method to initialize all settings based on active log file name.
    /// @param filename the file to write trace/log message. It also can include
    ///        file related settings, such as maxSize, maxNums.
    /// @param level trace or log level
    void initialize();

    /// write message without lock.
    void writeMessage_r(const std::string& msg);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif /*ROLLINGFILELOGGER_H_*/
