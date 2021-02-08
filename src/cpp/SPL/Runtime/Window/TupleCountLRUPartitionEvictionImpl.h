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

#ifndef SPL_RUNTIME_WINDOW_TUPLE_COUNT_LRU_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_TUPLE_COUNT_LRU_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/Window.h>
#include <SPL/Runtime/Window/WindowEvent.h>

/*!
 * \file TupleCountLRUPartitionEviction.h \brief Definition of the
 * SPL::TupleCountLRUPartitionEvictionImpl class.
 */

namespace SPL {
/// Class that responds selects least recently used partitions for eviction (tuple count)
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class TupleCountLRUPartitionEvictionImpl : public WindowEvent<T, G, D, S>
{
    typedef WindowEvent<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;

    /// Constructor
    TupleCountLRUPartitionEvictionImpl() {}

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
        // Remove all leading empty partitions, and one that isn't
        for (PartitionSelectionIterator it = begin; it != end; it++) {
            it.markForRemoval();
            if (window.getTupleCount(it.partition()) != 0) {
                break;
            }
        }
    }
};

}

#endif /* SPL_RUNTIME_WINDOW_TUPLE_COUNT_LRU_PARTITION_EVICTION_IMPL_H */
