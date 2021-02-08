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

#include <stdio.h>
#include <unicode/udata.h>
#include <unicode/ustream.h>

#include <TRC/ConsoleLogger.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/RegEx.h>
#include <UTILS/ResourceBundleHelper.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

ConsoleLogger::ConsoleLogger(int level, bool toStdout)
  : Logger(level)
  , _toStdout(toStdout)
{
    _format = getLogFormat();
}

void ConsoleLogger::writeMessage(int level,
                                 const string& aspect,
                                 const string& args,
                                 const string& function,
                                 const string& file,
                                 int line,
                                 const string& message)
{
    if (_useAutoMutex) {
        AutoMutex __my_debug_lck(log_mutex);
        output(formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                             distillery_gettid(), Directory::sbasename(file), function, line,
                             _format, message, false));
    } else {
        output(formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(),
                             distillery_gettid(), Directory::sbasename(file), function, line,
                             _format, message, false));
    }
}

void ConsoleLogger::writeMessage(const string& aspect,
                                 const string& key,
                                 const vector<string>& substitutionTexts)
{
    ResourceBundleHelper rb;
    stringstream strm;
    strm << PrintMessageToString(rb, key, substitutionTexts);
    writeMessage(0 /*level not matter here*/, aspect, "", "", "", 0, strm.str());
}

void ConsoleLogger::output(const string& msg)
{
    if (_toStdout)
        cout << msg << endl;
    else
        cerr << msg << endl;
}

int ConsoleLogger::getFD() const
{
    return 1;
}
