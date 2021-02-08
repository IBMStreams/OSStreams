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

#include <SPL/Debugger/SdbConsoleLogger.h>
#include <SPL/Debugger/StandaloneClient.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/TRCUtils.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;
using namespace SPL;

SdbConsoleLogger::SdbConsoleLogger(int level)
  : ConsoleTracer(level)
  , _standaloneClient(NULL)
{
    setFormat("log");
}

void SdbConsoleLogger::output(const string& msg)
{
    if (_standaloneClient != NULL) {
        _standaloneClient->asyncPrintMessage(msg);
    }
}

void SdbConsoleLogger::setFormat(std::string format)
{
    _format = Distillery::debug::getLogFormat(format);
    _formatName = format;
}

void SdbConsoleLogger::setLogLevel(int level)
{
    Distillery::debug::set_log_level(level);
}

SdbConsoleTracer::SdbConsoleTracer(int level)
  : SdbConsoleLogger(level)
{
    setFormat("default");
}

void SdbConsoleTracer::setFormat(std::string format)
{
    if (format == "default" || format == "") {
        _format = FORMAT_DEFAULT;
    } else if (format == "long") {
        _format = FORMAT_LONG;
    } else if (format == "short") {
        _format = FORMAT_SHORT;
    } else if (format == "spc") {
        _format = FORMAT_SPC;
    }
    _formatName = format;
}

void SdbConsoleTracer::setTraceLevel(int level)
{
    Distillery::debug::set_app_trclevel(level);
}
