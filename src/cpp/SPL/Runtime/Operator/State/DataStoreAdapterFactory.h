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
 * \file DataStoreAdapterFactory.h \brief Definition of SPL::DataStoreAdapterFactory class
 */

#ifndef SPL_DSA_DATA_STORE_ADAPTER_FACTORY_H
#define SPL_DSA_DATA_STORE_ADAPTER_FACTORY_H

#ifndef DOXYGEN_SKIP_FOR_USERS

#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/noncopyable.hpp>

namespace SPL {

/// \brief Factory class of Data Store Adapter
class DLL_PUBLIC DataStoreAdapterFactory : private boost::noncopyable
{
  public:
    /// Constructor
    /// Currently support three types of backend stores: fileSystem, redis and objectStorage
    /// @param adapterType the type of Data Store Adapter
    /// @throws DataStoreException if the type is invalid
    DataStoreAdapterFactory(const std::string& adapterType);

    /// Destructor
    ~DataStoreAdapterFactory();

    /// Create an instance of Data Store Adapter
    /// @param configString configuration for the Data Store Adapter
    /// @return the created Data Store Adapter instance
    /// @throws DataStoreException if a Data Store Adapter instance cannot be created
    DataStoreAdapter* createDataStoreAdapter(const std::string& configString);

    /// Constant string for fileSystem, redis and objectStorage adapter names
    static const std::string fileSystemAdapterStr;
    static const std::string redisAdapterStr;
    static const std::string objectStorageAdapterStr;

#ifndef DOXYGEN_SKIP_FOR_USERS
  private:
    static const std::string redisLibraryFile;         // DLL for redis adapter
    static const std::string objectStorageLibraryFile; // DLL for objectStorage adapter

    typedef DataStoreAdapter* (*create_t)(std::string);

    std::string type_; // type of Data Store Adapter
    void* dllHandle_;  // DLL handle
    create_t create_;  // function to create Data Store Adapter
#endif
};
} // namespace SPL

#endif // DOXYGEN_SKIP_FOR_USERS

#endif // SPL_DSA_DATA_STORE_ADAPTER_FACTORY_H
