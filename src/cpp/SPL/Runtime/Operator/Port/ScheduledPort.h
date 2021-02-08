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

#ifndef SPL_RUNTIME_OPERATOR_PORT_SCHEDULED_PORT_H
#define SPL_RUNTIME_OPERATOR_PORT_SCHEDULED_PORT_H

#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/ScheduledQueue.h>
#include <SPL/Runtime/Type/Tuple.h>

namespace SPL {
class OperatorImpl;
class ScheduledPort : public PortSignal
{
  public:
    ScheduledPort(OperatorImpl& oper, uint32_t index, ScheduledQueue& q);
    virtual ~ScheduledPort() {}

    void submit(Tuple& tuple) ALWAYS_INLINE
    {
        ScheduledData data(tuple, _port, index_, _opMetric);
        submit(data);
    }

    void submit(Punctuation& punct)
    {
        ScheduledData data(punct, _port, index_, _opMetric);
        submit(data);
    }

    virtual bool sendRawBufferData() { return false; }
    void submit(NativeByteBuffer& buffer) {}

    void shutdown();

  private:
    void submit(ScheduledData& data) { _queue.push(data); }

    ProcessSignal* _port;
    OperatorMetricsImpl& _opMetric;
    ScheduledQueue& _queue;
};
}

#endif /* SPL_RUNTIME_OPERATOR_PORT_SCHEDULED_PORT_H */
