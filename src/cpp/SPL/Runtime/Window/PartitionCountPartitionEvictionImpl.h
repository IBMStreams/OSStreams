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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_COUNT_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_PARTITION_COUNT_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionImpl.h>

namespace SPL {
/// Partitions should be evicted when there are too many partitions
template<class T, class G, class D, class S>
class PartitionCountPartitionEvictionImpl : public PartitionEvictionImpl<T, G, D, S>
{
    typedef PartitionEvictionImpl<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;
    typedef typename BaseClass::WindowEventType WindowEventType;
    typedef typename BaseClass::TimeOrderedPartition TimeOrderedPartition;

    /// Constructor
    /// @param count maximum number of allowed partitions
    PartitionCountPartitionEvictionImpl(uint32_t count)
      : BaseClass(BaseClass::PartitionCount)
      , count_(count)
    {}

    /// Constructor
    /// @param count maximum number of allowed partitions
    /// @param selector window event handler to select partitions
    PartitionCountPartitionEvictionImpl(uint32_t count, WindowEventType& selector)
      : BaseClass(BaseClass::PartitionCount, selector)
      , count_(count)
    {}

    /// Get the maximum number of allowed partitions
    /// @return the maximum number of allowed partitions
    uint32_t getCount() const { return count_; }

    /// Do partitions need to be evicted from the window?
    /// @param window Window being managed
    /// @param partitions list of partitions in the window ordered by last use
    /// @return 'true' if one or more partitions need to be removed from the Window
    virtual bool evictionNeeded(WindowType const& window,
                                TimeOrderedPartition const& partitions,
                                typename TimeOrderedPartition::size_type partitionsCount)
    {
        return partitionsCount > count_;
    }

  private:
    // TODO const
    uint32_t count_;
};
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_COUNT_PARTITION_EVICTION_IMPL_H */
