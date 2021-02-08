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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_COUNT_EVICTION_POLICY_H
#define SPL_RUNTIME_WINDOW_PARTITION_COUNT_EVICTION_POLICY_H

/*!
 * \file PartitionCountPartitionEvictionPolicy.h \brief Definition of the SPL::PartitionCountPartitionEvictionPolicy class.
 */

#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>

namespace SPL
{
    /// @ingroup Window
    /// @brief Partitions should be evicted when there are too many partitions
    class PartitionCountPartitionEvictionPolicy : public PartitionEvictionPolicy {
    public:
        /// Constructor
        /// @param count maximum number of allowed partitions
        /// @param how partition selection policy
        PartitionCountPartitionEvictionPolicy (uint32_t count, How how)
            : PartitionEvictionPolicy(PartitionCount, how), count_(count) {}

        /// Get the maximum number of allowed partitions
        /// @return the maximum number of allowed partitions
        uint32_t getCount() const { return count_; }

        /// Clone routine - copy myself
        /// @return a copy of myself
        virtual PartitionCountPartitionEvictionPolicy& clone() const
            { return *new PartitionCountPartitionEvictionPolicy(count_, getHow());}

    private:
        uint32_t count_;
    };
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_COUNT_EVICTION_POLICY_H */
