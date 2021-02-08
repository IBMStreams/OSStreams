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

#ifndef SPL_RUNTIME_WINDOW_IMPL_STAR_COUNT_SLIDING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_STAR_COUNT_SLIDING_WINDOW_IMPL_H

#include <SPL/Runtime/Window/SlidingWindowImpl.h>

namespace SPL {
template<class T, class G, class D, class S>
class StarCountSlidingWindowImpl : public virtual SlidingWindowImpl<T, G, D, S>
{
  public:
#include "SlidingTypedefs.h"
    typedef typename std::tr1::unordered_map<PartitionType, uint32>::const_iterator
      PartitionUint32MapConstIterator;
    typedef
      typename std::tr1::unordered_map<PartitionType, uint32>::iterator PartitionUint32MapIterator;

    StarCountSlidingWindowImpl(WindowType& window)
      : SlidingWindowImpl<T, G, D, S>(window)
    {
        WindowPolicy const& trigger1 = window.getTriggerPolicy();
        CountWindowPolicy const* ctrigger = dynamic_cast<CountWindowPolicy const*>(&trigger1);
        assert(ctrigger);

        tsize_ = ctrigger->getSize();
    }

    virtual ~StarCountSlidingWindowImpl() {}

    void trigger(DataType& gdata, TupleType const& tuple, PartitionType const& partition)
    {
        if (tsize_ > 0) {
            std::pair<PartitionUint32MapIterator, bool> itp =
              triggers_.insert(std::make_pair(partition, 0));
            uint32_t trigger2 = ++(itp.first->second);
            if (trigger2 == tsize_) {
                this->emitOnWindowTriggerEvent(partition);
                itp.first->second = 0;
            }
        }
    }

    virtual void partitionEvicted(PartitionType const& partition) { triggers_.erase(partition); }

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = static_cast<size_t>(14695981039346656037ULL);
        for (PartitionUint32MapConstIterator it = triggers_.begin(); it != triggers_.end(); ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);
            result ^= std::tr1::hash<size_t>()(it->second);
        }
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        WRITE_MARKER(ckptStream, "StarCountSlidingWindowImpl");
        ckptStream << static_cast<uint64_t>(triggers_.size());
        for (PartitionUint32MapConstIterator it = triggers_.begin(); it != triggers_.end(); ++it) {
            ckptStream << it->first;
            ckptStream << it->second;
        }
    }

    void doReset(Checkpoint& ckptStream)
    {
        VERIFY_MARKER(ckptStream, "StarCountSlidingWindowImpl");
        for (uint64_t count = ckptStream.getUInt64(); count > 0; --count) {
            PartitionType p;
            ckptStream >> p;
            uint32 t;
            ckptStream >> t;
            triggers_.insert(std::make_pair(p, t));
        }
    }

    void doResetToInitialState(int cause) { triggers_.clear(); }

    void print(std::ostream& stream) const
    {
        stream << "StarCountSlidingWindowImpl={";
        stream << "tsize_=" << tsize_
               << ", triggers_[size=" << static_cast<uint64_t>(triggers_.size()) << "]={";
        for (PartitionUint32MapConstIterator it = triggers_.begin(); it != triggers_.end(); ++it) {
            if (it != triggers_.begin()) {
                stream << ", ";
            }
            const PartitionType p = it->first;
            const uint32 t = it->second;
            stream << '<' << p << ',' << t << '>';
        }
        stream << "}";
    }

  protected:
    /* TODO const */ uint32_t tsize_;
    std::tr1::unordered_map<PartitionType, uint32> triggers_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_STAR_COUNT_SLIDING_WINDOW_IMPL_H */
