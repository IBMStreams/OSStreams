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

#ifndef SPL_RUNTIME_WINDOW_IMPL_DELTA_TIME_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_DELTA_TIME_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/DeltaStarSlidingWindowImpl.h>
#include <SPL/Runtime/Window/StarTimeSlidingWindowImpl.h>

namespace SPL {

template<class T, class G, class D, class S>
class DeltaTimeSlidingWindowImpl
  : public virtual DeltaStarSlidingWindowImpl<T, G, D, S>
  , public virtual StarTimeSlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    DeltaTimeSlidingWindowImpl(WindowType& window, OperatorImpl& oper)
      : SlidingWindowImpl<T, G, D, S>(window)
      , DeltaStarSlidingWindowImpl<T, G, D, S>(window)
      , StarTimeSlidingWindowImpl<T, G, D, S>(window, oper)
    {}

    DeltaTimeSlidingWindowImpl(WindowType& window,
                               OperatorImpl& oper,
                               PartitionEvictionImpl<T, G, D, S>& pEP)
      : SlidingWindowImpl<T, G, D, S>(window, pEP)
      , DeltaStarSlidingWindowImpl<T, G, D, S>(window)
      , StarTimeSlidingWindowImpl<T, G, D, S>(window, oper)
    {}

    virtual ~DeltaTimeSlidingWindowImpl() {}

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        Distillery::AutoMutex am(this->mutex_);

        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        DataType& gdata = this->insertPartition(partition);
        DeltaStarSlidingWindowImpl<T, G, D, S>::evict(gdata, tuple, partition);
        DeltaStarSlidingWindowImpl<T, G, D, S>::insert(gdata, tuple, partition);
    }

#ifndef NDEBUG
    virtual size_t hash() const
    {
        // Common base, left parent, right parent
        size_t result = SlidingWindowImpl<T, G, D, S>::hash();
        result ^= DeltaStarSlidingWindowImpl<T, G, D, S>::hash();
        result ^= StarTimeSlidingWindowImpl<T, G, D, S>::hash();
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckpt) const
    {
        // Common base, left parent, right parent
        SlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        DeltaStarSlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        StarTimeSlidingWindowImpl<T, G, D, S>::doCheckpoint(ckpt);
        WRITE_HASH(ckpt);
    }

    void doReset(Checkpoint& ckpt)
    {
        // Common base, left parent, right parent
        SlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        DeltaStarSlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        StarTimeSlidingWindowImpl<T, G, D, S>::doReset(ckpt);
        VERIFY_HASH(ckpt);
    }

    void doResetToInitialState(int cause)
    {
        // Right parent, left parent, common base
        StarTimeSlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
        DeltaStarSlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
        SlidingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        // Common base, left parent, right parent
        stream << "DeltaTimeSlidingWindowImpl={";
        SlidingWindowImpl<T, G, D, S>::print(stream);
        stream << ", ";
        DeltaStarSlidingWindowImpl<T, G, D, S>::print(stream);
        stream << ", ";
        StarTimeSlidingWindowImpl<T, G, D, S>::print(stream);
        stream << "}";
    }
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_DELTA_TIME_SLIDING_WINDOW_IMPL_H */
