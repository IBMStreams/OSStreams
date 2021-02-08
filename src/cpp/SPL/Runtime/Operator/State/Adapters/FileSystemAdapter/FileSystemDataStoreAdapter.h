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

#ifndef SPL_DSA_FILESYSTEM_DATA_STORE_ADAPTER_H
#define SPL_DSA_FILESYSTEM_DATA_STORE_ADAPTER_H

#include <SPL/Runtime/Operator/State/DataStoreAdapter.h>
#include <SPL/Runtime/Operator/State/DataStoreEntry.h>
#include <SPL/Runtime/Utility/Visibility.h>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <string>

namespace bf = boost::filesystem;

using boost::property_tree::ptree;

namespace SPL {
class DLL_PUBLIC FileSystemDataStoreAdapter : public DataStoreAdapter
{
  public:
    /// Constructor
    /// @param adapterConfig adapter configuration
    /// @throws DataStoreException if FileSystemDataStoreAdapter instance cannot be created
    FileSystemDataStoreAdapter(const std::string& adapterConfig)
      : ckptDirectory_()
      , compactionInterval_()
    {
        APPTRC(L_DEBUG, "Parsing adapter config:\n" << adapterConfig, SPL_CKPT);
        std::string configString;
        std::stringstream ss;
        uint64_t compactionInterval = 0;
        ss << adapterConfig;
        ptree pt;
        try {
            read_json(ss, pt);
            configString = pt.get<std::string>("Dir", "");
            boost::algorithm::trim(configString); // remove whitespaces
            compactionInterval = pt.get<uint64_t>("CompactionInterval", 0);
        } catch (std::exception const& e) {
            THROW(DataStore, "Cannot parse configuration (" << adapterConfig << "): " << e.what());
        }

        //check if this exists as a directory
        if (bf::is_directory(configString)) {
            ckptDirectory_ = configString;
            APPTRC(L_DEBUG, "Root checkpoint directory is: " << ckptDirectory_, SPL_CKPT);
        } else {
            THROW(DataStore, "Invalid checkpoint directory specified: " << configString);
        }

        compactionInterval_ = compactionInterval;
    }

    /// Destructor
    ~FileSystemDataStoreAdapter() {}

    /// Return the type of this Data Store Adapter
    /// @return a string to identify the type of this Data Store Adapter
    std::string getDSAType() { return "fileSystem"; }

    /// Get a Data Store Entry.
    /// To create a new Data Store Entry, set option.create_if_missing = true and
    /// option.error_if_exist = true; To open an existing Data Store Entry, set
    /// option.create_if_missing = false and option.error_if_exist = false.
    /// @param name name of the Data Store Entry
    /// @param option contains control options
    /// @return the Data Store Entry handle
    /// @throws DataStoreException if the Data Store Entry handle cannot be obtained
    DataStoreEntry* getDataStoreEntry(const std::string& name, const Option& option);

    /// Remove a Data Store Entry
    /// @param name name of the Data Store Entry
    /// @throws DataStoreException if the Data Store Entry cannot be removed
    void removeDataStoreEntry(const std::string& name);

    /// Check if a Data Store Entry of the given name exists
    /// @param name name of the Data Store Entry
    /// @return true if the Data Store Entry of the given name exists, false otherwise
    /// @throws DataStoreException if any error happens during operation
    bool isExistingDataStoreEntry(const std::string& name)
    {
        std::stringstream ckpt;
        ckpt << ckptDirectory_ << "/" << name;
        return bf::exists(ckpt.str());
    }

    /// Get the names of all Data Store Entries which prefix-match the given prefix.
    /// For example, names "/a/b/c", and "/a/b/d" both prefix-match "/a/b".
    /// @param prefix the prefix to match with
    /// @param names return a set of matching Data Store Entry names
    /// @throws DataStoreException if the names cannot be obtained due to error
    void getDataStoreEntryNames(const std::string& prefix,
                                std::tr1::unordered_set<std::string>& names)
    {
        std::string matchStr = ckptDirectory_ + "/" + prefix;
        size_t ckptDirLength = ckptDirectory_.size() + 1;
        // search through the lowest level of common parent directory
        try {
            bf::path parentPath(matchStr);
            for (bf::directory_iterator iter(parentPath), end; iter != end; ++iter) {
                if (boost::starts_with(iter->path().string(), matchStr) &&
                    bf::is_directory(iter->path())) {
                    names.insert(iter->path().string().substr(ckptDirLength));
                }
            }
        } catch (bf::filesystem_error const& e) {
            THROW(DataStore, "Cannot get Data Store Entry names with prefix "
                               << prefix << ": received exception: " << e.what());
        }
    }

    /// Remove all Data Store Entries which prefix-match the given prefix
    /// @param prefix the prefix to match with
    /// @throws DataStoreException if there is any error during operation
    void removeDataStoreEntries(const std::string& prefix)
    {
        try {
            removeDirectory(prefix, true);
        } catch (DataStoreException const& e) {
            THROW_NESTED(DataStore, "Cannot remove Data Store Entries with prefix " << prefix, e);
        }
    }

    /// Create a given directory on backend file system
    /// It internally use boost::create_directories() to create directory, including
    /// any parent directories that do not exist.
    /// @param dirPath path to the directory to create
    /// @throws DataStoreException if the directory cannot be created
    void createDirectory(const std::string& dirPath)
    {
        try {
            bf::path myPath(ckptDirectory_ + "/" + dirPath);
            bf::create_directories(myPath);
        } catch (bf::filesystem_error const& e) {
            THROW(DataStore, "Cannot create " << dirPath << ": received exception: " << e.what());
        }
    }

    /// Remove a given directory from backend file system
    /// @param dirPath path to the directory to remove
    /// @param recursive if set to true, remove sub-directories; if set to false, only remove
    /// the given directory if it's empty
    /// @throws DataStoreException if the directory cannot be removed
    void removeDirectory(const std::string& dirPath, const bool recursive)
    {
        try {
            bf::path myPath(ckptDirectory_ + "/" + dirPath);
            if (recursive == false) {
                bf::remove(myPath);
            } else {
                bf::remove_all(myPath);
            }
        } catch (bf::filesystem_error const& e) {
            THROW(DataStore, "Cannot remove " << dirPath << ": received exception: " << e.what());
        }
    }

  private:
    std::string ckptDirectory_;

  protected:
    DataStoreUpdateBatchImpl* createUpdateBatchImpl();

  private:
    uint64_t compactionInterval_;
};
}

#endif // SPL_DSA_FILESYSTEM_DATA_STORE_ADAPTER_H
