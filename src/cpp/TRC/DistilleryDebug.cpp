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

#include <limits.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <typeinfo>
#include <unicode/udata.h>
#include <unicode/ustream.h>

#include <TRC/DistilleryDebug.h>
#include <TRC/DistilleryDebugLogger.h>
#include <TRC/TRCUtils.h>
#include <UTILS/Directory.h>
#include <UTILS/DistilleryException.h>
#include <UTILS/ResourceBundleHelper.h>
#include <UTILS/SupportFunctions.h>

UTILS_NAMESPACE_USE
DEBUG_NAMESPACE_USE
using namespace std;

#undef USE_CLOCK_GETTIME

string Distillery::debug::cmdline = "<unknown>";
string Distillery::debug::cmd = "<unknown>";
string Distillery::debug::hostname = "<unknown>";
string Distillery::debug::identity = "<unknown>";
string Distillery::debug::dynamic_aspects;
static const char* dynamic_aspects_c_str = NULL;

Mutex Distillery::debug::dbg_mutex;
Mutex Distillery::debug::log_mutex;
Mutex Distillery::debug::app_mutex;
Mutex Distillery::debug::stdouterr_mutex;
Mutex Distillery::debug::audit_mutex;

int Distillery::debug::logger_max_level = -1;
int Distillery::debug::spc_trace_level = -1;
int Distillery::debug::logger_level = -1;
int Distillery::debug::app_trace_level = -1;
int Distillery::debug::audit_level = -1;

static Tracer* streamsTracer = NULL;
static Logger* logLogger = NULL;
static Tracer* appTracer = NULL;
static AuditLogger* auditLogger = NULL;

int64_t TimeStamp::TOD_ms(void)
{
    int64_t res;

#if USE_CLOCK_GETTIME

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    res = ts.tv_sec;
    res *= 1000;
    res += ts.tv_nsec / 1000000;

#else

    struct timeval tv;
    gettimeofday(&tv, 0);
    res = tv.tv_sec;
    res *= 1000;
    res += tv.tv_usec / 1000;

#endif

    return res;
}

int64_t TimeStamp::TOD_us(void)
{
    int64_t res;

#if USE_CLOCK_GETTIME

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    res = ts.tv_sec;
    res *= 1000000;
    res += ts.tv_nsec / 1000;

#else

    struct timeval tv;
    gettimeofday(&tv, 0);
    res = tv.tv_sec;
    res *= 1000000;
    res += tv.tv_usec;

#endif

    return res;
}

TimeStamp::TimeStamp(bool absolute)
{
    if (absolute) {
        _offset = 0;
    } else {
        _offset = TimeStamp::TOD_ms();
    }
}

int64_t TimeStamp::getms() const
{
    return TimeStamp::TOD_ms() - _offset;
}

void TimeStamp::print(ostream& strm) const
{
    static Mutex mtx;
    AutoMutex lck(mtx);

    strm << getms() << "ms";
}

ostream& Distillery::debug::operator<<(ostream& strm, const TimeStamp& t)
{
    t.print(strm);
    return strm;
}

TimeStamp Distillery::debug::spcnow;
TimeStamp Distillery::debug::spcnowa(true);

pid_t Distillery::debug::distillery_gettid()
{
    return gettid();
}

int Distillery::debug::expand_args(char* buffer, int max_size, ...)
{
    int s = 0;

    va_list ap;
    va_start(ap, max_size);
    char* v = va_arg(ap, char*);
    while (v) {
        s += snprintf(buffer + s, max_size - s, " %s", v);
        v = va_arg(ap, char*);
    }
    va_end(ap);

    return s;
}

void Distillery::debug::add_dynamic_aspect(const string& new_aspect)
{
    if (dynamic_aspects.length()) {
        dynamic_aspects += ",";
    }

    string new_aspect_tmp = new_aspect;

    string::size_type pos = new_aspect_tmp.find_first_of(" ,");
    while (pos != string::npos) {
        new_aspect_tmp[pos] = '_';
        pos = new_aspect_tmp.find_first_of(" ,", pos + 1);
    }

    dynamic_aspects += new_aspect_tmp;
    if (dynamic_aspects.length()) {
        dynamic_aspects_c_str = dynamic_aspects.c_str();
    }
}

void Distillery::debug::fadd_dynamic_aspect(const char* fmt, ...)
{
    char buffer[SPCDBG_ARGS_MAX_LENGTH] = "";

    va_list ap;
    va_start(ap, fmt);

    vsnprintf(buffer, SPCDBG_ARGS_MAX_LENGTH, fmt, ap);

    va_end(ap);

    add_dynamic_aspect(buffer);
}

int Distillery::debug::getInternalDebugLevel(int level)
{
    int ilvl = 0;
    switch (level) {
        case L_OFF:
            ilvl = iL_OFF;
            break;
        case L_ERROR:
            ilvl = iL_ERROR;
            break;
        case L_WARN:
            ilvl = iL_WARN;
            break;
        case L_INFO:
            ilvl = iL_INFO;
            break;
        case L_DEBUG:
            ilvl = iL_DEBUG;
            break;
        case L_TRACE:
            ilvl = iL_TRACE;
            break;
        default:
            break;
    }
    return ilvl;
}

void Distillery::debug::set_streams_tracer(Tracer* l)
{
    AutoMutex lck(dbg_mutex);
    streamsTracer = l;
    if (NULL != streamsTracer) {
        spc_trace_level = streamsTracer->getLevel();

        // Set logger_max_level, which is needed for application compiled with prior
        // releases.
        switch (spc_trace_level) {
            case iL_OFF:
                logger_max_level = L_ERROR;
                break;
            case iL_ERROR:
                logger_max_level = L_ERROR;
                break;
            case iL_WARN:
                logger_max_level = L_ERROR;
                break;
            case iL_INFO:
                logger_max_level = L_INFO;
                break;
            case iL_DEBUG:
                logger_max_level = L_DEBUG;
                break;
            case iL_TRACE:
                logger_max_level = L_TRACE;
                break;
            default:
                break;
        }
    }
}

void Distillery::debug::set_streams_trclevel(int level)
{
    AutoMutex lck(dbg_mutex);
    if (NULL != streamsTracer) {
        streamsTracer->setLevel(level);
        spc_trace_level = level;
        // Set logger_max_level, which is needed for application compiled with prior
        // releases.
        switch (spc_trace_level) {
            case iL_OFF:
                logger_max_level = L_ERROR;
                break;
            case iL_ERROR:
                logger_max_level = L_ERROR;
                break;
            case iL_WARN:
                logger_max_level = L_ERROR;
                break;
            case iL_INFO:
                logger_max_level = L_INFO;
                break;
            case iL_DEBUG:
                logger_max_level = L_DEBUG;
                break;
            case iL_TRACE:
                logger_max_level = L_TRACE;
                break;
            default:
                break;
        }
    }
}

void Distillery::debug::set_log_logger(Logger* l)
{
    AutoMutex lck(log_mutex);
    logLogger = l;
    if (NULL != logLogger) {
        logger_level = logLogger->getLevel();
    }
}

void Distillery::debug::set_log_level(int level)
{
    // AutoMutex lck(log_mutex) ;
    logger_level = level;
    if (NULL != logLogger) {
        logLogger->setLevel(level);
    }
}

void Distillery::debug::set_app_tracer(Tracer* l)
{
    AutoMutex lck(app_mutex);
    appTracer = l;
    if (NULL != appTracer) {
        app_trace_level = appTracer->getLevel();
    }
}

void Distillery::debug::set_app_trclevel(int level)
{
    AutoMutex lck(app_mutex);
    app_trace_level = level;
    if (NULL != appTracer) {
        appTracer->setLevel(level);
    }
}

void Distillery::debug::set_audit_logger(AuditLogger* l)
{
    AutoMutex lck(audit_mutex);
    auditLogger = l;
}

void Distillery::debug::set_audit_level(int level)
{
    // AutoMutex lck(audit_mutex) ;
    audit_level = level;
}

void Distillery::debug::write_msg(int level,
                                  const string& aspect,
                                  const string& args,
                                  const string& function,
                                  const string& file,
                                  int line,
                                  const string& msg)
{
    int ilevel = EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[level];
    if (streamsTracer) {
        streamsTracer->writeMessage(ilevel, aspect, args, function, file, line, msg);
    }
}

static UnicodeString bundle_msg(const string& key, const vector<string>& substitutionTexts)
{
    ResourceBundleHelper rb;
    return PrintMessageToString(rb, key, substitutionTexts);
}

void Distillery::debug::write_log(int level,
                                  const string& aspect,
                                  const string& function,
                                  const string& file,
                                  int line,
                                  const string& msg)
{
    if (logLogger) {
        // Note: Do not change this interface call. As SPL standalone application
        //      Need this same interface to match logging and trace format.
        logLogger->writeMessage(level, aspect, "", function, file, line, msg);
    }
}

void Distillery::debug::write_log(int level,
                                  const string& aspect,
                                  const string& function,
                                  const string& file,
                                  int line,
                                  const string& key,
                                  const vector<string>& substitutionTexts)
{
    if (logLogger) {
        try {
            logLogger->writeMessage(aspect, key, substitutionTexts);
        } catch (const Distillery::DistilleryException& e) {
            cerr << "Can not write log with error: " << e << endl;
        }
    }
}

void Distillery::debug::write_log(int level,
                                  const string& aspect,
                                  const string& function,
                                  const string& file,
                                  int line,
                                  const string& key,
                                  const vector<string>& substitutionTexts,
                                  const Distillery::DistilleryException& e)
{
    try {
        const string msgId = e.getExceptionCode().getMessageId();
        string errorMessage;
        if (msgId == DistilleryExceptionCode::NO_MESSAGE_ID) {
            errorMessage = e.getExplanation();
        } else {
            // Write "errorCode : formatted message"
            UnicodeString unMsg = bundle_msg(msgId, e.getSubstitutionText());
            std::stringstream s;
            s << unMsg;
            errorMessage.append(e.getExceptionCode().getMessageId()).append(" : ").append(s.str());
        }
        if (e.hasAdditionalData()) {
            // TODO additional data (see st-helper.cpp printRuntimeErrorMessage())
        }
        vector<string> st(substitutionTexts);
        st.push_back(errorMessage);
        write_log(level, aspect, function, file, line, key, st);
    } catch (const Distillery::DistilleryException& de) {
        cerr << "Can not write log with error: " << de << endl;
    }
}

void Distillery::debug::write_log(int level,
                                  const string& aspect,
                                  const string& function,
                                  const string& file,
                                  int line,
                                  const string& key,
                                  const vector<string>& substitutionTexts,
                                  const std::exception& e)
{
    vector<string> st(substitutionTexts);
    st.push_back(e.what());
    write_log(level, aspect, function, file, line, key, st);
}

void Distillery::debug::write_appmsg(int level,
                                     const string& aspect,
                                     const string& function,
                                     const string& file,
                                     int line,
                                     const string& msg)
{
    if (appTracer) {
        appTracer->writeMessage(level, aspect, "", function, file, line, msg);
    }
}

void Distillery::debug::write_audit(const string& type,
                                    bool success,
                                    const string& iid,
                                    const string& source,
                                    const string& user,
                                    const string& object,
                                    const string& operation,
                                    const string& msgId,
                                    const vector<string>& msgvars,
                                    const map<string, string>& opParams)
{
    if (auditLogger) {
        auditLogger->writeMessage(type, success, iid, source, user, object, operation, msgId,
                                  msgvars, opParams);
    }
}

// Write to file 1 without getting any mutexes or other locks
void Distillery::debug::lockFreeWrite(int level,
                                      const char* function,
                                      const char* file,
                                      int line,
                                      const char* fmt,
                                      ...)
{
    int ilevel = EXTERNAL_DEBUG_LEVEL_MAP_TO_INTERNAL[level];
    va_list ap;
    char buffer[4096];

    va_start(ap, fmt);
    int len = snprintf(buffer, sizeof buffer, "DEBUG%d (%d, %s %s:%d) ", ilevel, getpid(), function,
                       file, line);
    len += vsnprintf(buffer + len, (sizeof buffer) - len, fmt, ap);
    va_end(ap);
    buffer[len] = '\n';
    write(1, buffer, len + 1);
    fsync(1);
} /* End lockFreeWrite                  */

Logger::Logger(int level)
  : _level(level)
  , _useAutoMutex(true)
  , _sslOption("")
{}

Logger::~Logger() {}

void Logger::setUseAutoMutex(bool useAutoMutex)
{
    _useAutoMutex = useAutoMutex;
}

void Logger::setSSLOption(const string& sslOption)
{
    _sslOption = sslOption;
}

int Logger::increaseLevel(void)
{
    _level++;
    return _level;
}

int Logger::getLevel(void)
{
    return _level;
}

int Logger::setLevel(int new_level)
{
    int old_level = _level;
    _level = new_level;
    return old_level;
}

void Logger::writeMessage(const string& aspect,
                          const string& key,
                          const vector<string>& substitutionTexts)
{}

Tracer::Tracer(int level)
  : Logger(level)
{}

Tracer::~Tracer() {}

void Tracer::writeMessage(int level,
                          const string& aspect,
                          const string& args,
                          const string& function,
                          const string& file,
                          int line,
                          const string& message)
{
    // Tracer will not use this method, so nothing here.
}

void Tracer::writeMessage(const string& msg)
{
    // Method for some specific tracer, do nothing here at base class.
}

string Tracer::getMessages()
{
    // Method for some specific tracer, do nothing here at base class.
    return string("");
}

void Tracer::removeTraceFile()
{
    // Method for some specific tracer, do nothing here at base class.
}

AuditLogger::AuditLogger()
  : Logger(-1)
{}

AuditLogger::~AuditLogger() {}

void AuditLogger::writeMessage(int level,
                               const string& aspect,
                               const string& args,
                               const string& function,
                               const string& file,
                               int line,
                               const string& message)
{
    // AuditLogger will not use this method, so nothing here.
}

void AuditLogger::writeMessage(const string& type,
                               bool success,
                               const string& iid,
                               const string& source,
                               const string& user,
                               const string& object,
                               const string& operation,
                               const string& msgId,
                               const vector<string>& msgvars,
                               const map<string, string>& opParams)
{}
