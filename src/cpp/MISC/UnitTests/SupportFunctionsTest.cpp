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

#include <UTILS/ArgContainer.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HostInfo.h>
#include <UTILS/SupportFunctions.h>
#include <UTILS/Thread.h>
#include <cassert>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <unistd.h>

using namespace std;
UTILS_NAMESPACE_USE

class supp_func : public DistilleryApplication
{
  public:
    double g(double d) { return 0; }

    virtual int run(const vector<string>& remainings_args)
    {
        cout << "Streams is being run by '" << getUserName() << "'" << endl;
        cout << "Streams is installed at '" << getInstallationDirectory() << "'" << endl;
        cout << "Streams instance identifier is '" << getInstanceId() << "'" << endl;
        cout << "global license directory is '" << getLicenseDirectory() << "'" << endl;
        cout << "global 'bin' directory is '" << getBinDirectory() << "'" << endl;
        cout << "AAS key directory is '" << getUserKeyConfigurationDirectory(false) << "'" << endl;
        string instanceid("myinstance@" + getUserName());
        cout << "Streams instance template directory '" << instanceid
             << "' configuration directory is '"
             << getInstanceTemplateConfigurationDirectory(instanceid, false) << "'" << endl;
        cout << "Streams instance configuration directory '" << instanceid
             << "' configuration directory is '"
             << getInstanceConfigurationDirectory(instanceid, false) << "'" << endl;
        cout << "instance owner 'bin' directory is '"
             << getInstanceOwnerBinDirectory(instanceid, false) << "'" << endl;
        cout << "instance 'bin' directory is '" << getInstanceBinDirectory(instanceid, false) << "'"
             << endl;
        cout << "Streams instance Name Service directory '" << instanceid
             << "' configuration directory is '"
             << getInstanceNameServiceConfigurationDirectory(instanceid, false) << "'" << endl;
        cout << "Streams instance SWS directory '" << instanceid << "' configuration directory is '"
             << getInstanceSWSConfigurationDirectory(instanceid, false) << "'" << endl;
        cout << "This host has " << SystemStatics::getNCPUs() << " CPUs " << endl;
        cout << "10M is " << suffixedStr2double("10M") << endl;
        cout << "10u is " << suffixedStr2double("10u") << endl;
        double n = 4.5e-06;
        cout << n << " is " << double2suffixedStr(n) << endl;
        n = 4.5e-04;
        cout << n << " is " << double2suffixedStr(n) << endl;
        n = 4500;
        cout << n << " is " << double2suffixedStr(n) << endl;
        n = 4500000;
        cout << n << " is " << double2suffixedStr(n) << endl;

        try {
            cout << "eth0: " << HostInfo::getInterfaceAddr("eth0") << endl;
        } catch (HostInfoException& e) {
            cout << "eth0: <missing>" << endl;
        }

        try {
            cout << "eth1: " << HostInfo::getInterfaceAddr("eth1") << endl;
        } catch (HostInfoException& e) {
            cout << "eth1: <missing>" << endl;
        }

        stringstream ss;
        ss << "Henrique"
           << "\t"
           << "Andrade" << endl
           << "loves C++" << endl
           << " is testing this\n";
        cout << "Unsanitized string: [" << ss.str() << "]" << endl;
        cout << "Sanitized string: [" << removeUnprintableChars(ss.str()) << "]" << endl;
        cout << "Unprintable chars are replaced with space: ["
             << replaceUnprintableCharsWithSpace(ss.str()) << "]" << endl;

        int c;
        // first test
        ArgContainer arg("-f file -g 10 -h");
        cout << "argc " << arg.argc << endl;
        assert(arg.argc == 6);
        for (int j = 0; j < arg.argc; ++j) {
            cout << "argv[" << j << "] is [" << arg.argv[j] << "]" << endl;
        }
        while ((c = getopt(arg.argc, arg.argv, "f:g:h")) != -1) {
            cout << "Switch '" << c << "'";
            if (optarg) {
                cout << " Value '" << optarg << "'" << endl;
            } else {
                cout << endl;
            }
        }

        // another test
        ArgContainer arg2("-f /homes/hny2/hcma/DPS/src/inputfile");
        assert(arg2.argc == 3);
        for (int j = 0; j < arg2.argc; ++j) {
            cout << "argv[" << j << "] is [" << arg2.argv[j] << "]" << endl;
        }
        while ((c = getopt(arg2.argc, arg2.argv, "f:")) != -1) {
            cout << "Switch '" << static_cast<char>(c) << "'";
            if (optarg) {
                cout << " Value '" << optarg << "'" << endl;
            } else {
                cout << endl;
            }
        }
        //
        // another test
        ArgContainer arg3(
          " aaa   -i 4   -j 2 --oi \"\\thenr   ique\\n\"\"\" -j \"john   smith\" -p");
        assert(arg3.argc == 11);
        for (int j = 0; j < arg3.argc; ++j) {
            cout << "argv[" << j << "] is [" << arg3.argv[j] << "]" << endl;
        }

        // another test
        cout << timeStampToMicroseconds("01-JAN-1970", "00:00:01.100") << endl;
        // note that the time above is assumed to be in the local timezone
        // therefore the returned time is not 1.1 as expected (that would be
        // true only for computers set to UTC)
        assert(timeStampToMicroseconds("01-JAN-1970", "00:00:01.100") <
               static_cast<uint64_t>(86400) * 1000 * 1000);
        uint64_t t1 = timeStampToMicroseconds("04-OCT-2005", "14:18:19.324");
        uint64_t t2 = timeStampToMicroseconds("04-OCT-2005", "14:18:20.324");
        cout << "Time diff:  " << t2 - t1 << endl;
        assert((t2 - t1) > static_cast<uint64_t>(999 * 1000) &&
               (t2 - t1) < static_cast<uint64_t>(1001 * 1000));

        // another test
        cout << setprecision(32) << "YYYY_MM_DD_hh_mm_ssToMicroseconds test: "
             << YYYY_MM_DD_hh_mm_ssToMicroseconds("1970-01-01 00:00:01") << endl;
        cout << setprecision(32) << "YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds test: "
             << YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds("1970-01-01 00:00:01.010") << endl;
        // note that the time above is assumed to be in the local timezone
        // therefore the returned time is not 1 as expected (that would be
        // true only for computers set to UTC)
        assert(YYYY_MM_DD_hh_mm_ssToMicroseconds("1970-01-01 00:00:01") <
               static_cast<uint64_t>(86400) * 1000 * 1000);

        assert(YYYY_MM_DD_hh_mm_ssToMicroseconds("1970-01-10 00:00:01") ==
               MM_DD_YYYY_hh_mm_ssToMicroseconds("01-10-1970 00:00:01"));

        assert(YYYY_MM_DD_hh_mm_ssToMicroseconds("1970-10-01 00:00:01") ==
               DD_MM_YYYY_hh_mm_ssToMicroseconds("01-10-1970 00:00:01"));

        assert(YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds("1970-01-10 00:00:01.100") ==
               MM_DD_YYYY_hh_mm_ss_mmmToMicroseconds("01-10-1970 00:00:01.100"));

        assert(YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds("1970-10-01 00:00:01.100") ==
               DD_MM_YYYY_hh_mm_ss_mmmToMicroseconds("01-10-1970 00:00:01.100"));
        // another test
        cout << "CPU frequency is " << SystemStatics::getCPUFrequency() << endl;

        // another test

        uint64_t t = getTimeInNanosecs();
        sleep(2);
        cout << "Time elapsed " << getTimeInNanosecs() - t << " ns" << endl;

        // another test

        t = getTimeInMicrosecs();
        cout << "Time in microsecs: " << t << endl;
        sleep(2);
        cout << "Time elapsed " << getTimeInMicrosecs() - t << " us" << endl;

        // another test

        t = getWallTimeInMicrosecs();
        cout << "Wall time in microsecs: " << t << endl;
        sleep(2);
        cout << "Time elapsed " << getWallTimeInMicrosecs() - t << " us" << endl;

        // another test

        t = getWallTimeInMicrosecs() / 1000;
        cout << "Wall time in millisecs: " << t << endl;
        sleep(2);
        cout << "Time elapsed " << (getWallTimeInMicrosecs() / 1000) - t << " ms" << endl;

        // another test
        string s("IBM is Big Blue\r\nBig Blue is IBM\r\nBig is\rBlue\r\nBlue is\n "
                 "Big\r\nBig\r\r\ris Blue");
        istringstream i(s);
        uint32_t actuallen = 0;
        unsigned nlines = 0;
        do {
            char buf[256];
            actuallen = dosgetline(i, buf, 255);
            if (actuallen > 0) {
                buf[actuallen] = '\0';
                cout << "----------------------------" << endl;
                cout << "actual buffer [" << buf << "]" << endl;
                cout << "sanitized buffer [" << removeUnprintableChars(buf) << "]" << endl;
                nlines++;
            }
        } while (actuallen > 0);
        assert(nlines == 5);

        // another test
        for (uint64_t b = 0; b < 1000 * 1000 * 3; b++) {
            double f = log(static_cast<float>(b));
            g(f);
        }
        // cout << "Thread time " << Thread::getCPUTimeInNanosecs() << " ns" << endl;
        return 0;
    }
};

MAIN_APP(supp_func);
