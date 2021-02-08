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

/*
 * \file RedisSharding.h \brief Definition of the SPL::RedisSharding class.
 */

#ifndef SPL_DSA_REDIS_SHARDING_H
#define SPL_DSA_REDIS_SHARDING_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <stdint.h>
#include <string>

namespace SPL {
/// \brief Class that contains utility functions for sharding
class DLL_PUBLIC RedisSharding
{
  public:
    /// Get the server index for the given name
    /// @param name the name to map
    /// @param totalNumShards total number of shards
    /// @return index of the server to which the name is mapped
    static uint32_t mapNameToServer(const std::string& name, const uint32_t& totalNumShards);

    /// Set seed value
    /// @param value value to set
    static void setSeedValue(const uint32_t value);

    /// Seed value used for hashing
    static uint32_t seed;
};
} // namespace SPL

#endif // SPL_DSA_REDIS_SHARDING_H
