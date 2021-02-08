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
 * Implementation of the SPL::RedisUtils class.
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/RedisUtils.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <boost/lexical_cast.hpp>
#include <exception>
#include <hiredis/hiredis.h>
#include <string>
#include <strings.h>

using namespace std;
using namespace SPL;

bool RedisUtils::checkReplySimple(const redisContext* redisc,
                                  const redisReply* reply,
                                  std::string& errMsg)
{
    if (reply && reply->type != REDIS_REPLY_ERROR) {
        return true;
    }
    if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else {
        errMsg.assign(reply->str);
    }
    return false;
}

bool RedisUtils::checkReplyStatus(const redisContext* redisc,
                                  const redisReply* reply,
                                  std::string& errMsg)
{
    if (reply && reply->type == REDIS_REPLY_STATUS) {
        if (strcasecmp(reply->str, "ok") == 0) {
            return true;
        } else {
            errMsg.assign(reply->str);
        }
    } else if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else if (reply->type == REDIS_REPLY_ERROR) {
        errMsg.assign(reply->str);
    } else {
        errMsg = "returns non-status type: " + reply->type;
    }
    return false;
}

bool RedisUtils::checkReplyGET(const redisContext* redisc,
                               const redisReply* reply,
                               bool& isExisting,
                               std::string& errMsg)
{
    if (reply && reply->type == REDIS_REPLY_STRING) {
        isExisting = true;
        return true;
    }
    if (reply && reply->type == REDIS_REPLY_NIL) {
        isExisting = false;
        return true;
    } else if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else if (reply->type == REDIS_REPLY_ERROR) {
        errMsg.assign(reply->str);
    } else {
        errMsg = "returns non-string type: " + reply->type;
    }
    return false;
}

bool RedisUtils::checkReplyEXEC(const redisContext* redisc,
                                const redisReply* reply,
                                std::string& errMsg,
                                bool& isAborted)
{
    isAborted = false;
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        return true;
    }
    if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else if (reply->type == REDIS_REPLY_ERROR) {
        errMsg.assign(reply->str);
    } else if (reply->type == REDIS_REPLY_NIL) {
        errMsg.assign("transaction aborted");
        isAborted = true;
    } else {
        errMsg = "returns non-array type: " + reply->type;
    }
    return false;
}

bool RedisUtils::checkReplyArray(const redisContext* redisc,
                                 const redisReply* reply,
                                 std::string& errMsg)
{
    if (reply && reply->type == REDIS_REPLY_ARRAY) {
        return true;
    }
    if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else if (reply->type == REDIS_REPLY_ERROR) {
        errMsg.assign(reply->str);
    } else {
        errMsg = "returns non-array type: " + reply->type;
    }
    return false;
}

bool RedisUtils::checkReplyInteger(const redisContext* redisc,
                                   const redisReply* reply,
                                   std::string& errMsg)
{
    if (reply && reply->type == REDIS_REPLY_INTEGER) {
        return true;
    }
    if (reply == NULL) {
        errMsg.assign(redisc->errstr);
    } else if (reply->type == REDIS_REPLY_ERROR) {
        errMsg.assign(reply->str);
    } else {
        errMsg = "returns non-integer type: " + reply->type;
    }
    return false;
}

bool RedisUtils::isExistingInServer(redisContext* redisc, const std::string& key)
{
    assert(!key.empty());

    std::string errMsg;
    redisReply* reply = (redisReply*)redisCommand(redisc, "EXISTS %b", key.c_str(), key.size());
    if (RedisUtils::checkReplyInteger(redisc, reply, errMsg) == true) {
        bool isExisting = (reply->integer == 1LL);
        freeReplyObject(reply);
        return isExisting;
    } else {
        if (reply) {
            freeReplyObject(reply);
        }
        THROW(DataStore, "isExistingInServer() failed, server replied error: " << errMsg);
    }
}

uint32_t RedisUtils::getStringLen(uint32_t i)
{
    uint32_t len = 0;
    do {
        len++;
        i /= 10;
    } while (i);
    return len;
}

int RedisUtils::formatHDELCommand(char** cmd,
                                  const std::string& hashtableName,
                                  const std::tr1::unordered_set<std::string>& keys)
{
    assert(!hashtableName.empty());
    assert(!keys.empty());

    // construct a formatted command, the same command is sent to all replica servers
    // HDEL storeEntryName key1 key2 ... keyN
    // here we format the binary command directly:
    // 2A N+2 0D 0A 24 34 0D 0A HDEL_COMMAND 0D 0A 24 SE_NAME_SIZE 0D 0A SE_NAME 0D 0A 24 KEY1_SIZE
    // 0D 0A KEY1 0D 0A 24 Key2_SIZE 0D 0A Key2 0D 0A ... which prints out as follows:
    // *N+2\r\n$4\r\nHDEL\r\n$SE_NAME_SIZE\r\nSE_NAME\r\n$KEY1_SIZE\r\nKEY1\r\n$KEY2_SIZE\r\nKEY2\r\n...\r\n$KEYN_SIZE\r\nKEYN\r\n
    int htNameSize = hashtableName.size() + 1;
    int cmdSize =
      14 + getStringLen(keys.size() + 2) + getStringLen(htNameSize) + 2 + htNameSize + 2;
    for (std::tr1::unordered_set<std::string>::const_iterator iter = keys.begin();
         iter != keys.end(); ++iter) {
        int keySize = iter->size();
        cmdSize += 1 + getStringLen(keySize) + 2 + keySize + 2;
    }
    char* packedCmd = new char[cmdSize];
    int cmdLength = sprintf(packedCmd, "*%u\r\n$4\r\nHDEL\r\n$%u\r\nD%s", (uint32_t)keys.size() + 2,
                            htNameSize, hashtableName.c_str());
    char* keyStart = packedCmd + cmdLength;
    for (std::tr1::unordered_set<std::string>::const_iterator iter = keys.begin();
         iter != keys.end(); ++iter) {
        int kvSize = sprintf(keyStart, "\r\n$%u\r\n%s", (uint32_t)iter->size(), iter->c_str());
        keyStart += kvSize;
    }
    keyStart[0] = '\r';
    keyStart[1] = '\n';
    *cmd = packedCmd;
    return cmdSize;
}
