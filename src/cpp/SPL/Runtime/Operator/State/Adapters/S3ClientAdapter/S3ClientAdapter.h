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

#ifndef SPL_DSA_S3CLIENT_ADAPTER_H
#define SPL_DSA_S3CLIENT_ADAPTER_H

#include "S3ClientAdapterInterface.h"
#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3Errors.h>

namespace SPL {

/// \brief Class that represents an s3client to an ObjectStorage instance
class S3ClientAdapter : public S3ClientAdapterInterface
{
  public:
    /// Constructor
    S3ClientAdapter();

    /// Destructor
    virtual ~S3ClientAdapter();

    static S3ClientAdapter instance;

    virtual bool initialize(char const* endpoint,
                            bool isHttps,
                            char const* accessKey,
                            char const* secretKey,
                            char const* bucket,
                            FlexBuff& erro);

    virtual bool put(char const* key, char const* value, size_t const& size, FlexBuff& error);

    virtual bool getValueIfExists(char const* key,
                                  char* const preallocBuffer,
                                  const size_t& bufferSize,
                                  char*& value,
                                  size_t& returnSize,
                                  bool& isExisting,
                                  FlexBuff& error);

    virtual bool listKeys(char const* prefix, FlexBuff& keys, FlexBuff& error);

    virtual bool isExistingKey(char const* key, FlexBuff& error);

    virtual bool remove(char const* key, FlexBuff& error);

    virtual bool removeAll(char const* prefix, FlexBuff& error);

  private:
    // private initializer class initializes and de-initializes the aws api
    // therefore a static class is allocated
    struct initializer
    {
        initializer();
        ~initializer();
    };
    bool createBucket(FlexBuff& error);
    bool deleteBucket(FlexBuff& error);
    bool deleteObject(const char* keyName, FlexBuff& error);
    bool deleteObjects(Aws::Vector<Aws::S3::Model::Object> const& objects, FlexBuff& error);

    static Aws::SDKOptions options_;
    static initializer initializer_;
    Aws::Auth::AWSCredentials credentials_;
    Aws::Client::ClientConfiguration clientConfig_;
    Aws::UniquePtr<Aws::S3::S3Client> s3Client_;

    Aws::String bucketName_; // S3 object storage bucket

    static void putError(const char* operationName,
                         Aws::Client::AWSError<Aws::S3::S3Errors> const& awsError,
                         FlexBuff& errorString);

    // maximum number of objects to be returned by ListObjects request, set to maximum value of 1000
    static const long LIST_OBJECTS_SIZE = 1000;
};

} // namespace SPL

#endif // SPL_DSA_S3CLIENT_ADAPTER_H
