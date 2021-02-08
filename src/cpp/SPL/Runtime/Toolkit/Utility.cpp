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

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/ProcessingElement/ProcessingElement.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Toolkit/RuntimeException.h>
#include <SPL/Toolkit/Utility.h>
#include <UTILS/HostInfo.h>
#include <cstdio>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <boost/filesystem/operations.hpp>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace bf = boost::filesystem;
using namespace std;
using namespace SPL;

SPL::AtomicRenamer::AtomicRenamer(ProcessingElement& pe, const bf::path& toDir_) :
  _toDir(toDir_),
  _pe(pe)
{

    SPLAPPTRC(L_DEBUG, "Using '" << toDir_.string() << "' as the target directory...",
              SPL_OPER_DBG);
    if (!bf::exists(toDir_)) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                            SPL_APPLICATION_RUNTIME_DIR_NOT_EXIST(toDir_.string()), SPL_FUNC_DBG);
    }
    if (!bf::is_directory(toDir_)) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                            SPL_APPLICATION_RUNTIME_NOT_DIR(toDir_.string()), SPL_FUNC_DBG);
    }
}

SPL::AtomicRenamer::AtomicRenamer(ProcessingElement& pe, const rstring& toDir_) :
  _toDir(toDir_),
  _pe(pe)
{
    SPLAPPTRC(L_DEBUG, "Using '" << toDir_.string() << "' as the target directory...",
              SPL_OPER_DBG);
    if (!bf::exists(_toDir)) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                            SPL_APPLICATION_RUNTIME_DIR_NOT_EXIST(toDir_.string()), SPL_FUNC_DBG);
    }
    if (!bf::is_directory(_toDir)) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                            SPL_APPLICATION_RUNTIME_NOT_DIR(toDir_.string()), SPL_FUNC_DBG);
    }
}

static bool realRename(const bf::path& fromFile, const bf::path& toFile)
{
    if (std::rename(fromFile.string().c_str(), toFile.string().c_str()) < 0) {
        if (errno != EXDEV) {
            SPLTRACEMSGANDTHROW(
              SPLRuntimeOperator, L_ERROR,
              SPL_APPLICATION_RUNTIME_RENAME_FAILED(fromFile.string(), toFile.string(),
                                                    SPL::RuntimeUtility::getErrorNoStr()),
              SPL_FUNC_DBG);
        }
        return false;
    }
    return true;
}

const string SPL::AtomicRenamer::rename(const bf::path& file)
{
    SPLAPPTRC(L_DEBUG, "Moving file '" << file.string() << "' to '" << _toDir.string() << '\'',
              SPL_OPER_DBG);
    //
    // We want to rename from fromDir to _toDir atomically
    bf::path toFile = _toDir / file.leaf();

    if (realRename(file, toFile)) {
        return toFile.string(); // This is on the same filesystem
    }

    // Okay, we are across filesystems - do it the hard way
    bf::path renameDir = _toDir / ".rename";
    if (bf::exists(renameDir)) {
        if (!bf::is_directory(renameDir)) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                                SPL_APPLICATION_RUNTIME_NOT_DIR(renameDir.string()), SPL_FUNC_DBG);
        }
    } else {
        try {
            bf::create_directory(renameDir);
        } catch (...) {
            SPLTRACEMSGANDTHROW(SPL::SPLRuntimeOperator, L_ERROR,
                                SPL_APPLICATION_RUNTIME_DIR_NOT_EXIST(renameDir.string()),
                                SPL_FUNC_DBG);
        }
    }
    bf::path renameFile = renameDir / file.leaf();

    // Just invoke cp to do the dirty work of copying to rename directory

    // We want to handle any output from cp
    int outputPipe[2];
    if (pipe(outputPipe) != 0) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeOperator, L_ERROR,
          SPL_APPLICATION_RUNTIME_PIPE_FAILED(SPL::RuntimeUtility::getErrorNoStr()), SPL_FUNC_DBG);
    }
    fcntl(outputPipe[0], F_SETFD, FD_CLOEXEC);
    fcntl(outputPipe[1], F_SETFD, FD_CLOEXEC);

    pid_t pid = fork();
    if (pid == -1) {
        SPLTRACEMSGANDTHROW(
          SPLRuntimeOperator, L_ERROR,
          SPL_APPLICATION_RUNTIME_FORK_FAILED(SPL::RuntimeUtility::getErrorNoStr()), SPL_FUNC_DBG);
    }

    if (pid) {
        // parent - close write side
        close(outputPipe[1]);

        // wait for the child to finish
        while (!_pe.getShutdownRequested()) {
            int status;
            pid_t w_pid = waitpid(pid, &status, WNOHANG);
            if (w_pid <= 0) {
                _pe.blockUntilShutdownRequest(0.1);
            } else {
                // We have the status
                if (!WIFEXITED(status) || status != 0) {
                    // something happened.  Collect up the output from the command, and log it
                    char buf[1024];
                    std::string msg;
                    int rdCount;
                    while ((rdCount = read(outputPipe[0], &buf, sizeof buf)) > 0) {
                        msg += std::string(buf, rdCount);
                    }
                    close(outputPipe[0]); // ensure read side cloaed
                    SPLTRACEMSGANDTHROW(SPLRuntimeOperator, L_ERROR,
                                        SPL_APPLICATION_RUNTIME_RENAME_COPY_FAILED(
                                          file.string(), toFile.string(), msg),
                                        SPL_FUNC_DBG);
                }
                break;
            }
        }
        close(outputPipe[0]); // ensure read side cloaed

        // The file is now in the rename directory
        // Set the filemode from the old file
        struct stat stat;
        if (::stat(file.string().c_str(), &stat) == 0) {
            chmod(renameFile.string().c_str(), stat.st_mode & 0777);
        }

        // Now rename the file to the final resting place
        realRename(renameFile, toFile);

        // Old file needs to be removed
        bf::remove(file);

        // parent all done
        return toFile.string();
    }

    // child - close read side
    close(outputPipe[0]);

    // Make the pipe the stdout & stderr for cp
    dup2(outputPipe[1], 1);
    dup2(outputPipe[1], 2);
    close(outputPipe[1]); // ensure write side closed

    char* args[] = {
        (char*)"/bin/cp",
        (char*)"--no-dereference",
        (char*)"--preserve=links,mode,timestamps",
        (char*)"-f",
        (char*)"--",
        (char*)file.string().c_str(),
        (char*)renameFile.string().c_str(),
        NULL,
    };
    execv("/bin/cp", args);

    // We better not get here
    _exit(EXIT_FAILURE);
    return ""; // not reached
}

bool SPL::isEOFReceived(int fd)
{
    long opt;
    while (-1 == (opt = fcntl(fd, F_GETFL, NULL))) {
        if (errno != EINTR) {
            return false; // should not happen
        }
    }
    while (-1 == fcntl(fd, F_SETFL, opt | O_NONBLOCK)) {
        if (errno != EINTR) {
            return false; // should not happen
        }
    }
    char c;
    int i;
    while (1 == (i = ::read(fd, &c, 1))) {
        ;
    }
    while (-1 == fcntl(fd, F_SETFL, opt)) {
        if (errno != EINTR) {
            return false; // should not happen
        }
    }
    return (i == 0);
}

string SPL::getHostName(const string& interface)
{
    if (interface.empty()) {
        return Distillery::HostInfo::getMyHostName();
    }
    // Figure out the correct address for the interface
    try {
        return Distillery::HostInfo::getInterfaceAddr(interface);
    } catch (...) {
        SPLTRACEMSGANDTHROW(SPLRuntimeUDPSourceOperator, L_ERROR,
                            SPL_RUNTIME_UNABLE_TO_LOCATE_INTERFACE(interface), SPL_OPER_DBG);
    }
}

bf::path SPL::cleanPath(const bf::path& path)
{
    bf::path res;
    for (bf::path::iterator it = path.begin(); it != path.end(); ++it) {
        const string& c = (*it).string();
        if (c == ".") {
            continue; // We can ignore ./
        }
        if (c == "..") {
            if (bf::is_symlink(res) || res.empty() || res.filename() == "..") {
                res /= c; // We have to save it
            } else {
                res = res.parent_path(); // go up one level
            }
        } else {
            res /= c;
        }
    }
    // Don't remove everything (in case we were passed '.')
    if (res.empty()) {
        return ".";
    }
    return res;
}
