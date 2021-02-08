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

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <boost/scoped_array.hpp>

#include <SPL/Core/CompilerError.h>
#include <SPL/Core/CompilerException.h>
#include <SPL/Utility/Debug.h>
#include <SPL/Utility/ProcessLauncher.h>
#include <SPL/Utility/SysOut.h>
#include <SPL/Utility/Utility.h>

using namespace SPL;
using namespace std;
using boost::scoped_array;

static char const** getArguments(const string& name, vector<string> const& args)
{
    size_t argc = args.size() + 1;
    char const** argv = new char const*[argc + 1];

    argv[argc] = NULL;
    argv[0] = name.c_str();
    vector<string>::const_iterator it = args.begin();
    for (unsigned i = 0; it != args.end(); ++i, ++it) {
        argv[i + 1] = (*it).c_str();
    }
    return argv;
}

static string join(vector<string> const& args)
{
    ostringstream params;
    vector<string>::const_iterator it;
    for (it = args.begin(); it != args.end(); ++it) {
        params << "'" << *it << "' ";
    }
    return params.str();
}

static bool readFromPipe(ostream& ostr, int fd)
{
    char buf[1024];
    ssize_t cnt = read(fd, buf, 1023);
    if (cnt == 0) {
        return true;
    }
    if (cnt == -1) {
        if (errno == EAGAIN || errno == EINTR) {
            return false;
        }
        THROW(SPLCompiler, "Error reading from pipe: " << RuntimeUtility::getErrorNoStr());
    }
    buf[cnt] = '\0';
    ostr << buf;
    return false;
}

int ProcessLauncher::runProcess(string const& name,
                                vector<string> const& args,
                                bool silenceStdOut /*= false*/,
                                bool silenceStdErr /*= false*/,
                                ostream* stdOutResult /*=NULL*/,
                                ostream* stdErrResult /*=NULL*/)
{
    return run(name, args, silenceStdOut, silenceStdErr, stdOutResult, stdErrResult);
}

#define DUP2_CALL(call)                                                                            \
    {                                                                                              \
        if (0 > (call))                                                                            \
            THROW(SPLCompiler, "dup2() failed, with "                                              \
                                 << "error code: " << errno                                        \
                                 << ", meaning: " << RuntimeUtility::getErrorNoStr());             \
    }

int ProcessLauncher::run(string const& name,
                         vector<string> const& args,
                         bool silenceStdOut,
                         bool silenceStdErr,
                         ostream* stdOutResult,
                         ostream* stdErrResult)
{
    SPLDBG("Running external process '" << name << "'", Debug::Core);

    int ret = 0;
    int stdOutPipe[2]; // pipe for the std out
    int stdErrPipe[2]; // pipe for the std err
    if (stdOutResult != NULL) {
        if (pipe(stdOutPipe)) {
            THROW(SPLCompiler, "pipe() failed, with "
                                 << "error code: " << errno
                                 << ", meaning: " << RuntimeUtility::getErrorNoStr());
        }
    }
    if (stdErrResult != NULL) {
        if (pipe(stdErrPipe)) {
            THROW(SPLCompiler, "pipe() failed, with "
                                 << "error code: " << errno
                                 << ", meaning: " << RuntimeUtility::getErrorNoStr());
        }
    }
    pid_t pid = fork();
    if (pid == -1) {
        THROW(SPLCompiler, "fork() failed");
    }
    if (pid) {
        // We are in the parent process
        // Create descriptors for select
        int maxfd = 0; // max. fd to select from
        fd_set readSet, workSet;
        FD_ZERO(&readSet);
        // Set the in side of the pipe to non-blocking
        // Close unused side (out side) of pipes
        if (stdOutResult != NULL) {
            fcntl(stdOutPipe[0], F_SETFL, O_NONBLOCK);
            FD_SET(stdOutPipe[0], &readSet);
            maxfd = max(maxfd, stdOutPipe[0]);
            close(stdOutPipe[1]);
        }
        if (stdErrResult != NULL) {
            fcntl(stdErrPipe[0], F_SETFL, O_NONBLOCK);
            FD_SET(stdErrPipe[0], &readSet);
            maxfd = max(maxfd, stdErrPipe[0]);
            close(stdErrPipe[1]);
        }
        bool outDone = (stdOutResult == NULL);
        bool errDone = (stdErrResult == NULL);
        while (!outDone || !errDone) {
            memcpy(&workSet, &readSet, sizeof(readSet));
            select(maxfd + 1, &workSet, NULL, NULL, NULL);
            // we do not care about the select result, really...
            if (!outDone) {
                outDone = readFromPipe(*stdOutResult, stdOutPipe[0]);
            }
            if (!errDone) {
                errDone = readFromPipe(*stdErrResult, stdErrPipe[0]);
            }
        }
        // Close the used side (in side) of pipes
        if (stdOutResult != NULL) {
            close(stdOutPipe[0]);
        }
        if (stdErrResult != NULL) {
            close(stdErrPipe[0]);
        }
        // Wait for child process to end
        int status;
        wait(&status);
        if (!WIFEXITED(status)) {
            THROW(SPLCompiler, "Child process failed to exit");
        }
        ret = WEXITSTATUS(status);
    } else {
        // We are in the child process
        try {
            scoped_array<char const*> argv(getArguments(name, args));
            SPLDBG("Invoking '" << name << "' " << join(args), Debug::Core);
            if (silenceStdOut || silenceStdErr) {
                int devnull = open("/dev/null", O_RDWR);
                if (devnull == -1) {
                    THROW(SPLCompiler,
                          "Cannot open /dev/null: " << RuntimeUtility::getErrorNoStr());
                }
                if (silenceStdOut)
                    DUP2_CALL(dup2(devnull, STDOUT_FILENO));
                if (silenceStdErr)
                    DUP2_CALL(dup2(devnull, STDERR_FILENO));
            }
            // Close and replace stdout with the out side of the pipe
            // Close unused (in side) side of pipe
            if (stdOutResult != NULL && !silenceStdOut) {
                DUP2_CALL(dup2(stdOutPipe[1], STDOUT_FILENO));
                close(stdOutPipe[0]);
            }
            if (stdErrResult != NULL && !silenceStdErr) {
                DUP2_CALL(dup2(stdErrPipe[1], STDERR_FILENO));
                close(stdErrPipe[0]);
            }
            // Replace the child fork with a new process
            if (execvp(argv[0], const_cast<char* const*>(argv.get())) == -1) {
                THROW(SPLCompiler, "execvp() failed, with "
                                     << "error code: " << errno
                                     << ", meaning: " << RuntimeUtility::getErrorNoStr());
            }
        } catch (SPLCompilerException const& e) {
            SysOut::die(SPL_CORE_INTERNAL_ERROR(e.getExplanation()));
        }
    }
    SPLDBG("Completed running external process '" << name << "'", Debug::Core);
    return ret;
}
