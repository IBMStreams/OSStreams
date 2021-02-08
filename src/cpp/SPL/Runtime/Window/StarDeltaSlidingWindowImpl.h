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

#ifndef SPL_RUNTIME_WINDOW_IMPL_STAR_DELTA_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_STAR_DELTA_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/DeltaWindowPolicy.h>
#include <tr1/unordered_map>

namespace SPL {
template<class T, class G, class D, class S>
class StarDeltaSlidingWindowImpl : public virtual SlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    typedef typename WTDereferencer<TupleType>::deref_type DerefTupleType;
    typedef typename std::tr1::unordered_map<PartitionType, DerefTupleType>::const_iterator
      PartitionTupleMapConstIterator;

    StarDeltaSlidingWindowImpl(WindowType& window)
      : SlidingWindowImpl<T, G, D, S>(window)
    {
        WindowPolicy const& trigger1 = window.getTriggerPolicy();
        triggerPolicy_ = dynamic_cast<DeltaWindowPolicyBase<TupleType> const*>(&trigger1);
        assert(triggerPolicy_);
    }

    virtual ~StarDeltaSlidingWindowImpl() {}

    void trigger(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        DerefTupleType const& tupled = WTDereferencer<TupleType>::deref(tuple);
        if (triggerTuples_.count(partition) == 0) {
            triggerTuples_.insert(std::make_pair(partition, tupled));
        }

        DerefTupleType& lasttd = triggerTuples_[partition];
        TupleType const& lastt = WTReferencer<TupleType>::ref(lasttd);
        if (triggerPolicy_->checkBoundary(tuple, lastt)) {
            this->emitOnWindowTriggerEvent(partition);
            lasttd = tupled;
        }
    }

    virtual void partitionEvicted(PartitionType const& partition)
    {
        triggerTuples_.erase(partition);
    }

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = static_cast<size_t>(14695981039346656037ULL);
        for (PartitionTupleMapConstIterator it = triggerTuples_.begin(); it != triggerTuples_.end();
             ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);
            // XXX we don't hash tuples
        }
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        WRITE_MARKER(ckptStream, "StarDeltaSlidingWindowImpl");
        ckptStream << static_cast<uint64_t>(triggerTuples_.size());
        for (PartitionTupleMapConstIterator it = triggerTuples_.begin(); it != triggerTuples_.end();
             ++it) {
            PartitionType const& p = it->first;
            ckptStream << p;
            DerefTupleType const& dt = it->second;
            ckptStream << dt;
        }
    }

    void doReset(Checkpoint& ckptStream)
    {
        VERIFY_MARKER(ckptStream, "StarDeltaSlidingWindowImpl");
        for (uint64_t count = ckptStream.getUInt64(); count > 0; --count) {
            PartitionType p;
            ckptStream >> p;
            DerefTupleType dt;
            ckptStream >> dt;
            triggerTuples_.insert(std::make_pair(p, dt));
        }
    }

    void doResetToInitialState(int cause) { triggerTuples_.clear(); }

    void print(std::ostream& stream) const
    {
        stream << "StarDeltaSlidingWindowImpl={";
        stream << "triggerTuples_[size=" << static_cast<uint64_t>(triggerTuples_.size()) << "]={";
        for (PartitionTupleMapConstIterator it = triggerTuples_.begin(); it != triggerTuples_.end();
             ++it) {
            if (it != triggerTuples_.begin()) {
                stream << ", ";
            }
            PartitionType const& p = it->first;
            DerefTupleType const& t = it->second;
            stream << '<' << p << ',' << t << '>';
        }
        stream << "}";
    }

  protected:
    // Copy of the last tuple that triggered (we need to take special care for pointers to tuples)
    std::tr1::unordered_map<PartitionType, DerefTupleType> triggerTuples_;
    // TODO const
    DeltaWindowPolicyBase<TupleType> const* triggerPolicy_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_STAR_DELTA_SLIDING_WINDOW_IMPL_H */
