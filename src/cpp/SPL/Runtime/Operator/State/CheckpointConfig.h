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
 * \file CheckpointConfig.h \brief Definition of SPL::CheckpointConfig class.
 */
#ifndef SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONFIG_H
#define SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONFIG_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreAdapterFactory.h>
#include <SPL/Runtime/Utility/Mutex.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>

namespace SPL {
/// \breif Class that represents the checkpointing backend store configuration.
/// This is a singleton class.
class DLL_PUBLIC CheckpointConfig : private boost::noncopyable
{
  public:
    /// Desctructor
    ~CheckpointConfig();

    /// Get CheckpointConfig instance
    /// @return a handle to the singleton CheckpointConfig instance
    /// @throws DataStoreException if the instance cannot be obtained
    static CheckpointConfig* instance();

    /// Get a handle to the checkpointing backend Data Store Adapter
    /// @return a handle to the Data Store Adapter
    DataStoreAdapter* getDataStoreAdapter();

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    /// Default Constructor
    /// The constructor retrieves checkpointing configuration from platform, that is, the
    /// domain or instance level checkpointRepository and checkpointRepositoryConfiguration
    /// properties. If either of these two properties is invalid, this function throws a
    /// DataStoreException. Otherwise, it uses the configuration to initialize data store
    /// adapter to the checkpointing backend store.
    /// @throws DataStoreException if an instance cannot be constructed
    CheckpointConfig();

    static CheckpointConfig* instance_;            // singleton instance
    static SPL::Mutex mutex_;                      // for thread safety
    DataStoreAdapterFactory* storeAdapterFactory_; // factory for DataStoreAdapter
    DataStoreAdapter* storeAdapter_;               // Data Store Adapter instance
#endif
};

} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_RUNTIME_OPERATOR_STATE_CHECKPOINT_CONFIG_H
