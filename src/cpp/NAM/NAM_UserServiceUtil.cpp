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

#include <NAM/NAMMessages.h>
#include <NAM/NAM_Exceptions.h>
#include <NAM/NAM_UserServiceUtil.h>
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/SupportFunctions.h>
#include <sstream>
#include <vector>

using namespace std;
UTILS_NAMESPACE_USE
NAM_NAMESPACE_USE
SAM_NAMESPACE_USE
DEBUG_NAMESPACE_USE

const string APP_DIR = "app";

UserValueRecord::UserValueRecord(void)
  : _appScope("")
  , _label("")
  , _value("")
{}

UserValueRecord::UserValueRecord(const string& appScope, const string& label, const string& value)
  : _appScope(appScope)
  , _label(label)
  , _value(value)
{}

string UserValueRecord::getValue(void) const
{
    return _value;
}

string UserValueRecord::getApplicationScope(void) const
{
    return _appScope;
}

string UserValueRecord::getLabel(void) const
{
    return _label;
}

string UserValueRecord::toString(void) const
{
    // Note: Can not use single ':', as label may include this char.
    stringstream ss;
    ss << _appScope << ";;" << _label << ";;" << _value;
    return ss.str();
}

void UserValueRecord::setObject(const string& serializedData)
{
    vector<string> tok;
    tokenize(serializedData, tok, ";;", false);
    if (tok.size() != 3) {
        THROW(Distillery, "Invalid UserValueRecord format: " << QT(serializedData),
              NAMInvalidRecord, serializedData.c_str());
    }
    _appScope = tok[0];
    _label = tok[1];
    _value = tok[2];
}

UserValueRecord::~UserValueRecord(void) {}

void NAM_NAMESPACE::registerUserObject(const PEID& peid,
                                       const string& appScope,
                                       const string& label,
                                       const string& value,
                                       bool update)
{
    SPCDBG(L_DEBUG,
           "Enter: registerUserObject for PE:" << peid << " appScope:" << appScope
                                               << " label:" << label << " value:" << value
                                               << " with update:" << update,
           NAM_USER_SERVICE);
    // Check appScope and label:
    if (appScope.empty() || label.empty()) {
        THROW(NAM, "Unable to register object as empty applicationScope or label not allowed.",
              NAMUnableToRegisterUserObject);
    }

    NAM_NameService* ns = NAM_NameService::getInstance();

    // Try to create app directory first. This call will do nothing if
    // app dir already there.
    ns->createSubdir(APP_DIR);

    // Note:
    // 1. In order for Runtime to clean the entry if PE crash, we better to have
    //    PEID with the entry name.
    // 2. We have to make sure that there is only one entry to the name service
    //    for each appScope:label.
    // 3. As we are using PEID and appScope/label in same time, we can not
    //    directly use the name service logic for the duplicating checking.
    // 4. Some solutions that have been discussed for this problem:
    //    (1). Do name existing checking first: This way can not check those
    //         records on the wire to NAM DN server (or FS), so can not close
    //         gap totally.
    //    (2). Add PEID to the record content: This way, for Runtime cleanup, we
    //         have to scan all NAM entries (under app/) and parsing content to
    //         check PEID, too much cost if many entries.
    //    (3). Solution below, register two entries to NAM for each record.
    //         Will consume space, but OK as entries not big.
    //         (we do this right now for PE and PE ports also).
    // 5. Solution here: register two entries:
    //    (1). app/applicationScope:label
    //    (2). app/PEID:applicationScope:label
    //    Register the 1st entry first to guarantee uniqueness, then register the
    //    2nd one to help cleanup later.

    // Composing the entry names
    stringstream nameWithoutPeid, nameWithPeid;
    nameWithoutPeid << APP_DIR << "/" << appScope << ":" << label;
    nameWithPeid << APP_DIR << "/" << peid << ":" << appScope << ":" << label;
    SPCDBG(L_TRACE,
           "Going to register nameWithoutPeid=" << nameWithoutPeid.str()
                                                << " nameWithPeid=" << nameWithPeid.str(),
           NAM_USER_SERVICE);
    // Create UserValueRecord object to register
    UserValueRecord nr(appScope, label, value);

    // Register entries.....
    // Step 1: register first entry: app/applicationScope:label
    try {
        ns->registerObject(nameWithoutPeid.str(), nr);
        SPCDBG(L_INFO, "Done register " << nameWithoutPeid.str(), NAM_USER_SERVICE);
    } catch (const NameExistsException& e) {
        if (update) {
            // Need to check if existing entry is with the same PEID.
            SPCDBG(L_TRACE, "Need to check " << nameWithPeid.str() << "existing before updating",
                   NAM_USER_SERVICE);
            vector<string> names = ns->listObjects(nameWithPeid.str());
            if (1 == names.size()) {
                ns->updateObject(nameWithoutPeid.str(), nr);
                SPCDBG(L_INFO, "Done update " << nameWithoutPeid.str(), NAM_USER_SERVICE);
            } else {
                // We come here when we found entry without PEID, but not found entry
                // for with the PEID, it means that some other PEID already register
                // with the appScope and label. throw.
                throw;
            }
        } else {
            throw;
        }
    }
    // No need to catch NAMException, etc, let them throw back to upper layer.

    // step 2: register 2nd entry: app/PEID:applicationScope:label
    try {
        ns->registerObject(nameWithPeid.str(), nr);
        SPCDBG(L_INFO, "Done register " << nameWithPeid.str(), NAM_USER_SERVICE);
    } catch (const NameExistsException& e) {
        if (update) {
            ns->updateObject(nameWithPeid.str(), nr);
            SPCDBG(L_INFO, "Done update " << nameWithPeid.str(), NAM_USER_SERVICE);
        } else {
            // As we always keep two entries for one user object, in case of this,
            // step 1 registering will throw, should never come here...
        }
    } catch (const NAMException& e) {
        // In case hit unexpected exception for registering the 2nd entry,
        // unregister the 1st one and throw.
        SPCDBG(L_ERROR,
               "Register " << nameWithPeid.str() << " hit exception " << e.getExplanation()
                           << ". Unregistering entry of " << nameWithoutPeid.str(),
               NAM_USER_SERVICE);
        ns->unregisterObject(nameWithoutPeid.str(), 10);
        throw;
    }
    SPCDBG(L_DEBUG, "Exit", NAM_USER_SERVICE);
}

void NAM_NAMESPACE::unregisterUserObject(const PEID& peid)
{
    SPCDBG(L_DEBUG, "Enter: unregisterUserObject for PE:" << peid, NAM_USER_SERVICE);
    NAM_NameService* ns = NAM_NameService::getInstance();

    // Note: there are two entries for one user object:
    // "app/PEID:applicationScope:userLabel" and "app/applicationScope:userLabel"
    // Need to unregister both.

    stringstream key;
    key << APP_DIR << "/" << peid << ":*";
    vector<string> names = ns->listObjects(key.str());
    for (unsigned int i = 0; i < names.size(); i++) {
        // There should be one entry like app/PEID:applicationScope:userLabel,
        // But using loop safer than hard-coded taking names[0].
        string nameWithPeid = names[i];
        size_t ind = nameWithPeid.find(":");
        string label = nameWithPeid.substr(ind + 1);
        stringstream nameWithoutPeid;
        nameWithoutPeid << APP_DIR << "/" << label;
        ns->unregisterObject(nameWithPeid);
        SPCDBG(L_INFO, "Unregistered object " << nameWithPeid, NAM_USER_SERVICE);
        ns->unregisterObject(nameWithoutPeid.str());
        SPCDBG(L_INFO, "Unregistered object " << nameWithoutPeid.str(), NAM_USER_SERVICE);
    }
}

void NAM_NAMESPACE::unregisterUserObject(const string& appScope, const string& label)
{
    SPCDBG(L_DEBUG, "Enter: unregisterUserObject with appScope:" << appScope << " label:" << label,
           NAM_USER_SERVICE);
    NAM_NameService* ns = NAM_NameService::getInstance();
    stringstream key;
    key << APP_DIR << "/*" << appScope << ":" << label;
    SPCDBG(L_TRACE, "Listing objects for key: " << key.str(), NAM_USER_SERVICE);
    vector<string> names = ns->listObjects(key.str());
    for (unsigned int i = 0; i < names.size(); i++) {
        ns->unregisterObject(names[i]);
        SPCDBG(L_INFO, "Unregistered object " << names[i], NAM_USER_SERVICE);
    }
}

vector<UserValueRecord> NAM_NAMESPACE::listUserObjects(const string& scopeFilter,
                                                       const string& labelFilter)
{
    SPCDBG(L_DEBUG,
           "Enter: listUserObjects with scopeFilter:" << scopeFilter
                                                      << " labelFilter:" << labelFilter,
           NAM_USER_SERVICE);
    vector<UserValueRecord> records;

    NAM_NameService* ns = NAM_NameService::getInstance();
    stringstream realFilter;
    realFilter << APP_DIR << "/*" << scopeFilter << ":" << labelFilter;
    SPCDBG(L_TRACE, "Listing objects with key " << realFilter.str(), NAM_USER_SERVICE);
    vector<string> names = ns->listObjects(realFilter.str());
    for (unsigned int i = 0; i < names.size(); i++) {
        // As there are two entries for each user object:
        // "app/PEID:applicationScope:userLabel" and "app/applicationScope:userLabel"
        // As we do not want duplicated return records, we have to filter out one.
        SPCDBG(L_TRACE, "Getting record for entry: " << names[i], NAM_USER_SERVICE);
        UserValueRecord nr;
        try {
            ns->lookupObject(names[i], nr);
            stringstream nameSS;
            nameSS << APP_DIR << "/" << nr.getApplicationScope() << ":" << nr.getLabel();
            if (0 == names[i].compare(nameSS.str())) {
                // Just add "app/applicationScope:userLabel" formatted entry into return list.
                records.push_back(nr);
                SPCDBG(L_DEBUG, "Added record: " << nr.toString(), NAM_USER_SERVICE);
            }
        } catch (const NameNotFoundException& ne) {
            // After listing all entries, as we look up entry again, it is possible
            // that someone already remove it. In this case, we log and continue.
            SPCDBG(L_WARN, "Entry " << names[i] << " not found. " << ne.getExplanation(),
                   NAM_USER_SERVICE);
        }
    }

    SPCDBG(L_DEBUG, "End...", NAM_USER_SERVICE);
    return records;
}
