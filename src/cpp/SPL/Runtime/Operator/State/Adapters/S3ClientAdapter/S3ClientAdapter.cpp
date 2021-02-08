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
 * Implementation of the SPL::S3Client class.
 */

#include "S3ClientAdapter.h"
#include <assert.h>
#include <exception>
#include <set>
#include <stdlib.h>

#include <aws/core/Aws.h>
#include <aws/core/auth/AWSCredentialsProvider.h>
#include <aws/core/client/ClientConfiguration.h>
#include <aws/core/utils/HashingUtils.h>
#include <aws/core/utils/logging/AWSLogging.h>
#include <aws/core/utils/logging/ConsoleLogSystem.h>
#include <aws/core/utils/logging/DefaultLogSystem.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/S3Errors.h>
#include <aws/s3/model/Bucket.h>
#include <aws/s3/model/CreateBucketRequest.h>
#include <aws/s3/model/DeleteBucketRequest.h>
#include <aws/s3/model/DeleteObjectRequest.h>
#include <aws/s3/model/DeleteObjectsRequest.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/HeadObjectRequest.h>
#include <aws/s3/model/ListObjectsRequest.h>
#include <aws/s3/model/Object.h>
#include <aws/s3/model/PutObjectRequest.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>

#define S3CA_DEBUG

// This trace macro S3CA_TRACE traces to stdout if S3CA_DEBUG is defined
#ifdef S3CA_DEBUG
#define S3CA_TRACE(msg)                                                                            \
    do {                                                                                           \
        SPLAPPTRC(L_TRACE, msg, SPL_CKPT);                                                         \
    } while (false)
#else
#define S3CA_TRACE(msg)
#endif

// This macro defines the trace level of the s3 client lib
// use	 Off, Fatal, Error, Warn, Info, Debug or Trace
#ifndef S3CA_TRACE_LEVEL
#define S3CA_TRACE_LEVEL Warn
#endif

using namespace SPL;

S3ClientAdapterInterface& S3ClientAdapterInterface::getInstance()
{
    return S3ClientAdapter::instance;
}

S3ClientAdapterInterface::~S3ClientAdapterInterface() {}

S3ClientAdapter S3ClientAdapter::instance;

Aws::SDKOptions S3ClientAdapter::options_;
S3ClientAdapter::initializer S3ClientAdapter::initializer_;

S3ClientAdapter::initializer::initializer()
{
    S3CA_TRACE("-----> S3ClientAdapter::initializer::initializer()");
    Aws::Utils::Logging::InitializeAWSLogging(
      Aws::MakeShared<Aws::Utils::Logging::ConsoleLogSystem>(
        "S3CA_TRACE", Aws::Utils::Logging::LogLevel::S3CA_TRACE_LEVEL));

    // options_.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Trace;
    // must not set options trace level, because trace is off if it is set twice
    Aws::InitAPI(options_);
    S3CA_TRACE("<----- S3ClientAdapter::initializer::initializer()");
}

S3ClientAdapter::initializer::~initializer()
{
    S3CA_TRACE("-----> S3ClientAdapter::initializer::~initializer()");
    Aws::ShutdownAPI(options_);
    Aws::Utils::Logging::ShutdownAWSLogging();
    S3CA_TRACE("<----- S3ClientAdapter::initializer::~initializer()");
}

S3ClientAdapter::S3ClientAdapter()
  : credentials_()
  , clientConfig_()
  , s3Client_()
  , bucketName_()
{
    S3CA_TRACE("<-----> S3ClientAdapter::S3ClientAdapter()");
}

S3ClientAdapter::~S3ClientAdapter()
{
    S3CA_TRACE("<-----> S3ClientAdapter::~S3ClientAdapter()");
}

bool S3ClientAdapter::initialize(char const* endpoint,
                                 bool isHttps,
                                 char const* accessKey,
                                 char const* secretKey,
                                 char const* bucket,
                                 FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::initialize (bucket=" << bucket << "...)");

    bucketName_.assign(bucket);

    bool result = false;

    clientConfig_.scheme = isHttps ? Aws::Http::Scheme::HTTPS : Aws::Http::Scheme::HTTP;
    clientConfig_.endpointOverride = endpoint;
    clientConfig_.region = "ibmCloudObjectStorage";
    clientConfig_.requestTimeoutMs = 30000;
    clientConfig_.connectTimeoutMs = 10000;
    // In each PE there is a pool of 4 threads for checkpointing/restoring all operators in the PE,
    // and another separate pool of 2 threads for asynchronously removing old checkpoints of all
    // operators in the PE
    clientConfig_.maxConnections = 10; // so 10 should be sufficient
    // Use dual stack endpoint in the endpoint calculation.
    // It is your responsibility to verify that the service supports ipv6 in the region you select.
    // clientConfig_.useDualStack = true;   // true seems to be default

    credentials_.SetAWSAccessKeyId(accessKey);
    credentials_.SetAWSSecretKey(secretKey);
    s3Client_ = Aws::MakeUnique<Aws::S3::S3Client>("S3CLIENT", credentials_, clientConfig_);

    bool isFound = false;
    // -----------------------------------------------------
    // list all buckets and all objects
    auto outcome = s3Client_->ListBuckets();
    if (outcome.IsSuccess()) {
        S3CA_TRACE("------ IBM COS - S3 buckets:");
        Aws::Vector<Aws::S3::Model::Bucket> const& bucket_list = outcome.GetResult().GetBuckets();
        for (auto const& abucket : bucket_list) {
            S3CA_TRACE("------ * " << abucket.GetName());
            if (bucketName_ == abucket.GetName()) {
                isFound = true;
            }
        }
        if (isFound) {
            result = true;
        } else {
            if (createBucket(error)) {
                result = true;
            }
        }
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        putError("ListBuckets", requestError, error);
    }
    S3CA_TRACE("<----- S3ClientAdapter::initialize (bucket=" << bucket << "...) result=" << result);
    return result;
}

bool S3ClientAdapter::put(char const* key, char const* value, size_t const& size, FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::put (" << key << ")");
    Aws::S3::Model::PutObjectRequest request;
    request.WithBucket(bucketName_).WithKey(key);

    bool result;
    auto requestStream = Aws::MakeShared<Aws::StringStream>("s3client");
    S3CA_TRACE("------ PutObject going to write " << size << " bytes for key " << key);
    requestStream->write(value, size);
    request.SetBody(requestStream);
    request.SetContentLength(size);

    auto outcome = s3Client_->PutObject(request);

    if (outcome.IsSuccess()) {
        result = true;
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        putError("PutObject", requestError, error);
    }
    S3CA_TRACE("<-----  S3ClientAdapter::put (" << key
                                                << ") success = " << (result ? "Yes" : "No"));
    return result;
}

bool S3ClientAdapter::getValueIfExists(char const* key,
                                       char* const preallocBuffer,
                                       const size_t& bufferSize,
                                       char*& value,
                                       size_t& returnSize,
                                       bool& isExisting,
                                       FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::getValueIfExists ("
               << key << ", " << reinterpret_cast<void const* const>(preallocBuffer) << "...)");

    bool result = false;
    if (preallocBuffer) {
        value = preallocBuffer; // return prealloc buffer in value
    }
    Aws::S3::Model::GetObjectRequest getObjectRequest;
    getObjectRequest.SetBucket(bucketName_);
    getObjectRequest.SetKey(key);
    S3CA_TRACE("------ GetObject: " << key);
    Aws::S3::Model::GetObjectOutcome getObjectOutcome = s3Client_->GetObject(getObjectRequest);
    if (!getObjectOutcome.IsSuccess()) {
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = getObjectOutcome.GetError();
        isExisting = false;
        returnSize = 0; //??
        auto errtype = requestError.GetErrorType();
        if ((errtype == Aws::S3::S3Errors::RESOURCE_NOT_FOUND) ||
            (errtype == Aws::S3::S3Errors::NO_SUCH_KEY)) {
            result = true;
            S3CA_TRACE("------ S3ClientAdapter::getValueIfExists ("
                       << key << ") errtype=" << static_cast<int>(errtype));
        } else {
            putError("GetObject", requestError, error);
        }
        // do not touch returnSize and value
    } else {
        isExisting = true;
        Aws::IOStream& resultStream = getObjectOutcome.GetResult().GetBody();
        size_t resultSize = getObjectOutcome.GetResult().GetContentLength();
        S3CA_TRACE("------ GetObject GetContentLength(): " << resultSize);
        if (preallocBuffer) {
            if (resultSize <= bufferSize) {
                result = true;
            } else {
                returnSize = 0; //??
                std::stringstream sStream;
                sStream << "GetObject error: provided buffer too small for read content. "
                        << "buffer size = " << bufferSize
                        << ", size of read content = " << resultSize;
                S3CA_TRACE("------ " << sStream.str());
                error.put(sStream.str().c_str(), sStream.str().size());
            }
        } else {
            value = new char[resultSize];
            if (value) {
                result = true;
            } else {
                returnSize = 0; //??
                error.put("GetObject error: out of memory!");
            }
        }
        if (result) {
            resultStream.read(value, resultSize);
            size_t nBytesRead = resultStream.gcount();
            returnSize = nBytesRead;
            S3CA_TRACE("------ GetObject #bytes read from stream: " << nBytesRead);
        }
    }
    S3CA_TRACE("<----- S3ClientAdapter::getValueIfExists ("
               << key << ", " << reinterpret_cast<void const* const>(preallocBuffer)
               << " result=" << result);
    return result;
}

bool S3ClientAdapter::listKeys(char const* prefix, FlexBuff& keys, FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::listKeys(= " << prefix << "...)");
    Aws::S3::Model::ListObjectsRequest request;
    request.SetBucket(bucketName_);
    request.SetPrefix(prefix);
    request.SetMaxKeys(LIST_OBJECTS_SIZE);
    //	request.SetDelimiter ("/");		// request returns nothing if delimiter is set.
    Aws::S3::Model::ListObjectsOutcome requestOutcome;

    bool success = true;
    bool resultIsTruncated = true;

    std::vector<std::string> collectedKeys;
    do {
        requestOutcome = s3Client_->ListObjects(request);
        if (requestOutcome.IsSuccess()) {
            auto& result = requestOutcome.GetResult();
            Aws::Vector<Aws::S3::Model::Object> const& object_list = result.GetContents();
            for (auto const& s3_object : object_list) {
                S3CA_TRACE("------ * " << s3_object.GetKey());
                collectedKeys.push_back(s3_object.GetKey().c_str());
            }
            resultIsTruncated = result.GetIsTruncated();
            S3CA_TRACE("------ result.GetIsTruncated() = " << resultIsTruncated);
            if (resultIsTruncated) {
                // Aws::String marker = result.GetMarker();    // GetMarker returns an empty string
                request.SetMarker(collectedKeys.back().c_str());
                S3CA_TRACE("------ marker for next request = " << collectedKeys.back());
            }
        } else {
            success = false;
            const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError =
              requestOutcome.GetError();
            putError("ListObjects", requestError, error);
        }
    } while (resultIsTruncated && success);
    keys.put(collectedKeys);
    S3CA_TRACE("<----- S3ClientAdapter::listKeys success = " << (success ? "Yes" : "No"));
    return success;
}

bool S3ClientAdapter::remove(const char* key, FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::remove (" << key << ")");
    bool success = deleteObject(key, error);
    S3CA_TRACE("<----- S3ClientAdapter::remove (" << key
                                                  << ") success = " << (success ? "Yes" : "No"));
    return success;
}

bool S3ClientAdapter::removeAll(const char* prefix, FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::removeAll(prefix = " << prefix << "...)");
    Aws::S3::Model::ListObjectsRequest request;
    request.SetBucket(bucketName_);
    request.SetPrefix(prefix);
    request.SetMaxKeys(LIST_OBJECTS_SIZE);
    //	request.SetDelimiter ("/");		// request returns nothing if delimiter is set.
    Aws::S3::Model::ListObjectsOutcome requestOutcome;

    bool success = true;
    bool resultIsTruncated = true;

    do {
        requestOutcome = s3Client_->ListObjects(request);
        if (requestOutcome.IsSuccess()) {
            auto& result = requestOutcome.GetResult();
            Aws::Vector<Aws::S3::Model::Object> const& object_list = result.GetContents();

            // Delete the objects in the list.
            deleteObjects(object_list, error);

            // if there are more, get the next batch
            resultIsTruncated = result.GetIsTruncated();
            S3CA_TRACE("------ result.GetIsTruncated() = " << resultIsTruncated);
            if (resultIsTruncated) {
                // Aws::String marker = result.GetMarker();    // GetMarker returns an empty string
                request.SetMarker(object_list.back().GetKey());
                S3CA_TRACE("------ marker for next request = " << object_list.back().GetKey());
            }
        } else {
            success = false;
            const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError =
              requestOutcome.GetError();
            putError("ListObjects", requestError, error);
        }
    } while (resultIsTruncated);
    S3CA_TRACE("<----- S3ClientAdapter::removeAll success = " << (success ? "Yes" : "No"));
    return success;
}

bool S3ClientAdapter::isExistingKey(const char* key, FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::isExistingKey (" << key << ")");
    Aws::S3::Model::HeadObjectRequest request;
    request.WithBucket(bucketName_).WithKey(key);
    auto outcome = s3Client_->HeadObject(request);
    bool result;
    if (outcome.IsSuccess()) {
        result = true;
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        auto errType = requestError.GetErrorType();
        if (!((errType == Aws::S3::S3Errors::RESOURCE_NOT_FOUND) ||
              (errType == Aws::S3::S3Errors::NO_SUCH_KEY))) {
            putError("HeadObject", requestError, error);
        }
    }
    S3CA_TRACE("<----- S3ClientAdapter::isExistingKey ("
               << key << ") success = " << (result ? "Yes" : "No"));
    return result;
}

bool S3ClientAdapter::createBucket(FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::createBucket bucketName = " << bucketName_);
    Aws::S3::Model::CreateBucketRequest request;
    request.SetBucket(bucketName_);
    auto outcome = s3Client_->CreateBucket(request);
    bool result;
    if (outcome.IsSuccess()) {
        result = true;
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        putError("CreateBucket", requestError, error);
    }
    S3CA_TRACE("<----- S3ClientAdapter::createBucket bucketName = " << bucketName_
                                                                    << " result=" << result);
    return result;
}

bool S3ClientAdapter::deleteBucket(FlexBuff& error)
{
    S3CA_TRACE("-----> S3ClientAdapter::deleteBucket bucketName = " << bucketName_ << ")");
    bool result;
    Aws::S3::Model::DeleteBucketRequest request;
    request.SetBucket(bucketName_);

    auto outcome = s3Client_->DeleteBucket(request);

    if (outcome.IsSuccess()) {
        result = true;
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        putError("DeleteBucket", requestError, error);
    }
    S3CA_TRACE("<----- S3ClientAdapter::deleteBucket bucketName = " << bucketName_
                                                                    << " result=" << result);
    return result;
}

bool S3ClientAdapter::deleteObject(const char* keyName, FlexBuff& error)
{
    S3CA_TRACE("-----> deleteObject key = " << keyName);
    Aws::S3::Model::DeleteObjectRequest request;
    request.WithBucket(bucketName_).WithKey(keyName);
    auto outcome = s3Client_->DeleteObject(request);
    bool result;
    if (outcome.IsSuccess()) {
        result = true;
    } else {
        result = false;
        const Aws::Client::AWSError<Aws::S3::S3Errors>& requestError = outcome.GetError();
        putError("DeleteObject", requestError, error);
    }
    S3CA_TRACE("<----- deleteObject key = " << keyName << " result=" << result);
    return result;
}

namespace SPL {
// This is a workaround for a bug in
// Aws::S3::Model::DeleteObjectsRequest::GetBody
class DeleteObjectsRequest : public Aws::S3::Model::DeleteObjectsRequest
{
  public:
    DeleteObjectsRequest()
      : Aws::S3::Model::DeleteObjectsRequest()
    {}
    // Override to work around bug in super class
    std::shared_ptr<Aws::IOStream> GetBody() const override
    {
        S3CA_TRACE("-----> GetBody");
        Aws::String payload = SerializePayload();
        std::shared_ptr<Aws::IOStream> body;
        if (!payload.empty()) {
            S3CA_TRACE("------ add payload");
            body = Aws::MakeShared<Aws::StringStream>("SPL.DeleteObjectsRequest");
            *body << payload;
        } else {
            S3CA_TRACE("------ no payload");
        }
        S3CA_TRACE("<----- GetBody");
        return body;
    }
};
} // namespace SPL

bool S3ClientAdapter::deleteObjects(Aws::Vector<Aws::S3::Model::Object> const& objects,
                                    FlexBuff& error)
{
    S3CA_TRACE("-----> deleteObjects");
    bool success = true;

    SPL::DeleteObjectsRequest deleteRequest;
    deleteRequest.SetBucket(bucketName_);
    Aws::S3::Model::Delete deletes;
    deletes.SetQuiet(true);

    S3CA_TRACE("------ DeleteObjects");
    for (auto const& s3_object : objects) {
        S3CA_TRACE("------ * " << s3_object.GetKey());
        Aws::S3::Model::ObjectIdentifier id;
        id.SetKey(s3_object.GetKey());
        deletes.AddObjects(id);
    }

    deleteRequest.SetDelete(deletes);

    // Send DeleteObjectsRequest
    Aws::S3::Model::DeleteObjectsOutcome deleteOutcome;
    S3CA_TRACE("------ DeleteObjects request sent");
    deleteOutcome = s3Client_->DeleteObjects(deleteRequest);
    S3CA_TRACE("------ DeleteObjects response received");
    if (deleteOutcome.IsSuccess()) {
        // continue
        S3CA_TRACE("------ DeleteObjects success = true");
    } else {
        S3CA_TRACE("------ DeleteObjects success = false");
        /*
          const Aws::Vector<Aws:S3::Error> errors = deleteOutcome.GetErrors();
          for (auto const & error: errors) {
          // TODO handle individual errors
          }
        */
        auto const& awsError = deleteOutcome.GetError();
        putError("DeleteObjects", awsError, error);
        success = false;
    }

    S3CA_TRACE("<----- deleteObjects result= " << success);
    return success;
}

void S3ClientAdapter::putError(const char* operationName,
                               Aws::Client::AWSError<Aws::S3::S3Errors> const& awsError,
                               FlexBuff& errorString)
{
    Aws::StringStream sStream;
    sStream << operationName << " error: " << awsError.GetExceptionName() << " - "
            << awsError.GetMessage() << " (" << static_cast<int>(awsError.GetErrorType())
            << "); should retry: " << (awsError.ShouldRetry() ? "true" : "false");
    S3CA_TRACE("------ " << sStream.str());
    errorString.put(sStream.str().c_str(), sStream.str().size());
}
