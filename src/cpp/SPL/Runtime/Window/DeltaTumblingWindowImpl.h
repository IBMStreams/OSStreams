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

#ifndef SPL_RUNTIME_WINDOW_IMPL_DELTA_TUMBLING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_DELTA_TUMBLING_WINDOW_IMPL_H

namespace SPL {

template<class T, class G, class D, class S>
class DeltaTumblingWindowImpl : public TumblingWindowImpl<T, G, D, S>
{
  public:
#include "TumblingTypedefs.h"
    typedef typename std::tr1::unordered_map<PartitionType, TupleType>::const_iterator
      PartitionTupleMapConstIterator;

    DeltaTumblingWindowImpl(WindowType& window)
      : TumblingWindowImpl<T, G, D, S>(window)
    {
        initialize(window);
        WindowPolicy const& evict = window.getEvictionPolicy();
        evictionPolicy_ = dynamic_cast<DeltaWindowPolicyBase<TupleType> const*>(&evict);
        assert(evictionPolicy_);
    }

    DeltaTumblingWindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& pEP)
      : TumblingWindowImpl<T, G, D, S>(window, pEP)
    {
        initialize(window);
    }

    virtual ~DeltaTumblingWindowImpl() {}

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        DataType& gdata = this->insertPartition(partition);

        if (this->isUsingSummarization()) { // not keeping the tuples
            // eviction
            typename std::tr1::unordered_map<PartitionType, TupleType>::iterator it =
              oldestTuples_.find(partition);
            if (it != oldestTuples_.end()) {
                TupleType const& oldest = it->second;
                if (evictionPolicy_->checkBoundary(tuple, oldest)) {
                    this->closeWindowForSummarizer(partition);
                    this->emitBeforeWindowFlushEvent(partition);
                    this->setTupleCount(partition, 0);
                    this->emitAfterWindowFlushEvent(partition);
                    this->removeWindowForSummarizer(partition);
                    oldestTuples_.erase(partition);
                    this->eraseTupleCount(partition);
                    this->removePartition(partition);
                }
            }

            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            it = oldestTuples_.find(partition);
            if (it == oldestTuples_.end()) {
                oldestTuples_.insert(std::make_pair(partition, tuple));
                this->openWindowForSummarizer(partition);
            }
            this->addTupleForSummarizer(tuple, partition);
            this->emitAfterTupleInsertionEventForConstTuple(tuple, partition);
            this->incrementTupleCount(partition);
            this->tupleInserted(partition, tuple);
        } else { // keeping the tuples
            // eviction
            if (!gdata.empty()) {
                TupleType const& oldest = *gdata.begin();
                if (evictionPolicy_->checkBoundary(tuple, oldest)) {
                    this->emitBeforeWindowFlushEvent(partition);
                    gdata.clear();
                    this->emitAfterWindowFlushEvent(partition);
                }
            }

            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            gdata.push_back(tuple);
            this->emitAfterTupleInsertionEvent(gdata.back(), partition);
            this->tupleInserted(partition, tuple);
        }
    }

    virtual void partitionEvicted(PartitionType const& partition)
    {
        TumblingWindowImpl<T, G, D, S>::partitionEvicted(partition);
        oldestTuples_.erase(partition);
    }

#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = TumblingWindowImpl<T, G, D, S>::hash();

        for (PartitionTupleMapConstIterator it = oldestTuples_.begin(); it != oldestTuples_.end();
             ++it) {
            PartitionType const partition = it->first;
            result ^= std::tr1::hash<PartitionType>()(partition);
            // XXX We don't hash tuples
        }
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        // Save parent first
        TumblingWindowImpl<T, G, D, S>::doCheckpoint(ckptStream);

        // Save this object's state
        WRITE_MARKER(ckptStream, "DeltaTumblingWindowImpl");
        ckptStream << static_cast<uint64_t>(oldestTuples_.size());
        for (PartitionTupleMapConstIterator it = oldestTuples_.begin(); it != oldestTuples_.end();
             ++it) {
            ckptStream << it->first;
            ckptStream << Referencer<TupleType>::dereference(it->second);
        }
        WRITE_HASH(ckptStream);
    }

    void doReset(Checkpoint& ckptStream)
    {
        // Load parent first
        TumblingWindowImpl<T, G, D, S>::doReset(ckptStream);

        // Load this object's state
        VERIFY_MARKER(ckptStream, "DeltaTumblingWindowImpl");
        for (uint64_t count = ckptStream.getUInt64(); count > 0; --count) {
            PartitionType partition;
            ckptStream >> partition;
            TupleType t;
            ckptStream >> t;
            oldestTuples_.insert(std::make_pair(partition, t));
        }
        VERIFY_HASH(ckptStream);
    }

    void doResetToInitialState(int cause)
    {
        // Erase all entries
        oldestTuples_.clear();
        initialize(this->window_);

        // Initialize parent
        TumblingWindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        stream << "DeltaTumblingWindowImpl={";

        // Print parent first
        TumblingWindowImpl<T, G, D, S>::print(stream);

        stream << ", oldestTuples_[size=" << static_cast<uint64_t>(oldestTuples_.size()) << "]={";
        for (PartitionTupleMapConstIterator it = oldestTuples_.begin(); it != oldestTuples_.end();
             ++it) {
            stream << "partition=" << it->first << ", Tuple=" << it->second;
        }
        stream << "}}";
    }

  private:
    DeltaWindowPolicyBase<TupleType> const* evictionPolicy_;
    std::tr1::unordered_map<PartitionType, TupleType> oldestTuples_;

    void initialize(WindowType& window)
    {
        WindowPolicy const& evict = window.getEvictionPolicy();
        evictionPolicy_ = dynamic_cast<DeltaWindowPolicyBase<TupleType> const*>(&evict);
        assert(evictionPolicy_);
    }
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_DELTA_TUMBLING_WINDOW_IMPL_H */
