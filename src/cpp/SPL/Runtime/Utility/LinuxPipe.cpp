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

#include <SPL/Runtime/Utility/LinuxPipe.h>

#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>

#include <cassert>
#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>

#include <sstream>
#include <vector>

using namespace std;
using namespace SPL;

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

static bool writeFull(int fd, char const* buf, size_t count)
{
    while (count > 0) {
        ssize_t n = ::write(fd, buf, count);
        if (n == -1 && errno != EINTR) {
            return false;
        } else if (n > 0) {
            count -= n;
            buf += n;
        }
    }
    return true;
}

/*
static bool readFull(int fd, char * buf, size_t count)
{
    while(count>0) {
        ssize_t n = ::read(fd, buf, count);
        if(n==0 || (n==-1 && errno!=EINTR)) {
            return false;
        } else if(n>0) {
            count -= n;
            buf += n;
        }
    }
    return true;
}
*/

string LinuxPipe::getTermInfoExplanation()
{
    AutoMutex am(mutex_);
    ostringstream explanation;
    if (termInfo_.reason == LinuxPipe::Shutdown) {
        explanation << "Shutdown";
    } else if (termInfo_.reason == LinuxPipe::Unknown) {
        explanation << "Unknown";
    } else if (termInfo_.reason == LinuxPipe::Exit) {
        explanation << "Exit";
        explanation << ", Exit code: " << termInfo_.exitCode;
    } else {
        assert(!"cannot happen");
    }
    return explanation.str();
}

void LinuxPipe::setup(string const& command)
{
    if (pipe(stdInPipe_)) {
        THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
    }
    if (pipe(stdOutPipe_)) {
        THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
    }
    if (pipe(stdErrPipe_)) {
        THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
    }
    // Setup fds that need to be closed upon the exec call
    // We could use pipe2, but it is not a standard routine
    fcntl(stdInPipe_[WRITE], F_SETFD, FD_CLOEXEC);
    fcntl(stdOutPipe_[READ], F_SETFD, FD_CLOEXEC);
    fcntl(stdErrPipe_[READ], F_SETFD, FD_CLOEXEC);
    pid_t pid = fork();
    if (pid == -1) {
        THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
    }
    if (pid) {
        child_ = pid;
        // We are in the parent process
        // Close unused side of pipes
        close(stdInPipe_[READ]);
        close(stdOutPipe_[WRITE]);
        close(stdErrPipe_[WRITE]);
        // Set the in sides of the pipes to non-blocking
        fcntl(stdOutPipe_[READ], F_SETFL, O_NONBLOCK);
        fcntl(stdErrPipe_[READ], F_SETFL, O_NONBLOCK);
        // Seteup select descriptors
        FD_ZERO(&readSet_);
        FD_SET(stdOutPipe_[READ], &readSet_);
        FD_SET(stdErrPipe_[READ], &readSet_);
        stdOutClosed_ = stdErrClosed_ = false;
        maxfd_ = 1 + max(stdOutPipe_[READ], stdErrPipe_[READ]);
        return;
    } else {
        // We are in the child process
        // Close and replace standard in with the input side of the pipe
        if (0 > dup2(stdInPipe_[READ], STDIN_FILENO)) {
            THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
        }
        // Close and replace standard out with the output side of the pipe
        if (0 > dup2(stdOutPipe_[WRITE], STDOUT_FILENO)) {
            THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
        }
        // Close and replace standard err with the output side of the pipe
        if (0 > dup2(stdErrPipe_[WRITE], STDERR_FILENO)) {
            THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
        }
        // Run the pipe
        string name = "/bin/sh";
        vector<string> args;
        args.push_back("-c");
        args.push_back(command);
        char const** argv(getArguments(name, args));
        // Replace the child fork with a new process
        if (execvp(argv[0], const_cast<char* const*>(argv)) == -1) {
            THROW_STRING(SPLRuntime, RuntimeUtility::getErrorNoStr());
        }
    }
}

void LinuxPipe::writeLine(string const& line)
{
    int fd = stdInPipe_[WRITE];
    if (!writeFull(fd, line.data(), line.length())) {
        terminate();
        THROW_STRING(SPLRuntime, getTermInfoExplanation());
    }
    if (!writeFull(fd, "\n", 1)) {
        terminate();
        THROW_STRING(SPLRuntime, getTermInfoExplanation());
    }
}

static int readFromPipe(string& buffer, int fd)
{
    char buf[1024];
    ssize_t cnt = read(fd, buf, 1023);
    if (cnt > 0) {
        buf[cnt] = '\0';
        buffer += buf;
    }
    return cnt;
}

bool LinuxPipe::readLine(LineOutput& out)
{
    string outLine, errLine;
    bool hasOutLine = false, hasErrLine = false;
    while (!hasOutLine && !hasErrLine) {
        size_t pos = stdOutResult_.find("\n");
        if (pos != string::npos) {
            hasOutLine = true;
            outLine = stdOutResult_.substr(0, pos);
            stdOutResult_ = stdOutResult_.substr(pos + 1);
        }
        pos = stdErrResult_.find("\n");
        if (pos != string::npos) {
            hasErrLine = true;
            errLine = stdErrResult_.substr(0, pos);
            stdErrResult_ = stdErrResult_.substr(pos + 1);
        }
        if (hasOutLine || hasErrLine) {
            out = LineOutput(hasOutLine ? (&outLine) : NULL, hasErrLine ? (&errLine) : NULL);
            return false;
        }
        // Both are closed
        if (stdOutClosed_ && stdErrClosed_) {
            terminate();
            if (termInfo_.reason == Shutdown) {
                return true;
            }
            THROW_STRING(SPLRuntime, getTermInfoExplanation());
        }
        fd_set workSet;
        memcpy(&workSet, &readSet_, sizeof(readSet_));
        select(maxfd_, &workSet, NULL, NULL, NULL);
        // we do not care about the select result, really...
        if (!stdOutClosed_) {
            int res = readFromPipe(stdOutResult_, stdOutPipe_[READ]);
            stdOutClosed_ = (res == 0);
            if (res == -1 && errno != EAGAIN && errno != EINTR) {
                terminate();
                THROW_STRING(SPLRuntime, getTermInfoExplanation());
            }
        }
        if (!stdErrClosed_) {
            int res = readFromPipe(stdErrResult_, stdErrPipe_[READ]);
            stdErrClosed_ = (res == 0);
            if (res == -1 && errno != EAGAIN && errno != EINTR) {
                terminate();
                THROW_STRING(SPLRuntime, getTermInfoExplanation());
            }
        }
    }
    return false;
}

void LinuxPipe::terminate()
{
    AutoMutex am(mutex_);
    if (terminated_) {
        return;
    }
    terminated_ = true;
    int status;
    int res = waitpid(child_, &status, WNOHANG);
    if (res == child_) {
        if (WIFEXITED(status)) {
            termInfo_.reason = Exit;
            termInfo_.exitCode = WEXITSTATUS(status);
            if (shutdown_ && termInfo_.exitCode == 0) {
                termInfo_.reason = Shutdown;
            }
        } else {
            termInfo_.reason = Unknown;
            termInfo_.exitCode = 0;
        }
    } else {
        if (shutdown_ && res == 0) {
            termInfo_.reason = Shutdown;
        } else {
            termInfo_.reason = Unknown;
        }
        termInfo_.exitCode = 0;
        ::kill(child_, SIGKILL);
        ::waitpid(child_, &status, 0);
    }
    if (shutdown_) {
        cv_.signal();
    } else {
        close(stdInPipe_[WRITE]);
    }
    close(stdOutPipe_[READ]);
    close(stdErrPipe_[READ]);
}

void LinuxPipe::shutdown(bool wait /*=true*/)
{
    AutoMutex am(mutex_);
    if (shutdown_) {
        return;
    }
    shutdown_ = true;
    if (terminated_) {
        return;
    }
    close(stdInPipe_[WRITE]);
    if (wait) {
        while (!terminated_) {
            cv_.wait(mutex_);
        }
    }
}
