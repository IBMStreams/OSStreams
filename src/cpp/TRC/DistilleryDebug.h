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

//----------------------------------------------------------------------------
//
// Title-
//       DistilleryDebug.h
//
// Purpose-
//       Debugging convenience routines
//
//----------------------------------------------------------------------------
#ifndef __STREAMS_DEBUG_H__
#define __STREAMS_DEBUG_H__

#include <assert.h>
#include <cstdarg>
#include <inttypes.h>
#include <iostream>
#include <map>
#include <sstream>
#include <syscall.h>
#include <syslog.h>
#include <unistd.h>
#include <vector>

#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCTypes.h>
#include <UTILS/DSTAutoConf.h>
#include <UTILS/Mutex.h>

UTILS_NAMESPACE_BEGIN

// Forward declarations
class DistilleryException;

DEBUG_NAMESPACE_BEGIN

// Some common info needed by logging/tracing
extern std::string cmdline;
extern std::string cmd;
extern std::string hostname;
extern std::string identity;
extern std::string dynamic_aspects;

// Mutex to guard logging or tracing
extern Mutex dbg_mutex;       // for service tracing
extern Mutex log_mutex;       // for logging
extern Mutex app_mutex;       // for application tracing
extern Mutex stdouterr_mutex; // for stdout/stderr tracing
extern Mutex audit_mutex;     // for audit logging

class TimeStamp
{
  protected:
    int64_t _offset;

  public:
    static int64_t TOD_ms(void);
    static int64_t TOD_us(void);

    TimeStamp(bool absolute = false);

    int64_t getms() const;
    void print(std::ostream& strm) const;
};

std::ostream& operator<<(std::ostream& strm, const TimeStamp& t);

extern TimeStamp spcnow;
extern TimeStamp spcnowa;

template<class InputIterator>
std::string join(InputIterator first, InputIterator last, const std::string& sep = ",")
{
    std::stringstream res;
    InputIterator it = first;
    while (it != last) {
        if (it != first) {
            res << sep;
        }
        res << *it;
        it++;
    }
    return res.str();
}

template<class Elem>
std::string joinArray(Elem* array, unsigned int size, const std::string& sep = ",")
{
    std::stringstream res;
    for (unsigned int i = 0; i < size; i++) {
        if (i != 0) {
            res << sep;
        }
        res << array[i];
    }
    return res.str();
}

pid_t distillery_gettid();
int expand_args(char* buffer, int max_size, ...);
void add_dynamic_aspect(const std::string& new_aspect);
void fadd_dynamic_aspect(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
void write_msg(int level,
               const std::string& aspect,
               const std::string& args,
               const std::string& function,
               const std::string& file,
               int line,
               const std::string& msg);

void write_log(int level,
               const std::string& aspect,
               const std::string& function,
               const std::string& file,
               int line,
               const std::string& msg);

void write_log(int level,
               const std::string& aspect,
               const std::string& function,
               const std::string& file,
               int line,
               const std::string& key,
               const std::vector<std::string>& substitutionTexts);

void write_log(int level,
               const std::string& aspect,
               const std::string& function,
               const std::string& file,
               int line,
               const std::string& key,
               const std::vector<std::string>& substitutionTexts,
               const Distillery::DistilleryException& e);

void write_log(int level,
               const std::string& aspect,
               const std::string& function,
               const std::string& file,
               int line,
               const std::string& key,
               const std::vector<std::string>& substitutionTexts,
               const std::exception& e);

void write_appmsg(int level,
                  const std::string& aspect,
                  const std::string& function,
                  const std::string& file,
                  int line,
                  const std::string& msg);

void write_audit(const std::string& type,
                 bool success,
                 const std::string& iid,
                 const std::string& source,
                 const std::string& user,
                 const std::string& object,
                 const std::string& operation,
                 const std::string& msgId,
                 const std::vector<std::string>& msgvars,
                 const std::map<std::string, std::string>& opParams);

int getInternalDebugLevel(int level);

// Write something to file 1 without using any mutexes or other locks.
void lockFreeWrite(int level,
                   const char* function,
                   const char* file,
                   int line,
                   const char* fmt,
                   ...) __attribute__((format(printf, 5, 6)));

template<typename T>
std::string argfmt(const char* entity, const T& v)
{
    std::ostringstream oss;
    oss << entity << '[' << v << ']';
    return oss.str();
}

#define FORMAT_DEFAULT                                                                             \
    "@trctime@ [@tid@] @trclevel@ @aspect@ M[@file@:@function@:@line@] @args@ - @message@"
#define FORMAT_LONG "DEBUG@level@ {@aspect@} @time@ (@tid@, @function@, @file@:@line@) @message@"
#define FORMAT_SHORT "<@level@> @message@"
#define FORMAT_SPC "DEBUG@level@ @time@ (@tid@, @function@, @file@:@line@) @message@"
#define FORMAT_LOG "[@timestamp@] [@instance@] [@host@] [@aspect@] @message@"
#define FORMAT_SYSLOG "[IBM InfoSphere Streams] [@instance@] [@host@] [@aspect@] @message@"

// Static array map of external debug level to internal ones.
static const int EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[7] = { iL_ERROR, iL_INFO, iL_DEBUG,
                                                             iL_TRACE, iL_OFF,  iL_WARN };

DEBUG_NAMESPACE_END
UTILS_NAMESPACE_END

#define PEIDFMT(a) Distillery::debug::argfmt("P", a).c_str()
#define JOBIDFMT(a) Distillery::debug::argfmt("J", a).c_str()
#define PORTIDFMT(a) Distillery::debug::argfmt("PORTID", a).c_str()

// Following values are visible to external interface, for backward compatibility,
// please do not make any updates.
// Streams maintain future updates of debugging levels internally from TRCTypes.h.
#define L_ERROR 0 // Errors
#define L_INFO 1  // Configuration, RPCs, main internal APIs
#define L_DEBUG 2 // Methods
#define L_TRACE 3 // Everything else
#define L_OFF 4   // Off
#define L_WARN 5  // warnings

// For backward compatibility, please be careful when updating following macros.
// Best not change all interface methods and variables they are using.

#ifdef DST_DBG_INST

#define SPCDBG_TST(lvl, code)                                                                      \
    do {                                                                                           \
        volatile static unsigned long __dbgpreproc___##lvl = 0;                                    \
        if (__dbgpreproc___##lvl != 0) {                                                           \
            code;                                                                                  \
        }                                                                                          \
    } while (0)

#else

// Following is the old copy of SPCDBG_TST with Streams v2.0 and earlier.
// When doing any update, please keep the old copy also has expected behavior
// for backward compatibility purpose.
// #define SPCDBG_TST(lvl, code) do {
//    if (((lvl) <= Distillery::debug::logger_max_level))
//    {
//      code ;
//    }
// } while(0)

#define SPCDBG_TST(lvl, code)                                                                      \
    do {                                                                                           \
        if (__builtin_expect((Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl] <=      \
                              Distillery::debug::spc_trace_level),                                 \
                             0)) {                                                                 \
            code;                                                                                  \
        }                                                                                          \
    } while (0)

#endif

#define SPCDBG_ARGS_MAX_LENGTH 1024
#define SPCDBG_NT(lvl, msg, aspect, args...)                                                       \
    do {                                                                                           \
                                                                                                   \
        char expanded_args[SPCDBG_ARGS_MAX_LENGTH] = "";                                           \
        Distillery::debug::expand_args(expanded_args, SPCDBG_ARGS_MAX_LENGTH, ##args, NULL);       \
        Distillery::debug::write_msg((lvl), aspect, expanded_args, __FUNCTION__, __FILE__,         \
                                     __LINE__, msg);                                               \
    } while (0)

#define SPCDBG_S(lvl, msg, aspect, args...)                                                        \
    SPCDBG_TST(                                                                                    \
      lvl, do { SPCDBG_NT((lvl), (msg), (aspect), ##args); } while (0))

// Because backward compatibility, SPCDBG macro behavior can not be changed.
// No like SPCLOG, APPTRC, etc new introduced macros for v2.1 release,
// SPCDBG has to keep same behavior with prior releases, so when we support
// more debug levels, we will leave actual checking to write_msg().
#define SPCDBG(lvl, stmt, aspect, args...)                                                         \
    SPCDBG_TST(                                                                                    \
      lvl, do {                                                                                    \
          std::stringstream __my_strm;                                                             \
          __my_strm << stmt;                                                                       \
          SPCDBG_NT((lvl), __my_strm.str(), (aspect), ##args);                                     \
      } while (0))

#define SPCLOG(lvl, msgKey, substitutionTexts, aspect)                                             \
    do {                                                                                           \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        assert(ilvl <= iL_INFO);                                                                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            Distillery::debug::write_log(ilvl, aspect, __FUNCTION__, __FILE__, __LINE__, (msgKey), \
                                         (substitutionTexts));                                     \
        }                                                                                          \
    } while (0)

#define SPCLOG_CODE(lvl, msgKey, aspect, code)                                                     \
    do {                                                                                           \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        assert(ilvl <= iL_INFO);                                                                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            std::vector<std::string> msgvars;                                                      \
            code;                                                                                  \
            Distillery::debug::write_log(ilvl, aspect, __FUNCTION__, __FILE__, __LINE__, (msgKey), \
                                         msgvars);                                                 \
        }                                                                                          \
    } while (0)

#define SPCLOG_NO_BUNDLE(lvl, msg, aspect)                                                         \
    do {                                                                                           \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        assert(ilvl <= iL_INFO);                                                                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            Distillery::debug::write_log(ilvl, aspect, __FUNCTION__, __FILE__, __LINE__, (msg));   \
        }                                                                                          \
    } while (0)

// Log DistilleryException and std::exception
#define SPCLOG_EX(lvl, msgKey, substitutionTexts, ex, aspect)                                      \
    do {                                                                                           \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        assert(ilvl <= iL_INFO);                                                                   \
        if (__builtin_expect((ilvl <= Distillery::debug::logger_level), 0)) {                      \
            Distillery::debug::write_log(ilvl, aspect, __FUNCTION__, __FILE__, __LINE__, (msgKey), \
                                         (substitutionTexts), (ex));                               \
        }                                                                                          \
    } while (0)

#define APPTRC(lvl, stmt, aspect)                                                                  \
    do {                                                                                           \
        assert(lvl <= L_WARN);                                                                     \
        assert(lvl >= L_ERROR);                                                                    \
        int ilvl = Distillery::debug::EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[lvl];                   \
        if (__builtin_expect((ilvl <= Distillery::debug::app_trace_level), 0)) {                   \
            std::stringstream __my_strm;                                                           \
            __my_strm << stmt;                                                                     \
            Distillery::debug::write_appmsg(ilvl, aspect, __FUNCTION__, __FILE__, __LINE__,        \
                                            __my_strm.str());                                      \
        }                                                                                          \
    } while (0)

// Auditing
#define SPCAUDIT(success, source, user, obj, op, msgKey, msgvars)                                    \
    do {                                                                                             \
        if (__builtin_expect((Distillery::debug::audit_level > iL_OFF), 0)) {                        \
            std::map<std::string, std::string> opParams;                                             \
            Distillery::debug::write_audit(Distillery::debug::STREAMS_AUDIT_TYPE_STANDARD,           \
                                           (success), "", (source), (user), (obj), (op), (msgKey),   \
                                           (msgvars), opParams);                                     \
        }                                                                                            \
        /**if (__builtin_expect((Distillery::debug::audit_level>iL_INFO),0) && msgs.size()>=2)     \
        {                                                                                          \
          Distillery::debug::write_audit(Distillery::debug::STREAMS_AUDIT_TYPE_ADVANCED,           \
                                            (success), "", (source), (user), (obj),                \
                                            (op), (msgKey), (msgvars), opParams) ;                 \
        } **/ \
    } while (0)

#define SPCAUDIT_WITH_INST(success, iid, source, user, obj, op, msgKey, msgvars)                   \
    do {                                                                                           \
        if (__builtin_expect((Distillery::debug::audit_level > iL_OFF), 0)) {                      \
            std::map<std::string, std::string> opParams;                                           \
            Distillery::debug::write_audit(Distillery::debug::STREAMS_AUDIT_TYPE_STANDARD,         \
                                           (success), (iid), (source), (user), (obj), (op),        \
                                           (msgKey), (msgvars), opParams);                         \
        }                                                                                          \
    } while (0)

// Auditing with some common code
#define SPCAUDIT_CODE(success, source, user, obj, op, msgKey, code)                                \
    do {                                                                                           \
        if (__builtin_expect((Distillery::debug::audit_level > iL_OFF), 0)) {                      \
            std::map<std::string, std::string> opParams;                                           \
            std::vector<std::string> msgvars;                                                      \
            code;                                                                                  \
            Distillery::debug::write_audit(Distillery::debug::STREAMS_AUDIT_TYPE_STANDARD,         \
                                           (success), "", (source), (user), (obj), (op), (msgKey), \
                                           msgvars, opParams);                                     \
        }                                                                                          \
    } while (0)

#define SPCAUDIT_CODE_WITH_INST(success, iid, source, user, obj, op, msgKey, code)                 \
    do {                                                                                           \
        if (__builtin_expect((Distillery::debug::audit_level > iL_OFF), 0)) {                      \
            std::map<std::string, std::string> opParams;                                           \
            std::vector<std::string> msgvars;                                                      \
            code;                                                                                  \
            Distillery::debug::write_audit(Distillery::debug::STREAMS_AUDIT_TYPE_STANDARD,         \
                                           (success), (iid), (source), (user), (obj), (op),        \
                                           (msgKey), msgvars, opParams);                           \
        }                                                                                          \
    } while (0)

#define freeWrite(level, fmt, args...)                                                             \
    SPCDBG_TST(level, Distillery::debug::lockFreeWrite(level, __FUNCTION__, __FILE__, __LINE__,    \
                                                       fmt, ##args))

#define HEX(h) "0x" << std::hex << h << std::dec
#define DEC(d) std::dec << (d)
#define OCT(o) "0" << std::oct << (o) << std::dec
#define QT(v) "'" << v << "'"
#define PVAR(v) " " << #v << "=" << v << " "
#endif

// Local Variables: ***
// mode:c++ ***
// End: ***
