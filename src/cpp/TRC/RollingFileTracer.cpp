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

#include <TRC/DistilleryDebug.h>
#include <TRC/RollingFileHelper.h>
#include <TRC/RollingFileTracer.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

const static int KILOBYTES = 1024;

// Note: Do not call FileTracer constructor with RollingFileTracer constructors,
//       Since FileTracer constructor will open another ostream for the file.

RollingFileTracer::RollingFileTracer(const string& filename)
  : _archivedNum(0)
  , _maxFileNum(get_environment_variable("STREAMS_MAXIMUM_NUMBER_LOGFILE_EACH_COMPONENT", 3))
  , _maxFileSize(get_environment_variable("STREAMS_MAXIMUM_LOGFILE_SIZE", 5000))
  , _doLog(true)
  , _type(RollingFileTracer::SPC_TRC)
  , _useStderr(true)
  , _removeTraceFile(false)
{
    _level = 0;
    _filename = filename;
    initialize(false);
}

RollingFileTracer::RollingFileTracer(const string& filename, int level, bool usePid)
  : _archivedNum(0)
  , _maxFileNum(get_environment_variable("STREAMS_MAXIMUM_NUMBER_LOGFILE_EACH_COMPONENT", 3))
  , _maxFileSize(get_environment_variable("STREAMS_MAXIMUM_LOGFILE_SIZE", 5000))
  , _doLog(true)
  , _type(RollingFileTracer::SPC_TRC)
  , _useStderr(true)
  , _removeTraceFile(false)
{
    _level = level;
    _filename = filename;
    initialize(usePid);
}

RollingFileTracer::RollingFileTracer(const string& filename, int level, writingType type)
  : _archivedNum(0)
  , _maxFileNum(get_environment_variable("STREAMS_MAXIMUM_NUMBER_LOGFILE_EACH_COMPONENT", 3))
  , _maxFileSize(get_environment_variable("STREAMS_MAXIMUM_LOGFILE_SIZE", 5000))
  , _doLog(true)
  , _type(type)
  , _useStderr(true)
  , _removeTraceFile(false)
{
    _level = level;
    _filename = filename;
    initialize(false);
}

void RollingFileTracer::initialize(bool usePid)
{
    _format = getTrcFormat();
    _useAutoMutex = true;

    if (_type == RollingFileTracer::STDOUT_STDERR) {
        _useStderr = false;
    }

    rfh_parseFileSettings(_filename, _maxFileSize, _maxFileNum);
    if (0 == _maxFileNum || 0 == _maxFileSize) {
        _doLog = false;
    }

    // Add pid to the file name to support redundant services. But not for PEC.
    if (usePid && _filename.find("/pec.") == string::npos) {
        stringstream newfilenameSS;
        int size = _filename.size();
        newfilenameSS << _filename.substr(0, size - 4) << "_" << getpid() << ".out";
        _filename.assign(newfilenameSS.str());
        _removeTraceFile = true;
    }

    // Initial trace/log file directory and adopt all existing trace/log files.
    rfh_initialFileDir(_filename);
    rfh_adoptExistingLogFiles(_filename, _archivedNum);

    // Note: No need to do error checking for strm.open here, as we will try to
    // open file again when writeMessage.
    _strm.open(_filename.c_str(), ios::out | ios::app);
}

void RollingFileTracer::writeMessage(int level,
                                     const string& aspect,
                                     const string& args,
                                     const string& function,
                                     const string& file,
                                     int line,
                                     const string& message)
{
    if (_doLog) {
        if (_type != RollingFileTracer::STDOUT_STDERR) {
            string formatedMsg = formatMessage(level, aspect, args, function, file, line, message);
            writeMessage(formatedMsg);
        } else {
            writeMessage(message);
        }
    }
}

void RollingFileTracer::writeMessage(const string& msg)
{
    if (_doLog) {
        if (_useAutoMutex) {
            if (_type == RollingFileTracer::APP_TRC) {
                AutoMutex lck(app_mutex);
                writeMessage_r(msg);
            } else if (_type == RollingFileTracer::SPC_TRC) {
                AutoMutex lck(dbg_mutex);
                writeMessage_r(msg);
            } else if (getAvailableFileSystemSpace(_filename.c_str()) > msg.size()) {
                AutoMutex lck(stdouterr_mutex);
                writeMessage_r(msg);
            }
        } else {
            writeMessage_r(msg);
        }
    }
}

void RollingFileTracer::writeMessage_r(const string& msg)
{
    int filesize = rfh_getFileSize(_filename, _useStderr);
    int totalsize = filesize + msg.size();
    // If some system error when getting file size, or file been deleted,
    // but somehow we have stream open, close it first.
    if (-1 == filesize && _strm.is_open()) {
        _strm.flush();
        _strm.close();
    }
    // Doing checking and rollover if necessary.
    // If file size is 0, even if message size is bigger than maximum setting,
    // no roll needed.
    else if ((filesize > 0) && (totalsize > _maxFileSize * KILOBYTES)) {
        // Close stream, then roll over the file.
        if (_strm.is_open()) {
            _strm.flush();
            _strm.close();
        }
        rfh_rollover(_archivedNum, _maxFileNum, _filename, _useStderr);
    }
    // Open stream if closed.
    if (!_strm.is_open()) {
        // Open stream if it is not opened for writing
        _strm.open(_filename.c_str(), ios::out | ios::app);
        if (!_strm.is_open()) {
            if (_useStderr) {
                cerr << "Can not open file " << _filename
                     << " to write logs. With error: " << _strm.rdstate() << endl;
            }
            _strm.clear(); // clear error bit history.
            return;
        }
    }
    // Writing message. adding an endl to the end of message.
    if (_type != RollingFileTracer::STDOUT_STDERR) {
        // For Streams trace and log, we need to add endl at the end.
        _strm << msg << endl;
    } else {
        // For stdout/stderr, it will be up to original code to add endl or not.
        _strm << msg;
    }
    if (!_strm) {
        if (_useStderr) {
            cerr << "Can not write message (" << msg << ") to file " << _filename
                 << " with error: " << _strm.rdstate() << endl;
        }
        _strm.clear(); // clear error bit history.
    }
    _strm.flush();
    if (!_strm) {
        if (_useStderr && (getAvailableFileSystemSpace(_filename.c_str()) > (msg.size() + 512))) {
            cerr << "Can not flush message (" << msg << ") to file " << _filename
                 << " with error: " << _strm.rdstate() << endl;
        }
        _strm.clear(); // clear error bit history.
    }
}

string RollingFileTracer::formatMessage(int level,
                                        const string& aspect,
                                        const string& args,
                                        const string& function,
                                        const string& file,
                                        int line,
                                        const string& message)
{
    string formatedMsg;

    // If having format set, format the message
    if (_format.length()) {
        int64_t timestamp = debug::TimeStamp::TOD_ms();
        formatedMsg =
          debug::formatMessage(level, aspect, args, timestamp, getpid(), distillery_gettid(),
                               Directory::sbasename(file), function, line, _format, message);
    }
    // If no format set, using default. We almost never be here with current
    // getTrcFormat() behavior, but keep here as last catch.
    else {
        stringstream ss;
        time_t now = time(NULL);
        struct tm local_tm;
        struct tm* local_tm_ptr = &local_tm;
        local_tm_ptr = localtime_r(&now, local_tm_ptr);
        char time[KILOBYTES];
        strftime(time, KILOBYTES, "%a %b %d %H:%M:%S", local_tm_ptr);

        ss << "DEBUG" << level << " " << time << " {" << aspect << "}"
           << " [" << getInstance() << "]"
           << " (" << dec << gettid() << ", " << function << ", " << Directory::sbasename(file)
           << ":" << dec << line << ")"
           << " " << message;
        formatedMsg = ss.str();
    }
    return formatedMsg;
}

void RollingFileTracer::removeTraceFile()
{
    if (_removeTraceFile) {
        remove(_filename.c_str());
    }
}

RollingFileTracer::~RollingFileTracer()
{
    if (_strm.is_open()) {
        _strm.flush();
        _strm.close();
    }
    removeTraceFile();
}
