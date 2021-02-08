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
 * Implementation of SPL::S3ClientInterface class
 */
#ifndef SPL_DSA_S3CLIENT_ADAPTER_INTERFACE_H
#define SPL_DSA_S3CLIENT_ADAPTER_INTERFACE_H

#include "FlexBuff.h"
#include <cstddef>

namespace SPL {

class S3ClientAdapterInterface
{
  public:
    virtual ~S3ClientAdapterInterface();

    static S3ClientAdapterInterface& getInstance();

    /// Initialize the S3ClientAdapterInterface instance
    /// @param endpoint the name of the end-point identifier of the ObjectStorage
    /// @param isHttps set to true to use HTTPS as protocol, otherwise HTTP is used.
    /// @param accessKey the access key of the ObjectStorage
    /// @param secretKey the secret authentication token
    /// @param bucket the bucket name
    /// @param error returns the error string in case of error (return false)
    /// @return true in case of success, false otherwise
    virtual bool initialize(char const* endpoint,
                            bool isHttps,
                            char const* accessKey,
                            char const* secretKey,
                            char const* bucket,
                            FlexBuff& error) = 0;

    /// Put a key value pair into the bucket
    /// @param key the key, a c-string
    /// @param value the value, a char* which may contain binary data
    /// @param size the length of the value data
    /// @param error returns the error string in case of error (return false)
    /// @return true in case of success, false otherwise
    virtual bool put(char const* key, char const* value, size_t const& size, FlexBuff& error) = 0;

    /// Get(read) the value of the given key from this Data Store Entry if it exists. If the key
    /// does not exist, the isExisting is set to false and the return value is true. In all other
    /// errors, the return value is false. Upon return, value points to an buffer which contains the
    /// retrieved value, and returnSize returns the size (in Bytes) of the retrieved value. If
    /// preallocBuffer has a value equal to NULL, a buffer with an appropriate size is allocated and
    /// is returned as value pointer. If preallocBuffer has a value not equal to NULL, the
    /// preallocBuffer is used as storage area for the results and is returned as value pointer. If
    /// the size of the returned values exceed the bufferSize, false is returned and an appropriate
    /// error text is generated. The user is responsible for de-allocating the buffer after use with
    /// delete[].
    /// @param key the key, a c-string
    /// @param preallocBuffer the preallocated buffer buffer. NULL if no prealloc buffer is used
    /// @param bufferSize the size of the preallocated buffer. Don't care if preallocBuffer == NULL
    /// @param value the method returns the pointer to returned value.
    ///			If a preallocated buffer is used, value returns preallocBuffer in any case
    ///			If no preallocated buffer is used, value returns a valid pointer only if the
    /// operation was a success and the key exists
    /// @param returnSize returns the number of bytes written into the value buffer
    ///			returnSize returns a valid size only if the operation was a success and the
    /// key exists. It returns 0 otherwise
    /// @param isExisting returns the indication whether the key-value mapping exists
    ///			returns a valid value only if the operation was a success
    /// @param error returns the error string in case of error (return false)
    /// @return true in case of success, false otherwise
    virtual bool getValueIfExists(char const* key,
                                  char* const preallocBuffer,
                                  const size_t& bufferSize,
                                  char*& value,
                                  size_t& returnSize,
                                  bool& isExisting,
                                  FlexBuff& error) = 0;

    /// list all keys with the given prefix
    /// @param prefix the prefix for the searched keys
    /// @param keys returns all found keys with prefix
    /// @param returns the error string in case of error (return false)
    /// @param error returns the error string in case of error (return false)
    /// @return true in case of success, false otherwise
    virtual bool listKeys(char const* prefix, FlexBuff& keys, FlexBuff& error) = 0;

    /// Removes a data store entry with the given key
    /// @param key the key, a c-string
    /// @param error returns the error string in case of error (return false)
    /// @return true if the removal succeeded, false otherwise
    virtual bool remove(char const* key, FlexBuff& error) = 0;

    /// removes all key that start with the given prefix
    /// @param error returns the error string in case of error (return false)
    /// @param prefix the all key with this prefix are removed
    /// @return true if the removal completely succeeded, false otherwise
    virtual bool removeAll(char const* prefix, FlexBuff& error) = 0;

    /// Checks if an key exist
    /// @param key the key, a c-string
    /// @param error returns the error string in case of error (return false)
    /// @return true if exists, false otherwise
    virtual bool isExistingKey(char const* key, FlexBuff& error) = 0;
};

}

#endif // SPL_DSA_S3CLIENT_ADAPTER_INTERFACE_H
