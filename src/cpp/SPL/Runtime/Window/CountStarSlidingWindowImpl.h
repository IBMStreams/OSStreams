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

#ifndef SPL_RUNTIME_WINDOW_IMPL_COUNT_STAR_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_COUNT_STAR_SLIDING_WINDOW_IMPL_H

namespace SPL {
template<class T, class G, class D, class S>
class CountStarSlidingWindowImpl : public virtual SlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    CountStarSlidingWindowImpl(WindowType& window)
      : SlidingWindowImpl<T, G, D, S>(window)
    {
        WindowPolicy const& evict1 = window.getEvictionPolicy();
        CountWindowPolicy const* cevict = dynamic_cast<CountWindowPolicy const*>(&evict1);
        assert(cevict);
        wsize_ = cevict->getSize();
    }

    virtual ~CountStarSlidingWindowImpl() {}

    void evict(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        if (wsize_ > 0) {
            // eviction
            if (gdata.size() == wsize_) {
                this->emitBeforeTupleEvictionEvent(*gdata.begin(), partition);
                TupleType rtuple = *gdata.begin();
                gdata.pop_front();
                this->emitAfterTupleEvictionEvent(rtuple, partition);
            }
        }
    }

    void insert(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        if (wsize_ > 0) {
            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            gdata.push_back(tuple);
            this->emitAfterTupleInsertionEvent(gdata.back(), partition);
            this->tupleInserted(partition, gdata.back());
            if (gdata.size() == wsize_) {
                this->emitOnWindowInitialFullEventIfNeeded(partition);
            }
        }
    }

  protected:
    void print(std::ostream& stream) const
    {
        stream << "CountStarSlidingWindowImpl={size=" << this->wsize_ << "}";
    }

    // TODO const
    uint32_t wsize_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_COUNT_STAR_SLIDING_WINDOW_IMPL_H */
