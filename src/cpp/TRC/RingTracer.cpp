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

#include <signal.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/RingTracer.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

static RingTracer* current_instance = NULL;

static void signal_handler(int sig)
{
    if (current_instance) {
        current_instance->dump(cerr);
    }
}

RingTracer::RingTracer(Tracer* output_tracer, unsigned int size)
  : Tracer(0)
  , _size(size)
  , _output_tracer(output_tracer)
{
    _format = getTrcFormat();
    current_instance = this;
    signal(SIGUSR2, &signal_handler);
}

RingTracer::~RingTracer()
{
    delete _output_tracer;
}

void RingTracer::writeMessage(int level,
                              const string& aspect,
                              const string& args,
                              const string& function,
                              const string& file,
                              int line,
                              const string& message)
{
    string msg =
      formatMessage(level, aspect, args, debug::TimeStamp::TOD_ms(), getpid(), distillery_gettid(),
                    Directory::sbasename(file), function, line, _format, message);

    writeMessage(msg);
}

void RingTracer::writeMessage(const string& message)
{
    AutoMutex lck(_mutex);
    _ring.push_back(message);
    while (_ring.size() > _size) {
        _ring.pop_front();
    }
}

void RingTracer::dump(ostream& strm)
{
    AutoMutex lck(_mutex);

    list<string>::const_iterator it = _ring.begin();
    while (it != _ring.end()) {
        _output_tracer->writeMessage(*it);
        it++;
    }
}
