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

#ifndef PATHPERMISSION_H_
#define PATHPERMISSION_H_

#include <UTILS/UTILSTypes.h>
#include <string>
#include <vector>

UTILS_NAMESPACE_BEGIN

class PathPermission
{
  public:
    PathPermission();
    PathPermission(const std::string& user, int permission);
    PathPermission(const std::string& scheme, const std::string& user, int permission);

    // static data members
    static const PathPermission System;

    // well known user id string values
    static const char* SYSTEM;

    // permission values
    static const int READ = 1 << 0;
    static const int WRITE = 1 << 1;
    static const int CREATE = 1 << 2;
    static const int DELETE = 1 << 3;
    static const int ADMIN = 1 << 4;
    static const int CRUD = CREATE | READ | WRITE | DELETE;
    static const int ALL = READ | WRITE | CREATE | DELETE | ADMIN;

    /**
     * Returns the permission scheme (i.e. digest, world, etc)
     *
     * @return String
     */
    inline std::string getScheme() const { return _scheme; }

    /**
     * Returns the user/password digest value
     *
     * @return String
     */
    inline std::string getUser() const { return _user; }

    /**
     * Returns the actual user permission(s)
     *
     * @return int
     */
    inline int getPermission() const { return _permission; }

    /**
     * Returns special Permission object for the current authorized user
     *
     * @param permission
     * @return Permission
     */
    static PathPermission current(int permission);

    /**
     * Returns "system" - all in a list
     *
     * @return List<Permission>
     */
    static std::vector<PathPermission> system();

    /**
     * Returns combined string or name paced user/password
     *
     * @param user
     * @return String
     */
    static std::string getUserPw(const std::string& user);

  private:
    /**
     * Returns password for the requested user
     *
     * @param user
     * @return String
     */
    static std::string getPw(const std::string& user);

    /**
     * Returns digest version of combined name spaced user/password
     *
     * @param namespace
     * @param user
     * @return String
     */
    static std::string getUserPwDigest(const std::string& user);

    // data members
    std::string _scheme;
    std::string _user;
    int _permission;
};

UTILS_NAMESPACE_END

#endif /* PATHPERMISSION_H_ */
