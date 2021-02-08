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

#ifndef SPL_RUNTIME_WINDOW_IMPL_TIME_COUNT_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_TIME_COUNT_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/StarCountSlidingWindowImpl.h>
#include <SPL/Runtime/Window/TimeStarSlidingWindowImpl.h>

namespace SPL {

template<class T, class G, class D, class S>
class TimeCountSlidingWindowImpl
  : public TimeStarSlidingWindowImpl<T, G, D, S>
  , public StarCountSlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    TimeCountSlidingWindowImpl(WindowType& window, OperatorImpl& oper)
      : SlidingWindowImpl<T, G, D, S>(window)
      , TimeStarSlidingWindowImpl<T, G, D, S>(window, oper)
      , StarCountSlidingWindowImpl<T, G, D, S>(window)
    {}

    TimeCountSlidingWindowImpl(WindowType& window,
                               OperatorImpl& oper,
                               PartitionEvictionImpl<T, G, D, S>& pEP)
      : SlidingWindowImpl<T, G, D, S>(window, pEP)
      , TimeStarSlidingWindowImpl<T, G, D, S>(window, oper)
      , StarCountSlidingWindowImpl<T, G, D, S>(window)
    {}

    virtual ~TimeCountSlidingWindowImpl() {}

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        Distillery::AutoMutex am(this->mutex_);

        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        DataType& gdata = this->insertPartition(partition);
        TimeStarSlidingWindowImpl<T, G, D, S>::insert(gdata, tuple, partition);
        StarCountSlidingWindowImpl<T, G, D, S>::trigger(gdata, tuple, partition);
    }

    virtual void partitionEvicted(PartitionType const& partition)
    {
        TimeStarSlidingWindowImpl<T, G, D, S>::partitionEvicted(partition);
        StarCountSlidingWindowImpl<T, G, D, S>::partitionEvicted(partition);
        SlidingWindowImpl<T, G, D, S>::partitionEvicted(partition);
    }

#ifndef NDEBUG
    size_t hash() const
    {
        // Common base, left parent, right parent
        size_t result = SlidingWindowImpl<T, G, D, S>::hash();
        result ^= TimeStarSlidingWindowImpl<T, G, D, S>::hash();
        result ^= StarCountSlidingWindowImpl<T, G, D, S>::hash();
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckpt) const
    {
        // Common base, left parent, right parent
        SlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        TimeStarSlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        StarCountSlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        WRITE_HASH(ckpt);
    }

    void doReset(Checkpoint& ckpt)
    {
        // Common base, left parent, right parent
        SlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        TimeStarSlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        StarCountSlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        VERIFY_HASH(ckpt);
    }

    void doResetToInitialState(int cause)
    {
        // Right parent, left parent, common base
        StarCountSlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
        TimeStarSlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
        SlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        // Common base, left parent, right parent
        stream << "TimeCountSlidingWindowImpl={";
        SlidingWindowImpl<T, G, D, S>::print(stream);
        stream << ", ";
        TimeStarSlidingWindowImpl<T, G, D, S>::print(stream);
        stream << ", ";
        StarCountSlidingWindowImpl<T, G, D, S>::print(stream);
        stream << "}";
    }
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_TIME_COUNT_SLIDING_WINDOW_IMPL_H */
