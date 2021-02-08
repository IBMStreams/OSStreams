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
//
// Title-
//       RingTracer.h
//
// Purpose-
//       Debugging Tracer using a ring buffer
//
//----------------------------------------------------------------------------

#ifndef __RING_TRACER_H__
#define __RING_TRACER_H__

#include <list>
#include <string>

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>

UTILS_NAMESPACE_BEGIN
DEBUG_NAMESPACE_BEGIN

class RingTracer : public Tracer
{

  protected:
    unsigned int _size;
    std::list<std::string> _ring;
    Mutex _mutex;
    Tracer* _output_tracer;

  public:
    RingTracer(Tracer* output_tracer, unsigned int size);
    virtual ~RingTracer();

    /* ----------------- trace interface ----------------- */

    virtual void writeMessage(int level,
                              const std::string& aspect,
                              const std::string& args,
                              const std::string& function,
                              const std::string& file,
                              int line,
                              const std::string& message);

    virtual void writeMessage(const std::string& message);

    /* ---------------------------------------------------- */

    void dump(std::ostream& strm);
};

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
