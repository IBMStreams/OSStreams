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

#ifndef NAM_USERSERVICEUTIL_H
#define NAM_USERSERVICEUTIL_H

#include <NAM/NAM_NameService.h>
#include <NAM/NAM_Namespace.h>
#include <SAM/SAMTypes.h>

NAM_NAMESPACE_BEGIN

class UserValueRecord : public NameRecord
{
  public:
    UserValueRecord(void);
    UserValueRecord(const std::string& appScope,
                    const std::string& label,
                    const std::string& value);
    std::string getValue(void) const;
    std::string getApplicationScope(void) const;
    std::string getLabel(void) const;
    std::string toString(void) const;
    void setObject(const std::string& serializedData);
    virtual ~UserValueRecord(void);

  private:
    std::string _appScope;
    std::string _label;
    std::string _value;
};

/**
 * Following functions to support NAM actions for user objects.
 */

/// Register an user object to the name service
/// @param peid PE id
/// @param label user label for the registered object
/// @param value user object value
/// @param update flag to check if wanting to update the existing NAM record.
void registerUserObject(const SAM_NAMESPACE::PEID& peid,
                        const std::string& appScope,
                        const std::string& label,
                        const std::string& value,
                        bool update = false);

/// Unregister an user object from the name service
/// @param peid PE id
void unregisterUserObject(const SAM_NAMESPACE::PEID& peid);

/// Unregister an user object from the name service
/// @param appScope application scope
/// @param label user label for the registered object
void unregisterUserObject(const std::string& appScope, const std::string& label);

/// List user objects registered to the name service
/// @param scopeFilter application scope filter.
/// @param labelFilter label filter.
/// @return list of UserRecord objects.
std::vector<UserValueRecord> listUserObjects(const std::string& scopeFilter = "*",
                                             const std::string& labelFilter = "*");

NAM_NAMESPACE_END

#endif
