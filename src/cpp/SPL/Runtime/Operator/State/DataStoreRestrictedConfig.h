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
 * \file DataStoreRestrictedConfig.h \brief Definition of the SPL::DataStoreRestrictedConfig class.
 */

#ifndef SPL_DSA_DATA_STORE_RESTRICTED_CONFIG_H
#define SPL_DSA_DATA_STORE_RESTRICTED_CONFIG_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>
#include <map>
#include <stdint.h>
#include <string>

namespace SPL {
/// \brief Class that defines an interface for caching and retriving restricted configuration
/// information (e.g., Redis server password). Such restricted configuration is
/// in the form of string key-value pairs, and is stored as Streams platform's restricted
/// properties.
///
/// SPL::DataStoreRestrictedConfig is a singleton class and provides a process-wide cache
/// for the restricted configuration information, and makes it accessible to data store adpater
/// implementations. The main reason to provide this interface instead of letting data store adapter
/// implementation directly use PEC_NAMESPACE::DistributedPlatform::getRestrictedConfig() is that
/// the latter is only available within application PEs but not in other processes such as the
/// ApplicationCheckpointCleaner in platform's app service.
class DLL_PUBLIC DataStoreRestrictedConfig : private boost::noncopyable
{
  public:
    /// Get the value of a given configuration.
    /// @param name name of the configuration property
    /// @return value of the configuration property
    /// @throws DataStoreException if the requested configuration is not found
    static std::string get(const std::string& name);

    /// Add a configuration property
    /// @param name name of the configuration property
    /// @param value value of the configuration property
    static void add(const std::string& name, const std::string& value);

    /// Destructor
    ~DataStoreRestrictedConfig();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Constructor
    DataStoreRestrictedConfig();

    static std::map<std::string, std::string> map_; // map of cached properties
    static SPL::Mutex mutex_;                       // for thread safety
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_RESTRICTED_CONFIG_H
