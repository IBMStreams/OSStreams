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
 * Implementation of the SPL::HiredisPatch class
 */

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Operator/State/Adapters/RedisAdapter/HiredisPatch.h>
#include <SPL/Runtime/Operator/State/DataStoreException.h>
#include <assert.h>
#include <errno.h>
#include <exception>
#include <hiredis/hiredis.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

using namespace std;
using namespace SPL;

void HiredisPatch::redisAsyncDisconnectForReuse(struct redisAsyncContext* ac)
{
    ac->err = 0;
    ac->errstr = NULL;
    ac->data = NULL;
    ac->replies.head = NULL;
    ac->replies.tail = NULL;
    (ac->c).flags &= ~REDIS_DISCONNECTING;
    (ac->c).flags &= ~REDIS_FREEING;
}

int HiredisPatch::redisAsyncHandleConnectInternal(redisAsyncContext* ac)
{
    redisContext* c = &(ac->c);

    if (redisCheckSocketError(c) == REDIS_ERR) {
        /* Try again later when connect(2) is still in progress. */
        if (errno == EINPROGRESS) {
            return REDIS_OK;
        }

        if (ac->onConnect) {
            ac->onConnect(ac, REDIS_ERR);
        }
        return REDIS_ERR;
    }

    /* Mark context as connected. */
    c->flags |= REDIS_CONNECTED;
    if (ac->onConnect) {
        ac->onConnect(ac, REDIS_OK);
    }
    return REDIS_OK;
}

void HiredisPatch::redisSetErrorInternal(redisContext* c, int type, const char* str)
{
    size_t len;

    c->err = type;
    if (str != NULL) {
        len = strlen(str);
        len = len < (sizeof(c->errstr) - 1) ? len : (sizeof(c->errstr) - 1);
        memcpy(c->errstr, str, len);
        c->errstr[len] = '\0';
    } else {
        /* Only REDIS_ERR_IO may lack a description! */
        assert(type == REDIS_ERR_IO);
        strerror_r(errno, c->errstr, sizeof(c->errstr));
    }
}

void HiredisPatch::redisSetErrorFromErrnoInternal(redisContext* c, int type, const char* prefix)
{
    char buf[128] = { 0 };
    size_t len = 0;

    if (prefix != NULL) {
        len = snprintf(buf, sizeof(buf), "%s: ", prefix);
    }
    strerror_r(errno, buf + len, sizeof(buf) - len);
    redisSetErrorInternal(c, type, buf);
}

int HiredisPatch::redisCheckSocketError(redisContext* c)
{
    int err = 0;
    socklen_t errlen = sizeof(err);

    if (getsockopt(c->fd, SOL_SOCKET, SO_ERROR, &err, &errlen) == -1) {
        redisSetErrorFromErrnoInternal(c, REDIS_ERR_IO, "getsockopt(SO_ERROR)");
        return REDIS_ERR;
    }
    if (err) {
        errno = err;
        redisSetErrorFromErrnoInternal(c, REDIS_ERR_IO, NULL);
        return REDIS_ERR;
    }
    return REDIS_OK;
}

int HiredisPatch::redisBufferWrite(redisContext* c,
                                   const char* command,
                                   const uint64_t& size,
                                   uint64_t& cursor)
{
    /* Return early when the context has seen an error. */
    if (c->err) {
        return REDIS_ERR;
    }

    int nwritten;
    if (cursor < size) {
        nwritten = send(c->fd, command + cursor, (size - cursor), MSG_NOSIGNAL);
        if (nwritten > 0) {
            cursor += nwritten;
            return REDIS_OK;
        } else if (nwritten == -1) {
            if (((errno == EAGAIN || errno == EWOULDBLOCK) && !(c->flags & REDIS_BLOCK)) ||
                (errno == EINTR)) {
                /* Try again later */
            } else {
                redisSetErrorInternal(c, REDIS_ERR_IO, NULL);
                return REDIS_ERR;
            }
        }
    }
    return REDIS_OK;
}

void* HiredisPatch::redisCommand(redisContext* c, const char* command, const uint64_t& size)
{
    void* reply;
    uint64_t cursor = 0;

    /* keep writing until all bytes are sent */
    do {
        if (HiredisPatch::redisBufferWrite(c, command, size, cursor) == REDIS_ERR) {
            return NULL;
        }
    } while (cursor != size);

    /* keep waiting until a reply is received */
    do {
        if (redisBufferRead(c) == REDIS_ERR) {
            return NULL;
        }
        if (redisGetReplyFromReader(c, &reply) == REDIS_ERR) {
            return NULL;
        }
    } while (reply == NULL);
    return reply;
}
