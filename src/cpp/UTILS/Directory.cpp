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

#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Directory.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <fstream>
#include <libgen.h>
#include <limits.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <wordexp.h>

UTILS_NAMESPACE_USE
using namespace std;
DEBUG_NAMESPACE_USE

//--------------------------------------------------- DirEntry class

DirEntry::DirEntry(const string& myname)
  : name(myname)
  , type(0)
  , permissions(0)
{}

DirEntry::DirEntry(const string& myname, const unsigned mytype, const unsigned mypermissions)
  : name(myname)
  , type(mytype)
  , permissions(mypermissions)
{}

DirEntry::DirEntry(const DirEntry& mye)
  : name(mye.name)
  , type(mye.type)
  , permissions(mye.permissions)
{}

const string& DirEntry::getName(void) const
{
    return name;
}

const string DirEntry::getBaseName(void) const
{
    char* s = strdup(name.c_str());
    char* t = ::basename(s);
    string str(t);

    free(s);

    return str;
}

unsigned DirEntry::getType(void) const
{
    return type;
}

unsigned DirEntry::getPermissions(void) const
{
    return permissions;
}

ostream& UTILS_NAMESPACE::operator<<(ostream& o, const DirEntry& de)
{
    // saves original format AND changes the settings so that only these 2 flags
    // are on.
    _Ios_Fmtflags originalformat = o.flags(ios::oct | ios::showbase);
    o << "[" << de.name << "] [" << de.type << "] [" << de.permissions << "]";
    o.flags(originalformat);

    return o;
}

DirEntry::~DirEntry(void) {}

//--------------------------------------------------- LessThanDirEntry class

bool LessThanDirEntry::operator()(const DirEntry& de1, const DirEntry& de2) const
{
    //  cout << "Comparing: " << de1.getName() << " " << de2.getName() << endl;
    return de1.getName() < de2.getName();
}

//--------------------------------------------------- Directory class

Directory::Directory(const string& myPath, const bool myAbsolutePath, const unsigned myFilterType)
  : path(myPath)
  , absolutePath(myAbsolutePath)
  , filterType(myFilterType)
{
    readEntries();
}

Directory::Directory(const Directory& myd)
  : std::set<DirEntry, LessThanDirEntry>(myd)
  , path(myd.path)
  , absolutePath(myd.absolutePath)
  , filterType(myd.filterType)
{}

void Directory::readEntries(void)
{
    SPCDBG(L_DEBUG, "readEntries for " << path, CORE_UTILS);

    DIR* dirp = opendir(path.c_str());
    ERRNO_EX(dirp == NULL, Directory, "reading entries");
    dirent* dp;
    while ((dp = readdir(dirp)) != NULL) {
        string rn;
        SPCDBG(L_DEBUG, "file '" << dp->d_name << "' belongs to directory '" << path << "'",
               CORE_UTILS);
        try {
            rn = realpath(dp->d_name);
        } catch (DirectoryException& de) {
            // it is probably an invalid link!
        } catch (...) {
            SPCDBG(L_ERROR, "unexpected exception occurred", CORE_UTILS);
            throw;
        }
        if (rn.size() == 0) {
            continue;
        }
        struct stat64 sa;
        int ret = lstat64(rn.c_str(), &sa);
        unsigned etype = DIR_UNDEFINED;
        if (ret < 0) {
            if (errno == ENOENT) {
                // It is probably an invalid link, let's store it
                etype = DIR_LINK;
            } else {
                closedir(dirp);
                ERRNO_EX(true, Directory, "stating64ing file '" << rn << "'");
            }
        }
        if (etype == DIR_UNDEFINED) {
            if (S_ISREG(sa.st_mode)) {
                etype = DIR_FILE;
            }
            if (S_ISLNK(sa.st_mode)) {
                etype = DIR_LINK;
            } else if (S_ISDIR(sa.st_mode)) {
                unsigned len = strlen(dp->d_name);
                if (dp->d_name[len - 1] == '.' &&
                    (len == 1 || dp->d_name[len - 2] == '.' || dp->d_name[len - 2] == '/')) {
#ifdef DEBUG
                    cout << "dots: " << dp->d_name << endl;
#endif
                    etype = DIR_DOTS;
                } else {
                    etype = DIR_DIRECTORY;
                }
            }
        }
        if (etype == DIR_DOTS && (filterType & NODOTS) != 0) {
            continue;
        }
        if ((etype == DIR_DIRECTORY || etype == DIR_DOTS) && (filterType & NODIRECTORIES) != 0) {
            continue;
        }
        if (etype == DIR_FILE && (filterType & NOFILES) != 0) {
            continue;
        }
        if (absolutePath) {
            DirEntry de(rn, etype, sa.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            insert(de);
        } else {
            DirEntry de(dp->d_name, etype, sa.st_mode & (S_IRWXU | S_IRWXG | S_IRWXO));
            insert(de);
        }
    }
    closedir(dirp);
}

void Directory::mkdir(const string& mydirname, const unsigned permissions)
{
    stringstream fn;
    if (mydirname[0] == '/') {
        fn << mydirname;
    } else {
        fn << path << "/" << mydirname;
    }
    int ret = ::mkdir(fn.str().c_str(), permissions);
    if (ret < 0) {
        if (errno == ENOENT) {
            string parent(sdirname(mydirname));
            if (parent == "" || parent == "/") {
                ERRNO_EX(true, Directory, "creating directory '" << mydirname << "'");
            } else {
                // create parent first
                mkdir(parent, permissions);
                // then, create the desired directory
                mkdir(mydirname, permissions);
            }
        } else if (errno == EEXIST) {
            struct stat64 ls;
            int lsret = lstat64(mydirname.c_str(), &ls);
            if (S_ISLNK(ls.st_mode)) {
                // let's see if the link points to a directory
                struct stat64 s;
                int sret = stat64(mydirname.c_str(), &s);
                if (sret == 0) {
                    if (lsret == 0 && !S_ISDIR(s.st_mode)) {
                        THROW(Directory,
                              "location '" << mydirname << "' is a symbolic link to a file");
                    }
                } else {
                    if (lsret == 0) {
                        THROW(Directory,
                              "location '" << mydirname << "' is a dangling symbolic link");
                    }
                }
                SPCDBG(L_DEBUG,
                       "directory '"
                         << mydirname
                         << "' already exists (through a symbolic link)... no need to create it",
                       CORE_UTILS);
            } else {
                struct stat64 s;
                int sret = stat64(mydirname.c_str(), &s);
                if (sret == 0 && !S_ISDIR(s.st_mode)) {
                    THROW(Directory, "a file named '" << mydirname
                                                      << "' exists -- directory cannot be created");
                }
                SPCDBG(L_DEBUG,
                       "directory '" << mydirname << "' already exists... no need to create it",
                       CORE_UTILS);
            }
        } else {
            ERRNO_EX(true, Directory, "creating directory '" << mydirname << "'");
        }
    }
    clear();
    readEntries();
}

void Directory::smkdir(const string& mydirname, const unsigned permissions)
{
    int ret = ::mkdir(mydirname.c_str(), permissions);
    if (ret < 0) {
        if (errno == ENOENT) {
            string parent(sdirname(mydirname));
            if (parent == "" || parent == "/") {
                ERRNO_EX(true, Directory, "creating directory '" << mydirname << "'");
            } else {
                // create parent first
                smkdir(parent, permissions);
                // then, create the desired directory
                smkdir(mydirname, permissions);
            }
        } else if (errno == EEXIST) {
            struct stat64 ls;
            int lsret = lstat64(mydirname.c_str(), &ls);
            if (S_ISLNK(ls.st_mode)) {
                // let's see if the link points to a directory
                struct stat64 s;
                int sret = stat64(mydirname.c_str(), &s);
                if (sret == 0) {
                    if (lsret == 0 && !S_ISDIR(s.st_mode)) {
                        THROW(Directory,
                              "location '" << mydirname << "' is a symbolic link to a file");
                    }
                } else {
                    if (lsret == 0) {
                        THROW(Directory,
                              "location '" << mydirname << "' is a dangling symbolic link");
                    }
                }
                SPCDBG(L_DEBUG,
                       "directory '"
                         << mydirname
                         << "' already exists (through a symbolic link)... no need to create it",
                       CORE_UTILS);
            } else {
                struct stat64 s;
                int sret = stat64(mydirname.c_str(), &s);
                if (sret == 0 && !S_ISDIR(s.st_mode)) {
                    THROW(Directory, "a file named '" << mydirname
                                                      << "' exists -- directory cannot be created");
                }
                SPCDBG(L_DEBUG,
                       "directory '" << mydirname << "' already exists... no need to create it",
                       CORE_UTILS);
            }
        } else {
            ERRNO_EX(true, Directory, "creating directory '" << mydirname << "'");
        }
    }
}

unsigned Directory::getPermissions(const string& filename) const
{
    string rn;
    if (absolutePath) {
        rn = realpath(filename);
    } else {
        rn = filename;
    }
    ConstDirEntryIterator de = find(DirEntry(rn));
    if (de == end()) {
        THROW(Directory, "file '" << rn << "' not found when getting permissions");
    }
    return de->getPermissions();
}

void Directory::chmod(const string& filename, const unsigned permissions)
{
    string rn;
    if (absolutePath) {
        rn = realpath(filename);
    } else {
        rn = filename;
    }
    int ret = ::chmod(rn.c_str(), permissions);
    ERRNO_EX(ret < 0, Directory, "chmoding '" << rn << "'");
    clear();
    readEntries();
}

void Directory::schmod(const string& filename, const unsigned permissions)
{
    int ret = ::chmod(filename.c_str(), permissions);
    ERRNO_EX(ret < 0, Directory, "chmoding '" << filename << "'");
}

void Directory::unlink(const string& filename)
{
    string rn;
    if (absolutePath) {
        rn = realpath(filename);
    } else {
        rn = filename;
    }
    int ret = ::unlink(rn.c_str());
    ERRNO_EX(ret < 0, Directory, "removing '" << filename << "'");
    clear();
    readEntries();
}

void Directory::sunlink(const string& filename)
{
    int ret = ::unlink(filename.c_str());
    ERRNO_EX(ret < 0, Directory, "removing '" << filename << "'");
}

void Directory::rename(const string& oldfilename, const string& newfilename)
{
    string old_rn, new_rn;
    if (absolutePath) {
        old_rn = realpath(oldfilename);
        new_rn = realpath(newfilename);
    } else {
        old_rn = oldfilename;
        new_rn = newfilename;
    }
    int ret = ::rename(old_rn.c_str(), new_rn.c_str());
    ERRNO_EX(ret < 0, Directory, "renaming '" << old_rn << "' to '" << new_rn << "'");
    clear();
    readEntries();
}

void Directory::srename(const string& oldfilename, const string& newfilename)
{
    int ret = ::rename(oldfilename.c_str(), newfilename.c_str());
    ERRNO_EX(ret < 0, Directory, "renaming '" << oldfilename << "' to '" << newfilename << "'");
}

void Directory::rmdir(const string& mydirname, const bool recursive)
{
    string rn = realpath(mydirname);
    if (!isADirectory(rn)) {
        char msg[80];
        snprintf(msg, 80, "error 'entry is not a directory' deleting '%s'", mydirname.c_str());
        THROW(Directory, msg);
    }
    int ret = ::rmdir(rn.c_str());
    if (ret < 0) {
        if (errno == ENOTEMPTY && recursive) {
            Directory d(rn, NODOTS);
            for (ConstDirEntryIterator i = d.begin(); i != d.end(); ++i) {
                if (i->getType() == DIR_FILE) {
                    ret = ::unlink(i->getName().c_str());
                    ERRNO_EX(ret < 0, Directory, "deleting '" << i->getName() << "'");
                } else if (i->getType() == DIR_DIRECTORY) {
                    rmdir(i->getName(), recursive);
                }
            }
            ret = ::rmdir(rn.c_str());
            ERRNO_EX(ret < 0, Directory, "deleting '" << rn << "'");
            clear();
            readEntries();
        } else {
            ERRNO_EX(true, Directory, "deleting '" << rn << "'");
        }
    }
}

void Directory::srmdir(const string& mydirname, const bool recursive)
{
    int ret = ::rmdir(mydirname.c_str());
    if (ret < 0) {
        if (errno == ENOTEMPTY && recursive) {
            Directory d(mydirname, NODOTS);
            for (ConstDirEntryIterator i = d.begin(); i != d.end(); ++i) {
                if (i->getType() == DIR_FILE || i->getType() == DIR_LINK) {
                    SPCDBG(L_DEBUG, "unlinking '" << i->getName() << "'", CORE_UTILS);
                    ret = ::unlink(i->getName().c_str());
                    ERRNO_EX(ret < 0, Directory, "deleting '" << i->getName() << "'");
                } else if (i->getType() == DIR_DIRECTORY) {
                    SPCDBG(L_DEBUG, "rmdir'ing '" << i->getName() << "'", CORE_UTILS);
                    srmdir(i->getName(), recursive);
                }
            }
            ret = ::rmdir(mydirname.c_str());
            ERRNO_EX(ret < 0, Directory, "deleting '" << mydirname << "'");
        } else {
            ERRNO_EX(true, Directory, "deleting '" << mydirname << "'");
        }
    }
}

const string Directory::sbasename(const string& filename)
{
    char* s = strdup(filename.c_str());
    char* t = ::basename(s);
    assert(t);
    string str(t);

    free(s);

    return str;
}

const string Directory::sdirname(const string& filename)
{
    char* s = strdup(filename.c_str());
    char* t = ::dirname(s);
    assert(t);
    string str(t);

    free(s);

    return str;
}

const string Directory::removeExtension(const string& filename)
{
    string noext(filename);
    size_t from = noext.rfind(".");
    if (from != string::npos) {
        noext.erase(from);
    }
    return noext;
}

string Directory::realpath(const string& filename) const
{
    stringstream fn;
    char absName[PATH_MAX];
    if (filename[0] != '/') {
        fn << path << "/" << filename;
    } else {
        fn << filename;
    }

    struct stat64 s;
    int ret = lstat64(fn.str().c_str(), &s);
    if (ret == 0 && S_ISLNK(s.st_mode)) {
        string t(srealpath(sdirname(fn.str())) + "/" + filename);
        return t;
    }

    char* rp = ::realpath(fn.str().c_str(), absName);
    ERRNO_EX(rp == NULL, Directory, "obtaining absolute name for '" << filename << "'");
    return string(rp);
}

const string Directory::srealpath(const string& filename)
{
    char absName[PATH_MAX];
    char* rp = ::realpath(filename.c_str(), absName);
    ERRNO_EX(rp == NULL, Directory, "obtaining absolute name for '" << filename << "'");
    return string(rp);
}

const string Directory::sgetcwd(void)
{
    char cwd[PATH_MAX];
    char* ret = getcwd(cwd, PATH_MAX);
    ERRNO_EX(ret == NULL, Directory, "obtaining current working directory");
    return string(cwd);
}

bool Directory::isADirectory(const string& mydirname) const
{
    string rn;
    if (absolutePath) {
        try {
            rn = realpath(mydirname);
        } catch (DirectoryException& de) {
            return false;
        }
    } else {
        rn = sbasename(mydirname);
    }
    ConstDirEntryIterator de = find(DirEntry(rn));
    /**
    if (de!=end())
      cout << "Found: " << *de << endl;
    else
      cout << "Not found" << endl;
    **/
    return de != end() && (de->getType() == DIR_DIRECTORY || de->getType() == DIR_DOTS);
}

bool Directory::sisADirectory(const string& dirname)
{
    struct stat s;
    int ret = stat(dirname.c_str(), &s);
    if (ret < 0 || !S_ISDIR(s.st_mode)) {
        return false;
    }

    return true;
}

bool Directory::isAFile(const string& filename) const
{
    string rn;
    if (absolutePath) {
        try {
            rn = realpath(filename);
        } catch (DirectoryException& de) {
            return false;
        }
    } else {
        rn = filename;
    }
    ConstDirEntryIterator de = find(DirEntry(rn));
    return de != end() && de->getType() == DIR_FILE;
}

bool Directory::sisAFile(const string& filename)
{
    struct stat64 s;
    int ret = lstat64(filename.c_str(), &s);
    if (ret < 0 || !S_ISREG(s.st_mode)) {
        return false;
    }

    return true;
}

bool Directory::isASymbolicLink(const string& linkname) const
{
    string rn;
    if (absolutePath) {
        try {
            rn = realpath(linkname);
        } catch (DirectoryException& de) {
            return false;
        }
    } else {
        rn = linkname;
    }
    ConstDirEntryIterator de = find(DirEntry(rn));
    return de != end() && de->getType() == DIR_LINK;
}

bool Directory::sisASymbolicLink(const string& linkname)
{
    struct stat64 s;
    int ret = lstat64(linkname.c_str(), &s);
    if (ret < 0 || !S_ISLNK(s.st_mode)) {
        return false;
    }

    return true;
}

bool Directory::sisAnExecutableFile(const string& filename)
{
    struct stat64 s;
    int ret = lstat64(filename.c_str(), &s);
    if (ret < 0 || !S_ISREG(s.st_mode) || ((s.st_mode & S_IXUSR) != S_IXUSR)) {
        return false;
    }

    return true;
}

bool Directory::sisAReadableFile(const string& filename)
{
    ifstream f(filename.c_str());
    if (!f) {
        f.close();
        return false;
    }
    f.close();
    return true;
}

vector<string> Directory::sexpandname(const string& filename)
{
    wordexp_t p;
    char** w;
    vector<string> expansions;

    wordexp(filename.c_str(), &p, 0);
    w = p.we_wordv;
    for (unsigned i = 0; i < p.we_wordc; i++) {
        expansions.push_back(w[i]);
    }
    wordfree(&p);
    return expansions;
}

void Directory::schown(const std::string& filename, const uid_t& owner, const gid_t& group)
{
    int ret = ::chown(filename.c_str(), owner, group);
    ERRNO_EX(ret < 0, Directory, "chowning '" << filename << "'");
}

Directory::~Directory(void) {}

IMPL_EXCEPTION(Distillery, Directory, Utils);
