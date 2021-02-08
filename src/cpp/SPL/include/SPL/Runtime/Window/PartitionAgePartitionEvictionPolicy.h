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

#ifndef SPL_RUNTIME_WINDOW_PARTITION_AGE_EVICTION_POLICY_H
#define SPL_RUNTIME_WINDOW_PARTITION_AGE_EVICTION_POLICY_H

/*!
 * \file PartitionAgePartitionEvictionPolicy.h \brief Definition of the SPL::PartitionAgePartitionEvictionPolicy class.
 */
#include <SPL/Runtime/Window/PartitionEvictionPolicy.h>

namespace SPL
{
    /// @ingroup Window
    /// @brief Eviction policy based on the age of partition.
    class PartitionAgePartitionEvictionPolicy : public PartitionEvictionPolicy {
    public:
        /// Constructor
        /// @param age time in seconds for partitions before eviction
        PartitionAgePartitionEvictionPolicy (double age)
            : PartitionEvictionPolicy(PartitionAge, LRU), age_(age) {}

        /// Get the partition age before eviction
        /// @return the partition age before eviction
        double getAge() const { return age_; }

        /// Clone routine - copy myself
        /// @return a copy of myself
        virtual PartitionAgePartitionEvictionPolicy& clone() const
            { return *new PartitionAgePartitionEvictionPolicy(age_);}

    private:
        double age_;
    };
}

#endif /* SPL_RUNTIME_WINDOW_PARTITION_AGE_EVICTION_POLICY_H */
