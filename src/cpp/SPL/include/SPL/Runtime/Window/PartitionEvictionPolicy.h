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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_EVICTION_POLICY_H
#define SPL_RUNTIME_WINDOW_PARTITION_EVICTION_POLICY_H

/*!
 * \file PartitionEvictionPolicy.h \brief Definition of the SPL::PartitionEvictionPolicy class.
 */

namespace SPL
{
    /// @ingroup Window
    /// @brief Class that represents when partitions should be evicted from the window
    class PartitionEvictionPolicy
    {
    public:
        /// When should partition evictions be done?
        enum When
        {
            PartitionAge,       //!< Partition not updated recently
            PartitionCount,     //!< Too many partitions
            TupleCount          //!< Too many tuples
        };

        /// Which partitions should be evicted
        enum How
        {
            LRU,                //!< Oldest unreferenced partitions should be evicted
            OperatorDefined,    /*! callback to onWindowPartitionEvictionSelection
                                event to select partitions to evict */
        };

        /// Destructor
        ///
        virtual ~PartitionEvictionPolicy() {}

        /// Clone routine - copy myself
        /// @return a copy of myself
        virtual PartitionEvictionPolicy& clone() const = 0;

        /// Get the reason for partition eviction
        /// @return when partitions should be evicted
        When getWhen() const { return when_; }

        /// Get the action for partition eviction
        /// @return how partitions should be evicted
        How getHow() const { return how_; }

    protected:
        /// Constructor
        ///
        PartitionEvictionPolicy(When when, How how) : when_(when), how_(how) {}

    private:
        When when_;
        How how_;
    };
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_EVICTION_POLICY_H */
