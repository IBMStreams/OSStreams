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

#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/Port/Punctuation.h>
#include <SPL/Toolkit/HotFile.h>

std::streamsize SPL::HotFile::read(char* s, std::streamsize n)
{
    while (true) {
        ssize_t bytesRead = ::read(_fd, s, n);
        if (bytesRead == (ssize_t)-1) {
            return -1; // Something terrible happened (or we are shutting down)
        }
        if (bytesRead != 0) {
            _position += bytesRead;
            return bytesRead;
        }

        // We got EOF.  Wait, and try again
        // Has the file decreased in size?
        struct stat stat;
        if (fstat(_fd, &stat) == 0) {
            if (stat.st_size < _position) {
                // We got smaller.  restart at the beginning
                ::lseek(_fd, 0, SEEK_SET);
                _op.submit(Punctuation::WindowMarker, 0);
                _position = 0;
            }
        }
        _pe.blockUntilShutdownRequest(1.0);
    }
    // NOTREACHED
    return 0;
}
