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

// Implementation of several general-purpose helper functions

#ifndef SUPPORTFUNCTIONS_H
#define SUPPORTFUNCTIONS_H

#include <UTILS/DistilleryException.h>
#include <UTILS/HostToNetwork.h>
#include <UTILS/SpinLock.h>
#include <UTILS/UTILSTypes.h>
#include <algorithm>
#include <cctype>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <endian.h>
#include <locale>
#include <netinet/in.h>
#include <sys/time.h>
#include <unicode/locid.h>
#include <unicode/stringpiece.h>
#include <unicode/ustream.h>
#include <unicode/ustring.h>
#include <unicode/utypes.h>

// forward declarations
U_NAMESPACE_BEGIN // from unicode/utypes.h
  class SimpleDateFormat;
U_NAMESPACE_END

// Forward declarations.
//
namespace xmlns {
namespace prod {
namespace streams {
namespace runtime {
namespace utils {
class MessageElementType;
class additionalExceptionDataType;
}
}
}
}
}

UTILS_NAMESPACE_BEGIN

// forward declarations
class SBuffer;

/// one second in milliseconds
const unsigned ONE_SECOND = 1000 * 1000;
const unsigned ONE_MILLISECOND = 1000;
const std::string STREAMS_ID_UNSET = "::UNSET::";
const std::string STREAMS_SERVICE_STATE_UP = "UP";

DECL_EXCEPTION(UTILS_NAMESPACE, FailedRead, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, FailedSystemCall, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidStringFormat, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, OldKernelVersion, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, InvalidPopen, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, FailedConversion, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, SpuriousCharacterFound, Utils);
DECL_EXCEPTION(UTILS_NAMESPACE, CommandTimeout, Utils);

/// This class defines constants that require procedural initialization, e.g.,
/// by reading kernel data structures on initialization
class SystemStatics
{
  public:
    static std::string const& get_STREAMS_DOMAIN_ID_envVar();
    static std::string const& get_STREAMS_INSTANCE_ID_envVar();
    static std::string const& get_STREAMS_DEFAULT_IID_envVar();
    static std::string const& get_STREAMS_INSTALL_envVar();
    static std::string const& get_STREAMS_NS_envVar();
    static std::string const& get_STREAMS_DEFAULT_APPLICATION_SCOPE(); // not an env var?

    /// Obtain the clock frequency for the CPU
    /// @return the frequency in MHz
    static float getCPUFrequency();

    /// Obtain the number of CPUs available in the node
    /// @return number of available CPUs
    static unsigned getNCPUs();

    /// Obtain the number of CPUs available in the container environment
    /// @return number of available CPUs if it is in container environment
    //  @return otherwise return -1
    static int getContainerNCPUs();

    /// Obtain timebase on a PowerPC system
    /// @return PowerPC system's timebase
    static uint64_t getPPCTimeBase();

    /// Obtain the number of ticks per nanosecond
    /// @return the number of ticks per nanosecond
    static double getTicksPerNanosec();

  private:
    static unsigned initNCPUs();
    static int initContainerNCPUs();
    static float initCPUFrequency();
    static double initTicksPerNanosec();
    static uint64_t initPPCTimeBase();

    static UTILS_NAMESPACE::SpinLock slock;
};

/// Invoke mktime in a thread-safe way by using the tm->tm_zone to
/// temporarily set the environment's TZ variable
/// @param tm broken-down time
/// @return the same as mktime, but respectes tm->tm_zone field.
time_t get_mktime(struct tm* tm);

/// Invoke localtime_r in a thread-safe way by using an optional tzone to
/// temporarily set the environment's TZ variable
/// @param time time to convert
/// @param tm converted time in struct tm
/// @param tzone Explicit timezone, or NULL for local timezone
/// @return the same as localtime_r, but may set TZ temporarily
void get_localtime(time_t* time, struct tm* tm, const char* tzone = NULL);

#define ERRNO_EX(cond, ex, msg)                                                                    \
    do {                                                                                           \
        if (cond) {                                                                                \
            char err[512];                                                                         \
            char* err_ptr = strerror_r(errno, err, 512);                                           \
            if (err_ptr)                                                                           \
                THROW(ex, msg << " (errno=" << errno << "): " << err_ptr);                         \
            else                                                                                   \
                THROW(ex, msg << " (errno=" << errno << "): could not retrieve errno message");    \
        }                                                                                          \
    } while (0)

/// Returns the string corresponding to an error number
/// @param myerrno the errno for the error number
/// @return the string corresponding to the error
inline std::string errno2str(const int myerrno)
{
    char err[512];
    char* err_ptr = strerror_r(errno, err, 512);
    if (err_ptr) {
        return std::string(err_ptr);
    }
    return std::string("could not retrieve errno message");
}

/// Obtain the wall clock time
/// @return the wall clock time in seconds
/// @throws FailedSystemCallException -- gettimeofday failed
inline double getWallClockTime(void)
{
    struct timeval tval;
    int ret = gettimeofday(&tval, NULL);
    if (ret != 0) {
        char buffer[1024];
        THROW(FailedSystemCall, "gettimeofday: " << strerror_r(errno, buffer, 1024));
    }
    return tval.tv_sec + tval.tv_usec / static_cast<double>(1000000.0);
}

/// Obtain the wall clock time
/// @return the wall clock time in seconds
/// @throws FailedSystemCallException -- gettimeofday failed
inline uint64_t getWallClockTimeInSec(void)
{
    struct timeval tval;
    int ret = gettimeofday(&tval, NULL);
    if (ret != 0) {
        char buffer[1024];
        THROW(FailedSystemCall, "gettimeofday: " << strerror_r(errno, buffer, 1024));
    }
    return tval.tv_sec;
}

#if defined(__powerpc__)

static __inline__ uint64_t rdtsc(void)
{
    uint64_t result = 0;
    uint32_t upper, lower, tmp;
    __asm__ volatile("0:               \n"
                     "\tmftbu   %0     \n"
                     "\tmftb    %1     \n"
                     "\tmftbu   %2     \n"
                     "\tcmpw    %2,%0  \n"
                     "\tbne     0b     \n"
                     : "=r"(upper), "=r"(lower), "=r"(tmp));
    result = upper;
    result = result << 32;
    result = result | lower;

    return result;
}

inline uint64_t getTimeInNanosecs(void)
{
    static float ticks = SystemStatics::getTicksPerNanosec();
    return static_cast<uint64_t>(static_cast<double>(rdtsc()) / ticks);
}

inline uint64_t getTimeInMicrosecs(void)
{
    static float ticks = SystemStatics::getTicksPerNanosec() * 1000;
    return static_cast<uint64_t>(static_cast<double>(rdtsc()) / ticks);
}

#elif defined(__i386__) || defined(__x86_64__) || defined(__amd64__)

#ifndef rdtsc
#define rdtsc(low, high) __asm__ __volatile__("rdtsc" : "=a"(low), "=d"(high))
#endif

/// obtain time in nanoseconds using Intel's rdtsc instruction
// WE THOUGHT YOU SHOULD KNOW THE CAVEATS -- from Wikipedia
//
// In the x86 assembly language, the RDTSC instruction is a mnemonic for read
// time stamp counter[1]. The instruction returns a 64-bit value in registers
// EDX:EAX that represents the count of ticks from processor reset. The
// instruction was introduced in the Pentium line of processors. Its opcode is
// 0F 31. This instruction was not formally part of the X86 assembly language
// at Pentium and was recommended for use by expert users or System Programmers
// only (Intel reference required). Pentium competitors such as the Cyrix 6x86
// did not always have a TSC and may consider this instruction illegal. Use of
// this instruction in Linux distributions precludes Linux from booting where
// the CPU does not support RDTSC. Cyrix included a Time Stamp Counter in their
// MII CPU architecture as RDTSC was formally included in the X86 assembly
// language in Pentium II.

// The RDTSC instruction has, until recently, been an excellent
// high-resolution, low-overhead way of getting CPU timing information. With
// the advent of multi-core/hyperthreaded CPUs, systems with multiple CPUs, and
// "hibernating" operating systems, RDTSC often no longer provides reliable
// results. The issue has two components: rate of tick and whether all cores
// (processors) have identical values in their time-keeping registers. There is
// no longer any promise that the timestamp counters of multiple CPUs on a
// single motherboard will be synchronized. So, you can no longer get reliable
// timestamp values unless you lock your program to using a single CPU. Even
// then, the CPU speed may change due to power-saving measures taken by the OS
// or BIOS, or the system may be hibernated and later resumed (resetting the
// time stamp counter). Also it makes the program not portable to anything
// other than x86. Recent Intel processors include a constant rate TSC
// (identified by the constant_tsc flag in Linux's /proc/cpuinfo). With these
// processors the TSC reads at the processors maximum rate regardless of the
// actual CPU running rate. While this makes time keeping more consistent, it
// can skew benchmarks, where a certain amount of spin-up time is spent at a
// lower clock rate before the OS switches the processor to the higher rate.
// This has the effect of making things seem like they require more processor
// cycles than they normally would.
// @return time in nanoseconds
inline uint64_t getTimeInNanosecs(void)
{
    static float freq = SystemStatics::getCPUFrequency();
    uint32_t v1, v2;
    rdtsc(v1, v2);

    uint64_t l = v2;
    l <<= 32LL;
    l |= v1;

    return static_cast<uint64_t>(static_cast<double>(l) / (freq / 1000.0));
}

inline uint64_t getTimeInMicrosecs(void)
{
    static float freq = SystemStatics::getCPUFrequency();
    uint32_t v1, v2;
    rdtsc(v1, v2);

    uint64_t l = v2;
    l <<= 32LL;
    l |= v1;

    return static_cast<uint64_t>(static_cast<double>(l) / freq);
}

#else

inline uint64_t getTimeInNanosecs(void)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec;
}

inline uint64_t getTimeInMicrosecs(void)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 * 1000 + ts.tv_nsec / 1000;
}

#endif

inline uint64_t getWallTimeInMicrosecs(void)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 * 1000 + ts.tv_nsec / 1000;
}

inline uint64_t getWallTimeInNanosecs(void)
{
    timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return ts.tv_sec * 1000 * 1000 * 1000 + ts.tv_nsec;
}

/// Duplicate a string -- like strdup, but using the new operator
/// @param s string to be duplicated
/// @return a pointer to the duplicated string (the caller must deallocate it)
char* StrDup(const char* s);

/// Convert a string with a value suffix to a number (e.g., "10K" == 10000)
/// @param str is a string (valid suffixes are u=micro, m=milli, K=kilo,
/// M=mega, G=giga
/// @return a double value
/// @throws InvalidStringFormatException -- non-compliant format
double suffixedStr2double(const char* str);

/// Convert a numeric value into a string with a value suffix
/// (e.g., 10000 == "10K") -- THIS FUNCTION IS NOT REENTRANT
/// @param n is a number
/// @return a string
/// @throws InvalidStringFormatException -- non-compliant format
char* double2suffixedStr(const double n);

/// Obtain the Distillery_NS from the environment
/// @return the Distillery Name Server configuration string
const std::string getNS(void);

/// Obtain the Distillery_ID from the environment
/// @return the Distillery identification string
const std::string getInstance(void);

/// Obtain the installation location for InfoSphere Streams.
/// @return the installation location
std::string getInstallationDirectory(void);

/// Obtain the installation owner.
/// @return the installation location
std::string getInstallationOwner(void);

/// Obtain the installation owner home directory
/// @return the location
/// Note: Jingdong: The implementation of this method not work well.
///       Need some investigation if we need it. Or cleanup in the future.
std::string getInstallationOwnerHomeDir(void);

/// Obtain InfoSphere Streams system bin directory
/// @return the directory of system bin
std::string getSysImplBinDirectory(void);

/// Obtain the user name for the user running this process
/// @return the user name
std::string getUserName(void);

/// Obtain the process's instance identifier
/// @return the Distillery instance identifier
std::string getInstanceId(void);

/// Obtain the process's domain identifier
/// @return the Distillery domain identifier
std::string getDomainId(void);

/// Obtain the instance owner user id
/// @return instance owner user id.
std::string getInstanceOwner(void);

/// Verify user name for the user running this process
/// is the Instance owner
/// @return true if process is running under Instance owner id
bool verifyInstanceOwner(void);

/// Obtain the process's default instance identifier
/// @return the Distillery default instance identifier
std::string getDefaultInstanceId(void);

/// Obtain the Spade default instance identifier
/// Deprecated - used by the SPADE compiler (destined for removal in Humber); see
/// getStdDefaultInstance*Id()
/// @return the Spade default instance identifier
std::string getSpadeDefaultInstanceShortId(void);

/// Obtain the SPL default instance identifier
/// Deprecated - used by the SPL compiler (destined for removal in Humber); see
/// getStdDefaultInstance*Id()
/// @return the SPL default instance identifier
std::string getSPLDefaultInstanceShortId(void);

/// Obtain the standard default instance identifier
/// @return the standard default instance identifier
std::string getStdDefaultInstanceShortId(void);

/// Obtain the Spade default fully qualified instance identifier
/// Deprecated - used by the SPADE compiler (destined for removal in Humber); see
/// getStdDefaultInstance*Id()
/// @return the Spade default fully qualified instance identifier
std::string getSpadeDefaultInstanceId(void);

/// Obtain the SPL default fully qualified instance identifier
/// Deprecated - used by the SPL compiler (destined for removal in Humber); see
/// getStdDefaultInstance*Id()
/// @return the SPL default fully qualified instance identifier
std::string getSPLDefaultInstanceId(void);

/// Obtain the standard default fully qualified instance identifier
/// @return the standard default fully qualified instance identifier
std::string getStdDefaultInstanceId(void);

/// Obtain the path to the user directory containing the authentication keys
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getUserKeyConfigurationDirectory(bool createDirectory = true);

/// Obtain the path to the user's logs directory. This is the path where
/// client side logs are stored.
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getUserLogsDirectory(bool createDirectory = true);

/// Obtain the path to the instance-specific directory
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there

/// Obtain the path to the instance-specific directory containing
/// executable cluster management tools
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceBinDirectory(const std::string& instanceid, bool createDirectory = true);

/// Obtain the path to the instance owner user directory containing executable
/// cluster management tools
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceOwnerBinDirectory(const std::string& instanceid,
                                         bool createDirectory = true);

/// Obtain the path to InfoSphere Streams admin user directory containing
/// executable cluster management tools
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getBinDirectory(bool createDirectory = true);

/// Obtain the path to InfoSphere Streams license directory containing
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getLicenseDirectory(bool createDirectory = true);

/// Obtain the location where instance template files reside. If the instance
/// for which the template directory is not specified, the location for the
/// InfoSphere Streams administrator is returned
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceTemplateConfigurationDirectory(const std::string& instanceid = "",
                                                      bool createDirectory = true);

/// Obtain the path to the instance configuration directory for
/// the instance given by the instanceid parameter
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceConfigurationDirectory(const std::string& instanceid,
                                              bool createDirectory = true);

/// Obtain the path to the instance Name Service directory for the instance
/// given by the instanceid parameter
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceNameServiceConfigurationDirectory(const std::string& instanceid,
                                                         bool createDirectory = true);

/// Obtain the path to the instance SWS directory for the instance given
/// by the instanceid parameter
/// @param instanceid the string representing  the instance id
/// @param createDirectory flag indicating whether the function should attempt
/// to create the directory if not there
std::string getInstanceSWSConfigurationDirectory(const std::string& instanceid,
                                                 bool createDirectory = true);

/// Compute the MD5 digest for the contents of a serialization buffer
/// @param in the input buffer
/// @param out the output buffer where the digest is stored
void computeMD5Digest(const void* in, size_t inLen, SBuffer& out);

std::string computeMD5Digest(std::istream& strm);

unsigned long copyStream(std::istream& istrm, std::ostream& ostrm);

/// Compute the SHA1 digest for the contents of a serialization buffer
/// @param in the input buffer
/// @param out the output buffer where the digest is stored
void computeSHA1Digest(const SBuffer& in, SBuffer& out);

/// Compute the Base64-encoded SHA1 digest for the contents of a string
/// @param buf the input buffer
/// @param bufsize the input buffer size
/// @param out the output string
void computeBase64SHA1Digest(const char* buf, const unsigned bufsize, std::string& out);

/// Obtain executable's complete path
/// @param exe name of the executable
/// @return executable's path, if found. An empty string is returned, when
/// executable is not found
std::string obtainExecutableFullPath(const std::string& exe);

/// Remove unprintable characters from a string
/// Note: please do not use this method with string including multi-byte chars.
/// @param str string to be "sanitized"
/// @return the "sanitized" string
std::string removeUnprintableChars(const std::string& str);

/// Remove escaping chars from a string
/// This is to match behavior of Java Properties.load(). Please
/// check Java documentation for more details.
/// @param str string to be "sanitized"
/// @return the "sanitized" string
std::string removeEscapingChars(const std::string& str);

/// Remove character is character is present in string
/// @param str string to be "sanitized"
/// @param c the "offending" character
/// @return the "sanitized" string
std::string removeCharIfPresent(const std::string& str, const char c);

/// Replace unprintable characters with a blank space
/// @param str string to be "sanitized"
/// @return the "sanitized" string
std::string replaceUnprintableCharsWithSpace(const std::string& str);

/// Sanitize a string for encoding it in XML
/// @param str string to be sanitized
std::string xmlSanitize(const std::string& str);

/// Tokenize a comma-separated string (CSV)
/// @param str string to be tokenized
/// @param tokens vector with the list of tokens
/// @param quoted handle quoted tokens
void csvtokenize(const std::string& str,
                 std::vector<std::string>& tokens,
                 const bool quoted = false);

/// Tokenize a string
/// @param str string to be tokenized
/// @param tokens vector with the list of tokens
/// @param delimiters string with the characters delimiting each token
/// @param keepEmptyTokens keep empty tokens
void tokenize(const std::string& str,
              std::vector<std::string>& tokens,
              const std::string& delimiter,
              bool keepEmptyTokens);

/// Convert a timestamp into a string (in local time)
/// @param t Timestamp to convert (as returned by time(2))
std::string time2str(time_t t);

/// Convert a timestamp into a UnicodeString (in local time with globalization)
/// @param t Timestamp to convert (as returned by time(2))
UnicodeString time2str_global(time_t t);

/// Get a date/time formatter for the iso8601 format: 2010-03-16T13:41:53-05:00
/// NOTE, the caller owns the returned storage and must delete() it.
icu::SimpleDateFormat* iso8601DateTimeFormatter();

/// Convert a timestamp into a UnicodeString using the provided formatter
/// @param t Timestamp to convert (as returned by time(2))
/// @param fmt ICU Formatter to use
UnicodeString time2str_fmt(time_t t, icu::SimpleDateFormat& fmt);

/// Perform a case-insensitive string comparison
/// We used to support a function for doing this but it is easier to
/// just use boost::iequals - which is all this function would do now.

/// Sleep for an certain amount of time defined as fix +/- variable
/// (in milli-seconds)
/// @param fix Fix base of milli-seconds to sleep
/// @param variable Variable amount of milli-seconds to sleep
int sleep_random(int fix, int variable);

/// used by for_each to delete pointer to objects stored by containers
/// (Scott Meyers' item 6 -- Effective STL)
struct DeleteObject
{

    template<typename T>
    void operator()(const T* ptr) const
    {
        delete ptr;
    }

    template<typename X, typename Y>
    void operator()(std::pair<const X, Y*>& p) const
    {
        delete p.second;
    }
};

/// Return the value of an environment variable or a default value if not
/// defined
/// @param name Name of the environment variable
/// @param defaultval Default value to use if the variable is not defined
std::string get_environment_variable(const std::string& name, const std::string& defaultval);

/// Return the value of an environment variable or a default value if not
/// defined
/// @param name Name of the environment variable
/// @param defaultval Default value to use if the variable is not defined
int get_environment_variable(const std::string& name, int defaultval);

/// Return the value of an environment variable or a default value if not
/// defined
/// @param name Name of the environment variable
/// @param defaultval Default value to use if the variable is not defined
double get_environment_variable(const std::string& name, double defaultval);

/// Return the input string with spaces suffixing it being removed
/// @param str input string
/// @param t character to be removed from right side of input string
/// @return input string without spaces suffixing it
inline std::string rtrim(const std::string& source, const std::string& t = " ")
{
    std::string str = source;
    return str.erase(str.find_last_not_of(t) + 1);
}

/// split a string that has dots in it in the substring between dots
/// for example: 150.164.10.2 returns a vector with "150", "164", "10", and "2"
///              150.164.10. returns a vector with "150", "164", "10", and ""
/// @param s the string to be parsed
/// @param vs the vector with the substrings
inline void splitDottedString(const std::string& s, std::vector<std::string>& vs)
{
    char temp[s.size() + 1];
    unsigned pos = 0;
    for (unsigned i = 0; i < s.size(); ++i) {
        if (s[i] == '.') {
            temp[pos] = '\0';
            pos = 0;
            vs.push_back(std::string(temp));
        } else {
            temp[pos] = s[i];
            pos++;
        }
    }
    // remaining piece of the string, if any
    temp[pos] = '\0';
    vs.push_back(std::string(temp));
}

/// Convert UTF8 string to lower case characters only
/// @param s input string in UTF8
/// returns a lower-case version of the string
inline std::string tolowercase(const std::string& s)
{
    StringPiece sp1(s);

    /// Convert from UTF8 to UTF16, convert to lower case
    /// and then convert back to UTF8
    UnicodeString us1 = UnicodeString::fromUTF8(sp1);
    us1.toLower(Locale("en_US"));
    std::string s2;
    us1.toUTF8String(s2);
    return (s2);
}

/// Convert UTF8 string to upper case characters only
/// @param s input string in UTF8
/// returns an upper case version of the string
inline std::string touppercase(const std::string& s)
{
    StringPiece sp1(s);

    /// Convert from UTF8 to UTF16, convert to upper case
    /// and then convert back to UTF8
    UnicodeString us1 = UnicodeString::fromUTF8(sp1);
    us1.toUpper(Locale("en_US"));
    std::string s2;
    us1.toUTF8String(s2);
    return (s2);
}

/********************
 * Jingdong: Following methods are not work well, so comment out.
 * I contact ICU, waiting for their suggestion.
 * If get any back, I will work on these methods again.

typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_type;

/// Convert wstring to uft8 encoded string
/// @param ws input wstring
/// @return utf8 encoded string
inline std::string wStringToUFT8String(const std::wstring& ws) {
  const wchar_t *fb = ws.data();
  const wchar_t *fe = fb+ws.size();
  const wchar_t *fn;
  std::vector<char> s(ws.size()*4);
  char *tb = &s.front();
  char *te = tb+s.size();
  char *tn;

  std::mbstate_t st = std::mbstate_t ();
  bzero(&st, sizeof(st));
  std::locale loc("en_US.UTF-8");
  const codecvt_type& codecvt = std::use_facet<codecvt_type>(loc);
  codecvt.out(st, fb, fe, fn, tb, te, tn);
  if(fn != fe) {
    THROW(Distillery, "wStringToUFT8String conversion failure");
  }
  return std::string(tb, tn);
  //UnicodeString us(reinterpret_cast<const UChar *>(ws.c_str()));
  //std::string tmpStr;
  //us.toUTF8String(tmpStr);
  //return tmpStr;
}

/// Convert UTF8 encoded string to wstring
/// @param s input utf8 encoded string
/// @return wstring
inline std::wstring uft8StringToWString(const std::string& s) {
  const char *fb = s.data();
  const char *fe = fb+s.size();
  const char *fn;
  std::vector<wchar_t> ws(s.size());
  wchar_t *tb = &ws.front();
  wchar_t *te = tb+ws.size();
  wchar_t *tn;

  std::mbstate_t st;
  bzero(&st, sizeof(st));
  std::locale loc("en_US.UTF-8");
  const codecvt_type& codecvt(std::use_facet<codecvt_type>(loc));
  codecvt.in(st, fb, fe, fn, tb, te, tn);
  if(fn != fe) {
    THROW(Distillery, "uft8StringToWString conversion failure");
  }
  return std::wstring(tb, tn);
  //std::wstringstream wss;
  //wss << s.c_str();
  //return wss.str();
}
***********************************************/

/// Return the input string with spaces prefixing it being removed
/// @param str input string
/// @param t character to be removed from left side of input string
/// @return input string without spaces prefixing it
inline std::string ltrim(const std::string& source, const std::string& t = " ")
{
    std::string str = source;
    return str.erase(0, source.find_first_not_of(t));
}

/// Return the input string with spaces suffixing and prefixing it being
/// removed
/// @param str input string
/// @param t character to be removed from left and right sides of input string
/// @return input string without spaces prefixing and suffixing it
inline std::string trim(const std::string& source, const std::string& t = " ")
{
    std::string str = source;
    return ltrim(rtrim(str, t), t);
}

#define async_safe_printf(fd, fmt, args...)                                                        \
    do {                                                                                           \
        char _signal_printf_buffer[4096];                                                          \
        int _signal_printf_buffer_size = snprintf(_signal_printf_buffer, 4096, fmt, ##args);       \
        write(fd, _signal_printf_buffer, _signal_printf_buffer_size);                              \
    } while (0)

/// Converts a string to a different (numerical) type -- unsafe as it does
/// not test for failed conversions (but it goes faster)
/// @param t converted value
/// @param s input string
/// @param f numerical base
template<class T>
void fromStringUnsafe(T& t, const std::string& s, std::ios_base& (*f)(std::ios_base&) = std::dec)
{
    std::istringstream iss(s);
    iss >> f >> t;
}

/// Converts a string to a different (numerical) type
/// @param t converted value
/// @param s input string
template<class T>
void fromString(T& t, const std::string& s)
{
    if (s.empty()) {
        t = static_cast<T>(0);
    } else {
        std::istringstream iss(s);
        iss >> t;
        if (iss.fail()) {
            THROW(FailedConversion, "string '" << s << "' conversion failed");
        }
        if (!iss.eof()) {
            THROW(SpuriousCharacterFound, "string '" << s << "' contains spurious character");
        }
    }
}

// The following had been commented out, but needed to be uncommented for the definition
// in  SupportFunctions.cpp to be picked up and used.

/// Converts a string to a string type.  Function is defined in C++ file,
/// but needs to be prototyped here.
/// @param t converted string
/// @param s input string
template<>
void fromString<>(std::string& t, const std::string& s);

/// Converts a numerical type to a string
/// @param t value to be converted
/// @return the string with the string-equivalent of the numeric value
template<class T>
std::string toString(const T& t)
{
    std::ostringstream oss;
    oss << t;
    if (oss.fail()) {
        THROW(FailedConversion, "value '" << t << "' conversion failed");
    }
    return oss.str();
}

/// Prints at most @c n entries of the given vector to a string.
/// @param v vector of values to print
/// @param n print at most @c n entries
/// @return the string representation of the vector
template<typename T>
std::string toString(const std::vector<T>& v, const size_t n = 0)
{
    std::ostringstream oss;
    oss << "vector[" << v.size() << "] {";
    size_t count = (n == 0) ? v.size() : (size_t)n;
    if (count > v.size()) {
        count = v.size();
    }
    size_t idx = 0;
    for (typename std::vector<T>::const_iterator i = v.begin(); idx < count; ++idx) {
        if (idx != 0) {
            oss << ", ";
        }
        oss << *i++;
    }
    if (idx < v.size()) {
        oss << " ...";
    }
    oss << "}";
    return oss.str();
}

template<typename K, typename V>
std::string toString(const std::map<K, V>& v)
{
    std::ostringstream oss;
    for (typename std::map<K, V>::const_iterator i = v.begin(); i != v.end(); ++i) {
        if (i != v.begin()) {
            oss << ",";
        }
        oss << i->first << ":" << i->second;
    }
    return oss.str();
}

/// Prints at most @c n entries of the given array to a string.
/// @param a array of values to print
/// @param len the size of the array
/// @param n print at most @c n entries
/// @return the string representation of the array
template<typename T>
struct arrayToString
{
    std::string operator()(const T* a, const size_t len, bool useHex, const size_t n = 0);
};

// Note: definition with extern linkage
template<typename T>
std::string arrayToString<T>::operator()(const T* a, const size_t len, bool useHex, const size_t n)
{
    std::ostringstream oss;
    if (useHex) {
        oss << std::hex << std::showbase;
    }
    size_t count = (n == 0) ? len : n;
    if (count > len) {
        count = len;
    }
    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            oss << ", ";
        }
        oss << a[i];
    }
    if (count < len) {
        oss << " ...";
    }
    return oss.str();
}

/// Converts a month string into a number from 0-11
/// @param monthstr must include at least a 3-character suffix for a month
/// in English (e.g, JAN, FEB, MAR, MAY, APR, etc)
/// @return a number from 0 to 11
int monthstr2num(const char* monthstr);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param dmy a string with a format of the form "04-OCT-2005"
/// @param hmsmilli a string with a format of the form "14:18:19.324"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// @return number of microseconds since Epoch
uint64_t timeStampToMicroseconds(const char* dmy, const char* hmsmilli, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param ymdhms  a string with a format of the form "YYYYMMDDhhmmss"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t YYYYMMDDhhmmss_ToMicroseconds(const char* ymdhms, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param ymdhms  a string with a format of the form "YYYY-MM-DD hh:mm:ss"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t YYYY_MM_DD_hh_mm_ssToMicroseconds(const char* ymdhms, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param mdyhms  a string with a format of the form "MM-DD-YYYY hh:mm:ss"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t MM_DD_YYYY_hh_mm_ssToMicroseconds(const char* mdyhms, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param dmyhms  a string with a format of the form "DD-MM-YYYY hh:mm:ss"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t DD_MM_YYYY_hh_mm_ssToMicroseconds(const char* dmyhms, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param ymdhmsmmm  a string with a format of the form "YYYY-MM-DD
/// hh:mm:ss.mmm"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds(const char* ymdhmsmmm, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param mdyhmsmmm  a string with a format of the form "MM-DD-YYYY
/// hh:mm:ss.mmm"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t MM_DD_YYYY_hh_mm_ss_mmmToMicroseconds(const char* mdyhmsmmm, const char* tmzone = NULL);

/// Converts a time stamp into the number of microseconds since Epoch
/// @param dmyhmsmmm  a string with a format of the form "DD-MM-YYYY
/// hh:mm:ss.mmm"
/// @param tmzone time zone, local time zone by default (e.g. "UTC")
/// (note: this is a fixed format, any separator in the proper place is ok)
/// @return number of microseconds since Epoch
uint64_t DD_MM_YYYY_hh_mm_ss_mmmToMicroseconds(const char* dmyhmsmmm, const char* tmzone = NULL);

/// Read all bytes from a file descriptor into a buffer
/// @param fd file descriptor
/// @param buf buffer
/// @param count number of bytes to be read
/// @return number of bytes read (!=count indicates EOF)
inline size_t readAll(int fd, void* buf, size_t count)
{
    size_t nread = 0;
    do {
        ssize_t nthisread = read(fd, reinterpret_cast<char*>(buf) + nread, count - nread);
        if (nthisread < 0) {
            if (errno != EINTR && errno != EAGAIN) {
                // not an interrupt or a temporary failure
                ERRNO_EX(true, FailedRead, "failed read");
            }
        } else if (nthisread == 0) {
            break; // end of file
        } else {   // some data
            nread += static_cast<size_t>(nthisread);
        }
    } while (nread < count);
    return nread;
}

// Linear congruential uint32_t random number generator
// Note: X_{n+1} = ( a * X_n + c ) mod m c and m are relatively prime Numerical
// receipts in C recommends: a = 1664525, c = 1013904223, m = 2^{32} min value:
// 0, max value: 2^{32}-1
// @return a randomly generated integer
inline uint32_t cheap_rand_uint32(void)
{
    static uint32_t v = 17; // seed
    const uint32_t a = 1664525;
    const uint32_t c = 1013904223;
    return (v = a * v + c);
}

// read a line assuming that the end of line marker is \r\n
// @param i the input stream
// @param buffer output buffer with line (buffer is not NULL-terminated)
// @param maxbuflen buffer length
uint32_t dosgetline(std::istream& i, char* buffer, uint32_t maxbuflen);

// Linear congruential float random number generator in [0, 1) range
// @return a randomly generated floating point number
inline float cheap_rand_float(void)
{
    return static_cast<float>(static_cast<float>(cheap_rand_uint32()) /
                              static_cast<float>(4294967296.0));
}

/// Obtain the number of candidate CPUs that might be used for scheduling
/// a thread/process
/// @param pid the (optional) thread/process id (if not defined, will use
/// use the process id of the process)
/// @return the number of candidate CPUs
unsigned getNCurrentCandidateCPUs(pid_t pid = getpid());

/// Obtain the collection of candidate CPUs that might be used for
/// scheduling a thread/process
/// @param pid the (optional) thread/process id (if not defined, will use
/// use the process id of the process)
/// @return the collection of candidate CPUs
std::vector<int32_t> getCurrentCandidateCPUs(pid_t pid = getpid());

/// Set a single CPU as candidate for running a thread/process
/// @param cpu the CPU to be added (from 0 up to getNCPUs())
/// @param pid the (optional) thread/process id (if not defined, will use
/// use the process id of the process)
void setCPU(const int32_t cpu, pid_t pid = getpid());

/// Add a single CPU as candidate for running a thread/process (in
/// addition to other CPUs already set as a candidate)
/// @param cpu the CPU to be added (from 0 up to getNCPUs())
/// @param pid the (optional) thread/process id (if not defined, will use
/// use the process id of the process)
void addCPU(const int32_t cpu, pid_t pid = getpid());

/// Add all physical CPUs as candidates for running a thread
/// @param pid the (optional) thread/process id (if not defined, will use
/// use the process id of the process)
void useAllAvailableCPUs(pid_t pid = getpid());

/// Replacement a string pattern with another one
/// @param str original string
/// @param pattern pattern to be replaced
/// @param pattern replacement pattern
void replaceAllPatternOccurrences(std::string& str,
                                  const std::string& pattern,
                                  const std::string& replacement);

/// Set a flag on a file descriptor
/// @param fd file descriptor
/// @param flag flag or flags to add
void setFDFlags(int fd, int flag);

/// Remove a flag on a file descriptor
/// @param fd file descriptor
/// @param flag flag or flags to remove
void unsetFDFlags(int fd, int flag);

/// Set the "clone on exec" flag on a file descriptor
/// @param fd file descriptor
/// @param flag flag or flags to add
void setCloseOnExecFlags(int fd);

/// Write content of a pipe into stream.
/// @param strm output stream to write pipe content to
/// @param fd file descriptor
void flush_pipe(std::ostream& strm, int fd);

/// Execute a command, feeding stdin and capturing stdout and
/// stderr. It returns the status code of the program. An exception
/// will be thrown for any other condition.
/// @param args arguments of the command, as expected by execvp
/// @param in Value to feed to the program as "stdin"
/// @param out Stream where to store the standard output of the command
/// @param err Stream where to store the error output of the command
/// @param timeout Value in second to wait for before killing the command (= 0 for no timeout)
/// @param closeOnExecFDs set FD_CLOEXEC flag for all parent FDs except stdout/stdin/stderr
/// @param limitCommandTrace limit the amount of data traced for the command line (=0 for no limit,
/// >0 the number of characters to trace)
/// @return It returns the value of status obtained by waitpid
int runCommand(const std::vector<std::string>& args,
               const std::string& in,
               std::ostream& out,
               std::ostream& err,
               unsigned int timeout,
               bool closeOnExecFDs = false,
               int limitCommandTrace = 0);

/// log additional exception data messages
/// @param l_level level to log messages at
/// @param adtlnExData additional exception data
void logAdditionalExceptionData(
  int l_level,
  const xmlns::prod::streams::runtime::utils::additionalExceptionDataType& adtlExData);

// log additional exception message
/// @param l_level level to log messages at
/// @param msg message to be logged.
void logAdditionalExceptionMsg(int l_level,
                               const xmlns::prod::streams::runtime::utils::MessageElementType& msg);

/// trace additional exception data messages
/// @param l_level level to trace messages at
/// @param adtlnExData additional exception data
void traceAdditionalExceptionData(
  int l_level,
  const xmlns::prod::streams::runtime::utils::additionalExceptionDataType& adtlExData);

// trace additional exception message
/// @param l_level level to trace messages at
/// @param msg message to be logged.
void traceAdditionalExceptionMsg(
  int l_level,
  const xmlns::prod::streams::runtime::utils::MessageElementType& msg);

// Get available diskspace of a specific path
/// @param path the pathname of any file within the filesystem
/// @return available space
unsigned long getAvailableFileSystemSpace(const char* path);

/// Utility method to return indented space string based on level.
/// @param level the level that the indented space going to be at.
std::string indentedSpaces(unsigned level);

UTILS_NAMESPACE_END

#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
