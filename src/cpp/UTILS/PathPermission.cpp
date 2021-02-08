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
#include <UTILS/PathPermission.h>
#include <UTILS/SupportFunctions.h>
#include <sstream>

#include <stdio.h>

using namespace std;
DEBUG_NAMESPACE_USE
UTILS_NAMESPACE_USE

// constants
const char* PathPermission::SYSTEM = "system";

// static objects
const PathPermission PathPermission::System = PathPermission(SYSTEM, ALL);

PathPermission::PathPermission()
  : _scheme("digest")
  , _user("")
  , _permission(ALL)
{}

PathPermission::PathPermission(const string& user, int permission)
  : _scheme("digest")
  , _user(getUserPwDigest(user))
  , _permission(permission)
{}

PathPermission::PathPermission(const string& scheme, const string& user, int permission)
  : _scheme(scheme)
  , _user(user)
  , _permission(permission)
{}

vector<PathPermission> PathPermission::system()
{
    vector<PathPermission> perms;
    perms.push_back(System);
    return perms;
}

string PathPermission::getUserPw(const string& user)
{
    return user + ":" + getPw(user);
}

string PathPermission::getPw(const string& user)
{
    string pw = "";

    // system
    if (user == SYSTEM) {
        for (int index = user.length() - 1; index >= 0; --index) {
            pw += user[index];
            stringstream ss;
            ss << index;
            pw += ss.str();
        }
    }
    // domain specific
    else {
        int total = 0;
        for (unsigned int index = 0; index < user.length(); ++index) {
            signed char ch = user[index];
            total += (int)ch;
        }

        // Using UnicodeString to get first and last char
        StringPiece spUser(user);
        UnicodeString usUser = UnicodeString::fromUTF8(spUser);
        SPCDBG(L_TRACE,
               "User " << user << ", length " << user.length() << "; unicodestring " << usUser
                       << ", length " << usUser.length() << ", total " << total,
               CORE_UTILS);
        UChar ucFirst = usUser[0];
        UnicodeString firstCharUniStr(ucFirst);
        string firstChar;
        firstCharUniStr.toUTF8String(firstChar);
        UChar ucLast = usUser[usUser.length() - 1];
        UnicodeString lastCharUniStr(ucLast);
        string lastChar;
        lastCharUniStr.toUTF8String(lastChar);

        // Calculate the password
        stringstream ss;
        ss << firstChar << (total * 3) << (total * 5) << (total * 7) << lastChar;
        pw = ss.str();
    }
    // SPCDBG(L_INFO, "*****getPw: "<< pw << " for user " << user, CORE_UTILS);
    return pw;
}

string PathPermission::getUserPwDigest(const string& user)
{
    string userPw = getUserPw(user);

    string digest;
    computeBase64SHA1Digest(userPw.c_str(), userPw.length(), digest);

    return user + ":" + digest;
}
