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

#ifndef SPL_RUNTIME_WINDOW_LRU_AGE_PARTITION_EVICTION_IMPL_H
#define SPL_RUNTIME_WINDOW_LRU_AGE_PARTITION_EVICTION_IMPL_H

#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>
#include <SPL/Runtime/Window/Window.h>
#include <SPL/Runtime/Window/WindowEvent.h>
#include <sys/time.h>

/*!
 * \file AgeLRUPartitionEviction.h \brief Definition of the SPL::AgeLRUPartitionCountEvictionImpl
 * class.
 */

namespace SPL {
/// Class that responds selects least recently used partitions for eviction (for age evictions)
template<class T,
         class G = int32_t,
         class D = std::deque<T>,
         class S = std::tr1::unordered_map<G, D> >
class AgeLRUPartitionEvictionImpl : public WindowEvent<T, G, D, S>
{
    typedef WindowEvent<T, G, D, S> BaseClass;

  public:
    typedef typename BaseClass::WindowType WindowType;

    /// Constructor
    /// @param age maximum time for partitions to stay
    AgeLRUPartitionEvictionImpl(double age)
      : age_(static_cast<TimeType>(age))
    {
        APPTRC(L_DEBUG, "Partition age set to " << age_, SPL_WINDOW_DBG);
    }

    virtual ~AgeLRUPartitionEvictionImpl() {}

  protected:
    typedef typename BaseClass::PartitionSelectionIterator PartitionSelectionIterator;
    typedef typename BaseClass::TimeType TimeType;

    /// This event is fired when some partitions must be selected to be removed.
    /// \link markForRemoval should be called for each partition that is to be removed
    /// @param window the window on which this event has fired
    /// @param begin iterator to first (oldest) partition to be examined
    /// @param end iterator to the element following the last (newest) partition to be examined
    virtual void onWindowPartitionEvictionSelection(WindowType& window,
                                                    PartitionSelectionIterator const& begin,
                                                    PartitionSelectionIterator const& end)
    {
        // Remove all the partitions that are too old
        struct timeval tval;
        gettimeofday(&tval, NULL);
        TimeType limit = tval.tv_sec - age_;
        APPTRC(L_DEBUG, "Evicting partitions last referenced before " << limit, SPL_WINDOW_DBG);
        for (PartitionSelectionIterator it = begin; it != end; it++) {
            TimeType lastReference = it.time();
            APPTRC(L_DEBUG, "Partition " << it.partition() << " last referenced " << lastReference,
                   SPL_WINDOW_DBG);

            // Check the last referenced time
            if (lastReference > limit) {
                break;
            }
            APPTRC(L_DEBUG, "Partition " << it.partition() << " marked for removal",
                   SPL_WINDOW_DBG);
            it.markForRemoval();
        }
    }

  private:
    // TODO const
    TimeType age_;
};

}

#endif /* SPL_RUNTIME_WINDOW_LRU_AGE_PARTITION_EVICTION_IMPL_H */
