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
#include <TRC/RollingFileHelper.h>
#include <TRC/RollingFileLogger.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/ResourceBundleHelper.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

const static int KILOBYTES = 1024;

RollingFileLogger::RollingFileLogger(const string& filename)
  : Logger(0)
  , _filename(filename)
  , _archivedNum(0)
  , _maxFileNum(get_environment_variable("STREAMS_MAXIMUM_NUMBER_LOGFILE_EACH_COMPONENT", 3))
  , _maxFileSize(get_environment_variable("STREAMS_MAXIMUM_LOGFILE_SIZE", 5000))
  , _doLog(true)
{
    initialize();
}

RollingFileLogger::RollingFileLogger(const string& filename, int level)
  : Logger(level)
  , _filename(filename)
  , _archivedNum(0)
  , _maxFileNum(get_environment_variable("STREAMS_MAXIMUM_NUMBER_LOGFILE_EACH_COMPONENT", 3))
  , _maxFileSize(get_environment_variable("STREAMS_MAXIMUM_LOGFILE_SIZE", 5000))
  , _doLog(true)
{
    initialize();
}

void RollingFileLogger::initialize()
{
    rfh_parseFileSettings(_filename, _maxFileSize, _maxFileNum);
    if (0 == _maxFileNum || 0 == _maxFileSize) {
        _doLog = false;
    }

    _format = getLogFormat();

    // Initial trace/log file directory and adopt all existing trace/log files.
    rfh_initialFileDir(_filename);
    rfh_adoptExistingLogFiles(_filename, _archivedNum);

    // Note: No need to do error checking for strm.open here, as we will try to
    // open file again when writeMessage.
    _strm.open(_filename.c_str(), ios::out | ios::app);
}

void RollingFileLogger::writeMessage(int level,
                                     const string& aspect,
                                     const string& args,
                                     const string& function,
                                     const string& file,
                                     int line,
                                     const string& message)
{
    if (_doLog) {
        int64_t timestamp = debug::TimeStamp::TOD_us();
        string formatedMsg =
          debug::formatMessage(level, aspect, args, timestamp, getpid(), distillery_gettid(),
                               Directory::sbasename(file), function, line, _format, message, false);
        if (_useAutoMutex) {
            AutoMutex lck(log_mutex);
            writeMessage_r(formatedMsg);
        } else {
            writeMessage_r(formatedMsg);
        }
    }
}

void RollingFileLogger::writeMessage(const string& aspect,
                                     const string& key,
                                     const vector<string>& substitutionTexts)
{
    ResourceBundleHelper rb;
    stringstream strm;
    strm << PrintMessageToString(rb, key, substitutionTexts);
    writeMessage(0 /*level not matter here*/, aspect, "", "", "", 0, strm.str());
}

void RollingFileLogger::writeMessage_r(const string& msg)
{
    int filesize = rfh_getFileSize(_filename);
    int totalsize = filesize + msg.size();
    // If some system error when getting file size, or file been deleted,
    // but somehow we have stream open, close it first.
    if (-1 == filesize && _strm.is_open()) {
        _strm.flush();
        _strm.close();
    }
    // Doing checking and rollover first if necessary.
    // If file size is 0, even if message size is bigger than maximum setting,
    // no roll needed.
    else if ((filesize > 0) && (totalsize > _maxFileSize * KILOBYTES)) {
        // Close stream, then roll over the file.
        if (_strm.is_open()) {
            _strm.flush();
            _strm.close();
        }
        rfh_rollover(_archivedNum, _maxFileNum, _filename);
    }

    if (!_strm.is_open()) {
        // Open stream if it is not opened for writing
        _strm.open(_filename.c_str(), ios::out | ios::app);
        if (!_strm.is_open()) {
            cerr << "Can not open file " << _filename
                 << " to write logs. With error: " << _strm.rdstate() << endl;
            _strm.clear(); // clear error bit history.
            return;
        }
    }
    // Writing message. adding an endl to the end of message.
    _strm << msg << endl;
    if (!_strm) {
        cerr << "Can not write message (" << msg << ") to file " << _filename
             << " with error: " << _strm.rdstate() << endl;
        _strm.clear(); // clear error bit history.
    }
    _strm.flush();
    if (!_strm) {
        cerr << "Can not flush message (" << msg << ") to file " << _filename
             << " with error: " << _strm.rdstate() << endl;
        _strm.clear(); // clear error bit history.
    }
}

RollingFileLogger::~RollingFileLogger()
{
    if (_strm.is_open()) {
        _strm.flush();
        _strm.close();
    }
}
