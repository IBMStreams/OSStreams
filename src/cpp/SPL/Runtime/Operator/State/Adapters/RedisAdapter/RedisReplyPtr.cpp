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
 * Implementation of SPL::RedisReplyPtr class.
 */
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisReplyPtr.h>

using namespace SPL;

RedisReplyPtr::RedisReplyPtr(redisReply* reply)
  : reply_(reply)
{}

RedisReplyPtr::~RedisReplyPtr()
{
    if (reply_) {
        freeReplyObject(reply_);
    }
}

void RedisReplyPtr::reset(redisReply* reply)
{
    if (reply_) {
        freeReplyObject(reply_);
    }
    reply_ = reply;
}
