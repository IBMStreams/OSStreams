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
 * \file HiredisPath.h \brief Definition of the SPL::HiredisPatch class.
 */

#ifndef SPL_DSA_HIREDIS_PATCH_H
#define SPL_DSA_HIREDIS_PATCH_H

#ifndef DOXYGEN_SKIP_FOR_USERS
#include <SPL/Runtime/Utility/Visibility.h>
#endif

#include <hiredis/async.h>
#include <stdint.h>

namespace SPL {
/// \brief the class that implements functions to optimize hiredis performance
class DLL_PUBLIC HiredisPatch
{
  public:
    /// "Disconnect" an asynchronous connection but make it re-usable for later communication
    /// This means: i) do not close socket; ii) do not free memory of redisc handle; iii) set the
    /// internal fields of redisc to initial values (clean up state flags, callbacks)
    /// @param ac asynchronous connection handle
    static void redisAsyncDisconnectForReuse(redisAsyncContext* ac);

    /// Internal function to check connection status
    /// @param ac asynchronous connection handle
    static int redisAsyncHandleConnectInternal(redisAsyncContext* ac);

    /// Internal function to set error string
    /// @param c redis connection context
    /// @param type error type
    /// @param str error string
    static void redisSetErrorInternal(redisContext* c, int type, const char* str);

    /// Internal function to set error string from errno
    /// @param c redis connection context
    /// @param type error type
    /// @param str error string
    static void redisSetErrorFromErrnoInternal(redisContext* c, int type, const char* prefix);

    /// Internal function to set error string from errno
    /// @param c redis connection context
    static int redisCheckSocketError(redisContext* c);

    /// Send out buffered commands to redis server
    /// @param c redis connection context
    /// @param cursor
    static int redisBufferWrite(redisContext* c,
                                const char* command,
                                const uint64_t& size,
                                uint64_t& cursor);

    /// Send a formatted command to redis server
    /// This function sends the command without copying it into hiredis internal output buffer
    /// @param c redis connection context
    /// @param command a formatted command
    /// @param size size of command
    /// @return reply from redis server
    static void* redisCommand(redisContext* c, const char* command, const uint64_t& size);
};
} // namespace SPL

#endif // SPL_DSA_HIREDIS_PATCH_H
