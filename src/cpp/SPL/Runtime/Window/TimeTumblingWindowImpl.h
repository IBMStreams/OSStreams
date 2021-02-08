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

#ifndef SPL_RUNTIME_WINDOW_IMPL_TIME_TUMBLING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_TIME_TUMBLING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/WindowThread.h>
#include <UTILS/Mutex.h>

namespace SPL {

template<class T, class G, class D, class S>
class TimeTumblingWindowImpl
  : public TumblingWindowImpl<T, G, D, S>
  , public WindowTriggerThread::EventHandler
{
  public:
#include "TumblingTypedefs.h"
    TimeTumblingWindowImpl(WindowType& window, OperatorImpl& oper)
      : TumblingWindowImpl<T, G, D, S>(window)
      , oper_(oper)
    {
        initialize(window);
    }

    TimeTumblingWindowImpl(WindowType& window,
                           OperatorImpl& oper,
                           PartitionEvictionImpl<T, G, D, S>& pEP)
      : TumblingWindowImpl<T, G, D, S>(window, pEP)
      , oper_(oper)
    {
        initialize(window);
    }

    void initialize(WindowType& window)
    {
        WindowPolicy const& evict = window.getEvictionPolicy();
        TimeWindowPolicy const* cevict = dynamic_cast<TimeWindowPolicy const*>(&evict);
        assert(cevict);

        size_ = cevict->getSize();
        // for tumbling windows, eviction is more like a trigger
        evictThread_.reset(new WindowTriggerThread(size_, oper_));
        evictThread_->registerHandler(this);
    }

    ~TimeTumblingWindowImpl() {}

    void createEvictionThread() { evictThread_->create(); }

    void joinEvictionThread()
    {
        APPTRC(L_INFO, "Joining eviction thread...", SPL_WINDOW_DBG);
        evictThread_->join();
        APPTRC(L_INFO, "Joined eviction thread.", SPL_WINDOW_DBG);
    }

    StorageType const& acquireData() const
    {
        mutex_.lock();
        return this->data_;
    }

    void releaseData() const { mutex_.unlock(); }

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        // Tuple insertion resumes the eviction thread if suspended
        evictThread_->resume();

        Distillery::AutoMutex am(mutex_);

        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        DataType& gdata = this->insertPartition(partition);
        if (this->isUsingSummarization()) { // not keeping the tuples
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            if (!this->hasSummarizer(partition)) {
                this->openWindowForSummarizer(partition);
            }
            this->addTupleForSummarizer(tuple, partition);
            this->emitAfterTupleInsertionEventForConstTuple(tuple, partition);
            this->incrementTupleCount(partition);
            this->tupleInserted(partition, tuple);
        } else { // keeping the tuples
            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            gdata.push_back(tuple);
            this->emitAfterTupleInsertionEvent(gdata.back(), partition);
            this->tupleInserted(partition, tuple);
        }
    }

    // eviction is more like trigger for tumbling windows
    bool windowTriggerThreadEvent(WindowTriggerThread const&)
    {
        Distillery::AutoMutex am(mutex_);

        // Perform any actions necessary before processing the trigger event
        this->beforeTriggerEventProcessing();

        if (this->isUsingSummarization()) { // not keeping the tuples
            if (this->data_.empty()) {
                // an empty window, fake a partition with no data
                PartitionType partition = PartitionType();
                this->insertPartition(partition);
                this->updateOrderedPartitionMap(partition);
                this->openWindowForSummarizer(partition);
                this->closeWindowForSummarizer(partition);
                this->emitBeforeWindowFlushEvent(partition);
                this->emitAfterWindowFlushEvent(partition);
                this->removeWindowForSummarizer(partition);
                this->removePartition(partition);
            } else {
                while (!this->data_.empty()) {
                    typename StorageType::iterator it = this->data_.begin();
                    PartitionType const& partition = it->first;
                    this->closeWindowForSummarizer(partition);
                    this->emitBeforeWindowFlushEvent(partition);
                    this->setTupleCount(partition, 0);
                    this->emitAfterWindowFlushEvent(partition);
                    this->removeWindowForSummarizer(partition);
                    this->eraseTupleCount(partition);
                    this->removePartition(partition);
                }
            }
        } else { // keeping the tuples
            // eviction
            if (this->data_.empty()) {
                // an empty window, fake a partition with no data
                PartitionType partition = PartitionType();
                this->insertPartition(partition);
                this->updateOrderedPartitionMap(partition);
                this->emitBeforeWindowFlushEvent(partition);
                this->emitAfterWindowFlushEvent(partition);
                this->removePartition(partition);
            } else {
                while (!this->data_.empty()) {
                    typename StorageType::iterator it = this->data_.begin();
                    PartitionType const& partition = it->first;
                    this->emitBeforeWindowFlushEvent(partition);
                    it->second.clear();
                    this->emitAfterWindowFlushEvent(partition);
                    this->removePartition(partition);
                }
            }
        }
        return !this->isShutdown_;
    }

    void drain()
    {
        AutoSuspender<WindowTriggerThread>::suspend(*evictThread_);
        TumblingWindowImpl<T, G, D, S>::drain();
    }

    void checkpoint(Checkpoint& ckptStream) const
    {
        // Suspend window tasks only if running outside a consistent region
        AutoSuspender<WindowTriggerThread> as(*this->evictThread_, false);
        TumblingWindowImpl<T, G, D, S>::checkpoint(ckptStream);
    }

    void reset(Checkpoint& ckptStream)
    {
        AutoSuspender<WindowTriggerThread> as(*this->evictThread_);
        TumblingWindowImpl<T, G, D, S>::reset(ckptStream);
    }

    void resetToInitialState()
    {
        AutoSuspender<WindowTriggerThread> as(*this->evictThread_);
        TumblingWindowImpl<T, G, D, S>::resetToInitialState();
    }

#ifndef NDEBUG
    size_t hash() const
    {
        // Parent first
        size_t result = TumblingWindowImpl<T, G, D, S>::hash();
        // TODO hash thread's data here
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        // Parent first
        TumblingWindowImpl<T, G, D, S>::doCheckpoint(ckptStream);

        WRITE_MARKER(ckptStream, "TimeTumblingWindowImpl");
        // TODO checkpoint thread's data here
    }

    void doReset(Checkpoint& ckptStream)
    {
        // Parent first
        TumblingWindowImpl<T, G, D, S>::doReset(ckptStream);

        VERIFY_MARKER(ckptStream, "TimeTumblingWindowImpl");
        // TODO restore thread's data
    }

    void doResetToInitialState(int cause)
    {
        // TODO reset window thread

        // Parent last
        TumblingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        stream << "TimeTumblingWindowImpl={";

        // Parent first
        TumblingWindowImpl<T, G, D, S>::print(stream);

        // TODO print window thread state
        stream << "}}";
    }

  private:
    double size_;
    OperatorImpl& oper_;
    mutable Distillery::Mutex mutex_;
    std::auto_ptr<WindowTriggerThread> evictThread_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_TIME_TUMBLING_WINDOW_IMPL_H */
