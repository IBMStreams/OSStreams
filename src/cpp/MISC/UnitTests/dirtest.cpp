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

#include <UTILS/Directory.h>
#include <UTILS/DistilleryApplication.h>
#include <cassert>
#include <iostream>
#include <stdio.h>

UTILS_NAMESPACE_USE

using namespace std;

class dirtest : public DistilleryApplication
{
  public:
    virtual int run(const vector<string>& remains)
    {
        if (Directory::sisAFile("/etc/passwd")) {
            cout << "/etc/passwd is a file" << endl;
        } else {
            cout << "/etc/passwd is not a file" << endl;
        }

        if (Directory::sisAFile("/etc/passwd2")) {
            cout << "/etc/passwd2 is a file" << endl;
        } else {
            cout << "/etc/passwd2 is not a file" << endl;
        }

        Directory d1("/tmp");
        cout << ">>> Listing all entries in the /tmp directory" << endl;
        for (ConstDirEntryIterator i = d1.begin(); i != d1.end(); ++i) {
            cout << *i << " is " << i->getBaseName() << endl;
        }

        Directory d2("/tmp", false, Directory::NOFILES | Directory::NODOTS);
        cout << ">>> Listing all directory entries in /tmp" << endl;
        for (ConstDirEntryIterator i = d2.begin(); i != d2.end(); ++i) {
            cout << *i << endl;
        }
        char testdir[128];
        snprintf(testdir, 128, "testdir.%d", getpid());
        try {
            d2.mkdir(testdir, 0200);
        } catch (DistilleryException& d) {
            cout << d.getType() << endl;
        }
        if (d1.isADirectory(testdir)) {
            cout << testdir << " is a directory according to d1 object" << endl;
        }
        if (d2.isADirectory(testdir)) {
            cout << testdir << " is a directory according to d2 object" << endl;
        }
        cout << "After mkdir..." << endl;
        for (ConstDirEntryIterator i = d2.begin(); i != d2.end(); ++i) {
            cout << *i << endl;
        }
        try {
            // d1 was not refreshed as we have d2 pointing to the same dir and the
            // entry was created in object d2
            d1.rmdir(testdir, false);
        } catch (DistilleryException& d) {
            cout << d.getExplanation() << endl;
        }
        // now, we removed it
        d2.rmdir(testdir, true);
        cout << "After rmdir..." << endl;
        for (ConstDirEntryIterator i = d2.begin(); i != d2.end(); ++i) {
            cout << *i << " is " << i->getBaseName() << endl;
        }

        if (!remains.empty()) {
            Directory dx(remains[0]);
            cout << ">>> Listing all directory entries in " << remains[0] << endl;
            for (ConstDirEntryIterator i = dx.begin(); i != dx.end(); ++i) {
                cout << *i << endl;
            }
        }

        try {
            Directory::srmdir("/tmp/dirtestxx", true);
        } catch (DistilleryException& d) {
        }
        Directory::smkdir("/tmp/dirtestxx/yy/zz", 0755);
        Directory::srmdir("/tmp/dirtestxx", true);

        cout << "Success..." << endl;
        return 0;
    }
};

MAIN_APP(dirtest)
