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
 * \file RedisConnection.h \brief Definition of the SPL::RedisConnection class.
 */

#ifndef SPL_DSA_REDIS_CONNECTION_H
#define SPL_DSA_REDIS_CONNECTION_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <boost/noncopyable.hpp>
#include <hiredis/hiredis.h>

namespace SPL {
/// Forward declaration
class RedisServer;

/// \brief Class that represents a synchronous connection to a Redis server. It is
/// a wrapper of redisContext to provide smart-pointer-like usage
class DLL_PUBLIC RedisConnection : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param server RedisServer handle
    /// @throws DataStoreException if a RedisConnection instance cannot be created
    RedisConnection(RedisServer* server);

    /// Destructor
    ~RedisConnection();

    /// Get Redis server handle
    /// @return the RedisServer handle
    RedisServer* getServer() const { return server_; }

    /// Get Redis client context handle
    /// @return the Redis client context handle
    redisContext* getRedisContext() const { return redisc_; }

    /// Overload -> operator
    redisContext* operator->() const { return redisc_; }

  private:
    RedisServer* server_;  // Redis server handle
    redisContext* redisc_; // Redis client context to use in hiredis APIs
};
} // namespace SPL

#endif // SPL_DSA_REDIS_CONNECTION_H
