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

#include <fstream>
#include <sys/utsname.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/FileTracer.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/HostInfo.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

FileTracer::FileTracer(const string& filename, int level)
  : Tracer(level)
  , _filename(filename)
{
    HostInfo hi(HostInfo::THIS_HOST);
    ;
    _filename = RegEx::search_replace("%ip", hi.getStringIPAddress(), "g", _filename);

    stringstream pid;
    pid << getpid();
    _filename = RegEx::search_replace("%pid", pid.str(), "g", _filename);

    string node = "unknown";
    struct utsname uname_s;
    if (uname(&uname_s) == 0) {
        node = uname_s.nodename;
    }

    _filename = RegEx::search_replace("%node", node, "g", _filename);

    _format = getTrcFormat();

    ofstream strm(_filename.c_str()); // truncate the file
}

FileTracer::~FileTracer() {}

void FileTracer::writeMessage(int level,
                              const string& aspect,
                              const string& args,
                              const string& function,
                              const string& file,
                              int line,
                              const string& message)
{
    AutoMutex lck(dbg_mutex);
    ofstream strm(_filename.c_str(), ios_base::out | ios_base::app);
    if (strm.is_open()) {
        strm << formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                              distillery_gettid(), UTILS_NAMESPACE::Directory::sbasename(file),
                              function, line, _format, message)
             << endl;
    }
}

void FileTracer::writeMessage(const std::string& msg)
{
    AutoMutex lck(dbg_mutex);
    ofstream strm(_filename.c_str(), ios_base::out | ios_base::app);
    if (strm.is_open()) {
        strm << msg << endl;
    }
}

void FileTracer::removeTraceFile() {}
