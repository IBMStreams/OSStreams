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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_COUNT_LRU_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_PARTITION_COUNT_LRU_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/Window.h>
#include <SPL/Runtime/Window/WindowEvent.h>

/*!
 * \file PartitionCountLRUPartitionEviction.h \brief Definition of the
 * SPL::PartitionCountLRUPartitionEvictionImpl class.
 */

namespace SPL {
/// Class that responds selects least recently used partitions for eviction (partition count)
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class PartitionCountLRUPartitionEvictionImpl : public WindowEvent<T, G, D, S>
{
    typedef WindowEvent<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;

    /// Constructor
    /// @param count maximum partition count allowed in the window
    PartitionCountLRUPartitionEvictionImpl(uint32_t count)
      : _count(count)
    {}

  protected:
    typedef typename BaseClass::PartitionSelectionIterator PartitionSelectionIterator;
    typedef typename BaseClass::TimeType TimeType;

    /// This event is fired when some partitions must be selected to be removed.
    /// \link markForRemoval should be called for each partition that is to be removed
    /// @param window the window on which this event has fired
    /// @param begin iterator to first (oldest) partition to be examined
    /// @param end iterator just past the last (newest) partition to be examined
    virtual void onWindowPartitionEvictionSelection(WindowType& window,
                                                    PartitionSelectionIterator const& begin,
                                                    PartitionSelectionIterator const& end)
    {
        // Remove enough partitions to bring the number of partitions
        // down to the limit.
        uint32_t count = window.getWindowStorage().size();
        APPTRC(L_DEBUG, "There are " << count << " partitions", SPL_WINDOW_DBG);
        for (PartitionSelectionIterator it = begin; it != end; it++) {
            if (_count >= count) {
                break;
            }
            APPTRC(L_DEBUG, "Marking partition " << it.partition() << " for removal",
                   SPL_WINDOW_DBG);
            it.markForRemoval();
            --count;
        }
    }

  private:
    // TODO const
    uint32_t _count;
};

}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_COUNT_LRU_PARTITION_EVICTION_IMPL_H */
