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
#include <errno.h>
#include <fstream>
#include <libgen.h>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>

#include <NAM/FS_NameService.h>
#include <NAM/NAMMessages.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Directory.h>
#include <UTILS/RegEx.h>
#include <UTILS/SupportFunctions.h>

using namespace std;
UTILS_NAMESPACE_USE;
NAM_NAMESPACE_USE;
DEBUG_NAMESPACE_USE;
using namespace UTILS_RUNTIME_NS::mr;

#define SCOPE_SUB "sub"
#define SCOPE_ONE "one"

FS_NameService::FS_NameService(const string& ns_arg, const string& distID)
  : _distillery_id(distID)
{
    SPCDBG(L_INFO, "FS_NameService::FS_NameService(" << QT(ns_arg) << ")", NAM_GET_INSTANCE);
    initServer(ns_arg);
}

FS_NameService::FS_NameService(const string& ns_arg, const string& domainID, const string& distID)
  : _distillery_id(distID)
{
    SPCDBG(L_INFO, "FS_NameService::FS_NameService(" << QT(ns_arg) << ")", NAM_GET_INSTANCE);
    initServer(ns_arg);
}

void FS_NameService::initServer(const std::string& ns_arg)
{
    SPCDBG(L_DEBUG, "initServer(" << QT(ns_arg) << ")", NAM_GET_INSTANCE);
    _stg_path = ns_arg.substr(3);
    if (!_stg_path.length()) {
        _stg_path = getenv("HOME");
        _stg_path += "/.fsnameserver";
    }

    try {
        Directory::smkdir(_stg_path, 0755);
    } catch (DirectoryException e) {
        THROW_NESTED(NAM, "Unable to create or access storage directory: " << _stg_path, e,
                     NAMUnableToAccessDir, _stg_path.c_str());
    }
    SPCDBG(L_DEBUG, "Exit", NAM_GET_INSTANCE);
}

FS_NameService::~FS_NameService() {}

string FS_NameService::createObjName(const string& name)
{
    string nsname;
    nsname = _stg_path;
    if (name.at(0) != '/') {
        nsname += "/";
    }
    nsname += name;
    nsname += "@";
    nsname += _distillery_id;

    SPCDBG(L_TRACE, "createObjName(" << QT(name) << ", " << QT(nsname) << ")", NAM_GENERAL);
    return nsname;
}

string FS_NameService::createObjDir(const string& name)
{
    string nsname;
    nsname = _stg_path;
    if (name.at(0) != '/') {
        nsname += "/";
    }
    nsname += name;

    SPCDBG(L_TRACE, "createObjDir(" << QT(name) << ", " << QT(nsname) << ")", NAM_GENERAL);
    return nsname;
}

void FS_NameService::addOrUpdateObject(const string& name,
                                       const NameRecord& obj,
                                       int numRetries,
                                       bool update)
{
    SPCDBG(L_DEBUG, "addOrUpdateObject(" << QT(name) << ")", NAM_REGISTER_ENTRY);
    string tmpfilename = createObjName(name + ".new");
    string filename = createObjName(name);
    string last_error = "unknown error";
    char errno_buffer[1024];

    if (Directory::sisAFile(filename) && !update) {
        THROW(NameExists, "Registering " << QT(filename) << " failed (already exists)",
              NAMNameExists, filename.c_str());
    }

    SPCDBG(L_TRACE, "Registering object " << name, NAM_REGISTER_ENTRY);
    if (numRetries == -1) {
        numRetries = max_retry;
    }

    bool abort = false;
    while (!abort) {
        ofstream of(tmpfilename.c_str(), ios::out | ios::trunc);
        if (of.is_open()) {
            of << obj.toString();
            of.flush();
            of.close();
            int rc = rename(tmpfilename.c_str(), filename.c_str());
            if (rc == 0) {
                SPCDBG(L_INFO, "Registered object " << name, NAM_REGISTER_ENTRY);
                return;
            } else {
                last_error = strerror_r(errno, errno_buffer, 1023);
                SPCDBG(L_WARN,
                       "Renaming " << QT(tmpfilename) << " to " << QT(filename)
                                   << " failed: " << last_error,
                       NAM_REGISTER_ENTRY);

                string trashfilename = createObjName(name + ".trash");
                rc = rename(filename.c_str(), trashfilename.c_str());
                if (rc == 0) {
                    rc = rename(tmpfilename.c_str(), filename.c_str());
                    if (rc == 0) {
                        if (unlink(trashfilename.c_str()) != 0) {
                            last_error = strerror_r(errno, errno_buffer, 1023);
                            SPCDBG(L_WARN,
                                   "Fail to remove the temp trash file " << QT(trashfilename)
                                                                         << ": " << last_error,
                                   NAM_REGISTER_ENTRY);
                        }
                        SPCDBG(L_INFO, "Registered object " << name, NAM_REGISTER_ENTRY);
                        return;
                    } else {
                        last_error = strerror_r(errno, errno_buffer, 1023);
                        SPCDBG(L_WARN,
                               "Renaming " << QT(tmpfilename) << " to " << QT(filename)
                                           << " failed again: " << last_error,
                               NAM_REGISTER_ENTRY);
                    }
                } else {
                    last_error = strerror_r(errno, errno_buffer, 1023);
                    SPCDBG(L_WARN,
                           "Renaming " << QT(filename) << " to " << QT(trashfilename)
                                       << " failed: " << last_error,
                           NAM_REGISTER_ENTRY);
                }
            }
        } else {
            last_error = strerror_r(errno, errno_buffer, 1023);
            SPCDBG(L_WARN, "Unable to open or create " << QT(tmpfilename) << ": " << last_error,
                   NAM_REGISTER_ENTRY);
        }

        numRetries--;
        if (max_retry < numRetries) {
            // we want to make sure that if someone sets max_retry to a lower
            // number that it gets picked up dynamically
            numRetries = max_retry;
        }

        if (numRetries < 0 || shutdown_request) {
            abort = true;
        } else {
            sleep(1);
        }
    }

    // failed to register..remove the .new file
    unlink(tmpfilename.c_str());
    THROW(NAM, "Registering an object failed  " << QT(name) << ": " << last_error,
          NAMRegisterFailed, name.c_str(), last_error.c_str());
}

void FS_NameService::registerObject(const string& name, const NameRecord& obj, int numRetries)
{
    SPCDBG(L_DEBUG, "registerObject: " << name, NAM_REGISTER_ENTRY);
    addOrUpdateObject(name, obj, numRetries, false);
    SPCDBG(L_DEBUG, "Exit", NAM_REGISTER_ENTRY);
}

void FS_NameService::unregisterObject(const string& name, int numRetries)
{
    SPCDBG(L_DEBUG, "unregisterObject(" << QT(name) << ")", NAM_UNREGISTER_ENTRY);

    string filename = createObjName(name);
    kickIt(filename.c_str());

    int rc = unlink(filename.c_str());
    if (rc) {
        char errno_buffer[1024];
        SPCDBG(L_WARN,
               "unlink(" << filename << ") failed: " << strerror_r(errno, errno_buffer, 1023),
               NAM_UNREGISTER_ENTRY);
    } else {
        SPCDBG(L_INFO, "Unregistered file " << filename, NAM_UNREGISTER_ENTRY);
    }
}

void FS_NameService::updateObject(const string& name, const NameRecord& obj, int numRetries)
{
    SPCDBG(L_DEBUG, "updateObject: " << name, NAM_REGISTER_ENTRY);
    addOrUpdateObject(name, obj, numRetries, true);
    SPCDBG(L_DEBUG, "Exit", NAM_REGISTER_ENTRY);
}

void FS_NameService::lookupObject(const string& name,
                                  NameRecord& nr,
                                  int numRetries,
                                  const bool force)
{
    SPCDBG(L_DEBUG, "lookupObject(" << QT(name) << ")", NAM_LOOKUP_ENTRY);
    string last_error = "unknown error";
    char errno_buffer[1024];

    string filename = createObjName(name);
    if (numRetries == -1) {
        numRetries = max_retry;
    }

    bool abort = false;
    while (!abort) {
        kickIt(filename.c_str());

        ifstream file(filename.c_str());
        if (file.is_open()) {
            stringstream ss;
            if (copyStream(file, ss) > 0) {
                try {
                    nr.setObject(ss.str());
                    SPCDBG(L_INFO,
                           "Done lookupObject for (" << QT(name) << "), got value: " << ss.str(),
                           NAM_LOOKUP_ENTRY);
                    return;
                } catch (...) {
                    SPCDBG(L_WARN, "Parsing object " << QT(name) << " failed", NAM_LOOKUP_ENTRY);
                    break;
                }
            } else {
                SPCDBG(L_WARN, "Lookup for " << QT(name) << " failed - empty file, sleeping 1 sec",
                       NAM_LOOKUP_ENTRY);
            }
        } else {
            last_error = strerror_r(errno, errno_buffer, 1023);
            SPCDBG(L_WARN, "Lookup for " << QT(name) << " failed: " << last_error,
                   NAM_LOOKUP_ENTRY);
        }

        numRetries--;
        if (max_retry < numRetries) {
            // we want to make sure that if someone sets max_retry to a lower
            // number that it gets picked up dynamically
            numRetries = max_retry;
        }

        if (numRetries < 0 || shutdown_request) {
            abort = true;
        } else {
            sleep(1);
        }
    }

    THROW(NameNotFound, "Lookup for " << QT(name) << " failed: " << last_error, NAMNameNotFound,
          name.c_str());
}

void FS_NameService::createSubdir(const string& name, int numRetries)
{
    // have to loop through the name hierarchy.
    string nsname = createObjDir(name);
    try {
        Directory::smkdir(nsname, 0755);
    } catch (DirectoryException e) {
        THROW_NESTED(NAM, "Unable to create or access storage directory: " << nsname, e,
                     NAMUnableToAccessDir, nsname.c_str());
    }
}

void FS_NameService::destroySubdir(const string& name, int numRetries)
{
    string nsname = createObjDir(name);
    try {
        Directory::srmdir(nsname, true);
    } catch (DirectoryException e) {
        THROW_NESTED(NAM, "Unable to remove directory: " << nsname, e, NAMUnableToRemoveDir,
                     nsname.c_str());
    }
}

vector<string> FS_NameService::listObjects(const string& pattern, int numRetries)
{
    SPCDBG(L_DEBUG, "Enter: listObjects with patten (" << QT(pattern) << ")", NAM_LIST_ENTRY);
    size_t ind = pattern.rfind("/");
    string newPattern = pattern;
    string loc = _stg_path;
    if (ind != string::npos) {
        newPattern = pattern.substr(ind + 1);
        loc = _stg_path + "/" + pattern.substr(0, ind);
    }
    SPCDBG(L_TRACE, "listObjects with modified patten (" << QT(newPattern) << ", " << loc << ")",
           NAM_LIST_ENTRY);

    vector<string> records = listObjects(newPattern, loc, SCOPE_SUB, numRetries);
    // adding the dir back to each entry.
    if (ind != string::npos) {
        for (unsigned int i = 0; i < records.size(); i++) {
            records[i] = pattern.substr(0, ind + 1) + records[i];
        }
    }
    SPCDBG(L_INFO, "Got listed objects (" << debug::join(records.begin(), records.end()) << ")",
           NAM_LIST_ENTRY);
    return records;
}

vector<string> FS_NameService::listObjects(const string& pattern,
                                           const string& startDir,
                                           const string& scope,
                                           int numRetries)
{
    vector<string> result;
    SPCDBG(L_DEBUG, "listObjects('" << QT(pattern) << ", " << startDir << ", " << scope << "')",
           NAM_LIST_ENTRY);

    RegEx re(RegEx::glob2regex(pattern));
    string did = "@";
    did += _distillery_id;

    // SCOPE_ONE means just search for current level, no sub-dir been searched.
    if (scope.compare(SCOPE_ONE) == 0) {
        try {
            Directory dir(startDir, false, Directory::NODOTS | Directory::NODIRECTORIES);
            for (ConstDirEntryIterator i = dir.begin(); i != dir.end(); ++i) {
                string name = i->getBaseName();
                SPCDBG(L_TRACE,
                       "Considering match between '" << QT(name) << "' and RE '" << QT(pattern)
                                                     << "' (did=" << _distillery_id << ")",
                       NAM_LIST_ENTRY);

                string::size_type pos = name.rfind(did);
                if (pos != string::npos) {
                    name = name.substr(0, pos);
                    if (re.match(name)) {
                        SPCDBG(L_TRACE,
                               "Match found: '" << QT(name) << "' (did=" << _distillery_id << ")",
                               NAM_LIST_ENTRY);
                        result.push_back(name);
                    }
                }
            }
        } catch (const DirectoryException& de) {
            // DirectoryException thrown from Directory if the directory not found.
            // In this case, log. Returned name list will be empty.
            SPCDBG(L_WARN,
                   "Caught DirectoryException " << de.getExplanation() << " due to directory '"
                                                << startDir << "' not found or accessible.",
                   NAM_LIST_ENTRY);
        }
    }
    // Default behavior will be search current dir and all sub-dir.
    else {
        vector<string> res = listObjects(startDir, re, did, numRetries, startDir);
        result.insert(result.end(), res.begin(), res.end());
    }
    SPCDBG(L_DEBUG, "Got listed objects (" << debug::join(result.begin(), result.end()) << ")",
           NAM_LIST_ENTRY);
    return result;
}

vector<string> FS_NameService::listObjects(const string& d,
                                           const RegEx& re,
                                           const string& did,
                                           int numRetries,
                                           const string& oriDir)
{
    vector<string> result;
    SPCDBG(L_DEBUG, "listObjects('" << d << ", " << re << ", " << did << "')", NAM_LIST_ENTRY);

    try {
        Directory dir(d, false, Directory::NODOTS);
        for (ConstDirEntryIterator i = dir.begin(); i != dir.end(); ++i) {
            string n = i->getName();
            if (dir.isAFile(n)) {
                string name = i->getBaseName();
                SPCDBG(L_TRACE,
                       "Considering match between '" << QT(name) << "' and RE '" << re
                                                     << "' (did=" << _distillery_id << ")",
                       NAM_LIST_ENTRY);

                string::size_type pos = name.rfind(did);
                if (pos != string::npos) {
                    name = name.substr(0, pos);
                    if (re.match(name)) {
                        if (d.compare(oriDir) != 0) {
                            string::size_type startpos = d.find(oriDir) + oriDir.length();
                            if (oriDir.compare("/") != 0) {
                                startpos += 1;
                            }
                            string relativeDir = d.substr(startpos);
                            name = relativeDir + "/" + name;
                        }
                        SPCDBG(L_TRACE,
                               "Match found: '" << QT(name) << "' (did=" << _distillery_id
                                                << ") with relative path of " << n,
                               NAM_LIST_ENTRY);
                        result.push_back(name);
                    }
                }
            } else if (dir.isADirectory(n)) {
                string absPath(d);
                if (d.compare("/") != 0) {
                    absPath += "/";
                }
                absPath += n;
                vector<string> res = listObjects(absPath, re, did, numRetries, oriDir);
                result.insert(result.end(), res.begin(), res.end());
            } else {
                SPCDBG(L_TRACE, "name of " << n << "is not a directory or a file.", NAM_LIST_ENTRY);
            }
        }
    } catch (const DirectoryException& de) {
        // DirectoryException thrown from Directory if the directory not found.
        // In this case, log. Returned name list will be empty.
        SPCDBG(L_WARN,
               "Caught DirectoryException " << de.getExplanation() << " due to directory '" << d
                                            << "' not found or accessible.",
               NAM_LIST_ENTRY);
    }

    SPCDBG(L_DEBUG, "Got listed objects (" << debug::join(result.begin(), result.end()) << ")",
           NAM_LIST_ENTRY);
    return result;
}

void FS_NameService::kickIt(const string& name)
{
    char* _name = strdup(name.c_str());
    char* dir = dirname(_name);
    DIR* d = opendir(dir);
    free(_name);
    if (d) {
        closedir(d);
    }
}

void FS_NameService::setMessageRecordingConfiguration(
  const UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
  int numRetries)
{
    // Nothing to do here.
}
void FS_NameService::getMessageRecordingConfiguration(
  UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
  int numRetries)
{
    // Nothing to do here.
}
