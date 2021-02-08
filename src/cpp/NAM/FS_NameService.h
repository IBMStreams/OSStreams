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

#ifndef _FS_NAMESERVICE_H_
#define _FS_NAMESERVICE_H_

#include <NAM/NAM_NameService.h>
#include <UTILS/Directory.h>
#include <UTILS/RegEx.h>
#include <UTILS/UTILSTypes.h>
#include <string>

NAM_NAMESPACE_BEGIN

/**
   \internal

   This class simplifies access to the FS name Service
*/
class FS_NameService : public NAM_NameService
{
  private:
    std::string _distillery_id;
    std::string _stg_path;

    std::string createObjName(const std::string& name);
    std::string createObjDir(const std::string& name);
    FS_NameService(const std::string& ns_arg, const std::string& distID);
    FS_NameService(const std::string& ns_arg,
                   const std::string& domainID,
                   const std::string& distID);
    void initServer(const std::string& ns_arg);

    // Method to get the matched object from sub-directories. Using this method recursivly.
    // @param dir directory name.
    // @param re regular expression of the filter
    // @param did a default pattern to filter all Sys S NAM entries, "@<distillery_id>"
    // @param numRetries maximum retry times. Not supported for this method currently.
    // @return the list of names of the entry in a vertor.
    std::vector<std::string> listObjects(const std::string& dir,
                                         const UTILS_NAMESPACE::RegEx& re,
                                         const std::string& did,
                                         int numRetries,
                                         const std::string& oriDir);

    // This is the public interface for Java version of listObjects.
    // Currently, Java and C++ version of listObjects interface are different.
    // But since there is not requirement to make them same, so keep this method as private.
    // If new requirement coming, make it public, and also add this interface to
    // NAM_NameService and update LDAP_NameService.
    // @param filter filter for searching
    // @param dir directory name.
    // @param scope flag to see if search sub-directories or not.
    // 	if scope="one", search current directory only;
    //	if scope="sub", search current directory and all its sub-dir.
    // @param numRetries maximum retry times. Not supported for this method currently.
    // @return the list of names of the entry in a vertor.
    std::vector<std::string> listObjects(const std::string& filter,
                                         const std::string& dir,
                                         const std::string& scope,
                                         int numRetries);

    /// Method to add or update the entry to the NAM database
    /// @param name name of the entry
    /// @param nr entry content.
    /// @param numRetries maximum retries if adding or updating fail.
    /// @param update flag to check if we want to add or update.
    void addOrUpdateObject(const std::string& name,
                           const NameRecord& nr,
                           int numRetries,
                           bool update = false);

    void kickIt(const std::string& name);

  public:
    virtual void registerObject(const std::string& name, const NameRecord& nr, int numRetries = 0);
    virtual void unregisterObject(const std::string& name, int numRetries = 0);
    virtual void updateObject(const std::string& name, const NameRecord& nr, int numRetries = 0);
    virtual void lookupObject(const std::string& name,
                              NameRecord& nr,
                              int numRetries = 0,
                              const bool force = false);
    virtual void createSubdir(const std::string& name, int numRetries = 0);
    virtual void destroySubdir(const std::string& name, int numRetries = 0);
    virtual std::vector<std::string> listObjects(const std::string& filter, int numRetries = 0);
    virtual void setMessageRecordingConfiguration(
      const UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
      int numRetries = 0);
    virtual void getMessageRecordingConfiguration(
      UTILS_RUNTIME_NS::mr::MessageRecordingConfigurationType& mrct,
      int numRetries = 0);

    virtual ~FS_NameService(void);
    friend class NAM_NameService;
};

NAM_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
