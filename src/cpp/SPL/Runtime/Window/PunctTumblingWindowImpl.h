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

#ifndef SPL_RUNTIME_WINDOW_IMPL_PUNCT_TUMBLING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_PUNCT_TUMBLING_WINDOW_IMPL_H

namespace SPL {

template<class T, class G, class D, class S>
class PunctTumblingWindowImpl : public TumblingWindowImpl<T, G, D, S>
{
  public:
#include "TumblingTypedefs.h"
    PunctTumblingWindowImpl(WindowType& window)
      : TumblingWindowImpl<T, G, D, S>(window)
    {
        assert(dynamic_cast<PunctWindowPolicy const*>(&window.getEvictionPolicy()));
    }

    PunctTumblingWindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& pEP)
      : TumblingWindowImpl<T, G, D, S>(window, pEP)
    {
        assert(dynamic_cast<PunctWindowPolicy const*>(&window.getEvictionPolicy()));
    }

    void insert(TupleType const& tuple, PartitionType const& partition)
    {
        // Perform any actions necessary before inserting a tuple
        this->beforeTupleInserted();

        size_t nParts = this->data_.size();
        DataType& gdata = this->insertPartition(partition);

        if (this->isUsingSummarization()) { // not keeping the tuples
            bool newPartition = (this->data_.size() != nParts);
            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            if (newPartition) {
                this->openWindowForSummarizer(partition);
            }
            this->addTupleForSummarizer(tuple, partition);
            this->emitAfterTupleInsertionEventForConstTuple(tuple, partition);
            this->incrementTupleCount(partition);
            this->tupleInserted(partition, tuple);
        } else { // keeping the tuples
            // insertion
            this->emitBeforeTupleInsertionEvent(tuple, partition);
            gdata.push_back(tuple);
            this->emitAfterTupleInsertionEvent(gdata.back(), partition);
            this->tupleInserted(partition, tuple);
        }
    }

    void insert(Punctuation const& punct)
    {
        // Only window and final punctuations are processed
        Punctuation::Value v = punct.getValue();
        if (Punctuation::WindowMarker != v && Punctuation::FinalMarker != v) {
            APPTRC(L_TRACE, "Window ignores punctuation " << punct, SPL_WINDOW_DBG);
            return;
        }

        // Must handle the case where we received a punct, and nothing is available
        if (this->data_.empty()) {
            TumblingWindowImpl<T, G, D, S>::emitOnEmptyWindowPunctEvent();
            return;
        }

        // Perform any actions necessary before processing the punctuation
        this->beforePunctInserted();

        typename StorageType::iterator it;
        if (this->isUsingSummarization()) { // not keeping the tuples
            while (this->data_.size() > 0) {
                it = this->data_.begin();
                // Don't make it reference, due to erase in removePartition
                PartitionType const partition = it->first;
                // eviction
                this->closeWindowForSummarizer(partition);
                this->emitBeforeWindowFlushEvent(partition);
                this->setTupleCount(partition, 0);
                this->emitAfterWindowFlushEvent(partition);
                this->removeWindowForSummarizer(partition);
                this->eraseTupleCount(partition);
                this->removePartition(partition);
            }
        } else {
            while (this->data_.size() > 0) {
                it = this->data_.begin();
                // Don't make it reference, due to erase in removePartition
                PartitionType const partition = it->first;
                // eviction
                this->emitBeforeWindowFlushEvent(partition);
                it->second.clear();
                this->emitAfterWindowFlushEvent(partition);
                this->removePartition(partition);
            }
        }
    }
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_PUNCT_TUMBLING_WINDOW_IMPL_H */
