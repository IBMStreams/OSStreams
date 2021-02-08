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
#include <TRC/ThColorConsoleTracer.h>
#include <pthread.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

const char* const ThColorConsoleTracer::_thcolors[] = {
    "0;30",    "0;31",    "0;32",    "0;33",    "0;34",    "0;35",    "0;36",
    "0;37",    "1;30",    "1;31",    "1;32",    "1;33",    "1;34",    "1;35",
    "1;36",    "1;37",    "0;31;40", "0;32;40", "0;33;40", "0;34;40", "0;35;40",
    "0;36;40", "0;37;40", "1;31;40", "1;32;40", "1;33;40", "1;34;40", "1;35;40",
    "1;36;40", "1;37;40", "0;30;47", "0;31;47", "0;32;47", "0;34;47", "0;35;47",
    "0;36;47", "1;30;47", "1;31;47", "1;32;47", "1;34;47", "1;35;47", "1;36;47"
};

ThColorConsoleTracer::ThColorConsoleTracer(const string& color, int level, bool toStdout)
  : ColorConsoleTracer(color, level, toStdout)
{
    if (_use_colors)
        _mytid = pthread_self();
}

ThColorConsoleTracer::~ThColorConsoleTracer() {}

void ThColorConsoleTracer::output(const string& msg)
{
    if (_use_colors) {
        unsigned colorid = (pthread_self() - _mytid) % ((sizeof(_thcolors) / sizeof(char*)) + 1);
        stringstream m;
        if (!colorid)
            m << "\033[" << _color << "m" << msg << "\033[00m";
        else
            m << "\033[" << _thcolors[colorid - 1] << "m" << msg << "\033[00m";
        ConsoleTracer::output(m.str());
    } else {
        ConsoleTracer::output(msg);
    }
}
