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
 * \file RedisConfig.h \brief Definition of the SPL::RedisConfig class.
 */

#ifndef SPL_DSA_REDIS_CONFIG_H
#define SPL_DSA_REDIS_CONFIG_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <sstream>
#include <string>

namespace SPL {
/// Forward declaration
class RedisServerPool;

/// \brief Class that contains utility functions to parse and verify Redis configuration
class DLL_PUBLIC RedisConfig
{
  public:
    /// Parse Redis configuration
    /// @param configStr a string containing configuration information
    /// @return a RedisServerPool instance based on configuration
    /// @throws DataStoreException if there is any error during parsing
    static RedisServerPool* parseConfiguration(const std::string& configStr);

    /// Verify Redis configuration
    /// @param configStr a string containing configuration information
    /// @param errMsg if parsing fails or configuration is invalid, return an error message
    /// @return true is the configuration is valid, false otherwise
    static bool verifyConfiguration(const std::string& configStr, std::stringstream& errMsg);
};
} // namespace SPL

#endif // SPL_DSA_REDIS_CONFIG_H
