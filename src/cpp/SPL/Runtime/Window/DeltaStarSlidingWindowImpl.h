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

#ifndef SPL_RUNTIME_WINDOW_IMPL_DELTA_STAR_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_DELTA_STAR_SLIDING_WINDOW_IMPL_H

namespace SPL {
template<class T, class G, class D, class S>
class DeltaStarSlidingWindowImpl : public virtual SlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    DeltaStarSlidingWindowImpl(WindowType& window)
      : SlidingWindowImpl<T, G, D, S>(window)
    {
        WindowPolicy const& evict1 = window.getEvictionPolicy();
        evictionPolicy_ = dynamic_cast<DeltaWindowPolicyBase<TupleType> const*>(&evict1);
        assert(evictionPolicy_);
        monotonic_ = true;
    }

    void evict(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        if (!gdata.empty()) {
            TupleType const& youngest = *gdata.rbegin();
            if (monotonic_) {
                monotonic_ = evictionPolicy_->verifyOrder(tuple, youngest);
            }
        }
        bool full = false;
        for (size_t i = 0; i < gdata.size();) {
            typename DataType::iterator it = gdata.begin() + i;
            if (evictionPolicy_->checkBoundary(tuple, *it)) {
                if (!full) {
                    full = true;
                    this->emitOnWindowInitialFullEventIfNeeded(partition);
                }
                this->emitBeforeTupleEvictionEvent(*it, partition);
                TupleType rtuple = *it;
                gdata.erase(it);
                this->emitAfterTupleEvictionEvent(rtuple, partition);
            } else if (!monotonic_) {
                ++i;
            } else {
                break;
            }
        }
    }

    void insert(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();
        this->emitBeforeTupleInsertionEvent(tuple, partition);
        gdata.push_back(tuple);
        this->emitAfterTupleInsertionEvent(gdata.back(), partition);
        this->tupleInserted(partition, gdata.back());
    }

#ifndef NDEBUG
    size_t hash() const { return std::tr1::hash<size_t>()(static_cast<size_t>(this->monotonic_)); }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckpt) const
    {
        WRITE_MARKER(ckpt, "DeltaStarSlidingWindowImpl");
        ckpt << static_cast<uint8_t>(this->monotonic_);
    }

    void doReset(Checkpoint& ckpt)
    {
        VERIFY_MARKER(ckpt, "DeltaStarSlidingWindowImpl");
        this->monotonic_ = static_cast<bool>(ckpt.getUInt8());
    }

    void doResetToInitialState(int cause) { this->monotonic_ = true; }

    void print(std::ostream& stream) const
    {
        stream << "DeltaStarSlidingWindowImpl={";
        stream << "monotonic_=" << this->monotonic_;
        stream << "}";
    }

  private:
    bool monotonic_;

    // TODO const - can we use constructor initialization here?
    DeltaWindowPolicyBase<TupleType> const* evictionPolicy_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_DELTA_STAR_SLIDING_WINDOW_IMPL_H */
