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
 * \file RedisUtils.h \brief Definition of the SPL::RedisUtils class.
 */

#ifndef SPL_DSA_REDIS_UTILS_H
#define SPL_DSA_REDIS_UTILS_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <hiredis/hiredis.h>
#include <stdint.h>
#include <string>
#include <tr1/unordered_set>
#include <vector>

namespace SPL {
class DLL_PUBLIC RedisUtils
{
  public:
    /// Check reply of a GET/HGET/GETRANGE commnd
    /// @param redisc a Redis connection
    /// @param reply reply for the command
    /// @param isExisting return whether the key to get exists or not
    /// @param errMsg return error message upon error
    /// @return true if the reply indicates success of the command, false if reply indicates error
    static bool checkReplyGET(const redisContext* redisc,
                              const redisReply* reply,
                              bool& isExisting,
                              std::string& errMsg);

    /// Check reply of a command which expects a simple string reply
    /// @param redisc a Redis connection
    /// @param reply reply for the command
    /// @param errMsg return error message upon error
    /// @return true if the reply indicates success of the command, false if reply indicates error
    static bool checkReplySimple(const redisContext* redisc,
                                 const redisReply* reply,
                                 std::string& errMsg);

    /// Check reply of a command which expects a status-type reply
    /// @param redisc a Redis connection
    /// @param reply reply for the command
    /// @param errMsg return error message upon error
    /// @return true if the reply indicates success of command, false if reply indicates error
    static bool checkReplyStatus(const redisContext* redisc,
                                 const redisReply* reply,
                                 std::string& errMsg);

    /// Check reply of a command which expects an ARRAY-type reply
    /// @param redisc a Redis connection
    /// @param reply reply for the command
    /// @param errMsg return error message upon error
    /// @return true if the reply indicates success of the command, false if reply indicates error
    static bool checkReplyArray(const redisContext* redisc,
                                const redisReply* reply,
                                std::string& errMsg);

    /// Check reply of a command which expects an INTEGER-type reply
    /// @param redisc a Redis connection
    /// @param reply reply for the command
    /// @param errMsg return error message upon error
    /// @return true if the reply indicates success of the command, false if reply indicates error
    static bool checkReplyInteger(const redisContext* redisc,
                                  const redisReply* reply,
                                  std::string& errMsg);

    /// Check reply of EXEC command
    /// @param redisc a Redis connection
    /// @param reply reply for EXEC command
    /// @param errMsg return error message upon error
    /// @param isAborted return true if the transaction is aborted, false otherwise
    /// @return true if the reply indicates success of EXEC command, false if reply indicates error
    static bool checkReplyEXEC(const redisContext* redisc,
                               const redisReply* reply,
                               std::string& errMsg,
                               bool& isAborted);

    /// Test whether a key exists on the specified Redis server instance
    /// @param redisc connection to a Redis server instance
    /// @param key the key to test
    /// @return return true if the key exists, false otherwise
    /// @throws DataStoreException if any error happens during operation
    static bool isExistingInServer(redisContext* redisc, const std::string& key);

    /// Get the size of an integer's string representation.
    /// For example, getIntegerStringLen(100) returns strlen("100") which is 3
    /// @return the size of an integer's string representation
    static uint32_t getStringLen(uint32_t i);

    /// Format a HDEL command with the given parameters
    /// The formatted command is placed in an internally-allocated buffer and returned to caller.
    /// The caller is responsible for deallocating the returned buffer via delete[].
    /// @cmd return a buffer containing the formatted command; return NULL if the command cannot be
    /// formatted
    /// @param hashtableName the name of the hash table
    /// @param keys the keys to delete
    /// @return the size of returned cmmand buffer (in Bytes); return -1 upon error
    static int formatHDELCommand(char** cmd,
                                 const std::string& hashtableName,
                                 const std::tr1::unordered_set<std::string>& keys);
};
} // namespace SPL

#endif // SPL_DSA_REDIS_UTILS_H
