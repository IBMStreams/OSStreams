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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_AGE_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_PARTITION_AGE_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionImpl.h>

namespace SPL {
/// Partitions should be evicted after a certain length of time
template<class T, class G, class D, class S>
class PartitionAgePartitionEvictionImpl : public PartitionEvictionImpl<T, G, D, S>
{
    typedef PartitionEvictionImpl<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;
    typedef typename BaseClass::WindowEventType WindowEventType;
    typedef typename BaseClass::TimeOrderedPartition TimeOrderedPartition;
    typedef typename BaseClass::TimeType TimeType;

    /// Constructor
    /// @param age time in seconds for partitions before eviction
    PartitionAgePartitionEvictionImpl(double age)
      : BaseClass(BaseClass::PartitionAge)
      , age_(static_cast<TimeType>(age))
    {}

    /// Constructor
    /// @param age time in seconds for partitions before eviction
    /// @param selector window event handler to select partitions
    PartitionAgePartitionEvictionImpl(double age, WindowEventType& selector)
      : BaseClass(BaseClass::PartitionAge, selector)
      , age_(static_cast<TimeType>(age))
    {}

    virtual ~PartitionAgePartitionEvictionImpl() {}

    /// Get the partition age before eviction
    /// @return the partition age before eviction
    double getAge() const { return age_; }

    /// Do partitions need to be evicted from the window?
    /// @param window Window being managed
    /// @param partitions list of partitions in the window ordered by last use
    /// @return 'true' if one or more partitions need to be removed from the Window
    virtual bool evictionNeeded(WindowType const& window,
                                TimeOrderedPartition const& partitions,
                                typename TimeOrderedPartition::size_type partitionsCount)
    {
        assert(partitionsCount > 0);
        struct timeval tval;
        gettimeofday(&tval, NULL);

        // If the oldest partition is > age_seconds older than now, we need to evict
        return partitions.begin()->second + age_ < static_cast<TimeType>(tval.tv_sec);
    }

  private:
    // TODO const
    TimeType age_;
};
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_AGE_PARTITION_EVICTION_IMPL_H */
