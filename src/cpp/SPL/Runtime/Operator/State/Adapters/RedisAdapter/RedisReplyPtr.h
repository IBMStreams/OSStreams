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
 * \file RedisReplyPtr \brief Definition of SPL::RedisReplyPtr class.
 */

#ifndef SPL_DSA_REDIS_REPLY_PTR_H
#define SPL_DSA_REDIS_REPLY_PTR_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <boost/noncopyable.hpp>
#include <hiredis/hiredis.h>

namespace SPL {
/// \brief Class that is a wrapper of redisReply to provide smart-pointer-like usage
class DLL_PUBLIC RedisReplyPtr : private boost::noncopyable
{
  public:
    /// Constructor
    /// @param reply the redis reply handle
    RedisReplyPtr(redisReply* reply);

    /// Destructor
    ~RedisReplyPtr();

    /// Get the redis reply handle
    /// @return the redis reply handle
    redisReply* get() const { return reply_; }

    /// Set the pointer to hold the given new redis reply handle
    /// The old handle (if there is any) will be recycled
    /// @param reply the new redis reply handle
    void reset(redisReply* reply);

    /// overload -> operator
    redisReply* operator->() const { return reply_; }

  private:
    redisReply* reply_; // redis reply handle
};
} // namespace SPL

#endif // SPL_DSA_REDIS_REPLY_PTR_H
