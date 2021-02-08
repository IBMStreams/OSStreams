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

#include <UTILS/LibTeclaHelper.h>
#include <cerrno>
#include <string.h>

using namespace std;
UTILS_NAMESPACE_USE

LibTeclaHelper::LibTeclaHelper(void)
  : gl(NULL)
{
    gl = new_GetLine(4096, 1024);
}

char* LibTeclaHelper::getCommandPTR(const string& promptMsg) const
{
    // Get a line from the user
    char* lastLineRead = gl_get_line(gl, promptMsg.c_str(), NULL, 0);

    if (lastLineRead) {
        int last_char = strlen(lastLineRead);
        if (lastLineRead[last_char - 1] == '\n') {
            lastLineRead[last_char - 1] = 0;
        }
    }

    return lastLineRead;
}

string LibTeclaHelper::getCommand(const string& promptMsg) const
{
    char* lastLineRead = getCommandPTR(promptMsg);
    if (lastLineRead == NULL) {
        GlReturnStatus ret = gl_return_status(gl);
        if (ret == GLR_NEWLINE) {
            return "";
        } else if (ret == GLR_BLOCKED) {
            THROW(EOFMarker, "Signal was caught");
        } else if (ret == GLR_EOF) {
            THROW(EOFMarker, "Signal was caught");
        } else if (ret == GLR_SIGNAL) {
            THROW(Signal, "Signal was caught");
        } else {
            char err[256];
            char* rc = strerror_r(errno, err, 256);
            if (rc == NULL) {
                strcpy(err, "could not retrieve errno message");
            } else {
                strcpy(err, rc);
            }
            THROW(LibTecla, "LibTecla error - return code=" << ret << " and errno=" << errno
                                                            << " corresponding to '" << err << "'");
        }
    }
    return string(lastLineRead);
}

void LibTeclaHelper::customizeCompletion(CplMatchFn* matchfn)
{
    // Replace the builtin filename completion callback with one which
    // searches for app-specific completions
    gl_customize_completion(gl, NULL, matchfn);
}

LibTeclaHelper::~LibTeclaHelper(void)
{
    del_GetLine(gl);
}

IMPL_EXCEPTION(Distillery, LibTecla, Utils);
IMPL_EXCEPTION(Distillery, Signal, LibTecla);
IMPL_EXCEPTION(Distillery, EOFMarker, LibTecla);
