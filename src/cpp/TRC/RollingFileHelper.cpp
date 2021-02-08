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

#include <dirent.h>
#include <libgen.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>

#include <TRC/RollingFileHelper.h>
#include <UTILS/Directory.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

void DEBUG_NAMESPACE::rfh_parseFileSettings(string& name, int& maxFileSize, int& maxFileNum)
{
    // Note: We do not do error checking for the filename,
    // since the log files names are hard-coded from spcrun-remote.pl (for
    // service startup) and HC (for PEC startup).
    // We can do error checking if necessary, but the behavior of hitting
    // error need to be discussed: pick up a default log file name (like
    // what we actual doing now by hard-coding), or throw error.

    vector<string> tokens;
    tokenize(name, tokens, ":", false);
    // There are maxLogFileSize and maxLogFileNum info.
    // In this case, the input format is: filename:maxLogFileSize:maxLogFileNum
    if (tokens.size() > 1) {
        name = tokens[0];
        fromString(maxFileSize, tokens[1]);
        if (tokens.size() > 2) {
            fromString(maxFileNum, tokens[2]);
        }
    }
}

void DEBUG_NAMESPACE::rfh_initialFileDir(const string& name)
{
    char* n = strdup(name.c_str());
    char* d = dirname(n);
    DIR* dir = opendir(d);
    free(n);
    if (dir) {
        closedir(dir);
    }
}

void DEBUG_NAMESPACE::rfh_adoptExistingLogFiles(const string& filename, int& archivedNum)
{
    // Parsing to get the directory and actual file name.
    // Note:
    // 1. With current way of instance/service/application start up,
    //    we do not need to do any error checking here, such as for a filename with
    //    tailing "/", as it will never happen. But if the service startup (in
    //    spcrun-remote.pl) or PEC startup (inside HCServiceImpl) behavior changed
    //    and make the log file name configuration exposed to end user, then we
    //    need to do error checking. The error checking shall be in the constructor
    //    part, not here.

    string dirStr = "/";            // The directory
    string fStr = string(filename); // The file name
    string::size_type pos = filename.rfind("/");
    if (pos != string::npos) {
        if (pos > 0) {
            dirStr = filename.substr(0, pos);
        }
        fStr = filename.substr(pos + 1);
    }

    // Going through all files under the directory to find any file that match.
    // The matched files will includes the active log file and archived log files.
    int number = 0;
    try {
        Directory dir(dirStr, true, Directory::NODOTS | Directory::NODIRECTORIES);
        for (ConstDirEntryIterator i = dir.begin(); i != dir.end(); ++i) {
            string name = i->getBaseName();
            string::size_type namePos = name.find(fStr);
            if (namePos != string::npos) {
                ++number;
            }
        }
    } catch (DirectoryException& de) {
        cerr << "Hit exception when trying to open directory " << dirStr
             << " with error: " << de.getExplanation() << endl;
        throw;
    } catch (...) {
        cerr << "Unexpected exception occurred when trying to open directory " << dirStr << "."
             << endl;
        throw;
    }

    // Adopt the existing archived log/trace file number.
    // There are following 3 cases:
    // 1. number=0 - No matching files exist
    // 2. number=1 - Only <service>.log/out/stdouterr file exists.
    // 3. number=n (n>1) <servie>.log, <service>.log.m (m>=1)
    //    exist. This is the case when we have a restarted service and there
    //    are archived log files.
    // In case of 1 and 2, archivedNum is 0, in case of 3, we need to adopt the number.
    if (number > 1) {
        // total number minus current active trace file -- <service>.out/stdouterr.
        archivedNum = number - 1;
    }
    // No need to think about "else". If number<=1, _archivedNum keeps to be 0.
}

int DEBUG_NAMESPACE::rfh_getFileSize(const string& name, bool useStderr)
{
    struct stat filestatus;
    int rc = stat(name.c_str(), &filestatus);
    if (-1 == rc) {
        // stat failed if any system error, file been deleted, etc.
        if (useStderr) {
            cerr << "Failed to get size of " << name << " with error: " << strerror(errno) << endl;
        }
        return -1;
    } else {
        return filestatus.st_size;
    }
}

void DEBUG_NAMESPACE::rfh_rollover(int& archivedNum,
                                   const int maxFileNum,
                                   const string& name,
                                   bool useStderr)
{
    int rc = 0;

    // Note: Steps for rolling over:
    // 1. Remove all extra (more than maximum) log files. In this case, if ever
    //    remove/unlink fail, we quit.
    // 2. Rename xxx.xxx.<n> to xxx.xxx.<n+1> (n>=1), and finally xxx.xxx to xxx.xxx.1.
    // (please check comments below for details).

    // When we reach maximum number of log files, we delete the extra ones.
    //
    // As for SELinux, it is possible that end user set policy so no one can
    // delete any log files, such as PE log files. In this case: the archived log
    // file number will keep increasing and the maxFileNum will not effect.
    // But once it is enabled to delete log files, all extra log files that
    // over the maxFileNum will be deleted.
    if (archivedNum >= (maxFileNum - 1)) {
        int tmpInt = archivedNum;
        for (int n = tmpInt; n >= (maxFileNum - 1); n--) {
            stringstream ss;
            ss << name;
            if (n > 0) {
                ss << "." << n;
            }
            // n==0 when _maxFileNum = 1,
            // we remove active log file when its size is over maxFileSize.
            rc = unlink(ss.str().c_str());
            if (-1 == rc) {
                if (useStderr) {
                    cerr << "Can not remove file " << ss.str() << " with error: " << strerror(errno)
                         << endl;
                }

                // Since SELinux policy for file deletion can be updated at runtime,
                // to avoid log file archiving number break, we break the deletion
                // loop if first try not work.
                // 1. This is OK if we hit false error for unlink, as next writeMessage
                //    will try to clean up all extra log files again.
                // 2. In the course of this deletion loop, if SELinux log file deletion
                //    policy enabled, next time writeMessage will catch it and delete all
                //    extra log files.
                // 3. In the course of this deletion loop, if SELinux log file deletion
                //    policy disabled, we will break as it disabled.
                break;
            } else {
                if (archivedNum > 0) {
                    archivedNum--;
                }
            }
        }
    }

    // Rename the rest (rolling).
    // It is tricky if any rename failed.
    // a. Right now, if any rename failed, we quit renaming and not increase
    //    _archivedNum. This is fine if we fail renaming in the middle, such as
    //    renamed 4 to 5, but failed renaming 3 to 4, then we will have 1, 2, 3, 5.
    //    Even we may see some log file number odd, but total number still OK.
    //    And finally, if rename ever pass, we will be normal again.
    //    This option much better than option b.
    // b. If renaming fail and we continue, we will have two problems:
    //    (1). There will be a gap of missing log messages in log files.
    //       Such as above example, if 4 rename to 5, 3 failed rename to 4, but
    //       we continue to rename 2 to 3 and 1 to 2. Then finally we actually
    //       replacing 3, and lost log messages in the original log file 3.
    //    (2). Hard to track _archivedNum even it is doable. If there is any error
    //       for _archivedNum, this logger rolling will totally fail.

    // Keep tracking if any rename fail.
    bool renamePass = true;
    // rename xxx.xxx.<n> to xxx.xxx.<n+1> (n>=1)
    for (int i = archivedNum; i > 0; i--) {
        stringstream ss1, ss2;
        ss1 << name << "." << i;
        int target = i + 1;
        ss2 << name << "." << target;
        // Check if the file exist or not. If not, no need to rename.
        // Reason that an archived log file not exist is that the rename failed
        // in the last run.
        if (Directory::sisAFile(ss1.str())) {
            rc = rename(ss1.str().c_str(), ss2.str().c_str());
            if (rc == -1) {
                if (useStderr) {
                    cerr << "Can not rename file " << ss1.str() << " to " << ss2.str()
                         << " with error: " << strerror(errno) << endl;
                }
                renamePass = false;
                break;
            }
        }
    }

    // rename xxx.xxx to xxx.xxx.1 if all renaming alright.
    if (renamePass) {
        stringstream firstArchivedFile;
        firstArchivedFile << name << ".1";
        rc = rename(name.c_str(), firstArchivedFile.str().c_str());
        if (rc == -1) {
            if (useStderr) {
                cerr << "Can not rename file " << name << " to " << firstArchivedFile.str()
                     << " with error: " << strerror(errno) << endl;
            }
            renamePass = false;
        }
    }

    // Increase _archivedNum
    // If any error for renaming, we will not increase it.
    if (renamePass) {
        archivedNum++;
    }
}

bool DEBUG_NAMESPACE::rfh_closeStream(FILE*& strm, bool checkClose, bool useStderr)
{
    bool closed = false;
    if (strm) {
        int rc = fclose(strm);
        if (-1 == rc) {
            if (useStderr) {
                cerr << "Failed to close opened stream with error: " << strerror(errno) << endl;
            }
            if (!checkClose) {
                strm = NULL;
            }
        } else {
            closed = true;
            strm = NULL;
        }
    }
    return closed;
}
