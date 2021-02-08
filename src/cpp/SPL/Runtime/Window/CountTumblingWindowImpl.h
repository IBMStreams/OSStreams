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

#ifndef SPL_RUNTIME_WINDOW_IMPL_COUNT_TUMBLING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_COUNT_TUMBLING_WINDOW_IMPL_H

#include <stdexcept>

namespace SPL {

template<class T, class G, class D, class S>
class CountTumblingWindowImpl : public TumblingWindowImpl<T, G, D, S>
{
  public:
#include "TumblingTypedefs.h"

    CountTumblingWindowImpl(WindowType& window)
      : TumblingWindowImpl<T, G, D, S>(window)
    {
        initialize(window);
    }

    CountTumblingWindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& pEP)
      : TumblingWindowImpl<T, G, D, S>(window, pEP)
    {
        initialize(window);
    }

    virtual ~CountTumblingWindowImpl() {}

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        if (size_ == 0) {
            return;
        }

        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        if (this->isUsingSummarization()) { // not keeping the tuples
            // insertion
            {
                this->insertPartition(partition);
                this->emitBeforeTupleInsertionEvent(tuple, partition);

                // If size not found for partition, then the partition has
                // just been created ==> create and notify summarizer.
                bool hasPartition = this->hasTupleCount(partition);
                this->incrementTupleCount(partition);
                if (!hasPartition) {
                    this->openWindowForSummarizer(partition);
                }

                this->addTupleForSummarizer(tuple, partition);
                this->emitAfterTupleInsertionEventForConstTuple(tuple, partition);
                this->tupleInserted(partition, tuple);
            }

            // window eviction
            // Note that tuple insertion may have triggered partition
            // eviction for non PartitionAge policies, so check first if
            // we have a partition.
            if (this->hasTupleCount(partition)) {
                if (this->getTupleCount(partition) == size_) {
                    this->closeWindowForSummarizer(partition);
                    this->emitBeforeWindowFlushEvent(partition);
                    this->setTupleCount(partition, 0);
                    this->emitAfterWindowFlushEvent(partition);
                    this->removeWindowForSummarizer(partition);
                    this->eraseTupleCount(partition);
                    this->removePartition(partition);
                }
            } else {
                // APPTRC(L_DEBUG, "Cannot evict window for partition: " << partition << " which was
                // just evicted.", SPL_WINDOW_DBG);
            }

        } else { // keeping the tuples
            // insertion
            {
                DataType& gdata = this->insertPartition(partition);
                this->emitBeforeTupleInsertionEvent(tuple, partition);
                gdata.push_back(tuple);
                this->emitAfterTupleInsertionEvent(gdata.back(), partition);
                this->tupleInserted(partition, tuple);
            }

            // window eviction
            // Note that tuple insertion may have triggered partition
            // eviction for non PartitionAge policies, so check first if
            // we have a partition.
            try {
                DataType& gdata = this->partitionAt(partition);
                if (gdata.size() == size_) {
                    this->emitBeforeWindowFlushEvent(partition);
                    gdata.clear();
                    this->emitAfterWindowFlushEvent(partition);
                    this->removePartition(partition);
                }
            } catch (std::out_of_range const& ex) { // no partition
            }
        }
    }

    void partitionEvicted(PartitionType const& partition)
    {
        TumblingWindowImpl<T, G, D, S>::partitionEvicted(partition);
        this->eraseTupleCount(partition);
    }

  protected:
#ifndef NDEBUG
    size_t hash() const
    {
        size_t result = TumblingWindowImpl<T, G, D, S>::hash();

        result ^= static_cast<size_t>(this->size_);
        result *= static_cast<size_t>(1099511628211ULL);
        return result;
    }
#endif

    void print(std::ostream& stream) const
    {
        stream << "CountTumblingWindowImpl={";

        // Print parent first
        TumblingWindowImpl<T, G, D, S>::print(stream);

        stream << " size_=" << this->size_;
        stream << "}";
    }

    void initialize(WindowType& window)
    {
        WindowPolicy const& evict = window.getEvictionPolicy();
        CountWindowPolicy const* cevict = dynamic_cast<CountWindowPolicy const*>(&evict);
        assert(cevict);
        size_ = cevict->getSize();
    }

  private:
    /* TODO const */ uint32_t size_;
};
};

#endif /* SPL_RUNTIME_WINDOW_IMPL_COUNT_TUMBLING_WINDOW_IMPL_H */
