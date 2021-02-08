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

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <endian.h>
#include <fcntl.h>
#include <fstream>
#include <functional>
#include <libtecla.h>
#include <linux/sockios.h>
#include <linux/types.h>
#include <memory>
#include <openssl/evp.h>
#include <poll.h>
#include <pwd.h>
#include <sstream>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unicode/datefmt.h>
#include <unicode/smpdtfmt.h>
#include <unicode/stringpiece.h>
#include <unicode/timezone.h>
#include <unicode/ustring.h>
#include <unistd.h>
#include <vector>

#include "Base64.h"
#include "Directory.h"
#include "Formatter.h"
#include "PthreadHelper.h"
#include "RAIIFct.h"
#include "SBuffer.h"
#include "SupportFunctions.h"
#include "UTILSMessages.h"
#include <TRC/DistilleryDebug.h>
#include <TRC/RuntimeTrcAspects.h>
#include <UTILS/Exception.h>

using namespace std;
DEBUG_NAMESPACE_USE
using namespace xmlns::prod::streams::runtime::utils;

UTILS_NAMESPACE_BEGIN

SpinLock SystemStatics::slock;

static string frequencyToken()
{
#if defined(__powerpc__)
    return "clock";
#else
    return "cpu MHz";
#endif
}

static string frequencyCleaning(const string& rhs)
{
#if defined(__powerpc__)
    return rhs.substr(0, rhs.size() - rhs.find("MHz"));
#else
    return rhs;
#endif
}

float SystemStatics::initCPUFrequency()
{
    float freq = 0;
    ifstream in("/proc/cpuinfo");
    if (in) {
        string line;
        while (getline(in, line)) {
            string lhs = line.substr(0, line.find(":"));
            if (lhs.find(frequencyToken()) != string::npos) {
                string rhs = line.substr(line.find(":") + 1, line.size());
                fromString(freq, frequencyCleaning(rhs));
                break;
            }
        }
        if (freq <= 0) {
            SPCDBG(L_ERROR,
                   "failed to find frequency in /proc/cpuinfo; CPU frequency is unknown and "
                   "related functions will not work correctly",
                   CORE_UTILS);
        }
        in.close();
    } else {
        SPCDBG(L_ERROR,
               "failed to open /proc/cpuinfo; CPU frequency is unknown and related functions will "
               "not work correctly",
               CORE_UTILS);
    }
    SPCDBG(L_DEBUG, "CPU has a clock frequency of " << freq << " MHz", CORE_UTILS);
    return freq;
}

unsigned SystemStatics::initNCPUs()
{
    long cpus = sysconf(_SC_NPROCESSORS_ONLN);
    if (cpus == -1) {
        SPCDBG(L_ERROR, "error reading number of online processors from sysconf; assuming 1",
               CORE_UTILS);
        cpus = 1;
    }
    return cpus;
}

// check if we are running in container environment and return the
// available CPU resources the container can use if it is, otherwise
// return -1
int SystemStatics::initContainerNCPUs()
{
    ifstream cpu_quota_f("/sys/fs/cgroup/cpu/cpu.cfs_quota_us");
    int cpu_quota_us;
    if (cpu_quota_f && cpu_quota_f >> cpu_quota_us) {
        if (cpu_quota_us != -1) {
            ifstream cpu_period_f("/sys/fs/cgroup/cpu/cpu.cfs_period_us");
            int cpu_period_us;
            if (cpu_period_f && cpu_period_f >> cpu_period_us) {
                return cpu_quota_us / cpu_period_us;
            }
        }
    }
    return -1;
}

uint64_t SystemStatics::initPPCTimeBase()
{
    uint64_t timebase = 0;
#if defined(__powerpc__)
    ifstream in("/proc/cpuinfo");
    if (in) {
        string line;
        while (getline(in, line)) {
            string lhs = line.substr(0, line.find(":"));
            if (lhs.find("timebase") != string::npos) {
                string rhs = line.substr(line.find(":") + 1, line.size());
                fromString(timebase, rhs);
                break;
            }
        }
        if (timebase == 0) {
            SPCDBG(L_ERROR,
                   "failed to find timebase in /proc/cpuinfo; CPU timebase is unknown and related "
                   "functions will not work correctly",
                   CORE_UTILS);
        }
        in.close();
    } else {
        SPCDBG(L_ERROR,
               "failed to open /proc/cpuinfo; CPU timebase is unknown and related functions will "
               "not work correctly",
               CORE_UTILS);
    }
    SPCDBG(L_DEBUG, "PowerPC timebase is " << timebase, CORE_UTILS);
#endif
    return timebase;
}

// WARNING: MAJOR HACK
// this is my (hcma) implementation of something that resembles the
// systemcfg_init function from asm/systemcfg.h -- that function does
// not build properly, but when it does, that one should be used
// instead
// This function is used to define the constant PPC_TICKS_PER_NANOSEC, so
// using that constant is much cheaper than calling this
double SystemStatics::initTicksPerNanosec(void)
{
    double tps = 0;
#if defined(__powerpc__)
    static uint64_t ppcTimeBase = SystemStatics::getPPCTimeBase();
    struct dst_systemcfg
    {
        __u8 eye_catcher[16]; // Eyecatcher: SYSTEMCFG:PPC640x00
        struct
        {                // Systemcfg version numbers
            __u32 major; // Major number0x10
            __u32 minor; // Minor number0x14
        } version;
        __u32 platform;           // Platform flags0x18
        __u32 processor;          // Processor type0x1C
        __u64 processorCount;     // # of physical processors0x20
        __u64 physicalMemorySize; // Size of real memory(B)0x28
        __u64 tb_orig_stamp;      // Timebase at boot0x30
        __u64 tb_ticks_per_sec;   // Timebase tics / sec0x38
        __u64 tb_to_xs;           // Inverse of TB to 2^200x40
        __u64 stamp_xsec;         // 0x48
        __u64 tb_update_count;    // Timebase atomicity ctr0x50
        __u32 tz_minuteswest;     // Minutes west of Greenwich0x58
        __u32 tz_dsttime;         // Type of dst correction0x5C
        __u32 dCacheL1Size;       // L1 d-cache size0x60
        __u32 dCacheL1LineSize;   // L1 d-cache line size0x64
        __u32 iCacheL1Size;       // L1 i-cache size0x68
        __u32 iCacheL1LineSize;   // L1 i-cache line size0x6C
        __u8 reserved0[3984];     // Reserve rest of page0x70
    };

    SPCDBG(L_DEBUG, "assuming a 64-bit PPC node and opening '/proc/ppc64/systemcfg'", CORE_UTILS);
    int fd = open("/proc/ppc64/systemcfg", O_RDONLY);
    static struct dst_systemcfg cfg;

    if (fd == -1) {
        SPCDBG(L_INFO, "failure to read '/proc/ppc64/systemcfg'", CORE_UTILS);
        SPCDBG(L_INFO, "assuming this is a Blue Gene node and attempting to open '/proc/cpuinfo'",
               CORE_UTILS);
        return ppcTimeBase / (1000 * 1000 * 1000.0);
    }
    int ret;
    do {
        ret = read(fd, &cfg, sizeof(struct dst_systemcfg));
        if (ret != sizeof(struct dst_systemcfg)) {
            SPCDBG(L_ERROR, "failure to read '/proc/ppc64/systemcfg'", CORE_UTILS);
            sleep(1);
        }
    } while (ret != sizeof(struct dst_systemcfg));
    close(fd);
    tps = cfg.tb_ticks_per_sec / (1000 * 1000 * 1000.0);
#else
    // stubbing out the module here is okay for x86 because it is
    // not called on x86
#endif
    return tps;
}

static UTILS_NAMESPACE::SpinLock timeLock_;

// There are no functiuons in GNU C library to get the system time from a broken
// down time structure with a specific time zone. There are special functions
// for UTC, such as timegm, but even that has the following note:
// "Portability note: mktime is essentially universally available. timegm is
// rather rare. For the most portable conversion from a UTC broken-down time to
// a simple time, set the TZ environment variable to UTC, call mktime, then set
// TZ back."
time_t get_mktime(struct tm* tm)
{
    time_t ret;
    {
        AutoSpinLock al(timeLock_);
        if (tm->tm_zone == NULL) {
            return ::mktime(tm); // relatively fast path
        }

        char* tz = ::getenv("TZ");

        // Are we already set to the given timezone?
        if (tz && strcmp(tz, tm->tm_zone) == 0) {
            ret = ::mktime(tm);
        } else {
            // have to do it the hard way
            ::setenv("TZ", tm->tm_zone, 1);
            ::tzset();
            ret = ::mktime(tm);
            if (tz) {
                ::setenv("TZ", tz, 1);
            } else {
                ::unsetenv("TZ");
            }
            ::tzset();
        }
    }
    return ret;
}

// Another function to work around the timezone problem
void get_localtime(time_t* time, struct tm* tm, const char* tzone)
{
    AutoSpinLock al(timeLock_);
    if (tzone == NULL) {
        localtime_r(time, tm);
        return;
    }

    char* tz = ::getenv("TZ");

    // Are we already set to the given timezone?
    if (tz && strcmp(tz, tzone) == 0) {
        localtime_r(time, tm);
        return;
    } else {
        // have to do it the hard way
        ::setenv("TZ", tzone, 1);
        ::tzset();
        localtime_r(time, tm);
        if (tz) {
            ::setenv("TZ", tz, 1);
        } else {
            ::unsetenv("TZ");
        }
        ::tzset();
    }
}

float SystemStatics::getCPUFrequency()
{
    static float const v = initCPUFrequency();
    return v;
}

unsigned SystemStatics::getNCPUs()
{
    static unsigned const v = initNCPUs();
    return v;
}

int SystemStatics::getContainerNCPUs()
{
    static int const v = initContainerNCPUs();
    return v;
}

double SystemStatics::getTicksPerNanosec()
{
    static double const v = initTicksPerNanosec();
    return v;
}

uint64_t SystemStatics::getPPCTimeBase()
{
    static uint64_t const v = initPPCTimeBase();
    return v;
}

string const& SystemStatics::get_STREAMS_DOMAIN_ID_envVar()
{
    static string v = "STREAMS_DOMAIN_ID";
    return v;
}

string const& SystemStatics::get_STREAMS_INSTANCE_ID_envVar()
{
    static string v = "STREAMS_INSTANCE_ID";
    return v;
}

string const& SystemStatics::get_STREAMS_DEFAULT_IID_envVar()
{
    static string v = "STREAMS_DEFAULT_IID";
    return v;
}

string const& SystemStatics::get_STREAMS_INSTALL_envVar()
{
    static string v = "STREAMS_INSTALL";
    return v;
}

string const& SystemStatics::get_STREAMS_NS_envVar()
{
    static string v = "STREAMS_NS";
    return v;
}

string const& SystemStatics::get_STREAMS_DEFAULT_APPLICATION_SCOPE()
{
    static string v = "Default";
    return v;
}

// The original function template for fromString in SupportFunctions.h
// is for conversion to numerical types. This specialization is from string type
// to string type. Function template specializations must be in the .cpp file
// The prototype for this is in SupportFunctions.h
template<>
void fromString<>(std::string& t, const std::string& s)
{
    t = s;
}

char* StrDup(const char* s)
{
    if (!s) {
        return NULL;
    }
    char* stmp = new char[strlen(s) + 1];
    strcpy(stmp, s);

    return stmp;
}

double suffixedStr2double(const char* str)
{
    auto_ptr<char> ptr(StrDup(str));
    char* myptr = ptr.get();
    char suffix = static_cast<int>(myptr[strlen(myptr) - 1]);
    double temp;
    double factor = 1;
    if (isalpha(suffix)) {
        myptr[strlen(myptr) - 1] = '\0';
        fromString(temp, myptr);
        switch (suffix) {
            case 'u':
                factor = 1.0 / (1000 * 1000);
                break;
            case 'm':
                factor = 1.0 / 1000;
                break;
            case 'K':
                factor = 1000;
                break;
            case 'M':
                factor = 1000 * 1000;
                break;
            case 'G':
                factor = 1000 * 1000 * 1000;
                break;
            default:
                THROW(InvalidStringFormat, "invalid format");
        }
        temp *= factor;
    } else {
        fromString(temp, myptr);
    }
    //  cout << setiosflags(ios::fixed) << "temp: " << temp << endl;
    return temp;
}

char* double2suffixedStr(const double n)
{
    /// WARNING: this function is not reentrant!!!!!
    static char str[256] = "";
    static char suffix[] = "um KMGTP";

    unsigned i = 0;
    double res;
    if (n < 1) {
        res = n * 1000 * 1000;
        do {
            if (res > 1) {
                break;
            }
            res *= 1000;
            i++;
        } while (1);
    } else {
        res = n;
        i = 2;
        do {
            if (res / 1000 < 1) {
                break;
            }
            res /= 1000;
            i++;
        } while (1);
    }
    if (i > strlen(suffix)) {
        THROW(InvalidStringFormat, "invalid format");
    }
    sprintf(str, "%.1f%c", res, suffix[i]);

    return str;
}

const string getNS(void)
{
    string distns(get_environment_variable(SystemStatics::get_STREAMS_NS_envVar(), ""));
    if (distns.size()) {
        return distns;
    } else {
        stringstream ss;
        THROW(Distillery, SystemStatics::get_STREAMS_NS_envVar()
                            << " is not set in the environment");
    }
}

const string getInstance(void)
{
    string distid = get_environment_variable(SystemStatics::get_STREAMS_INSTANCE_ID_envVar(), "");
    if (distid.size()) {
        return distid;
    } else {
        THROW(Distillery, SystemStatics::get_STREAMS_INSTANCE_ID_envVar()
                            << " is not set in the environment");
    }
}

string getUserName(void)
{
    uid_t uid = getuid();
    struct passwd pwd;
    struct passwd* result;
    long pwbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    char pwbuf[pwbufsize];
    int ret = getpwuid_r(uid, &pwd, pwbuf, pwbufsize, &result);
    if (ret != 0 || result == NULL) {
        THROW(Distillery, "failure to getpwuid_r for uid " << uid);
    }
    SPCDBG(L_DEBUG,
           "user id " << uid << " is '" << pwd.pw_name << "' and its login directory is '"
                      << pwd.pw_dir << "'",
           CORE_UTILS);
    return string(pwd.pw_name);
}

string getInstanceId(void)
{
    return getInstance();
}

string getDomainId(void)
{
    string domainid = get_environment_variable(SystemStatics::get_STREAMS_DOMAIN_ID_envVar(), "");
    if (domainid.size()) {
        return domainid;
    } else {
        THROW(Distillery, SystemStatics::get_STREAMS_DOMAIN_ID_envVar()
                            << " is not set in the environment");
    }
}

string getInstanceOwner(void)
{
    string instanceId = getInstanceId();
    vector<string> tokens;
    tokenize(instanceId, tokens, "@", false);
    if (tokens.size() != 2) {
        THROW(Distillery, "non-compliant instance identifier '" << instanceId << "'");
    }
    return tokens[1];
}

bool verifyInstanceOwner(void)
{
    string user = getUserName();
    string userName = getInstanceOwner();
    return (userName == user);
}

string getDefaultInstanceId(void)
{
    char* distdid = getenv(SystemStatics::get_STREAMS_DEFAULT_IID_envVar().c_str());
    if (distdid != NULL && *distdid != 0) {
        return distdid;
    } else {
        THROW(Distillery, SystemStatics::get_STREAMS_DEFAULT_IID_envVar()
                            << " is not set in the environment");
    }
}

string getSpadeDefaultInstanceShortId(void)
{
    return "spade";
}

string getSpadeDefaultInstanceId(void)
{
    string login = getpwuid(geteuid())->pw_name;
    return (getSpadeDefaultInstanceShortId() + "@" + login);
}

string getSPLDefaultInstanceShortId(void)
{
    return "spl";
}

string getSPLDefaultInstanceId(void)
{
    string login = getpwuid(geteuid())->pw_name;
    return (getSPLDefaultInstanceShortId() + "@" + login);
}

string getStdDefaultInstanceShortId(void)
{
    return "streams";
}

string getStdDefaultInstanceId(void)
{
    string login = getpwuid(geteuid())->pw_name;
    return (getStdDefaultInstanceShortId() + "@" + login);
}

string getInstallationDirectory(void)
{
    char* distInst = getenv(SystemStatics::get_STREAMS_INSTALL_envVar().c_str());
    if (distInst != NULL && *distInst != 0) {
        return string(distInst);
    } else {
        THROW(Distillery, SystemStatics::get_STREAMS_INSTALL_envVar()
                            << " is not set in the environment");
    }
}

string getInstallationOwner(void)
{

    string installDir = getInstallationDirectory();

    vector<string> cmd;
    cmd.push_back("/usr/bin/stat");
    cmd.push_back("-c");
    cmd.push_back("%U");
    cmd.push_back(installDir);

    ostringstream out;
    ostringstream err;

    int status = runCommand(cmd, "", out, err, 0);

    string outStr = out.str();

    SPCDBG(L_DEBUG,
           "installDir[" << installDir << "] installOwner[" << outStr << "] "
                         << "rc[" << status << "]",
           CORE_UTILS);
    return (outStr);
}

/// Obtain the installation owner home directory
/// @return the location
string getInstallationOwnerHomeDir(void)
{
    string installOwner = getInstallationOwner();
    ostringstream userHome;
    userHome << "~" << installOwner;
    vector<string> cmd;
    cmd.push_back("/bin/bash");
    cmd.push_back("-c");
    cmd.push_back("/bin/echo");
    cmd.push_back(userHome.str());

    ostringstream out;
    ostringstream err;

    int status = runCommand(cmd, "", out, err, 0);

    string outStr = out.str();
    SPCDBG(L_DEBUG,
           "installOwner[" << installOwner << "] installHomeDir[" << outStr << "]"
                           << "rc[" << status << "]",
           CORE_UTILS);
    return (outStr);
}

string getSysImplBinDirectory(void)
{
    string dir;
    dir.assign(getInstallationDirectory() + "/system/impl/bin");
    return dir;
}

/// \internal
/// Obtain the top-level configuration directory for Streams
/// @param createDirectory indicate whether the directory should be created
/// if it does not already exists
/// @return the top-level configuration directory for Streams
static string getTopLevelConfigurationDirectory(const bool createDirectory)
{
    // finding the name of the current executable
    stringstream fn;
    fn << "/proc/self/exe";
    SPCDBG(L_DEBUG, "opening file '" << fn.str() << "'", CORE_UTILS);
    size_t bufsize = 128;
    int ret;
    string execPath;
    do {
        char buf[bufsize];
        ret = readlink(fn.str().c_str(), buf, bufsize);
        if (ret < 0) {
            THROW(Distillery, "failure to read from '" << fn.str() << "'");
        }
        // in case we did not have enough space
        if (ret >= static_cast<int>(bufsize)) {
            bufsize *= 2;
            continue;
        }
        // ensuring NULL termination
        buf[ret] = '\0';
        execPath.assign(buf);
        break;
    } while (1);
    SPCDBG(L_DEBUG, "execution path is '" << execPath << "'", CORE_UTILS);

    // finding the owner of the current executable
    struct stat64 s;
    ret = stat64(execPath.c_str(), &s);
    if (ret < 0) {
        THROW(Distillery, "failure to stat64 '" << execPath << "'");
    }
    SPCDBG(L_DEBUG, "user id of the owner for '" << execPath << "' is " << s.st_uid, CORE_UTILS);
    struct passwd pwd;
    struct passwd* result;
    string conf;
    long pwbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    char pwbuf[pwbufsize];
    ret = getpwuid_r(s.st_uid, &pwd, pwbuf, pwbufsize, &result);
    if (ret != 0 || result == NULL) {
        SPCDBG(L_WARN,
               "failure to getpwuid_r for uid " << s.st_uid << ", defaulting to /tmp/streams",
               CORE_UTILS);
        conf = std::string("/tmp/streams");
    } else {
        SPCDBG(L_DEBUG,
               "user id " << s.st_uid << " is '" << pwd.pw_name << "' and its login directory is '"
                          << pwd.pw_dir << "'",
               CORE_UTILS);
        conf = std::string(pwd.pw_dir) + "/.streams";
    }

    // creating the directory, if asked and if the directory is not already there
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << conf << "'", CORE_UTILS);
        Directory::smkdir(conf, 0755);
        SPCDBG(L_DEBUG, "created directory '" << conf << "'", CORE_UTILS);
    }

    return conf;
}

/// \internal
/// Obtain the top-level configuration directory for an instance
/// @param instanceid the instance identifier
/// @param createDirectory indicate whether the directory should be created
/// if it does not already exists
/// @return the top-level instance configuration directory
static string getInstanceTopLevelConfigurationDirectory(const string& instanceid,
                                                        const bool createDirectory)
{
    SPCDBG(L_DEBUG, "retrieving instance configuration for instance '" << instanceid, CORE_UTILS);
    // finding the name of the user that owns this instance
    vector<string> tokens;
    tokenize(instanceid, tokens, "@", false);
    if (tokens.size() != 2) {
        THROW(Distillery, "non-compliant instance identifier '" << instanceid << "'");
    }
    string& userName = tokens[1];

    // finding the home directory for the user designated by userName
    SPCDBG(L_DEBUG, "user name is '" << userName << "'", CORE_UTILS);
    struct passwd pwd;
    struct passwd* result;
    long pwbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    char pwbuf[pwbufsize];
    int ret = getpwnam_r(userName.c_str(), &pwd, pwbuf, pwbufsize, &result);
    if (ret != 0 || result == NULL) {
        THROW(Distillery, "failure to getpwuid_r for user '" << userName << "'");
    }
    SPCDBG(L_DEBUG,
           "user name '" << userName << "' is " << pwd.pw_uid << " and its login directory is '"
                         << pwd.pw_dir << "'",
           CORE_UTILS);
    string conf(pwd.pw_dir);
    conf += "/.streams";

    // creating the directory, if asked and if the directory is not already there
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << conf << "'", CORE_UTILS);
        Directory::smkdir(conf, 0755);
        SPCDBG(L_DEBUG, "created directory '" << conf << "'", CORE_UTILS);
    }

    return conf;
}

/// \internal
/// Obtain the top-level user configuration directory
/// @param createDirectory indicate whether the directory should be created
/// if it does not already exists
/// @return the top-level user configuration directory
static string getUserTopLevelConfigurationDirectory(const bool createDirectory)
{
    // obtaining the uid for the user running this process
    uid_t uid = getuid();
    // finding the home directory for the user designated by uid
    SPCDBG(L_DEBUG, "user id is " << uid, CORE_UTILS);
    struct passwd pwd;
    struct passwd* result;
    long pwbufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
    char pwbuf[pwbufsize];
    int ret = getpwuid_r(uid, &pwd, pwbuf, pwbufsize, &result);
    if (ret != 0 || result == NULL) {
        THROW(Distillery, "failure to getpwuid_r for uid " << uid);
    }
    SPCDBG(L_DEBUG,
           "user id " << uid << " is '" << pwd.pw_name << "' and its login directory is '"
                      << pwd.pw_dir << "'",
           CORE_UTILS);
    string conf(pwd.pw_dir);
    conf += "/.streams";

    // creating the directory, if asked and if the directory is not already there
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << conf << "'", CORE_UTILS);
        Directory::smkdir(conf, 0755);
        SPCDBG(L_DEBUG, "created directory '" << conf << "'", CORE_UTILS);
    }

    return conf;
}

string getUserKeyConfigurationDirectory(bool createDirectory)
{
    string dir(getUserTopLevelConfigurationDirectory(false) + "/key");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0600);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getUserLogsDirectory(bool createDirectory)
{
    string dir(getUserTopLevelConfigurationDirectory(false) + "/logs");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceBinDirectory(const string& instanceid, bool createDirectory)
{
    if (instanceid.empty()) {
        THROW(Distillery, "instance id must be specified");
    }
    string dir(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/instances/" +
               instanceid + "/bin");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceOwnerBinDirectory(const string& instanceid, bool createDirectory)
{
    if (instanceid.empty()) {
        THROW(Distillery, "instance id must be specified");
    }
    string dir;
    dir.assign(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/bin");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getBinDirectory(bool createDirectory)
{
    string dir;
    dir.assign(getTopLevelConfigurationDirectory(false) + "/bin");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getLicenseDirectory(bool createDirectory)
{
    string dir;
    dir.assign(getTopLevelConfigurationDirectory(false) + "/license");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceTemplateConfigurationDirectory(const string& instanceid, bool createDirectory)
{
    string dir;
    if (instanceid.empty()) {
        dir.assign(getTopLevelConfigurationDirectory(false) + "/config");
    } else {
        dir.assign(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/config");
    }
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceConfigurationDirectory(const string& instanceid, bool createDirectory)
{
    if (instanceid.empty()) {
        THROW(Distillery, "instance id must be specified");
    }
    string dir(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/instances/" +
               instanceid + "/config");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceNameServiceConfigurationDirectory(const string& instanceid, bool createDirectory)
{
    if (instanceid.empty()) {
        THROW(Distillery, "instance id must be specified");
    }
    string dir(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/instances/" +
               instanceid + "/nameservice");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

string getInstanceSWSConfigurationDirectory(const string& instanceid, bool createDirectory)
{
    if (instanceid.empty()) {
        THROW(Distillery, "instance id must be specified");
    }
    string dir(getInstanceTopLevelConfigurationDirectory(instanceid, false) + "/sws");
    if (createDirectory) {
        SPCDBG(L_DEBUG, "creating directory '" << dir << "'", CORE_UTILS);
        Directory::smkdir(dir, 0755);
        SPCDBG(L_DEBUG, "created directory '" << dir << "'", CORE_UTILS);
    }
    return dir;
}

void computeMD5Digest(const void* in, size_t inLen, SBuffer& out)
{
    EVP_MD_CTX mdctx;
    const EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("md5");
    assert(md);
    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, in, inLen);
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    out.setICursor(0);
    out.addBlob(md_value, md_len);
}

string computeMD5Digest(istream& strm)
{
    EVP_MD_CTX mdctx;
    const EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("md5");
    assert(md);
    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    while (strm) {
        char c;
        strm.get(c);
        if (!strm) {
            break;
        }
        EVP_DigestUpdate(&mdctx, &c, 1);
    }
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);

    string res;
    for (unsigned int i = 0; i < md_len; i++) {
        int v = (md_value[i] >> 4) & 0xF;
        switch (v) {
            case 0 ... 9:
                res += '0' + v;
                break;
            case 10 ... 15:
                res += 'a' + (v - 10);
                break;
            default:
                ::abort();
        }

        v = md_value[i] & 0xF;
        switch (v) {
            case 0 ... 9:
                res += '0' + v;
                break;
            case 10 ... 15:
                res += 'a' + (v - 10);
                break;
            default:
                ::abort();
        }
    }
    return res;
}

void computeSHA1Digest(const SBuffer& in, SBuffer& out)
{
    EVP_MD_CTX mdctx;
    const EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("sha1");
    assert(md);
    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, in.getCharPtr(), in.getICursor());
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    out.setICursor(0);
    out.addBlob(md_value, md_len);
}

void computeBase64SHA1Digest(const char* buf, const unsigned bufsize, string& out)
{
    EVP_MD_CTX mdctx;
    const EVP_MD* md;
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;

    OpenSSL_add_all_digests();
    md = EVP_get_digestbyname("sha1");
    assert(md);
    EVP_MD_CTX_init(&mdctx);
    EVP_DigestInit_ex(&mdctx, md, NULL);
    EVP_DigestUpdate(&mdctx, buf, bufsize);
    EVP_DigestFinal_ex(&mdctx, md_value, &md_len);
    EVP_MD_CTX_cleanup(&mdctx);
    Base64::encode(reinterpret_cast<const char*>(md_value), md_len, out);
}

unsigned long copyStream(istream& istrm, ostream& ostrm)
{
    unsigned long count = 0;

    while (istrm) {
        char c;
        istrm.get(c);
        if (!istrm) {
            break;
        }
        ostrm << c;
        count++;
    }

    return count;
}

string obtainExecutableFullPath(const string& exe)
{
    ostringstream command;
    command << "sh -c \"type -p " << exe << "\" 2>&1";
    FILE* f = popen(command.str().c_str(), "r");
    if (!f) {
        THROW(InvalidPopen, "popen: " << ErrnoFormat());
    }
    char strbuf[1024];
    ostringstream response;
    while (!feof(f)) {
        char* res = fgets(strbuf, 1024, f);
        if (res == NULL) {
            THROW(Distillery, "fgets failed: " << ErrnoFormat());
        }
        size_t len = strlen(strbuf);
        if (strbuf[len - 1] == '\n') {
            strbuf[len - 1] = '\0';
            response << strbuf;
            break;
        } else {
            response << strbuf;
        }
    }
    pclose(f);
    return response.str();
}

bool isUnprintable(const int c)
{
    return !isprint(static_cast<int>(c));
}

char replaceWithSpace(const char c)
{
    if (!isprint(static_cast<int>(c))) {
        return ' ';
    }
    return c;
}

string removeUnprintableChars(const string& str)
{
    string sanitized(str);
    sanitized.erase(remove_if(sanitized.begin(), sanitized.end(), isUnprintable), sanitized.end());
    return sanitized;
}

string removeEscapingChars(const string& str)
{
    string sanitized;
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == '\\' && (i + 1) < str.size() &&
            (str[i + 1] == '\\' || str[i + 1] == ' ' || str[i + 1] == '=' || str[i + 1] == ':')) {
            // skip this '\' as it is escaping char.
            ++i;
        }
        sanitized += str[i];
    }
    return sanitized;
}

string removeCharIfPresent(const string& str, char c)
{
    string sanitized(str);
    sanitized.erase(remove_if(sanitized.begin(), sanitized.end(), bind2nd(equal_to<char>(), c)),
                    sanitized.end());
    return sanitized;
}

string replaceUnprintableCharsWithSpace(const string& str)
{
    string sanitized(str);
    transform(sanitized.begin(), sanitized.end(), sanitized.begin(), replaceWithSpace);
    return sanitized;
}

string xmlSanitize(const string& str)
{
    string sanitized;
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == '&') {
            sanitized += "&amp;";
        } else if (str[i] == '"') {
            sanitized += "&quot;";
        } else if (str[i] == '<') {
            sanitized += "&lt;";
        } else if (str[i] == '>') {
            sanitized += "&gt;";
        } else if (str[i] == '~') {
            // hcma: I have seen at least one case where &tilde; is not working!
            //      sanitized+="&tilde;";
            sanitized += "&#126;";
        }
        // bgedik: ^ is not an xml special character. I'm replacing
        //      &circ; (which is only valid in html), with &#94;
        //      we do not need this else case but it was already here
        else if (str[i] == '^') {
            //      sanitized+="&circ;";
            sanitized += "&#94;";
        } else {
            sanitized += str[i];
        }
    }
    return sanitized;
}

static void csvtokenizefull(const string& str, vector<string>& tokens)
{
    tokens.clear();
    if (str.empty()) {
        tokens.push_back("");
        return;
    }

    bool quote = false;
    string::const_iterator begs = str.begin();
    string::const_iterator spos = begs;
    string::const_iterator epos = begs;
    string::const_iterator ends = str.end();
    for (;;) {
        if (epos == ends) {
            if (quote) {
                THROW(InvalidStringFormat,
                      "csv string ('" << str
                                      << "') ended while searching for terminating quote (\")");
            }
            switch (*(epos - 1)) {
                case ',':
                    tokens.push_back("");
                    return;
                default:
                    tokens.push_back(str.substr(spos - begs, epos - spos));
                    return;
            }
        } else if (spos == epos) { // start of a token
            switch (*epos) {
                case ',':
                    tokens.push_back("");
                    spos = ++epos;
                    break;
                case '"':
                    quote = true;
                    ++epos;
                    break;
                default:
                    ++epos;
                    break;
            }
        } else if (!quote) { // continued regular token
            if (*epos == ',') {
                tokens.push_back(str.substr(spos - begs, epos - spos));
                spos = ++epos;
            } else {
                ++epos;
            }
        } else { // continued quoted token
            if (*epos == '"' && *(epos - 1) != '\\') {
                ++epos;
                quote = false;
                tokens.push_back(str.substr(spos - begs, epos - spos));
                if (epos != ends) {
                    if (*epos != ',') {
                        THROW(InvalidStringFormat,
                              "csv string ('"
                                << str
                                << "') has incorrect format: token ending quote (\") followed by a "
                                   "character other that comma (,) at index "
                                << (epos - begs));
                    }
                    spos = ++epos;
                } else {
                    return;
                }
            } else {
                ++epos;
            }
        }
    }
}

void csvtokenize(const string& str, vector<string>& tokens, const bool quoted /*=false*/)
{
    if (quoted) {
        csvtokenizefull(str, tokens);
    } else {
        tokenize(str, tokens, ",", true);
    }
}

void tokenize(const string& str,
              vector<string>& tokens,
              const string& delimiters,
              bool keepEmptyTokens)
{
    // Cleanup
    tokens.clear();

    string::size_type lastPos = 0;

    // Find first delimiter
    string::size_type pos = str.find_first_of(delimiters, lastPos);
    while (pos != string::npos) {
        // Found a token, add it to the vector
        if (pos != lastPos) {
            tokens.push_back(str.substr(lastPos, pos - lastPos));
            lastPos = pos + 1;
        } else {
            if (keepEmptyTokens) {
                tokens.push_back("");
            }
            lastPos++;
        }
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, lastPos);
    }
    if (lastPos != str.size()) {
        tokens.push_back(str.substr(lastPos));
    } else {
        if (keepEmptyTokens) {
            tokens.push_back("");
        }
    }
}

string time2str(time_t t)
{
    char time_buffer[1024];
    char* time_buffer_ptr = time_buffer;
    time_buffer_ptr = ctime_r(&t, time_buffer_ptr);
    time_buffer_ptr[strlen(time_buffer_ptr) - 1] = 0; // trim the newline

    return string(time_buffer);
}

UnicodeString time2str_global(time_t t)
{

    // set time_t into a double so we don't wrap when doing the conversion to
    // milliseconds.  Also put the constant for 1 miullisecond in a double.
    double tempTime = t;
    double tempOneMillisecond = ONE_MILLISECOND;

    UDate timeToConvert = (tempTime * tempOneMillisecond);

    auto_ptr<icu::DateFormat> dfmt(
      icu::DateFormat::createDateTimeInstance(icu::DateFormat::FULL, icu::DateFormat::MEDIUM));

    if (dfmt.get() == NULL) {
        THROW(DateTimeFormatError, "Date or timestamp formatting error", UTILSDateTimeFormatError);
    };

    UnicodeString cvtString;
    dfmt->format(timeToConvert, cvtString);

    return cvtString;
}

icu::SimpleDateFormat* iso8601DateTimeFormatter()
{
    const UnicodeString& pattern("yyyy-MM-dd'T'HH:mm:ssZ"); // e.g., "2010-03-16T13:41:53-05:00"
    UErrorCode status(U_ZERO_ERROR);
    auto_ptr<icu::SimpleDateFormat> sdf(new icu::SimpleDateFormat(pattern, status));
    if (U_FAILURE(status)) {
        THROW(DateTimeFormatError, "Unable to create date/time formatter - error: " << status,
              UTILSCantCreateDateTimeFormatter, toString(pattern).c_str(),
              toString(status).c_str());
    }

    return sdf.release();
}

UnicodeString time2str_fmt(time_t t, icu::SimpleDateFormat& fmt)
{
    // set time_t into a double so we don't wrap when doing the conversion to
    // milliseconds.  Also put the constant for 1 miullisecond in a double.
    double tempTime = t;
    double tempOneMillisecond = ONE_MILLISECOND;

    UDate timeToConvert = (tempTime * tempOneMillisecond);

    UnicodeString cvtString;

    return fmt.format(timeToConvert, cvtString);
}

int sleep_random(int fix, int variable)
{
    unsigned long duration = fix;

    duration += (rand() % (variable * 2)) - variable;

    // printf("%ld\n", duration*1000) ;

    return usleep(duration * 1000);
}

string get_environment_variable(const string& name, const string& defaultval)
{
    char* nsstr = getenv(name.c_str());
    if (nsstr) {
        SPCDBG(L_TRACE, "get_environment_variable(" << QT(name) << ") returns: " << nsstr,
               CORE_UTILS);
        return nsstr;
    } else {
        SPCDBG(L_TRACE,
               "get_environment_variable(" << QT(name) << ") returns default: " << defaultval,
               CORE_UTILS);
        return defaultval;
    }
}

int get_environment_variable(const string& name, int defaultval)
{
    char* nsstr = getenv(name.c_str());
    if (nsstr) {
        char* endptr = NULL;
        int res = strtol(nsstr, &endptr, 0);
        if ((endptr && *endptr) || endptr == nsstr) {
            SPCDBG(L_DEBUG,
                   "get_environment_variable(" << QT(name) << ") returns default: " << defaultval,
                   CORE_UTILS);
            return defaultval;
        }

        SPCDBG(L_DEBUG, "get_environment_variable(" << QT(name) << ") returns: " << res,
               CORE_UTILS);
        return res;
    } else {
        SPCDBG(L_DEBUG,
               "get_environment_variable(" << QT(name) << ") returns default: " << defaultval,
               CORE_UTILS);
        return defaultval;
    }
}

double get_environment_variable(const string& name, double defaultval)
{
    char* nsstr = getenv(name.c_str());
    if (nsstr) {
        char* endptr = NULL;
        double res = strtod(nsstr, &endptr);
        if ((endptr && *endptr) || endptr == nsstr) {
            SPCDBG(L_DEBUG,
                   "get_environment_variable(" << QT(name) << ") returns default: " << defaultval,
                   CORE_UTILS);
            return defaultval;
        }

        SPCDBG(L_DEBUG, "get_environment_variable(" << QT(name) << ") returns: " << res,
               CORE_UTILS);
        return res;
    } else {
        SPCDBG(L_DEBUG,
               "get_environment_variable(" << QT(name) << ") returns default: " << defaultval,
               CORE_UTILS);
        return defaultval;
    }
}

int monthstr2num(const char* monthstr)
{
    if (!(monthstr && strlen(monthstr) >= 3)) {
        THROW(Distillery, "invalid format");
    }
    switch (toupper(monthstr[0])) {
        case 'J':
            if (toupper(monthstr[2]) == 'L') {
                return 6;
            }
            if (toupper(monthstr[1]) == 'A') {
                return 0;
            }
            return 5;
        case 'F':
            return 1;
        case 'M':
            if (toupper(monthstr[2]) == 'R') {
                return 2;
            }
            return 4;
        case 'A':
            if (toupper(monthstr[1]) == 'P') {
                return 3;
            }
            return 7;
        case 'S':
            return 8;
        case 'O':
            return 9;
        case 'N':
            return 10;
        case 'D':
            return 11;
    }
    THROW(Distillery, "invalid month string '" << monthstr << "'");
    return 0;
}

uint64_t timeStampToMicroseconds(const char* dmy, const char* hmsmilli, const char* tmzone)
{
    // cout << "dmy: " << dmy << " hmsmilli: " << hmsmilli << endl;
    if (!(dmy && strlen(dmy) == 11)) {
        if (dmy) {
            THROW(Distillery,
                  "invalid format expecting something like '04-OCT-2005' got '" << dmy << "'");
        } else {
            THROW(Distillery, "invalid format expecting something like '04-OCT-2005'");
        }
    }
    if (!(hmsmilli && strlen(hmsmilli) == 12)) {
        if (hmsmilli) {
            THROW(Distillery, "invalid format expecting something like '14:18:19.324' got '"
                                << hmsmilli << "'");
        } else {
            THROW(Distillery, "invalid format expecting something like '14:18:19.324'");
        }
    }
    char day[3], monstr[4], year[5];
    strncpy(day, dmy, 2);
    day[2] = '\0';
    strncpy(monstr, dmy + 3, 3);
    monstr[3] = '\0';
    strncpy(year, dmy + 7, 4);
    year[4] = '\0';
    char hour[3], min[3], sec[3], milli[4];
    strncpy(hour, hmsmilli, 2);
    hour[2] = '\0';
    strncpy(min, hmsmilli + 3, 2);
    min[2] = '\0';
    strncpy(sec, hmsmilli + 6, 2);
    sec[2] = '\0';
    strncpy(milli, hmsmilli + 9, 3);
    milli[3] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = monthstr2num(monstr);
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000 + atoi(milli) * 1000;
}

uint64_t YYYYMMDDhhmmss_ToMicroseconds(const char* ymdhms, const char* tmzone)
{
    if (!(ymdhms && strlen(ymdhms) == 14)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(year, ymdhms, 4);
    year[4] = '\0';
    strncpy(month, ymdhms + 4, 2);
    month[2] = '\0';
    strncpy(day, ymdhms + 6, 2);
    day[2] = '\0';
    char hour[3], min[3], sec[3];
    strncpy(hour, ymdhms + 8, 2);
    hour[2] = '\0';
    strncpy(min, ymdhms + 10, 2);
    min[2] = '\0';
    strncpy(sec, ymdhms + 12, 2);
    sec[2] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000;
}

uint64_t YYYY_MM_DD_hh_mm_ssToMicroseconds(const char* ymdhms, const char* tmzone)
{
    if (!(ymdhms && strlen(ymdhms) == 19)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(year, ymdhms, 4);
    year[4] = '\0';
    strncpy(month, ymdhms + 5, 2);
    month[2] = '\0';
    strncpy(day, ymdhms + 8, 2);
    day[2] = '\0';
    char hour[3], min[3], sec[3];
    strncpy(hour, ymdhms + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhms + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhms + 17, 2);
    sec[2] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000;
}

uint64_t MM_DD_YYYY_hh_mm_ssToMicroseconds(const char* ymdhms, const char* tmzone)
{
    if (!(ymdhms && strlen(ymdhms) == 19)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(month, ymdhms, 2);
    month[2] = '\0';
    strncpy(day, ymdhms + 3, 2);
    day[2] = '\0';
    strncpy(year, ymdhms + 6, 4);
    year[4] = '\0';
    char hour[3], min[3], sec[3];
    strncpy(hour, ymdhms + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhms + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhms + 17, 2);
    sec[2] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000;
}

uint64_t DD_MM_YYYY_hh_mm_ssToMicroseconds(const char* ymdhms, const char* tmzone)
{
    if (!(ymdhms && strlen(ymdhms) == 19)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(day, ymdhms, 2);
    day[2] = '\0';
    strncpy(month, ymdhms + 3, 2);
    month[2] = '\0';
    strncpy(year, ymdhms + 6, 4);
    year[4] = '\0';
    char hour[3], min[3], sec[3];
    strncpy(hour, ymdhms + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhms + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhms + 17, 2);
    sec[2] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000;
}

uint64_t YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds(const char* ymdhmsmmm, const char* tmzone)
{
    if (!(ymdhmsmmm && strlen(ymdhmsmmm) == 23)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(year, ymdhmsmmm, 4);
    year[4] = '\0';
    strncpy(month, ymdhmsmmm + 5, 2);
    month[2] = '\0';
    strncpy(day, ymdhmsmmm + 8, 2);
    day[2] = '\0';
    char hour[3], min[3], sec[3], ms[4];
    strncpy(hour, ymdhmsmmm + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhmsmmm + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhmsmmm + 17, 2);
    sec[2] = '\0';
    strncpy(ms, ymdhmsmmm + 20, 3);
    ms[3] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000 + atoi(ms) * 1000;
}

uint64_t MM_DD_YYYY_hh_mm_ss_mmmToMicroseconds(const char* ymdhmsmmm, const char* tmzone)
{
    if (!(ymdhmsmmm && strlen(ymdhmsmmm) == 23)) {
        THROW(Distillery, "invalid format");
    }
    char day[3], month[3], year[5];
    strncpy(month, ymdhmsmmm, 2);
    month[2] = '\0';
    strncpy(day, ymdhmsmmm + 3, 2);
    day[2] = '\0';
    strncpy(year, ymdhmsmmm + 6, 4);
    year[4] = '\0';
    char hour[3], min[3], sec[3], ms[4];
    strncpy(hour, ymdhmsmmm + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhmsmmm + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhmsmmm + 17, 2);
    sec[2] = '\0';
    strncpy(ms, ymdhmsmmm + 20, 3);
    ms[3] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000 + atoi(ms) * 1000;
}

uint64_t DD_MM_YYYY_hh_mm_ss_mmmToMicroseconds(const char* ymdhmsmmm, const char* tmzone)
{
    assert(ymdhmsmmm && strlen(ymdhmsmmm) == 23);
    char day[3], month[3], year[5];
    strncpy(day, ymdhmsmmm, 2);
    day[2] = '\0';
    strncpy(month, ymdhmsmmm + 3, 2);
    month[2] = '\0';
    strncpy(year, ymdhmsmmm + 6, 4);
    year[4] = '\0';
    char hour[3], min[3], sec[3], ms[4];
    strncpy(hour, ymdhmsmmm + 11, 2);
    hour[2] = '\0';
    strncpy(min, ymdhmsmmm + 14, 2);
    min[2] = '\0';
    strncpy(sec, ymdhmsmmm + 17, 2);
    sec[2] = '\0';
    strncpy(ms, ymdhmsmmm + 20, 3);
    ms[3] = '\0';
    struct tm mytm;
    mytm.tm_year = atoi(year) - 1900;
    mytm.tm_mon = atoi(month) - 1;
    mytm.tm_mday = atoi(day);
    mytm.tm_hour = atoi(hour);
    mytm.tm_min = atoi(min);
    mytm.tm_sec = atoi(sec);
    mytm.tm_isdst = -1;
    mytm.tm_gmtoff = 0;
    mytm.tm_zone = tmzone;
    time_t t = get_mktime(&mytm);
    assert(t != static_cast<time_t>(-1));
    return static_cast<uint64_t>(t) * 1000 * 1000 + atoi(ms) * 1000;
}

uint32_t dosgetline(istream& input, char* buffer, uint32_t maxbuflen)
{
    uint32_t i = 0;
    bool got_cr = false;
    while (1) {
        if (i == maxbuflen) {
            THROW(Distillery, "buffer was not large enough");
        }
        char ch = input.get();
        if (input.eof()) {
            break;
        }
        if (ch == '\r') {
            if (got_cr) {
                buffer[i++] = '\r';
            } else {
                got_cr = true;
                ;
            }
        } else if (ch == '\n') {
            if (got_cr) {
                // found CR+LF
                break;
            } else {
                buffer[i++] = ch;
                got_cr = false;
            }
        } else {
            if (got_cr) {
                buffer[i++] = '\r';
                buffer[i++] = ch;
            } else {
                buffer[i++] = ch;
            }
            got_cr = false;
        }
    }
    return i;
}

unsigned getNCurrentCandidateCPUs(pid_t pid)
{
    unsigned ncpus = 0;
    cpu_set_t aff;
    ERRNO_EX(sched_getaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_getaffinity");
    for (unsigned i = 0; i < sizeof(cpu_set_t); ++i) {
        if (CPU_ISSET(i, &aff)) {
            ncpus++;
        }
    }
    return ncpus;
}

std::vector<int32_t> getCurrentCandidateCPUs(pid_t pid)
{
    std::vector<int32_t> cpus;
    cpu_set_t aff;
    ERRNO_EX(sched_getaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_getaffinity");
    for (unsigned i = 0; i < sizeof(cpu_set_t); ++i) {
        if (CPU_ISSET(i, &aff)) {
            cpus.push_back(i);
        }
    }
    return cpus;
}

void setCPU(const int32_t cpu, pid_t pid)
{
    cpu_set_t aff;
    CPU_ZERO(&aff);
    CPU_SET(cpu, &aff);
    ERRNO_EX(sched_setaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_setaffinity");
}

void addCPU(const int32_t cpu, pid_t pid)
{
    cpu_set_t aff;
    ERRNO_EX(sched_getaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_getaffinity");
    CPU_SET(cpu, &aff);
    ERRNO_EX(sched_setaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_setaffinity");
}

void useAllAvailableCPUs(pid_t pid)
{
    static unsigned nCPUs = SystemStatics::getNCPUs();
    cpu_set_t aff;
    CPU_ZERO(&aff);
    for (unsigned i = 0; i < nCPUs; ++i) {
        CPU_SET(i, &aff);
    }
    ERRNO_EX(sched_setaffinity(pid, sizeof(cpu_set_t), &aff), FailedSystemCall,
             "failed sched_setaffinity");
}

void replaceAllPatternOccurrences(string& str, const string& pattern, const string& replacement)
{
    do {
        size_t found = str.find(pattern);
        if (found == string::npos) {
            break;
        }
        str.replace(found, pattern.size(), replacement);
        // cout << "--> " << found << " " << pattern.size() << " " << str << endl;
    } while (1);
}

void setFDFlags(int fd, int flag)
{
    int flags;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        THROW(FailedSystemCall, "fcntl failed: " << ErrnoFormat());
    }

    flags |= flag;

    if (fcntl(fd, F_SETFD, flags) == -1) {
        THROW(FailedSystemCall, "fcntl failed: " << ErrnoFormat());
    }
}

void unsetFDFlags(int fd, int flag)
{
    int flags;

    flags = fcntl(fd, F_GETFD);
    if (flags == -1) {
        THROW(FailedSystemCall, "fcntl failed: " << ErrnoFormat());
    }

    flags &= ~flag;

    if (fcntl(fd, F_SETFD, flags) == -1) {
        THROW(FailedSystemCall, "fcntl failed: " << ErrnoFormat());
    }
}

void setCloseOnExecFlags(int fd)
{
    setFDFlags(fd, FD_CLOEXEC);
}

void flush_pipe(ostream& strm, int fd)
{
    char data[1024 + 1];
    ssize_t res = 0;
    do {
        res = read(fd, data, 1024);
        if (res > 0) {
            strm.write(data, res);
        }
    } while (res > 0);
}

void logAdditionalExceptionMsg(int l_level, const MessageElementType& msg)
{
    vector<string> vec(msg.substitutionText().begin(), msg.substitutionText().end());
    SPCLOG(l_level, msg.messageCode(), vec, CORE_UTILS);
}

void logAdditionalExceptionData(int l_level, const additionalExceptionDataType& extraData)
{

    // Within a node there is a primary message that scopes
    // all of the other "secondary" and "nested" messages nodes
    // (and the containing exception's msg scopes them all).
    //
    // Render nested msgs in preorder:
    // - the node's primary msg (if any)
    // - the node's secondary msgs (if any)
    // - the node's children node's (if any) recursively

    // primary msg
    if (extraData.primaryInformation().messageCode().length() > 0) {
        logAdditionalExceptionMsg(l_level, extraData.primaryInformation());
    }

    // secondary msgs
    for (additionalExceptionDataType::secondaryInformation_const_iterator sit =
           extraData.secondaryInformation().begin();
         sit != extraData.secondaryInformation().end(); ++sit) {
        logAdditionalExceptionMsg(l_level, *sit);
    }

    // children nodes
    for (additionalExceptionDataType::nestedAdditionalExceptionData_const_iterator nit =
           extraData.nestedAdditionalExceptionData().begin();
         nit != extraData.nestedAdditionalExceptionData().end(); ++nit) {
        logAdditionalExceptionData(l_level, *nit);
    }
}

void traceAdditionalExceptionMsg(int l_level, const MessageElementType& msg)
{
    stringstream substText;
    for (exceptionType::substitutionText_const_iterator substTextIter =
           msg.substitutionText().begin();
         substTextIter != msg.substitutionText().end(); ++substTextIter) {
        substText << *substTextIter << "; ";
    }
    SPCDBG(l_level,
           msg.messageCode() << ": "
                             << "substText[" << substText.str() << "]",
           CORE_UTILS);
}

void traceAdditionalExceptionData(int l_level, const additionalExceptionDataType& extraData)
{

    // Within a node there is a primary message that scopes
    // all of the other "secondary" and "nested" messages nodes
    // (and the containing exception's msg scopes them all).
    //
    // Render nested msgs in preorder:
    // - the node's primary msg (if any)
    // - the node's secondary msgs (if any)
    // - the node's children node's (if any) recursively

    // primary msg
    if (extraData.primaryInformation().messageCode().length() > 0) {
        traceAdditionalExceptionMsg(l_level, extraData.primaryInformation());
    }

    // secondary msgs
    for (additionalExceptionDataType::secondaryInformation_const_iterator sit =
           extraData.secondaryInformation().begin();
         sit != extraData.secondaryInformation().end(); ++sit) {
        traceAdditionalExceptionMsg(l_level, *sit);
    }

    // children nodes
    for (additionalExceptionDataType::nestedAdditionalExceptionData_const_iterator nit =
           extraData.nestedAdditionalExceptionData().begin();
         nit != extraData.nestedAdditionalExceptionData().end(); ++nit) {
        traceAdditionalExceptionData(l_level, *nit);
    }
}

int runCommand(const vector<string>& args,
               const std::string& in,
               std::ostream& out,
               std::ostream& err,
               unsigned int timeout,
               bool closeOnExecFDs,
               int limitCommandTrace)
{
    stringstream argss;
    argss << debug::join(args.begin(), args.end(), ",");
    string argsDebug = limitCommandTrace ? argss.str().substr(0, limitCommandTrace) : argss.str();
    SPCDBG(L_DEBUG, "Executing command: {" << argsDebug << "} with " << timeout << "sec timeout",
           CORE_UTILS);

    if (args.size() < 1) {
        THROW(InvalidArgument, "The size of the args vector must be 1 or higher");
    }

    if (in.length() > 256) // totally arbitrary -- must remain that way
                           // until I add the code to stream stdin
                           // while the app is running, otherwise there
                           // will be a livelock
    {
        THROW(InvalidArgument, "The size of the 'in' value must be very small");
    }

    int pipe_out[2];
    if (pipe(pipe_out) != 0) {
        THROW(FailedSystemCall, "pipe(out) failed: " << ErrnoFormat());
    }

    // set FD_CLOEXEC flag for all parent FDs except stdout/stdin/stderr
    if (closeOnExecFDs) {
        for (int fd = 0; fd < (int)(sizeof(fd_set) * 8); fd++) {
            if (fd != stdout->_fileno && fd != stderr->_fileno && fd != stdin->_fileno) {
                fcntl(fd, F_SETFD, FD_CLOEXEC);
            }
        }
    }

    RAIIFct<int> close_pipe_out_0(pipe_out[0], close);
    RAIIFct<int> close_pipe_out_1(pipe_out[1], close);

    setCloseOnExecFlags(pipe_out[1]);

    int pipe_err[2];
    if (pipe(pipe_err) != 0) {
        THROW(FailedSystemCall, "pipe(err) failed: " << ErrnoFormat());
    }

    RAIIFct<int> close_pipe_err_0(pipe_err[0], close);
    RAIIFct<int> close_pipe_err_1(pipe_err[1], close);

    setCloseOnExecFlags(pipe_err[1]);

    int pipe_in[2];
    if (pipe(pipe_in) != 0) {
        THROW(FailedSystemCall, "pipe(in) failed: " << ErrnoFormat());
    }

    RAIIFct<int> close_pipe_in_0(pipe_in[0], close);
    RAIIFct<int> close_pipe_in_1(pipe_in[1], close);

    setCloseOnExecFlags(pipe_in[0]);

    if (pipe_in[0] < 0 || pipe_in[1] < 0 || pipe_out[0] < 0 || pipe_out[1] < 0 || pipe_err[0] < 0 ||
        pipe_err[1] < 0) {
        THROW(FailedSystemCall,
              "Unexpected error: pipe() succeed but at least one value is negative");
    }

    pid_t childpid = fork();

    if (childpid == -1) {
        THROW(FailedSystemCall, "fork() failed: " << ErrnoFormat());
    }

    /*

             Pipe layout:


                  [1] -------- {in } --------> [0]
      +--------+                                    +---------+
      | parent |  [0] <------- {out} --------- [1]  |  child  |
      +--------+                                    +---------+
                  [0] <------- {err} --------- [1]
     */

    if (childpid == 0) {
        // in the child

#define CRY(stmt)                                                                                  \
    do {                                                                                           \
        stringstream strm3eeb7ad3306cdc1;                                                          \
        strm3eeb7ad3306cdc1 << stmt << endl;                                                       \
        write(2, strm3eeb7ad3306cdc1.str().c_str(), strm3eeb7ad3306cdc1.str().length());           \
    } while (0)

        close_pipe_in_1.now();  // Read side of pipe not used by child
        close_pipe_out_0.now(); // Write side of pipe not used by child
        close_pipe_err_0.now(); // Write side of pipe not used by child

        // map pipe_in[0] to stdin
        if (dup2(pipe_in[0], 0) != 0) {
            CRY("Unable to set stdin (dup2 failed): " << ErrnoFormat());
            _exit(1);
        }

        // map pipe_out[1] to stdout
        if (dup2(pipe_out[1], 1) != 1) {
            CRY("Unable to set stdout (dup2 failed): " << ErrnoFormat());
            _exit(1);
        }

        // map pipe_err[1] to stderr
        if (dup2(pipe_err[1], 2) != 2) {
            CRY("Unable to set stderr (dup2 failed): " << ErrnoFormat());
            _exit(1);
        }

        char** args_c = new char*[args.size() + 1];
        for (unsigned int i = 0; i < args.size(); i++) {
            args_c[i] = strdup(args[i].c_str());
        }
        args_c[args.size()] = NULL;

        // execute command
        execvp(args_c[0], args_c);

        // We can only get here if execvp failed
        CRY("execvp() failed: " << ErrnoFormat());
        _exit(1);
    } else {
        // in the parent

#define KILL_CHILD                                                                                 \
    do {                                                                                           \
        close_pipe_in_1.now();                                                                     \
        close_pipe_out_0.now();                                                                    \
        close_pipe_err_0.now();                                                                    \
        kill(childpid, SIGKILL);                                                                   \
        waitpid(-1, NULL, WNOHANG);                                                                \
    } while (0)

        SPCDBG(L_DEBUG, "Command {" << argsDebug << "} started with pid " << childpid, CORE_UTILS);

        int cmd_stdin = pipe_in[1];
        int cmd_stdout = pipe_out[0];
        int cmd_stderr = pipe_err[0];

        close_pipe_in_0.now();  // Read side of stdin not used by parent
        close_pipe_out_1.now(); // Write side of stdout not used by parent
        close_pipe_err_1.now(); // Write side of stder not used by parent

        setFDFlags(cmd_stdout, O_NONBLOCK);
        setFDFlags(cmd_stderr, O_NONBLOCK);

        // Send stdin to the pipe
        if (in.length()) {
            ssize_t rc = write(cmd_stdin, in.c_str(), in.length());

            if (rc < 0) {
                int e = errno;

                KILL_CHILD;

                THROW(FailedSystemCall, "Unable to write to the child stdin (pid "
                                          << childpid << "): " << ErrnoFormat(e));
            } else {
                if ((size_t)rc != in.length()) {
                    KILL_CHILD;

                    THROW(FailedSystemCall, "Unable to write all the data to the child stdin (pid "
                                              << childpid << "). Only " << rc << " bytes written");
                }
            }
        }

        // Close and flush stdin
        close_pipe_in_1.now();

        time_t ts_timeout = 0;
        if (timeout > 0) {
            ts_timeout = time(NULL) + timeout;
        }

        struct pollfd fds[2];
        fds[0].fd = cmd_stdout;
        fds[0].events = POLLIN;
        fds[0].revents = 0;
        fds[1].fd = cmd_stderr;
        fds[1].events = POLLIN;
        fds[1].revents = 0;

        ostream* strm_pipe[2] = { &out, &err }; // each element of the
                                                // array correspond to
                                                // the poll structure
                                                // waiting on the file
                                                // descriptor

        SPCDBG(L_DEBUG, "Waiting for command " << QT(args[0]) << " to terminate", CORE_UTILS);

        bool got_pollhup = false;
        int poll_timeout = 10;

        for (;;) {
            int rc = poll(fds, 2,
                          got_pollhup ? 0 : poll_timeout); // wait 250ms, unless we
                                                           // already got a POLLHUP event
            poll_timeout *= 2;
            if (poll_timeout > 250) {
                poll_timeout = 250;
            }

            got_pollhup = false;

            if (rc == 0) {
                // not output activity. is the process still alive?

                int status = 0;
                pid_t retpid = waitpid(childpid, &status, WNOHANG);

                if (retpid == childpid) {
                    // process is gone. determining how

                    if (WIFEXITED(status)) {
                        // nornmal termination

                        SPCDBG(L_DEBUG,
                               "Command " << QT(args[0]) << " exited normally with value "
                                          << WEXITSTATUS(status),
                               CORE_UTILS);
                    } else {
                        if (WIFSIGNALED(status)) {
                            // killed by a signal

                            SPCDBG(L_DEBUG,
                                   "Command " << QT(args[0]) << " was killed by signal "
                                              << WTERMSIG(status),
                                   CORE_UTILS);
                        } else {
                            // killed by aliens?

                            SPCDBG(L_INFO,
                                   "Command " << QT(args[0]) << " died from unknown cause (status "
                                              << status << ")",
                                   CORE_UTILS);
                        }
                    }

                    // Flush all remaining data
                    flush_pipe(out, cmd_stdout);
                    flush_pipe(err, cmd_stderr);

                    return status;
                } else {
                    if (retpid == 0) {
                        // process is still there. waiting longer
                    } else {
                        int e = errno;

                        KILL_CHILD;

                        THROW(FailedSystemCall,
                              "waitpid(" << childpid << ") return an error: " << ErrnoFormat(e));
                    }
                }
            } else {
                if (rc < 0) {
                    int e = errno;

                    KILL_CHILD;

                    THROW(FailedSystemCall, "poll() while waiting for pid "
                                              << childpid
                                              << " return an error: " << ErrnoFormat(e));
                } else {
                    // there is data in at least one pipe

                    for (int fd = 0; fd <= 1; fd++) {
                        if (fds[fd].revents) {
                            if (fds[fd].revents & POLLIN) {
                                flush_pipe(*(strm_pipe[fd]), fds[fd].fd);
                                fds[fd].revents &= ~POLLIN;
                            }

                            if (fds[fd].revents & POLLHUP) {
                                // Pipe is closed. cancel it from the poll list and
                                // make sure the next poll does not wait 250ms. we
                                // expect waitpid to tell how the process died.

                                fds[fd].fd = -1;
                                fds[fd].events = 0;
                                fds[fd].revents &= ~POLLHUP;

                                got_pollhup = true;
                            }

                            if (fds[fd].revents) {
                                // if there is any remaining events, kill the child

                                KILL_CHILD;

                                THROW(FailedSystemCall,
                                      "poll() while waiting for pid "
                                        << childpid << " return an unexpected status for fd " << fd
                                        << ": " << PollEventFormat(fds[fd].revents));
                            }
                        }
                    }
                }
            }

            if (ts_timeout) {
                if (time(NULL) > ts_timeout) {
                    KILL_CHILD;

                    THROW(CommandTimeout, "Timeout (" << timeout << "s) when executing command "
                                                      << QT(args[0]) << " (pid " << childpid
                                                      << ")");
                }
            }
        }
    }
}

unsigned long getAvailableFileSystemSpace(const char* path)
{
    struct statvfs fsinfo;
    if (statvfs(path, &fsinfo) != 0) {
        return 0;
    } else {
        return fsinfo.f_bavail * fsinfo.f_bsize;
    }
}

string indentedSpaces(unsigned level)
{
    stringstream ss("");
    for (unsigned i = 0; i < level; ++i) {
        ss << "  ";
    }
    return ss.str();
}

// Explicit instantiations for a few types
template struct arrayToString<size_t>;
template struct arrayToString<int>;

IMPL_EXCEPTION(Distillery, PthreadFailure, Utils);
IMPL_EXCEPTION(Distillery, FailedRead, Utils);
IMPL_EXCEPTION(Distillery, FailedSystemCall, Utils);
IMPL_EXCEPTION(Distillery, InvalidStringFormat, Utils);
IMPL_EXCEPTION(Distillery, OldKernelVersion, Utils);
IMPL_EXCEPTION(Distillery, InvalidPopen, Utils);
IMPL_EXCEPTION(Distillery, FailedConversion, Utils);
IMPL_EXCEPTION(Distillery, SpuriousCharacterFound, Utils);
IMPL_EXCEPTION(Distillery, CommandTimeout, Utils);

UTILS_NAMESPACE_END
