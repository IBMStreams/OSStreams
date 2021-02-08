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

#ifndef SPL_RUNTIME_OPERATOR_PORT_ACTIVE_QUEUE_H
#define SPL_RUNTIME_OPERATOR_PORT_ACTIVE_QUEUE_H

#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/Port/Data.h>
#include <SPL/Runtime/Operator/Port/Inline.h>
#include <SPL/Runtime/Operator/Port/ProcessSignal.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/Type/Tuple.h>
#include <SPL/Runtime/Utility/Thread.h>
#include <SPL/Toolkit/CircularQueue.h>
#include <UTILS/CV.h>
#include <UTILS/Mutex.h>

#include <list>

namespace SPL {
class PE;
class ProcessSignal;
class OperatorImpl;
class OperatorMetricsImpl;

class ActiveQueue
  : public PortSignal
  , public Thread
{
    static const size_t MaxQueueSize = 1000;

  public:
    enum CongestionPolicy
    {
        Wait,
        DropFirst,
        DropLast
    };

    /// Constructor
    /// @param oper operator that holds this port
    /// @param index index of the port
    /// @param congestionMode congestion mode
    /// @param maxSize max queue size
    ActiveQueue(OperatorImpl& oper,
                uint32_t index,
                CongestionPolicy congestionMode,
                bool singleThreadedOnInput,
                uint32_t maxSize = MaxQueueSize);

    /// Destructor
    virtual ~ActiveQueue();

    /// Get max queue size
    /// @return max queue size
    uint32_t getMaxQueueSize() const { return maxSize_; }

    /// Submit a tuple on this port
    /// @param tuple tuple to submit
    void submit(Tuple& tuple)
    {
        Data item(tuple);
        submit(item);
    }

    /// Submit a punctuation on this port
    /// @param punct punctuation to submit
    void submit(Punctuation& punct)
    {
        Data item(punct);
        submit(item);
    }

    /// Run the active queue thread
    /// @param threadArgs thread arguments
    /// @returns return status pointer
    void* run(void* threadArgs);

    /// Shutdown the active queue
    void shutdown();

    /// Drain the active queue
    void drain();

    /// If threaded port then we revert to the tuple creation/serialization behaviour for now
    virtual bool sendRawBufferData() { return false; }
    void submit(NativeByteBuffer& buffer) {}

  private:
    inline void submit(Data& item) ALWAYS_INLINE
    {
        if (UNLIKELY(congestionMode_ == DropFirst)) {
            pushOneForDropFirst(item);
        } else {
            pushOneForNonFirst(item);
        }
    }

    inline void pushOneForNonFirst(Data& item) ALWAYS_INLINE
    {
        if (LIKELY(singleThreadedOnInput_)) {
            pushOneForNonFirstCore(item);
        } else {
            Distillery::AutoMutex am(mutex_);
            pushOneForNonFirstCore(item);
        }
    }

    inline void pushOneForNonFirstCore(Data& item) ALWAYS_INLINE
    {
        size_t queueLength;
        if (LIKELY(congestionMode_ == Wait) || UNLIKELY(!item.isTuple())) {
            // Always have to wait for punctuations
            if (queueLF_->full(&queueLength)) {
                operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nEnqueueWaits);
                timespec delay = { 0, 100000000 }; // 0.1 sec timeout
                Distillery::AutoMutex am(mutexProd_);
                prodWait_.store(true, boost::memory_order_relaxed);
                while (queueLF_->full(&queueLength)) {
                    if (UNLIKELY(isShutdown_.load(boost::memory_order_acquire))) {
                        prodWait_.store(false, boost::memory_order_relaxed);
                        return;
                    }
                    prodCV_.waitFor(mutexProd_, delay);
                }
                prodWait_.store(false, boost::memory_order_relaxed);
            }
        } else if (queueLF_->full(&queueLength)) { // DropLast and full (and isTuple())
            operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesDropped);
            APPTRC(L_DEBUG, "Dropping a tuple due to a full active queue and DropFirst/Last policy",
                   SPL_DATA_DBG);
            return;
        }

        Data& rearItem = queueLF_->rear();
        if (LIKELY(item.isTuple())) { // we have a tuple
            rearItem.setType(Data::Tuple);
            rearItem.getTuple().assign(item.getTuple());
        } else { // we have a punctuation
            rearItem.setType(Data::Punct);
            Punctuation& ref = rearItem.getPunctuation();
            ref = item.getPunctuation();
        }

        updateMetricsInPush(item, (int64_t)queueLength + 1);
        bool wasEmpty = queueLF_->empty();
        queueLF_->push_back();

        if (wasEmpty) {
            Distillery::AutoMutex am(mutexCons_);
            consCV_.signal();
        }
    }

    inline void updateMetricsInPush(Data& item, int64_t queueLength) ALWAYS_INLINE
    {
        if (LIKELY(item.isTuple())) { // we have a tuple
            operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesQueued);
        } else { // we have a punctuation
            updatePunctMetricsInPush(item);
        }
        operMetric_.updateMaxItemsQueuedNoLock(index_, queueLength);
    }

    // outlined into a function to get it off the critical path
    void updatePunctMetricsInPush(Data& item)
    {
        if (item.getPunctuation().getValue() == Punctuation::WindowMarker) {
            operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nWindowPunctsQueued);
        } else if (item.getPunctuation().getValue() == Punctuation::FinalMarker) {
            operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nFinalPunctsQueued);
        } else if (item.getPunctuation().getValue() == Punctuation::DrainMarker ||
                   item.getPunctuation().getValue() == Punctuation::ResetMarker ||
                   item.getPunctuation().getValue() == Punctuation::ResumeMarker ||
                   item.getPunctuation().getValue() == Punctuation::WatermarkMarker ||
                   item.getPunctuation().getValue() == Punctuation::SwitchMarker) {
            // Ignore - no metrics maintained
        } else {
            assert(!"cannot happen");
        }
    }

    inline bool popOneForWait() ALWAYS_INLINE
    {
        if (queueLF_->empty()) {
            Distillery::AutoMutex am(mutexCons_);
            long ns = 100;
            timespec delay;
            delay.tv_sec = 0;
            while (queueLF_->empty()) {
                delay.tv_nsec = ns;
                consCV_.waitFor(mutexCons_, delay);
                if (ns < 10000000) {
                    ns *= 10;
                }
                if (UNLIKELY(isShutdown_.load(boost::memory_order_acquire) ||
                             isDraining_.load(boost::memory_order_acquire))) {
                    return false;
                }
            }
        }

        Data& item = queueLF_->front();
        if (LIKELY(item.isTuple())) {
            signal_->submit(item.getTuple());
            item.getTuple().clear();
        } else {
            assert(item.getPunctuation().getValue() != Punctuation::Invalid);
            signal_->submit(item.getPunctuation());
        }

        updateMetricsInPop(item);
        queueLF_->pop_front();

        size_t len;
        if (queueLF_->empty(&len) ||
            (prodWait_.load(boost::memory_order_relaxed) && len < maxSize_ / 2)) {
            Distillery::AutoMutex am(mutexProd_);
            prodCV_.signal();
        }

        return true;
    }

    inline void updateMetricsInPop(Data& item) ALWAYS_INLINE
    {
        if (LIKELY(item.isTuple())) { // we have a tuple
            operMetric_.decrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesQueued);
        } else { // we have a punctuation
            updatePunctMetricsInPop(item);
        }
    }

    // outlined into a function to get it off the critical path
    void updatePunctMetricsInPop(Data& item)
    {
        if (item.getPunctuation().getValue() == Punctuation::WindowMarker) {
            operMetric_.decrementQueueCounterNoLock(index_, OperatorMetrics::nWindowPunctsQueued);
        } else if (item.getPunctuation().getValue() == Punctuation::FinalMarker) {
            operMetric_.decrementQueueCounterNoLock(index_, OperatorMetrics::nFinalPunctsQueued);
        } else if (item.getPunctuation().getValue() == Punctuation::DrainMarker ||
                   item.getPunctuation().getValue() == Punctuation::ResetMarker ||
                   item.getPunctuation().getValue() == Punctuation::ResumeMarker ||
                   item.getPunctuation().getValue() == Punctuation::WatermarkMarker ||
                   item.getPunctuation().getValue() == Punctuation::SwitchMarker) {
            // Ignore, no metrics maintained
        } else {
            assert(!"cannot happen");
        }
    }

    void popOneForDropFirst();
    void pushOneForDropFirst(Data& item);
    bool dropTupleFromQueue();

    Distillery::CV prodCV_;
    Distillery::CV consCV_;
    Distillery::Mutex mutex_;
    Distillery::Mutex mutexProd_;
    Distillery::Mutex mutexCons_;

    CongestionPolicy const congestionMode_;
    uint32_t const maxSize_;
    bool const singleThreadedOnInput_;
    uint32_t currentSize_;
    boost::atomic<bool> prodWait_;
    boost::atomic<bool> isShutdown_;
    boost::atomic<bool> isDraining_;
    std::list<Data> queue_;
    CircularQueue<Data>* queueLF_; // lock-free
    ProcessSignal* signal_;
    OperatorMetricsImpl& operMetric_;
};
}

#endif /* SPL_RUNTIME_OPERATOR_PORT_ACTIVE_QUEUE_H */
