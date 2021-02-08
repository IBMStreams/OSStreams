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

#include <SPL/Runtime/Operator/Port/ActiveQueue.h>

#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>

using namespace Distillery;
using namespace SPL;
using namespace std;

ActiveQueue::ActiveQueue(OperatorImpl& oper,
                         uint32_t index,
                         CongestionPolicy congestionMode,
                         bool singleThreadedOnInput,
                         uint32_t maxSize /*=MaxQueueSize*/)
  : PortSignal(oper, Active, index, false)
  , congestionMode_(congestionMode)
  , maxSize_(maxSize ? maxSize : 1)
  , singleThreadedOnInput_(singleThreadedOnInput)
  , currentSize_(0)
  , prodWait_(false)
  , isShutdown_(false)
  , isDraining_(false)
  , queueLF_(NULL)
  , signal_(oper_.process_[index_].get())
  , operMetric_(oper.getContextImpl().getMetricsImpl())
{
    // we want to use a lock-free implementation for this case
    if (congestionMode_ != DropFirst) {
        queueLF_ = new CircularQueue<Data>(maxSize_);
    }
    oper.registerActiveQueue(*this, index);
    this->create();
    if (congestionMode_ != DropFirst) {
        for (size_t i = 0, iu = queueLF_->getCapacity(); i < iu; ++i) {
            Data& item = queueLF_->getRawData(i);
            item.setType(Data::Tuple);
            Tuple* tuple = signal_->createTuple().release();
            item.setTuple(*tuple);
            Punctuation* p = new Punctuation(Punctuation::Invalid);
            item.setPunctuation(*p);
        }
    }
}

ActiveQueue::~ActiveQueue()
{
    if (congestionMode_ != DropFirst) {
        for (size_t i = 0, iu = queueLF_->getCapacity(); i < iu; ++i) {
            Data& item = queueLF_->getRawData(i);
            if (item.hasTuple()) {
                delete &(item.getTuple());
            } else if (item.hasPunctuation()) {
                delete &(item.getPunctuation());
            }
        }
        delete queueLF_;
    } else {
        typedef std::list<Data>::iterator iter;
        for (iter it = queue_.begin(); it != queue_.end(); ++it) {
            if (it->hasTuple()) {
                delete &(it->getTuple());
            } else if (it->hasPunctuation()) {
                delete &(it->getPunctuation());
            }
        }
    }
}

void ActiveQueue::shutdown()
{
    isShutdown_.store(true, boost::memory_order_release);
    if (congestionMode_ != Wait) {
        Distillery::AutoMutex am(mutex_);
        prodCV_.broadcast();
        consCV_.broadcast();
    } else {
        {
            Distillery::AutoMutex am(mutexProd_);
            prodCV_.signal();
        }
        {
            Distillery::AutoMutex am(mutexCons_);
            consCV_.broadcast();
        }
    }
}

void ActiveQueue::drain()
{
    isDraining_.store(true, boost::memory_order_release);
    if (congestionMode_ != Wait) {
        Distillery::AutoMutex am(mutex_);
        prodCV_.broadcast();
        consCV_.broadcast();
    } else {
        {
            Distillery::AutoMutex am(mutexProd_);
            prodCV_.signal();
        }
        {
            Distillery::AutoMutex am(mutexCons_);
            consCV_.broadcast();
        }
    }
}

void* ActiveQueue::run(void* threadArgs)
{
    uint32_t opIndex = oper_.getContextImpl().getIndex();
    PEImpl& pe = PEImpl::instance();
    const string& opName = pe.getOperatorName(opIndex);
    registerThread(opName);

    OperatorTracker::init();
    try {
        if (congestionMode_ != DropFirst) {
            while (
              LIKELY(!(isShutdown_.load(boost::memory_order_acquire) ||
                       (isDraining_.load(boost::memory_order_acquire) && queueLF_->empty())))) {
                popOneForWait();
            }
        } else {
            while (LIKELY(!(isShutdown_.load(boost::memory_order_acquire) ||
                            (isDraining_.load(boost::memory_order_acquire) && queue_.empty())))) {
                popOneForDropFirst();
            }
        }
    } catch (const Distillery::DistilleryException& e) {
        pe.handleOperatorFailure("Exception when executing operator: ", e.getExplanation());
        pe.shutdown();
    } catch (const std::exception& e) {
        pe.handleOperatorFailure("Exception when executing operator: ", e.what());
        pe.shutdown();
    } catch (...) {
        pe.handleOperatorFailure("Unknown exception when executing operator: ",
                                 SPL_RUNTIME_UNKNOWN_EXCEPTION);
        pe.shutdown();
    }

    OperatorTracker::finalize();

    return NULL;
}

void ActiveQueue::popOneForDropFirst()
{
    Data item;
    {
        Distillery::AutoMutex am(mutex_);
        while (currentSize_ == 0) {
            consCV_.wait(mutex_);
            if (isShutdown_.load(boost::memory_order_acquire) ||
                isDraining_.load(boost::memory_order_acquire)) {
                return;
            }
        }

        item = *queue_.begin();
        --currentSize_;
        updateMetricsInPop(item);
        queue_.pop_front();

        if (currentSize_ == maxSize_ - 1) {
            prodCV_.signal();
        }
    }
    if (item.isTuple()) {
        auto_ptr<Tuple> tuple(&item.getTuple());
        signal_->submit(*tuple);
        tuple->clear();
    } else {
        auto_ptr<Punctuation> punct(&item.getPunctuation());
        signal_->submit(*punct);
    }
}

void ActiveQueue::pushOneForDropFirst(Data& item)
{
    Distillery::AutoMutex am(mutex_);
    if (currentSize_ == maxSize_) {
        bool wait = false;
        if (item.isTuple()) {            // we have a tuple
            if (!dropTupleFromQueue()) { // cannot find an earlier tuple
                operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesDropped);
                APPTRC(L_DEBUG, "Dropping a tuple due to a full active queue and DropFirst policy",
                       SPL_DATA_DBG);
                return; // to drop, we drop the current one
            }
        } else { // we have punctuation
            wait = !dropTupleFromQueue();
        }

        if (wait) {
            operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nEnqueueWaits);
            while (currentSize_ == maxSize_) {
                prodCV_.wait(mutex_);
                if (isShutdown_.load(boost::memory_order_acquire)) {
                    return;
                }
            }
        }
    }

    ++currentSize_;
    updateMetricsInPush(item, (int64_t)currentSize_);
    if (item.isTuple()) { // make a copy
        item.setTuple(*item.getTuple().clone());
    } else { // also make a copy
        item.setPunctuation(*(new Punctuation(item.getPunctuation())));
    }
    queue_.push_back(item);
    if (currentSize_ == 1) {
        consCV_.signal();
    }
}

bool ActiveQueue::dropTupleFromQueue()
{
    bool dropped = false;
    std::list<Data>::iterator it;
    for (it = queue_.begin(); it != queue_.end(); ++it) {
        if (it->isTuple()) {
            delete &(it->getTuple());
            --currentSize_;
            queue_.erase(it);
            dropped = true;
            break;
        }
    }

    if (dropped) {
        operMetric_.incrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesDropped);
        operMetric_.decrementQueueCounterNoLock(index_, OperatorMetrics::nTuplesQueued);
        APPTRC(L_DEBUG, "Dropping a tuple due to a full active queue and DropFirst policy",
               SPL_DATA_DBG);
    }
    return dropped;
}
