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

#ifndef SPL_RUNTIME_WINDOW_IMPL_TUMBLING_WINDOW_IMPL_H
#define SPL_RUNTIME_WINDOW_IMPL_TUMBLING_WINDOW_IMPL_H

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Window/WindowImpl.h>

namespace SPL {

template<class T, class G, class D, class S>
class TumblingWindowImpl : public WindowImpl<T, G, D, S>
{
  public:
#include "TumblingTypedefs.h"
    typedef typename std::tr1::unordered_map<PartitionType, SummarizerType*>::const_iterator
      SummarizerMapConstIterator;
    typedef typename std::tr1::unordered_map<PartitionType, SummarizerType*>::iterator
      SummarizerMapIterator;
    typedef
      typename std::tr1::unordered_map<PartitionType, uint32_t>::iterator CurrentSizesMapIterator;
    typedef typename std::tr1::unordered_map<PartitionType, uint32_t>::const_iterator
      CurrentSizesMapConstIterator;

    TumblingWindowImpl(WindowType& window)
      : WindowImpl<T, G, D, S>(window)
      , window_(window)
    {}
    TumblingWindowImpl(WindowType& window, PartitionEvictionImpl<T, G, D, S>& partitionEviction)
      : WindowImpl<T, G, D, S>(window, partitionEviction)
      , window_(window)
    {}
    virtual ~TumblingWindowImpl() {}

    void emitBeforeWindowFlushEvent(PartitionType const& partition)
    {
        AutoSetWindow asw(&window_);
        if (window_.beforeWindowFlush_) {
            window_.beforeWindowFlush_->beforeWindowFlushEvent(window_, partition);
        }
        this->partitionFlushed(partition);
    }

    void emitAfterWindowFlushEvent(PartitionType const& partition)
    {
        if (window_.afterWindowFlush_) {
            window_.afterWindowFlush_->afterWindowFlushEvent(window_, partition);
        }
    }

    void emitOnEmptyWindowPunctEvent()
    {
        if (window_.onEmptyWindowPunct_) {
            window_.onEmptyWindowPunct_->onEmptyWindowPunctEvent(window_);
        }
    }

    StorageType const& acquireData() const { return this->data_; }

    void releaseData() const {}

    void insert(Punctuation const& punct)
    {
        SPLTRACEMSG(L_ERROR, SPL_APPLICATION_RUNTIME_UNEXPECTED_PUNCTUATION_IN_WINDOW,
                    SPL_WINDOW_DBG);
        THROW_STRING(SPLRuntime, SPL_APPLICATION_RUNTIME_UNEXPECTED_PUNCTUATION_IN_WINDOW);
    }

    bool isUsingSummarization() const { return (this->window_.summarizerFactory_ != NULL); }

    bool hasSummarizer(PartitionType const& partition) { return summarizers_.count(partition) > 0; }

    void openWindowForSummarizer(PartitionType const& partition)
    {
        SummarizerType* summarizer = insertSummarizer(partition);
        summarizer->onOpenWindowEvent(partition);
    }

    void addTupleForSummarizer(TupleType const& tuple, PartitionType const& partition)
    {
        summarizers_[partition]->onTupleInsertionEvent(tuple);
    }

    void closeWindowForSummarizer(PartitionType const& partition)
    {
        summarizers_[partition]->onCloseWindowEvent();
    }

    void removeWindowForSummarizer(PartitionType const& partition)
    {
        SummarizerMapIterator it = summarizers_.find(partition);
        eraseSummarizer(it);
    }

    SummarizerType* getSummarizer(PartitionType const& partition)
    {
        // we are not doing summarization
        if (!isUsingSummarization()) {
            return NULL;
        }
        SummarizerMapIterator it = summarizers_.find(partition);
        if (it == summarizers_.end()) {
            return NULL;
        }
        return it->second;
    }

    virtual void partitionEvicted(PartitionType const& partition)
    {
        if (isUsingSummarization()) {
            currentSizes_.erase(partition);
        }
        removeWindowForSummarizer(partition);
    }

#ifndef NDEBUG
    virtual size_t hash() const
    {
        size_t result = WindowImpl<T, G, D, S>::hash();
        result ^= std::tr1::hash<size_t>()(summarizers_.size());

        for (SummarizerMapConstIterator it = summarizers_.begin(); it != summarizers_.end(); ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);
        }

        for (CurrentSizesMapConstIterator it = currentSizes_.begin(); it != currentSizes_.end();
             ++it) {
            result ^= std::tr1::hash<PartitionType>()(it->first);
            result ^= static_cast<size_t>(it->second);
            result *= static_cast<size_t>(1099511628211ULL);
        }
        return result;
    }
#endif

  protected:
    void doCheckpoint(Checkpoint& ckptStream) const
    {
        // Parent first
        WindowImpl<T, G, D, S>::doCheckpoint(ckptStream);

        // Save summarizers
        WRITE_MARKER(ckptStream, "TumblingWindowImpl");
        ckptStream << static_cast<uint64_t>(summarizers_.size());
        for (SummarizerMapConstIterator it = summarizers_.begin(); it != summarizers_.end(); ++it) {
            ckptStream << it->first;
            SummarizerType* s = it->second;
            s->onCheckpointEvent(ckptStream);
        }
        ckptStream << static_cast<uint64_t>(currentSizes_.size());
        for (CurrentSizesMapConstIterator it = currentSizes_.begin(); it != currentSizes_.end();
             ++it) {
            ckptStream << it->first;
            ckptStream << it->second;
        }
    }

    /*
     * Summarizers are not re-constructed if the number of partitions is
     * equal to the number of checkpointed partitions.  This guarantees
     * that if the number of partitions stays constant over the window's
     * life, summarizers are not reconstructed on reset.
     */
    void doReset(Checkpoint& ckptStream)
    {
        // Parent first
        WindowImpl<T, G, D, S>::doReset(ckptStream);

        // Load summarizers
        VERIFY_MARKER(ckptStream, "TumblingWindowImpl");

        uint64_t sumCount = ckptStream.getUInt64();

        // We recreate the summarizers list if the current list count
        // and the checkpointed list count are not equal
        if (sumCount != static_cast<uint64_t>(summarizers_.size())) {
            eraseAllSummarizers();
        }

        for (; sumCount > 0; --sumCount) {
            PartitionType partition;
            ckptStream >> partition;
            SummarizerType* s = getOrInsertSummarizer(partition);
            if (s != NULL) {
                s->onResetEvent(ckptStream);
            }
        }

        for (uint64_t count = ckptStream.getUInt64(); count > 0; --count) {
            PartitionType partition;
            ckptStream >> partition;
            uint32_t partitionSize;
            ckptStream >> partitionSize;
            currentSizes_.insert(std::make_pair(partition, partitionSize));
        }
    }

    /*
     * It does not delete the summarizers list if invoked to reinitialize
     * a window prior to restoring its state in a reset() (when the cause
     * parameter is set to INTERNAL).
     */
    void doResetToInitialState(int cause)
    {
        currentSizes_.clear();
        if (WindowImpl<T, G, D, S>::EXTERNAL == cause) {
            // Delete summarizers only if the reset was triggered externally
            eraseAllSummarizers();
        }

        // Parent last
        WindowImpl<T, G, D, S>::doResetToInitialState(cause);
    }

    void print(std::ostream& stream) const
    {
        stream << "TumblingWindowImpl={";

        // Parent first
        WindowImpl<T, G, D, S>::print(stream);

        // Save summarizers
        stream << " summarizers_[size=" << static_cast<uint64_t>(summarizers_.size()) << "]={";
        for (SummarizerMapConstIterator it = summarizers_.begin(); it != summarizers_.end(); ++it) {
            PartitionType const partition = it->first;
            stream << "(partition=" << partition;
            SummarizerType* s = it->second;
            stream << ", summarizer=" << std::hex << std::showbase << (void*)s << std::dec << "), ";
        }
        stream << "} currentSizes_[size=" << static_cast<uint64_t>(currentSizes_.size()) << "]={";
        for (CurrentSizesMapConstIterator it = currentSizes_.begin(); it != currentSizes_.end();
             ++it) {
            PartitionType const& partition = it->first;
            stream << "(partition,size)=(" << partition << "," << it->second << "), ";
        }

        stream << "}}";
    }

    typename DataType::size_type getTupleCount(PartitionType const& partition) const
    {
        if (this->isUsingSummarization()) { // not keeping the tuples
            typename DataType::size_type size = 0;
            CurrentSizesMapConstIterator it = currentSizes_.find(partition);
            if (it == currentSizes_.end()) {
                if (this->data_.find(partition) == this->data_.end()) {
                    std::ostringstream s;
                    s << partition;
                    throw std::out_of_range(s.str());
                } else {
                    size = 0;
                }
            } else {
                size = it->second;
            }
            return size;
        } else {
            return WindowImpl<T, G, D, S>::getTupleCount(partition);
        }
    }

  protected:
    WindowType& window_;

    void incrementTupleCount(PartitionType const& partition)
    {
        // If size not found for partition, then the partition has
        // just been created ==> create and notify summarizer.
        CurrentSizesMapIterator it = currentSizes_.find(partition);
        if (it != currentSizes_.end()) {
            ++(it->second);
        } else {
            currentSizes_.insert(std::make_pair(partition, 1));
        }
    }
    bool hasTupleCount(PartitionType const& partition) const
    {
        return currentSizes_.find(partition) != currentSizes_.end();
    }
    uint32_t getTupleCount(PartitionType const& partition)
    {
        return currentSizes_.find(partition)->second;
    }
    void setTupleCount(PartitionType const& partition, int32_t size)
    {
        currentSizes_[partition] = size;
    }
    void eraseTupleCount(PartitionType const& partition) { currentSizes_.erase(partition); }

  private:
    std::tr1::unordered_map<PartitionType, uint32_t> currentSizes_;

    std::tr1::unordered_map<PartitionType, SummarizerType*> summarizers_;

    SummarizerType* insertSummarizer(PartitionType const& partition)
    {
        SummarizerType* summarizer =
          this->window_.summarizerFactory_->createSummarizer(window_.getOperator());
        summarizers_.insert(std::make_pair(partition, summarizer));
        return summarizer;
    }

    SummarizerType* getOrInsertSummarizer(PartitionType const& partition)
    {
        SummarizerType*& summarizer = summarizers_[partition];
        if (summarizer == NULL) {
            summarizer = this->window_.summarizerFactory_->createSummarizer(window_.getOperator());
        }
        return summarizer;
    }

    void eraseSummarizer(SummarizerMapIterator& it)
    {
        if (it != summarizers_.end()) {
            SummarizerType* summarizer = it->second;
            delete summarizer;
            summarizers_.erase(it);
        }
    }

    void eraseAllSummarizers()
    {
        SummarizerMapIterator it;
        while (this->summarizers_.size() > 0) {
            it = this->summarizers_.begin();
            eraseSummarizer(it);
        }
    }
};

};

#endif /* SPL_RUNTIME_WINDOW_IMPL_TUMBLING_WINDOW_IMPL_H */
