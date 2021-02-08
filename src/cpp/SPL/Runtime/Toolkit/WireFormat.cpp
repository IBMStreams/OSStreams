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

#include <SPL/Toolkit/WireFormat.h>

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>

using namespace SPL;

WireFormatIO::WireFormatIO(int fd) : _fd(fd)
{
}

int WireFormatIO::pending()
{
    // No knowledge of any pending input for raw fd
    return 0;
}

ssize_t WireFormatIO::read(void * buffer, size_t len)
{
    return ::read(_fd, buffer, len);
}

ssize_t WireFormatIO::write(const void * buffer, size_t len)
{
    return ::send(_fd, buffer, len, MSG_NOSIGNAL);
}

void WireFormatIO::finish()
{
    // Nothing to do for raw fd
    return;
}

bool WireFormatIO::timedRead(char *buffer, uint32_t len)
{
    struct timeval timeOut = {60,0};
    fd_set fds;
    FD_ZERO (&fds);

    while (len) {
        int pendingBytes = pending();
        if (pendingBytes == 0) {
            FD_SET (_fd, &fds);
            int ret = ::select (_fd+1, &fds, NULL, NULL, &timeOut);
            if (ret < 0) {
                SPLAPPTRC(L_DEBUG, "select failed: " << strerror (errno), SPL_OPER_DBG);
                return false;
            } else if (ret != 1) {
                // We must have timed out
                SPLAPPTRC(L_DEBUG, "timeout waiting for read: ", SPL_OPER_DBG);
                return false;
            }
        }
        // Either pending was non-zero or select returned an fd, so we can read
        int n = read(buffer, 1);
        if (n == 1) {
            buffer++;
            len--;
        } else {
            // How can we fail?
            SPLAPPTRC(L_DEBUG, "timed read failed: " << strerror (errno), SPL_OPER_DBG);
            return false;
        }
    }

    // We have read it all
    return true;
}
