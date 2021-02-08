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

#ifndef SPL_RUNTIME_WINDOW_IMPL_STAR_TIME_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_STAR_TIME_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/WindowThread.h>

namespace SPL {

template<class T, class G, class D, class S>
class StarTimeSlidingWindowImpl
  : public virtual SlidingWindowImpl<T, G, D, S>
  , public WindowTriggerThread::EventHandler
{
  public:
#include "SlidingTypedefs.h"
    StarTimeSlidingWindowImpl(WindowType& window, OperatorImpl& oper)
      : SlidingWindowImpl<T, G, D, S>(window)
    {
        WindowPolicy const& trigger = window.getTriggerPolicy();
        TimeWindowPolicy const* ctrigger = dynamic_cast<TimeWindowPolicy const*>(&trigger);
        assert(ctrigger);

        tsize_ = ctrigger->getSize();
        triggerThread_.reset(new WindowTriggerThread(tsize_, oper));
        triggerThread_->registerHandler(this);
    }

    virtual ~StarTimeSlidingWindowImpl() {}

    void createTriggerThread() { triggerThread_->create(); }

    void joinTriggerThread()
    {
        APPTRC(L_INFO, "Joining trigger thread...", SPL_WINDOW_DBG);
        triggerThread_->join();
        APPTRC(L_INFO, "Joined trigger thread.", SPL_WINDOW_DBG);
    }

    StorageType const& acquireData() const
    {
        this->mutex_.lock();
        return this->data_;
    }

    void releaseData() const { this->mutex_.unlock(); }

    bool windowTriggerThreadEvent(WindowTriggerThread const&)
    {
        Distillery::AutoMutex am(this->mutex_);

        // Perform any actions necessary before processing the trigger event
        this->beforeTriggerEventProcessing();

        if (this->data_.empty()) {
            // an empty window, fake a partition with no data
            PartitionType partition;
            this->insertPartition(partition);
            this->updateOrderedPartitionMap(partition);
            this->emitOnWindowTriggerEvent(partition);
            this->removePartition(partition);
        } else {
            typedef typename StorageType::iterator iter;
            for (iter it = this->data_.begin(); it != this->data_.end(); ++it) {
                PartitionType const& partition = it->first;
                this->emitOnWindowTriggerEvent(partition);
            }
        }
        return !this->isShutdown_;
    }

    void beforeTupleInserted()
    {
        this->triggerThread_->resume();
        SlidingWindowImpl<T, G, D, S>::beforeTupleInserted();
    }

    void drain()
    {
        AutoSuspender<WindowTriggerThread>::suspend(*this->triggerThread_);
        SlidingWindowImpl<T, G, D, S>::drain();
    }

    void checkpoint(Checkpoint& ckptStream) const
    {
        // Suspend window tasks only if running outside a consistent region
        AutoSuspender<WindowTriggerThread> as(*this->triggerThread_, false);
        SlidingWindowImpl<T, G, D, S>::checkpoint(ckptStream);
    }

    void reset(Checkpoint& ckptStream)
    {
        AutoSuspender<WindowTriggerThread> as(*this->triggerThread_);
        SlidingWindowImpl<T, G, D, S>::reset(ckptStream);
    }

    void resetToInitialState()
    {
        AutoSuspender<WindowTriggerThread> as(*this->triggerThread_);
        SlidingWindowImpl<T, G, D, S>::resetToInitialState();
    }

#ifndef NDEBUG
    virtual size_t hash() const
    {
        size_t result = 0;
        // TODO hash thread's data here
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        WRITE_MARKER(ckptStream, "StarTimeSlidingWindowImpl");
        // TODO checkpoint thread's data here
    }

    void doReset(Checkpoint& ckptStream)
    {
        VERIFY_MARKER(ckptStream, "StarTimeSlidingWindowImpl");
        // TODO restore thread's data
    }

    void doResetToInitialState(int cause)
    {
        // TODO reset window thread
    }

    void print(std::ostream& stream) const
    {
        stream << "StarTimeSlidingWindowImpl={tsize_=" << this->tsize_;
        // TODO print window thread state
        stream << "}";
    }

  protected:
    // TODO const
    double tsize_;
    std::auto_ptr<WindowTriggerThread> triggerThread_;
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_STAR_TIME_SLIDING_WINDOW_IMPL_H */
