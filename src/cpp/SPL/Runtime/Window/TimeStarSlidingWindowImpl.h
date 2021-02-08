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

#ifndef SPL_RUNTIME_WINDOW_IMPL_TIME_STAR_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_TIME_STAR_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Window/WindowThread.h>
#include <UTILS/CV.h>

namespace SPL {

template<class T, class G, class D, class S>
class TimeStarSlidingWindowImpl
  : public virtual SlidingWindowImpl<T, G, D, S>
  , public WindowEvictionThread::EventHandler
{
  public:
#include "SlidingTypedefs.h"

    typedef std::tr1::unordered_map<typename SPL::WindowImpl<T, G, D, S>::PartitionType,
                                    std::deque<double> >
      TimestampsMapType;

    TimeStarSlidingWindowImpl(WindowType& window, OperatorImpl& oper)
      : SlidingWindowImpl<T, G, D, S>(window)
      , oper_(oper)
    {
        WindowPolicy const& evict = window.getEvictionPolicy();
        TimeWindowPolicy const* cevict = dynamic_cast<TimeWindowPolicy const*>(&evict);
        assert(cevict);

        wsize_ = cevict->getSize();
        evictThread_.reset(new WindowEvictionThread(wsize_, oper_));
        evictThread_->registerHandler(this);
    }

    virtual ~TimeStarSlidingWindowImpl() {}

    void shutdown()
    {
        WindowImpl<T, G, D, S>::shutdown();
        Distillery::AutoMutex am(this->mutex_);
        cv_.signal();
    }

    void createEvictionThread() { evictThread_->create(); }

    void joinEvictionThread()
    {
        APPTRC(L_INFO, "Joining eviction thread...", SPL_WINDOW_DBG);
        evictThread_->join();
        APPTRC(L_INFO, "Joined eviction thread.", SPL_WINDOW_DBG);
    }

    StorageType const& acquireData() const
    {
        this->mutex_.lock();
        return this->data_;
    }

    void releaseData() const { this->mutex_.unlock(); }

    // handle the event and return the timestamp of the oldest tuple (a
    // value < 0 means termination is under way)
    double windowEvictionThreadEvent(WindowEvictionThread const& thread)
    {
        Distillery::AutoMutex am(this->mutex_);
        if (this->isShutdown_) {
            return -1.0;
        }

        size_t size = 0;
        typedef typename StorageType::iterator iter;
        for (iter it = this->data_.begin(); it != this->data_.end(); ++it) {
            size += it->second.size();
        }
        if (size == 0) {
            if (evictThread_->isSuspended()) {
                // Handler was suspended, don't wait for tuples to be inserted
                return evictThread_->getTime();
            }
            // it is fine if we wake up prematurely
            cv_.wait(this->mutex_);
        }
        if (this->isShutdown_) {
            return -1.0;
        }

        double oldest = -1.0;
        double ctime = thread.getTime();
        typedef typename StorageType::iterator iter;
        for (iter it = this->data_.begin(); it != this->data_.end(); ++it) {
            PartitionType const& partition = it->first;
            DataType& gdata = it->second;
            std::deque<double>& tdata = timestamps_[partition];
            assert(gdata.size() == tdata.size());
            if (gdata.empty()) {
                continue;
            }
            while (!gdata.empty()) {
                double tstamp = *tdata.begin();
                if (ctime - tstamp > wsize_) {
                    this->emitBeforeTupleEvictionEvent(*gdata.begin(), partition);
                    TupleType rtuple = *gdata.begin();
                    gdata.pop_front();
                    tdata.pop_front();
                    this->emitAfterTupleEvictionEvent(rtuple, partition);
                } else {
                    if (oldest < 0 || tstamp < oldest) {
                        oldest = tstamp;
                    }
                    break;
                }
            }
        }

        // clean-up empty partitions
        typedef typename StorageType::iterator iter;
        for (iter it = this->data_.begin(); it != this->data_.end();) {
            PartitionType const& partition = it->first;
            DataType& gdata = it->second;
            if (gdata.empty()) {
                this->partitionRemoved(partition);
                timestamps_.erase(partition);
                it = this->data_.erase(it);
            } else {
                it++;
            }
        }

        while (oldest < 0) { // no tuples in window
            if (evictThread_->isSuspended()) {
                // Handler was suspended, don't wait for tuples to be inserted
                return evictThread_->getTime();
            }
            cv_.wait(this->mutex_);
            if (this->isShutdown_) {
                return -1.0;
            }

            for (iter it = this->data_.begin(); it != this->data_.end(); ++it) {
                PartitionType const& partition = it->first;
                DataType& gdata = it->second;
                std::deque<double>& tdata = timestamps_[partition];
                assert(gdata.size() == tdata.size());
                if (gdata.empty()) {
                    continue;
                }
                double tstamp = *tdata.begin();
                if (oldest < 0 || tstamp < oldest) {
                    oldest = tstamp;
                }
            }
        }
        return oldest;
    }

    void insert(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        // sliding windows with time based eviction policies
        // are assumed to be full when they start out
        this->emitOnWindowInitialFullEventIfNeeded(partition);

        this->emitBeforeTupleInsertionEvent(tuple, partition);
        gdata.push_back(tuple);
        std::deque<double>& tdata = timestamps_[partition];
        tdata.push_back(evictThread_->getTime());
        if (gdata.size() == 1 && this->data_.size() == 1) {
            this->cv_.signal();
        }
        this->emitAfterTupleInsertionEvent(gdata.back(), partition);
        this->tupleInserted(partition, gdata.back());
    }

    void beforeTupleInserted()
    {
        this->evictThread_->resume();
        SlidingWindowImpl<T, G, D, S>::beforeTupleInserted();
    }

    void partitionEvicted(PartitionType const& partition) { timestamps_.erase(partition); }

    void drain()
    {
        AutoSuspenderCV<WindowEvictionThread>::suspend(*this->evictThread_, this->mutex_,
                                                       this->cv_);
        SlidingWindowImpl<T, G, D, S>::drain();
    }

    void checkpoint(Checkpoint& ckptStream) const
    {
        // Suspend window tasks only if running outside a consistent region
        AutoSuspenderCV<WindowEvictionThread> ase(*this->evictThread_, this->mutex_, this->cv_,
                                                  false);

        // Here eviction task is suspended, mutex is not acquired

        SlidingWindowImpl<T, G, D, S>::checkpoint(ckptStream);
    }

    void reset(Checkpoint& ckptStream)
    {
        AutoSuspenderCV<WindowEvictionThread> ase(*this->evictThread_, this->mutex_, this->cv_);

        // Here eviction task is suspended, mutex is not acquired

        SlidingWindowImpl<T, G, D, S>::reset(ckptStream);
    }

    void resetToInitialState()
    {
        AutoSuspenderCV<WindowEvictionThread> ase(*this->evictThread_, this->mutex_, this->cv_);

        // Here eviction task is suspended, mutex is not acquired

        SlidingWindowImpl<T, G, D, S>::resetToInitialState();
    }

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = 0;
        for (typename TimestampsMapType::const_iterator it = timestamps_.begin();
             it != timestamps_.end(); ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);

            std::deque<double> const& stamps = it->second;
            for (std::deque<double>::const_iterator it2 = stamps.begin(); it2 != stamps.end();
                 ++it2) {
                result ^= std::tr1::hash<double>()(*it2);
            }
        }

        // TODO hash the eviction thread's data here
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        WRITE_MARKER(ckptStream, "TimeStarSlidingWindowImpl");
        ckptStream << static_cast<uint64_t>(timestamps_.size());
        for (typename TimestampsMapType::const_iterator it = timestamps_.begin();
             it != timestamps_.end(); ++it) {
            ckptStream << it->first;
            std::deque<double> const& stamps = it->second;
            ckptStream << static_cast<uint64_t>(stamps.size());
            for (std::deque<double>::const_iterator it2 = stamps.begin(); it2 != stamps.end();
                 ++it2) {
                ckptStream << *it2;
            }
        }

        // TODO checkpoint thread's data here
    }

    void doReset(Checkpoint& ckptStream)
    {
        VERIFY_MARKER(ckptStream, "TimeStarSlidingWindowImpl");
        for (uint64_t partitionCount = ckptStream.getUInt64(); partitionCount > 0;
             --partitionCount) {
            PartitionType partition;
            ckptStream >> partition;
            std::deque<double>& stamps = timestamps_[partition];
            for (uint64_t tupleCount = ckptStream.getUInt64(); tupleCount > 0; --tupleCount) {
                stamps.push_back(ckptStream.getDouble());
            }
        }

        // TODO restore thread's data
    }

    void doResetToInitialState(int cause)
    {
        timestamps_.clear();

        // TODO reset window thread
    }

    void print(std::ostream& stream) const
    {
        stream << "TimeStarSlidingWindowImpl={";
        stream << "timestamps_[size=" << static_cast<uint64_t>(timestamps_.size());
        stream << "]={";
        for (typename TimestampsMapType::const_iterator it = timestamps_.begin();
             it != timestamps_.end(); ++it) {
            PartitionType const partition = it->first;
            stream << "partition=" << partition;

            std::deque<double> const& stamps = it->second;
            stream << ", stamps[size=" << stamps.size();
            stream << "]={";
            size_t printCount = 10; // print at most this many timestamps
            for (std::deque<double>::const_iterator it2 = stamps.begin(); it2 != stamps.end();
                 ++it2) {
                stream << *it2 << ", ";
                if (printCount == 0) {
                    // we reached the maximum number of items to print
                    stream << "...";
                    break;
                } else {
                    printCount--;
                }
            }
            stream << "}, ";
        }
        stream << "} ";

        // TODO print window thread state
        stream << "}";
    }

  protected:
    mutable Distillery::CV cv_;

    // TODO const
    double wsize_;
    OperatorImpl& oper_;
    std::auto_ptr<WindowEvictionThread> evictThread_;
    TimestampsMapType timestamps_;
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_TIME_STAR_SLIDING_WINDOW_IMPL_H */
