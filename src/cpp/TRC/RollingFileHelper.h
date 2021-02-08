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

#ifndef ROLLINGFILEHELPER_H_
#define ROLLINGFILEHELPER_H_

#include <TRC/TRCTypes.h>
#include <stdio.h>
#include <string>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

/// Processing the log/trace file setting.
/// As for some case, the maxLogFileSize and maxLogFileNum setting will be
/// input together as filename, need to parse these information.
/// @param name file name
/// @param maxFileSize maximum file size
/// @param maxFileNum maximum number of log/trace files.
void rfh_parseFileSettings(std::string& name, int& maxFileSize, int& maxFileNum);

/// Initial the parent directory of a file.
/// @param name file name
void rfh_initialFileDir(const std::string& name);

/// Method to adopt all existing log or trace files when a RollingFile
/// object initialized. This is for HA support:
/// As when a service crash and restarted, it will adopt old log files
/// and continue.
/// @param name file name
/// @param archivedNum number of files that match the file name and archived.
void rfh_adoptExistingLogFiles(const std::string& name, int& archivedNum);

/// Method that actually doing the rollover of all log files.
void rfh_rollover(int& archivedNum,
                  const int maxFileNum,
                  const std::string& name,
                  bool useStderr = true);

/// Find the size of the active log file.
/// If all work alright, return the actual size of the file.
/// If any error happens, return -1.
int rfh_getFileSize(const std::string& name, bool useStderr = true);

/// Flush and close ofstream we are using to write messages and delete
/// file stream
/// @param strm current opened file stream
/// @param checkClose check to make sure file stream closed before delete FILE
///        pointer.
bool rfh_closeStream(FILE*& strm, bool checkClose, bool useStderr = true);

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif /*ROLLINGFILELOGGER_H_*/
