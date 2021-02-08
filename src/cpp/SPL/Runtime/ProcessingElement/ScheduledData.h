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

#ifndef SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_DATA_H
#define SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_DATA_H

#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/Data.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Toolkit/CircularQueue.h>

namespace SPL {
class ScheduledData
{
  public:
    ScheduledData()
      : _port(NULL)
      , _opIndex(0)
      , _opMetric(NULL)
      , _queue(NULL)
    {}
    ScheduledData(Tuple& tuple, ProcessSignal* sig, uint32_t i, OperatorMetricsImpl& m)
      : _data(tuple)
      , _port(sig)
      , _opIndex(i)
      , _opMetric(&m)
      , _queue(NULL)
    {}
    ScheduledData(Punctuation& punct, ProcessSignal* sig, uint32_t i, OperatorMetricsImpl& m)
      : _data(punct)
      , _port(sig)
      , _opIndex(i)
      , _opMetric(&m)
      , _queue(NULL)
    {}

    Data& data() { return _data; }
    ProcessSignal* port() { return _port; }

    void copy(ScheduledData& other)
    {
        _port = other._port;
        _opIndex = other._opIndex;
        _opMetric = other._opMetric;

        if (other._data.isTuple()) {
            _data.setType(Data::Tuple);
            _data.getTuple().assign(other._data.getTuple());
        } else {
            _data.setType(Data::Punct);
            Punctuation& punct = _data.getPunctuation();
            punct = other._data.getPunctuation();
        }
    }

    void incrementQueueCounter(int64_t queueLength)
    {
        if (_data.isTuple()) {
            _opMetric->incrementQueueCounterNoLock(_opIndex, OperatorMetrics::nTuplesQueued);
        } else {
            if (_data.getPunctuation().getValue() == Punctuation::WindowMarker) {
                _opMetric->incrementQueueCounterNoLock(_opIndex,
                                                       OperatorMetrics::nWindowPunctsQueued);
            } else if (_data.getPunctuation().getValue() == Punctuation::FinalMarker) {
                _opMetric->incrementQueueCounterNoLock(_opIndex,
                                                       OperatorMetrics::nFinalPunctsQueued);
            }
        }
        _opMetric->updateMaxItemsQueuedNoLock(_opIndex, queueLength);
    }

    void decrementQueueCounter()
    {
        if (_data.isTuple()) {
            _opMetric->decrementQueueCounterNoLock(_opIndex, OperatorMetrics::nTuplesQueued);
        } else {
            if (_data.getPunctuation().getValue() == Punctuation::WindowMarker) {
                _opMetric->decrementQueueCounterNoLock(_opIndex,
                                                       OperatorMetrics::nWindowPunctsQueued);
            } else if (_data.getPunctuation().getValue() == Punctuation::FinalMarker) {
                _opMetric->decrementQueueCounterNoLock(_opIndex,
                                                       OperatorMetrics::nFinalPunctsQueued);
            }
        }
    }

    void incrementWaitCounter()
    {
        _opMetric->incrementQueueCounterNoLock(_opIndex, OperatorMetrics::nEnqueueWaits);
    }

    void setQueue(CircularQueue<ScheduledData>* q) { _queue = q; }

    void pop()
    {
        assert(_queue != NULL);
        _queue->pop_front();
        _queue = NULL;
    }

  private:
    Data _data;
    ProcessSignal* _port;
    uint32_t _opIndex;
    OperatorMetricsImpl* _opMetric;
    CircularQueue<ScheduledData>* _queue;
};
}

#endif /* SPL_RUNTIME_PROCESSING_ELEMENT_SCHEDULED_DATA_H */
