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

//
// Class Description:
//
// Implements support for manipulating directory entries

#ifndef DIRECTORY_H
#define DIRECTORY_H

#include <UTILS/DistilleryException.h>
#include <UTILS/UTILSTypes.h>
#include <iostream>
#include <set>
#include <string>
#include <unistd.h>
#include <vector>

UTILS_NAMESPACE_BEGIN

/// This class implements an object representing a directory entry
class DirEntry
{
  public:
    /// Constructor
    /// @param myname directory entry name
    /// @param mytype directory entry type
    /// @param mypermissions directory entry permissions
    DirEntry(const std::string& myname, const unsigned mytype, const unsigned mypermissions);

    /// Search constructor
    /// @param myname directory entry name
    DirEntry(const std::string& myname);

    /// Copy constructor
    /// @param mye an existing DirEntry object
    DirEntry(const DirEntry& mye);

    /// Obtain the name of a directory entry
    /// @return the name of the entry
    const std::string& getName(void) const;

    /// Obtain the basename for a directory entry (excludes the directory)
    /// @return the basename of the entry
    const std::string getBaseName(void) const;

    /// Obtain the type of a directory entry
    /// @return the type of the entry
    unsigned getType(void) const;

    /// Obtain permissions for a directory entry
    /// @return the permissions
    unsigned getPermissions(void) const;

    /// Destructor
    ///
    ~DirEntry(void);

  private:
    /// the entry name
    std::string name;
    /// the entry type
    unsigned type;
    /// the entry permissions
    unsigned permissions;
    friend std::ostream& operator<<(std::ostream& o, const DirEntry& de);
    friend class Directory;
};

/// This class implements a "comparator" for DirEntries
class LessThanDirEntry
{
  public:
    bool operator()(const DirEntry& de1, const DirEntry& de2) const;
};

/// This class implements a container for the entries in filesystem directory
class Directory : public std::set<DirEntry, LessThanDirEntry>
{
  public:
    enum
    {
        ALLENTRIES = 0,
        NODIRECTORIES = 1,
        NOFILES = 2,
        NODOTS = 4,

        DIR_UNDEFINED = 0,
        DIR_FILE = 1,
        DIR_DIRECTORY = 2,
        DIR_LINK = 3,
        DIR_DOTS = 4
    };

    /// Constructor
    /// @param myPath directory location
    /// @param myAbsolutePath store (if true) entry with absolute path
    /// information
    /// @param myFilterType mask with entries which should be excluded
    Directory(const std::string& myPath,
              const bool myAbsolutePath = true,
              const unsigned myFilterType = ALLENTRIES);

    /// Copy constructor
    /// @param myd an existing Directory object
    Directory(const Directory& myd);

    /// Check if a directory exists
    /// @param dirname directory location
    /// @return true if the directory exists
    bool isADirectory(const std::string& dirname) const;

    /// Check if a directory exists -- static version
    /// @param dirname the directory location
    /// @return true if the directory exists
    static bool sisADirectory(const std::string& dirname);

    /// Check if a file exists
    /// @param filename the file name
    /// @return true if the file exists
    bool isAFile(const std::string& filename) const;

    /// Check if a file exists -- static version
    /// @param filename the file name
    /// @return true if the file exists
    static bool sisAFile(const std::string& filename);

    /// Check if a symbolic link exists
    /// @param linkname the symbolic link name
    /// @return true if the symbolic link exists
    bool isASymbolicLink(const std::string& linkname) const;

    /// Check if a symbolic link exists -- static version
    /// @param linkname the symbolic link name
    /// @return true if the symbolic link exists
    static bool sisASymbolicLink(const std::string& linkname);

    /// Check if a file is executable -- static version
    /// @param filename the file name
    /// @return true if the file is readable
    static bool sisAnExecutableFile(const std::string& filename);

    /// Check if a file is readable -- static version
    /// @param filename the file name
    /// @return true if the file is readable
    static bool sisAReadableFile(const std::string& filename);

    /// Remove a directory (and possibly its contents)
    /// @param dirname directory location
    /// @param recursive if true, removes recusively (i.e., including
    /// the directory contents)
    void rmdir(const std::string& dirname, const bool recursive);

    /// Remove a directory (and possibly its contents)
    /// -- static method version
    /// @param dirname directory location
    /// @param recursive if true, removes recusively (i.e., including
    /// the directory contents)
    static void srmdir(const std::string& dirname, const bool recursive);

    /// Obtain the basename for a directory entry (excludes the directory)
    /// @param filename file name to operate on
    /// @return the basename of the entry
    static const std::string sbasename(const std::string& filename);

    /// Obtain the dirname for a directory entry (excludes the directory)
    /// @param filename file name to operate on
    /// @return the dirname of the entry
    static const std::string sdirname(const std::string& filename);

    /// Remove the extension (defined as anything that follows the last
    /// dot in the filename)
    /// @param filename file name to operate on
    /// @return the file name without the extension
    static const std::string removeExtension(const std::string& filename);

    /// Obtain the file's absolute name
    /// @param filename file location
    /// @return the absolute path
    std::string realpath(const std::string& filename) const;

    /// Obtain the file's absolute name -- static version
    /// @param filename directory location
    /// @return the absolute path
    static const std::string srealpath(const std::string& filename);

    /// Obtain current work directory
    /// @return the current work directory
    static const std::string sgetcwd(void);

    /// Create a directory -- this method will create parent directories
    /// as necessary (akin to mkdir -p)
    /// @param dirname directory location
    /// @param permissions directory permissions
    void mkdir(const std::string& dirname, const unsigned permissions);

    /// Create a directory -- static version -- this method will create
    /// parent directories as necessary (akin to mkdir -p)
    /// @param dirname directory location
    /// @param permissions directory permissions
    static void smkdir(const std::string& dirname, const unsigned permissions);

    /// Change permissions for a directory entry
    /// @param filename directory entry
    /// @param permissions new permissions
    void chmod(const std::string& filename, const unsigned permissions);

    /// Change permissions for a directory entry -- static version
    /// @param filename directory entry
    /// @param permissions new permissions
    static void schmod(const std::string& filename, const unsigned permissions);

    /// Remove a file
    /// @param filename name of file to be removed
    void unlink(const std::string& filename);

    /// Remove a file --static version
    /// @param filename name of file to be removed
    static void sunlink(const std::string& filename);

    /// Rename a file
    /// @param oldfilename original file name
    /// @param newfilename new file name
    void rename(const std::string& oldfilename, const std::string& newfilename);

    /// Rename a file -- static version
    /// @param oldfilename original file name
    /// @param newfilename new file name
    static void srename(const std::string& oldfilename, const std::string& newfilename);

    /// Expand file name -- static version (employs 'wordexp', internally)
    /// @param filename original file name
    /// @return list of expansions
    static std::vector<std::string> sexpandname(const std::string& filename);

    /// Obtain permissions for a directory entry
    /// @param filename directory entry
    /// @return the file permissions
    unsigned getPermissions(const std::string& filename) const;

    /// Change owner/group for a directory entry -- static version
    /// If the owner or group is specified as -1, then that ID is not changed.
    /// @param filename directory entry
    /// @param owner owner to change to
    /// @param group group to change to
    static void schown(const std::string& filename, const uid_t& owner, const gid_t& group = (-1));

    /// Destructor
    ///
    ~Directory(void);

  private:
    /// Read directory entries into the object according to filter
    ///
    void readEntries(void);

    std::string path;
    bool absolutePath;
    unsigned filterType;
};

typedef Directory::const_iterator ConstDirEntryIterator;
typedef Directory::iterator DirEntryIterator;

DECL_EXCEPTION(UTILS_NAMESPACE, Directory, Utils);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
