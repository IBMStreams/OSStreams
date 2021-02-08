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

#ifndef SPL_RUNTIME_WINDOW_IMPL_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Window/WindowImpl.h>
#include <SPL/Runtime/Window/WindowTracker.h>
#include <UTILS/Mutex.h>

namespace SPL {

template<class T, class G, class D, class S>
class SlidingWindowImpl : public WindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    typedef
      typename std::tr1::unordered_set<PartitionType>::const_iterator PartitionSetConstIterator;

    SlidingWindowImpl(WindowType& window)
      : WindowImpl<T, G, D, S>(window)
      , window_(window)
    {}
    SlidingWindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& partitionEviction)
      : WindowImpl<T, G, D, S>(window, partitionEviction)
      , window_(window)
    {}
    virtual ~SlidingWindowImpl() {}

    virtual void joinTriggerThread() {}

    void emitBeforeTupleEvictionEvent(TupleType& tuple, PartitionType const& partition)
    {
        if (window_.beforeTupleEviction_) {
            window_.beforeTupleEviction_->beforeTupleEvictionEvent(window_, tuple, partition);
        }
        this->tupleRemoved(partition, tuple);
    }

    void emitAfterTupleEvictionEvent(TupleType& tuple, PartitionType const& partition)
    {
        if (window_.afterTupleEviction_) {
            window_.afterTupleEviction_->afterTupleEvictionEvent(window_, tuple, partition);
        }
    }

    void emitOnWindowInitialFullEventIfNeeded(PartitionType const& partition)
    {
        if (windowFull_.count(partition) == 0) {
            windowFull_.insert(partition);
            emitOnWindowInitialFullEvent(partition);
        }
    }

    virtual void partitionEvicted(PartitionType const& partition) { windowFull_.erase(partition); }

    void emitOnWindowInitialFullEvent(PartitionType const& partition)
    {
        if (window_.onWindowInitialFull_) {
            window_.onWindowInitialFull_->onWindowInitialFullEvent(window_, partition);
        }
    }

    void emitOnWindowTriggerEvent(PartitionType const& partition)
    {
        if (window_.onWindowTrigger_) {
            AutoSetWindow asw(&window_);
            window_.onWindowTrigger_->onWindowTriggerEvent(window_, partition);
        }
    }

    void insert(Punctuation const& punct)
    {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_UNEXPECTED_PUNCTUATION_IN_WINDOW,
                    SPL_WINDOW_DBG);
        THROW_STRING(SPLRuntime, SPL_APPLICATION_RUNTIME_UNEXPECTED_PUNCTUATION_IN_WINDOW);
    }

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = WindowImpl<T, G, D, S>::hash();

        for (PartitionSetConstIterator it = windowFull_.begin(); it != windowFull_.end(); ++it) {
            result ^= std::tr1::hash<PartitionType>()(*it);
        }
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        // Save parent first
        WindowImpl<T, G, D, S>::doCheckpoint(ckptStream);

        WRITE_MARKER(ckptStream, "SlidingWindowImpl");
        ckptStream << static_cast<uint64_t>(windowFull_.size());
        for (PartitionSetConstIterator it = windowFull_.begin(); it != windowFull_.end(); ++it) {
            ckptStream << *it;
        }
    }

    void doReset(Checkpoint& ckptStream)
    {
        // Load parent first
        WindowImpl<T, G, D, S>::doReset(ckptStream);

        VERIFY_MARKER(ckptStream, "SlidingWindowImpl");
        for (uint64_t count = ckptStream.getUInt64(); count > 0; --count) {
            PartitionType partition;
            ckptStream >> partition;
            windowFull_.insert(partition);
        }
    }

    void doResetToInitialState(int cause)
    {
        windowFull_.clear();

        // Initialize parent
        WindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        stream << "SlidingWindowImpl={";

        // Save parent first
        WindowImpl<T, G, D, S>::print(stream);

        stream << "windowFull_[size=" << static_cast<uint64_t>(windowFull_.size()) << "]={";
        for (PartitionSetConstIterator it = windowFull_.begin(); it != windowFull_.end(); ++it) {
            if (it != windowFull_.begin()) {
                stream << ", ";
            }
            PartitionType const partition = *it;
            stream << partition;
        }
        stream << "}}";
    }

  protected:
    WindowType& window_;
    mutable Distillery::Mutex mutex_;
    std::tr1::unordered_set<PartitionType> windowFull_;
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_SLIDING_WINDOW_IMPL_H */
