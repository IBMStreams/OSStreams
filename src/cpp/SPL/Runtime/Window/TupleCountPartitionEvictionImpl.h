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

#ifndef SPL_RUNTIME_WINDOW_TUPLE_COUNT_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_TUPLE_COUNT_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionImpl.h>

namespace SPL {
/// Partitions should be evicted when there are too many tuples
template<class T, class G, class D, class S>
class TupleCountPartitionEvictionImpl : public PartitionEvictionImpl<T, G, D, S>
{
    typedef PartitionEvictionImpl<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;
    typedef typename BaseClass::WindowEventType WindowEventType;
    typedef typename BaseClass::TimeOrderedPartition TimeOrderedPartition;
    typedef typename BaseClass::PartitionType PartitionType;
    typedef typename BaseClass::TupleType TupleType;
    typedef typename BaseClass::DataType DataType;

    /// Constructor
    /// @param count maximum number of allowed tuples
    TupleCountPartitionEvictionImpl(uint32_t count)
      : BaseClass(BaseClass::TupleCount)
      , count_(count)
      , currentTupleCount_(0)
    {}

    /// Constructor
    /// @param count maximum number of allowed tuples
    /// @param selector window event handler to select partitions
    TupleCountPartitionEvictionImpl(uint32_t count, WindowEventType& selector)
      : BaseClass(BaseClass::TupleCount, selector)
      , count_(count)
      , currentTupleCount_(0)
    {}

    virtual ~TupleCountPartitionEvictionImpl() {}

    /// Get the maximum number of allowed tuples
    /// @return the maximum number of allowed tuples
    uint32_t getCount() const { return count_; }

    /// Partition flushed
    /// @param window Window being managed
    /// @param partition Partition being evicted (flushed) by the Window
    /// @param data data in the partition being removed
    virtual void partitionFlushed(WindowType const& window, PartitionType const& partition)
    {
        currentTupleCount_ -= window.getTupleCount(partition);
    }

    /// Partition removed
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param data data in the partition being removed
    virtual void partitionRemoved(WindowType const& window, PartitionType const& partition)
    {
        currentTupleCount_ -= window.getTupleCount(partition);
    }

    /// Tuple inserted into window
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param tuple data in the partition being removed
    virtual void tupleInserted(WindowType const& window,
                               PartitionType const& partition,
                               TupleType const& tuple)
    {
        currentTupleCount_++;
    }

    /// Tuple removed from window
    /// @param window Window being managed
    /// @param partition Partition being removed by the Window
    /// @param tuple data in the partition being removed
    virtual void tupleRemoved(WindowType const& window,
                              PartitionType const& partition,
                              TupleType const& tuple)
    {
        currentTupleCount_--;
    }

    /// Do partitions need to be evicted from the window?
    /// @param window Window being managed
    /// @param partitions list of partitions in the window ordered by last use
    /// @return 'true' if one or more partitions need to be removed from the Window
    virtual bool evictionNeeded(WindowType const& window,
                                TimeOrderedPartition const& partitions,
                                typename TimeOrderedPartition::size_type partitionsCount)
    {
        return currentTupleCount_ > count_;
    }

  private:
    // Get the current tuple count
    // @return the number of tuples in the window across all partitions
    uint32_t getCurrentTupleCount() const { return currentTupleCount_; }

    // Set the the number of tuples in the window across all partitions
    // @param count current tuple count across all partitions
    void setCurrentTupleCount(uint32_t count) { currentTupleCount_ = count; }

    friend class WindowImpl<T, G, D, S>; // serializer

    uint32_t const count_;
    uint32_t currentTupleCount_;
};
}

#endif /* SPL_RUNTIME_WINDOW_TUPLE_COUNT_PARTITION_EVICTION_IMPL_H */
