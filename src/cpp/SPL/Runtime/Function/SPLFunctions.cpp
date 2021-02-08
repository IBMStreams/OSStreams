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

#include <SPL/Runtime/Function/BuiltinSPLFunctions.h>
#include <SPL/Runtime/Type/XmlInternal.h>

#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/Metric.h>
#include <SPL/Runtime/Common/MetricImpl.h>
#include <SPL/Runtime/Common/RuntimeException.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Operator/OperatorImpl.h>
#include <SPL/Runtime/Operator/OperatorMetricsHandler.h>
#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>
#include <SPL/Runtime/Operator/OperatorTracker.h>
#include <SPL/Runtime/Operator/Port/FilterEvaluator.h>
#include <SPL/Runtime/Operator/Port/OperatorInputPortImpl.h>
#include <SPL/Runtime/Operator/Port/OperatorOutputPortImpl.h>
#include <SPL/Runtime/Operator/Port/StreamProperty.h>
#include <SPL/Runtime/Operator/Port/StreamPropertyCollection.h>
#include <SPL/Runtime/Operator/Port/SubscriptionExpression.h>
#include <SPL/Runtime/Operator/State/ConsistentRegionContext.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Serialization/StreamByteBuffer.h>
#include <SPL/Runtime/Type/Meta/Type.h>
#include <SPL/Runtime/Type/UStringImpl.h>
#include <SPL/Runtime/Type/XmlHelper.h>
#include <SPL/Runtime/Utility/FormattableMessage.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>
#include <SPL/Runtime/Utility/MessageFormatter.h>
#include <SPL/Runtime/Utility/RuntimeUtility.h>
#include <SPL/Runtime/Window/TriggerInfoProvider.h>
#include <SPL/Runtime/Window/WindowCommon.h>
#include <SPL/Runtime/Window/WindowTracker.h>
#include <UTILS/DistilleryApplication.h>
#include <UTILS/HostInfo.h>
#include <UTILS/SupportFunctions.h>

#include <cerrno>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>

#include <boost/filesystem/path.hpp>
#include <boost/iostreams/concepts.hpp>
#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/stream_buffer.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/scoped_array.hpp>
#include <boost/shared_ptr.hpp>

#include <unicode/numfmt.h>
#include <unicode/parseerr.h>

#define THOUSAND 1000
#define MILLION 1000000
#define BILLION 1000000000

using namespace std;

namespace SPL {
// Used in SPLCast.h
// const string badComplexCast(const SPL::List& l)
// {
//     return badComplexCastMsg(l);
// }

const FormattableMessage badComplexCastMsg(const SPL::List& l)
{
    ostringstream os;
    os.imbue(locale::classic());
    os << l;
    return SPL_RUNTIME_INVALID_LIST_CAST_TO_COMPLEX(os.str());
}

// Used in BList.h and BString.h and Blob.cpp
void indexOutOfBound(size_t idx, int32_t lsize)
{
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                        SPL_APPLICATION_RUNTIME_INVALID_INDEX(idx, lsize - 1), SPL_FUNC_DBG);
}

namespace Functions {
namespace Utility {

class auto_array
{
    char* _p;

  public:
    auto_array(uint32_t l)
      : _p(new char[l])
    {}
    ~auto_array() { delete[] _p; }
    char* get() const { return _p; }
};

SPL::rstring mktemp(const SPL::rstring& templat)
{
    auto_array temp(templat.size() + 1);
    strcpy(temp.get(), templat.c_str());
    int r = ::mkstemp(temp.get());
    if (r == -1) {
        return "";
    }
    close(r);
    return temp.get();
}

SPL::rstring formatNumber(const SPL::int64 value)
{
    char buffer[1000];
#ifdef __LP64__
    snprintf(buffer, sizeof buffer - 1, "%I'ld", value);
#else
    snprintf(buffer, sizeof buffer - 1, "%I'lld", value);
#endif
    return buffer;
}

SPL::rstring formatNumber(const SPL::int64 value, SPL::uint32 width)
{
    char buffer[1000];
    if (width < sizeof buffer - 1) {
#ifdef __LP64__
        snprintf(buffer, sizeof buffer - 1, "%I'*ld", width, value);
#else
        snprintf(buffer, sizeof buffer - 1, "%I'*lld", width, value);
#endif
        return buffer;
    }
    auto_array ap(width + 10);
#ifdef __LP64__
    snprintf(ap.get(), width + 9, "%I'*ld", width, value);
#else
    snprintf(ap.get(), width + 9, "%I'*lld", width, value);
#endif
    return ap.get();
}

SPL::rstring formatNumber(const SPL::float64 value,
                          SPL::uint32 width,
                          SPL::uint32 precision,
                          bool scientific)
{
    const char* format = scientific ? "%'*.*e" : "%'*.*f";
    char buffer[1000];
    if (width < sizeof buffer - 1) {
        snprintf(buffer, sizeof buffer - 1, format, width, precision, value);
        return buffer;
    }
    auto_array ap(width + 10);
    snprintf(ap.get(), width + 9, format, width, precision, value);
    return ap.get();
}

SPL::rstring formatNumber(const SPL::float64 value)
{
    char buffer[1000];
    snprintf(buffer, sizeof buffer - 1, "%'g", value);
    return buffer;
}

SPL::int32 parseNumber(SPL::int64& number, SPL::ustring const& text)
{
    UErrorCode status = U_ZERO_ERROR;
    auto_ptr<NumberFormat> fmt(NumberFormat::createInstance(status));
    fmt->setParseIntegerOnly(true);
    assert(U_SUCCESS(status));
    Formattable result;
    ParsePosition pos(0);
    fmt->parse(text.impl(), result, pos);
    if (pos.getIndex() == 0) {
        return -1;
    }
    number = result.getInt64(status);
    if (!U_SUCCESS(status)) {
        return -1;
    }
    return pos.getIndex();
}

SPL::int32 parseNumber(SPL::int64& number, SPL::rstring const& text)
{
    return parseNumber(number, text, 10);
}

SPL::int32 parseNumber(SPL::int64& number, SPL::rstring const& text, SPL::int32 base)
{
    if (base != 0 && (base < 2 || base > 36)) {
        return -1;
    }

    const char* startP = text.c_str();
    char* endP = NULL;
    errno = 0;
    SPL::int64 result = ::strtoll(startP, &endP, base);
    if (startP == endP || errno != 0) { // something went wrong
        return -1;
    }
    number = result;
    return endP - startP; // return number of characters converted
}

SPL::int32 parseNumber(SPL::float64& number, SPL::ustring const& text)
{
    UErrorCode status = U_ZERO_ERROR;
    auto_ptr<NumberFormat> fmt(NumberFormat::createInstance(status));
    assert(U_SUCCESS(status));
    Formattable result;
    ParsePosition pos(0);
    fmt->parse(text.impl(), result, pos);
    if (pos.getIndex() == 0) {
        return -1;
    }
    number = result.getDouble(status);
    if (!U_SUCCESS(status)) {
        return -1;
    }
    return pos.getIndex();
}

SPL::int32 parseNumber(SPL::float64& number, SPL::rstring const& text)
{
    const char* startP = text.c_str();
    char* endP = NULL;
    errno = 0;
    SPL::float64 result = ::strtod(startP, &endP);
    if (startP == endP || errno != 0) { // something went wrong
        return -1;
    }
    number = result;
    return endP - startP; // return number of characters converted
}

SPL::rstring strerror(const SPL::int32 errnum)
{
    char buffer[1024];
    // This code is linux specific and may need to be updated for some systems
    // (Actually, strerror_r is in POSIX)
    char* e = strerror_r(errnum, buffer, sizeof buffer);
    return e;
}

SPL::rstring getEnvironmentVariable(SPL::rstring const& name, SPL::boolean& isSet)
{
    char* nsstr = getenv(name.c_str());
    if (nsstr) {
        isSet = true;
        return nsstr;
    } else {
        isSet = false;
        return "";
    }
}

SPL::rstring getEnvironmentVariable(SPL::rstring const& name, SPL::rstring const& defaultValue)
{
    SPL::boolean isSet = false;
    SPL::rstring result = getEnvironmentVariable(name, isSet);
    if (isSet) {
        return result;
    } else {
        return defaultValue;
    }
}

SPL::rstring getEnvironmentVariable(SPL::rstring const& name)
{
    SPL::boolean isSet = false;
    return getEnvironmentVariable(name, isSet);
}

SPL::boolean isShutdown()
{
    return ::SPL::PEImpl::instance().getShutdownRequested();
}

Mutex shutdownMutex;
void shutdownPE()
{
    {
        AutoMutex am(shutdownMutex);
        static boolean shutdownInProgress = false;
        if (shutdownInProgress) {
            return;
        }
        shutdownInProgress = true;
    }
    fflush(NULL);
    ::SPL::PEImpl::instance().shutdownFromWithinOperators();
}

void block(double const time)
{
    ::SPL::PEImpl::instance().blockUntilShutdownRequest(time);
}

void realAssert(const string& message, const string& file, uint32_t line)
{
    fflush(NULL);
    string const& opName =
      SPL::PEImpl::instance().getOperatorName(SPL::OperatorTracker::getCurrentOperator());
    ostringstream ostr;
    ostr.imbue(locale::classic());
    ostr << "Operator \"" << opName << "\", file \"" << file << "\", line " << line << ": ";
    ostr << message;
    if (!SPL::PEImpl::instance().isStandalone()) {
        SPLAPPTRC(L_ERROR, ostr.str(), SPL_FUNC_DBG);
    }
    cerr << ostr.str() << endl;
    Distillery::DistilleryApplication::_thisApp->removeSignalHandler(SIGABRT);
    ::abort();
}

static int32_t mapLogLevel(const Enum& level)
{
    // Defined as enum {error, info, trace, debug}
    // Map to new usage
    switch (level.getIndex()) {
        case 0:
            return iL_ERROR;
            break;
        case 1:
            return iL_INFO;
            break;
        case 2:
            return iL_DEBUG;
            break;
        case 3:
            return iL_TRACE;
            break;
    }
    return iL_ERROR;
}

static int32_t mapAppLogLevel(const Enum& level)
{
    // Defined as enum {error, warn, info}
    // Map to new usage
    switch (level.getIndex()) {
        case 0:
            return iL_ERROR;
            break;
        case 1:
            return iL_WARN;
            break;
        case 2:
            return iL_INFO;
            break;
    }
    return iL_ERROR;
}

static int32_t mapAppTrcLevel(const Enum& level)
{
    // Defined as enum {error, warn, info, trace, debug}
    // Map to new usage
    switch (level.getIndex()) {
        case 0:
            return iL_ERROR;
            break;
        case 1:
            return iL_WARN;
            break;
        case 2:
            return iL_INFO;
            break;
        case 3:
            return iL_DEBUG;
            break;
        case 4:
            return iL_TRACE;
            break;
    }
    return iL_ERROR;
}

void log(const Enum& level,
         const string& message,
         const string& aspect,
         const string& file,
         uint32_t line)
{
    int32_t l = mapLogLevel(level);
    if (l <= Distillery::debug::app_trace_level) {
        Distillery::debug::write_appmsg(l, SPL::spllogAspect(aspect), "log", file, line, message);
    }
}

void appLog(const Enum& level,
            const string& message,
            const string& aspect,
            const string& file,
            uint32_t line)
{
    int32_t l = mapAppLogLevel(level);
    if (l <= Distillery::debug::logger_level) {
        Distillery::debug::write_log(l, SPL::splAppLogAspect(aspect), "appLog", file, line,
                                     message);
    }
}

SPL::boolean isLoggable(const Enum& level)
{
    int32_t l = mapAppLogLevel(level);
    return (l <= Distillery::debug::logger_level);
}

void appTrc(const Enum& level,
            const string& message,
            const string& aspect,
            const string& file,
            uint32_t line)
{
    int32_t l = mapAppTrcLevel(level);
    if (l <= Distillery::debug::app_trace_level) {
        Distillery::debug::write_appmsg(l, SPL::splAppTrcAspect(aspect), "appTrc", file, line,
                                        message);
    }
}

SPL::boolean isTraceable(const Enum& level)
{
    int32_t l = mapAppTrcLevel(level);
    return (l <= Distillery::debug::app_trace_level);
}

static SPL::OperatorImpl& getCurrentOperator()
{
    using namespace SPL;
    uint32_t index = OperatorTracker::getCurrentOperator();
    vector<OperatorImpl*> const& ops = PEImpl::instance().getOperators();
    if (index >= ops.size()) { // do not use an assert here (in case this does happen)
        THROW(SPLRuntime, "Cannot happen: " << SPL_RUNTIME_INVALID_OPERATOR_INDEX(index));
    }
    return *ops[index];
}

SPL::int32 getChannel()
{
    return getCurrentOperator().getContextImpl().getChannel();
}

SPL::int32 getLocalChannel()
{
    return getCurrentOperator().getContextImpl().getLocalChannel();
}

SPL::list<SPL::int32> getAllChannels()
{
    return getCurrentOperator().getContextImpl().getAllChannels();
}

SPL::int32 getMaxChannels()
{
    return getCurrentOperator().getContextImpl().getMaxChannels();
}

SPL::int32 getLocalMaxChannels()
{
    return getCurrentOperator().getContextImpl().getLocalMaxChannels();
}

SPL::list<SPL::int32> getAllMaxChannels()
{
    return getCurrentOperator().getContextImpl().getAllMaxChannels();
}

SPL::rstring getThisOperatorName()
{
    return getCurrentOperator().getContextImpl().getName();
}

SPL::rstring getThisOperatorLogicalName()
{
    return getCurrentOperator().getContextImpl().getLogicalName();
}

SPL::boolean isControlInputPort(uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nports), SPL_FUNC_DBG);
    }
    return oper.getInputPortAt(port).isControl();
}

SPL::boolean isConsistentInputPort(uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nports), SPL_FUNC_DBG);
    }
    if (oper.getConsistentRegionContext() == NULL) {
        return false;
    }
    return oper.getConsistentRegionContext()->isConsistentInputPort(port);
}

SPL::boolean isInConsistentRegion()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return false;
    }
    return true;
}

SPL::boolean isStartOfConsistentRegion()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return false;
    }
    return oper.getConsistentRegionContext()->isStartOfRegion();
}

SPL::boolean isEndOfConsistentRegion()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return false;
    }
    return oper.getConsistentRegionContext()->isEndOfRegion();
}

SPL::boolean isTriggerOfConsistentRegion()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return false;
    }
    return oper.getConsistentRegionContext()->isTriggerOperator();
}

SPL::int32 getConsistentRegionIndex()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return -1;
    }
    return oper.getConsistentRegionContext()->getIndex();
}

SPL::float64 getDrainTimeout()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return -1.0f;
    }
    return oper.getConsistentRegionContext()->getDrainTimeout();
}

SPL::float64 getResetTimeout()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return -1.0f;
    }
    return oper.getConsistentRegionContext()->getResetTimeout();
}

SPL::int64 getSequenceId()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return -1;
    }
    return oper.getConsistentRegionContext()->getSequenceId();
}

SPL::int32 getResetAttempt()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return -1;
    }
    return oper.getConsistentRegionContext()->getResetAttempt();
}

void resetConsistentRegion()
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    if (oper.getConsistentRegionContext() == NULL) {
        return;
    }
    /// TODO: Wrap exception
    oper.getConsistentRegionContext()->resetRegion();
}

SPL::boolean evalPredicate(const SPL::rstring& splExpr, const Tuple& tuple, SPL::int32& error)
{
    // We assume that a common use of this function is to evaluate the same expression on each
    // tuple that comes to an operator. We cache the results of FilterExpression::fromString()
    // and FilterClause::create() because the difference in performance is close to 30x for
    // what we assume is a common use.
    typedef std::tr1::unordered_map<SPL::rstring, Filter::FilterClause*> ExpressionCache;
    static __thread ExpressionCache* expCache = NULL;
    if (expCache == NULL) {
        expCache = new ExpressionCache;
    }

    error = 0;

    ExpressionCache::iterator it = expCache->find(splExpr);
    if (it == expCache->end()) {
        FilterExpressionPtr exp = FilterExpression::fromString(tuple, splExpr);
        if (exp.get() == NULL) {
            error = 1;
            return false;
        }

        Filter::FilterClause* filter = NULL;
        try {
            filter = &Filter::FilterClause::create(*exp);
        } catch (...) {
            error = 2;
            return false;
        }

        // Yes, 100 is a magic number to use. It seems large enough, but not too large. We
        // can revisit if it ever becomes an issue.
        if (expCache->size() > 100) {
            for (ExpressionCache::iterator jt = expCache->begin(); jt != expCache->end(); ++jt) {
                delete jt->second;
            }
            expCache->clear();
        }
        std::pair<ExpressionCache::iterator, bool> res =
          expCache->insert(std::make_pair(splExpr, filter));
        assert(res.second);
        it = res.first;
    }

    return it->second->evaluate(tuple);
}

void getInputPortMetricValue(SPL::uint32 port, int nameEnum, SPL::int64& value)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_RUNTIME_INPUT_PORT_INDEX_OUT_OF_RANGE(port, nports), SPL_FUNC_DBG);
    }
    value = oper.getContextImpl()
              .getMetricsImpl()
              .getInputPortMetric(port, SPL::OperatorMetrics::InputPortMetricName(nameEnum))
              .getValueNoLock();
}

void getOutputPortMetricValue(SPL::uint32 port, int nameEnum, SPL::int64& value)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfOutputPorts();
    if (port >= nports) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidIndex, L_ERROR,
                            SPL_RUNTIME_OUTPUT_PORT_INDEX_OUT_OF_RANGE(port, nports), SPL_FUNC_DBG);
    }
    value = oper.getContextImpl()
              .getMetricsImpl()
              .getOutputPortMetric(port, SPL::OperatorMetrics::OutputPortMetricName(nameEnum))
              .getValueNoLock();
}

void getOperatorMetricValue(int nameEnum, SPL::int64& value)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    value = oper.getContextImpl()
              .getMetricsImpl()
              .getOperatorMetric(SPL::OperatorMetrics::SystemMetricName(nameEnum))
              .getValueNoLock();
}

void getCustomMetricValue(SPL::rstring const& name, SPL::int64& value)
{
    using namespace SPL;
    SPL::OperatorImpl& oper = getCurrentOperator();
    SPL::OperatorMetricsHandler* handler = oper.getContextImpl().getOperatorMetricsHandler();
    if (handler == NULL) {
        value = oper.getContext().getMetrics().getCustomMetricByName(name).getValue();
    } else {
        value = handler->getCustomMetricValue(name);
    }
}

void setCustomMetricValue(SPL::rstring const& name, SPL::int64 value)
{
    using namespace SPL;
    SPL::OperatorImpl& oper = getCurrentOperator();
    SPL::OperatorMetricsHandler* handler = oper.getContextImpl().getOperatorMetricsHandler();
    if (handler == NULL) {
        oper.getContext().getMetrics().getCustomMetricByName(name).setValue(value);
    } else {
        handler->setCustomMetricValue(name, value);
    }
}

void createCustomMetric(SPL::rstring const& name,
                        SPL::rstring const& description,
                        const Enum& kind,
                        SPL::int64 value)
{
    using namespace SPL;
    SPL::OperatorImpl& oper = getCurrentOperator();
    SPL::OperatorMetricsHandler* handler = oper.getContextImpl().getOperatorMetricsHandler();
    MetricImpl m(name, description, static_cast<Metric::Kind>(kind.getIndex()));
    if (handler == NULL) {
        OperatorMetrics& om = oper.getContext().getMetrics();
        om.createCustomMetric(m.getName(), m.getDescription(), m.getKind());
        om.getCustomMetricByName(m.getName()).setValue(value);
    } else {
        handler->createCustomMetric(m.getName(), m.getDescription(), m.getKind(), value);
        SPL::PEImpl::instance().getPlatform().addCustomMetric(
          m.getName(), m.getKindName(), m.getDescription(), oper.getContext().getName());
    }
}

SPL::list<SPL::rstring> getCustomMetricNames()
{
    SPL::OperatorImpl& oper = getCurrentOperator();
    SPL::OperatorMetricsHandler* handler = oper.getContextImpl().getOperatorMetricsHandler();
    vector<string> metricNames;
    if (handler == NULL) {
        metricNames = oper.getContext().getMetrics().getCustomMetricNames();
    } else {
        metricNames = handler->getCustomMetricNames();
    }

    SPL::list<SPL::rstring> res;
    res.reserve(metricNames.size());
    vector<string>::const_iterator it;
    for (it = metricNames.begin(); it != metricNames.end(); it++) {
        res.push_back(*it);
    }
    return res;
}

bool hasCustomMetric(SPL::rstring const& name)
{
    using namespace SPL;
    SPL::OperatorImpl& oper = getCurrentOperator();
    SPL::OperatorMetricsHandler* handler = oper.getContextImpl().getOperatorMetricsHandler();
    if (handler == NULL) {
        return oper.getContext().getMetrics().hasCustomMetric(name);
    } else {
        return handler->hasCustomMetric(name);
    }
}

SPL::rstring getIPAddress()
{
    using namespace Distillery;
    return HostInfo(HostInfo::THIS_HOST).getStringIPAddress();
}

SPL::rstring getConfiguredHostName()
{
    using namespace Distillery;
    return HostInfo::host2name(HostInfo(HostInfo::THIS_HOST).getHostName());
}

SPL::rstring getHostName()
{
    static long hnsz = sysconf(_SC_HOST_NAME_MAX);
    if (hnsz <= 0) {
        hnsz = 1024;
    }

    boost::scoped_array<char> hn(new char[hnsz]);
    // We want the real hostname here, not HostInfo::THIS_HOST
    int rc = gethostname(hn.get(), hnsz);
    if (rc) {
        SPLTRACEMSGANDTHROW(SPL::SPLRuntime, L_ERROR,
                            SPL_RUNTIME_GETHOSTNAME_FAILED(SPL::RuntimeUtility::getErrorNoStr()),
                            SPL_FUNC_DBG);
    }
    return SPL::rstring(hn.get());
}

static void assignString(Tuple& t, const string& attr, const string& value)
{
    ValueHandle av = t.getAttributeValue(attr);
    assert(av.getMetaType() == Meta::Type::RSTRING);
    rstring& rs = (rstring&)av;
    rs = value;
}

SPL::int32 getOutputPortExportProperties(List& properties, uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfOutputPorts();
    if (port >= nports) {
        return 1; // bad port
    }
    OperatorOutputPortImpl& op = oper.getOutputPortAt(port);
    if (op.getExportType() != OperatorOutputPort::ByProperties) {
        return 1; // bad port
    }

    // Fetch the information, and return to the user
    StreamPropertyCollection props;
    properties.removeAllElements();
    op.getStreamProperties(props);
    StreamPropertyCollection::const_iterator it;
    assert(properties.getElementMetaType() == Meta::Type::TUPLE);
    for (it = props.begin(); it != props.end(); it++) {
        ValueHandle vh = properties.createElement();
        // Populate the tuple
        Tuple& t = (Tuple&)vh;
        string type;
        switch (it->getType().getMetaType()) {
            case Meta::Type::INT64:
                type = "int64";
                break;
            case Meta::Type::FLOAT64:
                type = "float64";
                break;
            case Meta::Type::RSTRING:
                type = "rstring";
                break;
            case Meta::Type::LIST: {
                const List& l = (const List&)it->getConstValueHandle();
                switch (l.getElementMetaType()) {
                    case Meta::Type::INT64:
                        type = "list<int64>";
                        break;
                    case Meta::Type::FLOAT64:
                        type = "list<float64>";
                        break;
                    case Meta::Type::RSTRING:
                        type = "list<rstring>";
                        break;
                    default:
                        break;
                }
            } break;
            default:
                break;
        }
        assignString(t, "name", it->getName());
        assignString(t, "value", it->getValueString());
        assignString(t, "typ", type);
        properties.pushBack(vh);
    }

    return 0;
}

static SPL::int32 setProperties(OperatorOutputPortImpl& op, const vector<StreamProperty>& props)
{
    try {
        // We have to remove all the old ones, and replace them with the new ones
        StreamPropertyCollection spc;
        op.getStreamProperties(spc);
        if (spc.size()) {
            vector<string> names;
            StreamPropertyCollection::const_iterator it;
            for (it = spc.begin(); it != spc.end(); it++) {
                names.push_back(it->getName());
            }
            op.removeStreamProperties(names);
        }
        // Now assign the new values
        spc = props;
        op.addStreamProperties(spc);
    } catch (...) {
        return 3; // Set of properties failed
    }
    // All worked
    return 0;
}

SPL::int32 setOutputPortExportProperties(const List& properties, uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfOutputPorts();
    if (port >= nports) {
        return 1; // bad port
    }
    OperatorOutputPortImpl& op = oper.getOutputPortAt(port);
    if (op.getExportType() != OperatorOutputPort::ByProperties) {
        return 1; // bad port
    }
    vector<StreamProperty> props;

    // Walk the list<tuple> and create the properties
    if (properties.getElementMetaType() != Meta::Type::TUPLE) {
        return 2; // bad properties
    }

    // detect name clashes
    tr1::unordered_set<rstring> names;

    // Walk the list
    ConstListIterator it = properties.getBeginIterator();
    ConstListIterator end = properties.getEndIterator();
    for (; it != end; it++) {
        ConstValueHandle tv = *it;
        const Tuple& t = (const Tuple&)tv;
        ConstTupleIterator tEnd = t.getEndIterator();
        ConstTupleIterator it2 = t.findAttribute("name");
        if (it2 == tEnd) {
            return 2; // bad properties
        }
        const rstring& name = (const rstring&)(*it2).getValue();
        if (names.count(name) > 0) {
            return 2; // bad properties
        }
        names.insert(name);
        it2 = t.findAttribute("typ");
        if (it2 == tEnd) {
            return 2; // bad properties
        }
        const rstring& type = (const rstring&)(*it2).getValue();
        it2 = t.findAttribute("value");
        if (it2 == tEnd) {
            return 2; // bad properties
        }
        const rstring& value = (const rstring&)(*it2).getValue();
        istringstream s(value);
        if (type == "int64") {
            int64 v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else if (type == "float64") {
            float64 v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else if (type == "rstring") {
            rstring v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else if (type == "list<int64>") {
            list<int64> v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else if (type == "list<float64>") {
            list<float64> v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else if (type == "list<rstring>") {
            list<rstring> v;
            s >> v;
            props.push_back(StreamProperty(name, v));
        } else {
            return 2; // bad properties tuple
        }

        // Did we convert successfully?
        if (!s) {
            return 2; // bad properties tuple - conversion failed
        }
    }

    // We have now walked the list<tuple>, and added the properties
    return setProperties(op, props);
}

SPL::int32 setOutputPortExportProperties(const Tuple& properties, uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfOutputPorts();
    if (port >= nports) {
        return 1; // bad port
    }
    OperatorOutputPortImpl& op = oper.getOutputPortAt(port);
    if (op.getExportType() != OperatorOutputPort::ByProperties) {
        return 1; // bad port
    }
    vector<StreamProperty> props;

    // Walk the tuple and create the properties
    for (uint32_t i = 0, ui = properties.getNumberOfAttributes(); i < ui; i++) {
        const string& n = properties.getAttributeName(i);
        ConstValueHandle pv = properties.getAttributeValue(i);
        const Meta::Type& t = pv.getMetaType();
        switch (t.getValue()) {
            case Meta::Type::INT64:
                props.push_back(StreamProperty(n, (const SPL::int64&)pv));
                break;
            case Meta::Type::FLOAT64:
                props.push_back(StreamProperty(n, (const SPL::float64&)pv));
                break;
            case Meta::Type::RSTRING:
                props.push_back(StreamProperty(n, (const SPL::rstring&)pv));
                break;
            case Meta::Type::LIST:
            case Meta::Type::BLIST: {
                const List& l = (const List&)pv;
                const Meta::Type& e = l.getElementMetaType();
                list<int64> ilist;
                list<float64> flist;
                list<rstring> slist;
                const Meta::Type::Value et = e.getValue();
                for (uint32_t j = 0, uj = l.getSize(); j < uj; j++) {
                    ConstValueHandle ev = l.getElement(j);
                    switch (et) {
                        case Meta::Type::INT64:
                            ilist.push_back((const SPL::int64&)ev);
                            break;
                        case Meta::Type::FLOAT64:
                            flist.push_back((const SPL::float64&)ev);
                            break;
                        case Meta::Type::RSTRING:
                            slist.push_back((const SPL::rstring&)ev);
                            break;
                        default:
                            return 2; // invalid properties tuple
                    }
                }
                // now create the property
                switch (et) {
                    case Meta::Type::INT64:
                        props.push_back(StreamProperty(n, ilist));
                        break;
                    case Meta::Type::FLOAT64:
                        props.push_back(StreamProperty(n, flist));
                        break;
                    case Meta::Type::RSTRING:
                        props.push_back(StreamProperty(n, slist));
                        break;
                    default:
                        break;
                }
            } break;
            default:
                return 2; // bad properties tuple
        }
    }

    // We have now walked the tuple, and added the properties
    return setProperties(op, props);
}

SPL::rstring getInputPortImportSubscription(uint32 port, SPL::int32& error)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        error = 1;
        return ""; // bad port
    }
    OperatorInputPortImpl& ip = oper.getInputPortAt(port);
    if (ip.getImportType() != OperatorInputPort::BySubscription) {
        error = 1;
        return ""; // bad port
    }
    // Worked
    error = 0;
    return ip.getSubscriptionExpression()->toString();
}

SPL::int32 setInputPortImportSubscription(const SPL::rstring& subscription, uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        return 1; // bad port
    }
    OperatorInputPortImpl& ip = oper.getInputPortAt(port);
    if (ip.getImportType() != OperatorInputPort::BySubscription) {
        return 1; // bad port
    }
    SubscriptionExpressionPtr se = SubscriptionExpression::fromString(subscription);
    if (se.get() == NULL) {
        return 2; // invalid subscription
    }
    try {
        ip.setSubscriptionExpression(*se);
    } catch (...) {
        return 3; // Set of subscription expression failed
    }
    // All worked
    return 0;
}
SPL::rstring getInputPortImportFilterExpression(uint32 port, SPL::int32& error)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        error = 1;
        return ""; // bad port
    }
    OperatorInputPortImpl& ip = oper.getInputPortAt(port);
    if (!ip.isImportedPort()) {
        error = 1;
        return ""; // bad port
    }
    // Worked
    error = 0;
    FilterExpressionPtr f = ip.getFilterExpression();
    if (!f.get()) {
        return "";
    }
    return f->toString();
}

SPL::int32 setInputPortImportFilterExpression(const SPL::rstring& filter, uint32 port)
{
    using namespace SPL;
    OperatorImpl& oper = getCurrentOperator();
    uint32 nports = oper.getNumberOfInputPorts();
    if (port >= nports) {
        return 1; // bad port
    }
    OperatorInputPortImpl& ip = oper.getInputPortAt(port);
    if (!ip.isImportedPort()) {
        return 1; // bad port
    }
    if (filter.empty()) {
        // remove the filter
        try {
            ip.removeFilterExpression();
        } catch (...) {
            return 3; // Set of filter expression failed
        }
        return 0;
    }

    // Need the tuple for the filter expression
    auto_ptr<Tuple> tuple = ip.createTuple();
    FilterExpressionPtr fe = FilterExpression::fromString(*tuple, filter);
    if (fe.get() == NULL) {
        return 2; // invalid filter
    }
    try {
        ip.setFilterExpression(*fe);
    } catch (...) {
        return 3; // Set of filter expression failed
    }
    // All worked
    return 0;
}

SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   bool prependResname,
                                   const Tuple* substitutionValues,
                                   const SPL::rstring* locale)
{
    FormattableMessage msg(toolkitName, bundleName, "", resourceID, "", true);
    if (NULL != substitutionValues) {
        for (uint32_t i = 0, sz = substitutionValues->getNumberOfAttributes(); i < sz; ++i) {
            ConstValueHandle v = substitutionValues->getAttributeValue(i);
            switch (v.getMetaType()) {
                case Meta::Type::INT8:
                    msg % (int64_t)(int8)v;
                    break;
                case Meta::Type::UINT8:
                    msg % (int64_t)(uint8)v;
                    break;
                case Meta::Type::INT16:
                    msg % (int64_t)(int16)v;
                    break;
                case Meta::Type::UINT16:
                    msg % (int64_t)(uint16)v;
                    break;
                case Meta::Type::INT32:
                    msg % (int64_t)(int32)v;
                    break;
                case Meta::Type::UINT32:
                    msg % (int64_t)(uint32)v;
                    break;
                case Meta::Type::INT64:
                    msg % (int64_t)(int64)v;
                    break;
                case Meta::Type::UINT64:
                    msg % (int64_t)(uint64)v;
                    break;
                case Meta::Type::FLOAT32:
                    msg % (float32)v;
                    break;
                case Meta::Type::FLOAT64:
                    msg % (float64)v;
                    break;
                case Meta::Type::RSTRING:
                    msg % (SPL::rstring)v;
                    break;
                case Meta::Type::USTRING: {
                    string s = ustring::toUTF8(v);
                    msg % s;
                    break;
                }
                default:
                    SPLTRACEMSGANDTHROW(
                      SPLRuntimeUnsupportedSubstitutionType, L_ERROR,
                      SPL_RUNTIME_UNSUPPORTED_SUBST_TYPE(substitutionValues->getAttributeName(i)),
                      SPL_FUNC_DBG);
            }
        }
    }

    if (NULL != locale) {
        return msg.format(*locale);
    } else {
        return msg.format();
    }
}

SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const Tuple& substitutionValues)
{
    return loadAndFormatResource(toolkitName, bundleName, resourceID, true, &substitutionValues,
                                 NULL);
}

SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID)
{
    return loadAndFormatResource(toolkitName, bundleName, resourceID, true, NULL, NULL);
}

SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const SPL::rstring& locale)
{
    return loadAndFormatResource(toolkitName, bundleName, resourceID, true, NULL, &locale);
}

SPL::rstring loadAndFormatResource(const SPL::rstring& toolkitName,
                                   const SPL::rstring& bundleName,
                                   const SPL::rstring& resourceID,
                                   const Tuple& substitutionValues,
                                   const SPL::rstring& locale)
{
    return loadAndFormatResource(toolkitName, bundleName, resourceID, true, &substitutionValues,
                                 &locale);
}

SPL::int32 getApplicationConfiguration(SPL::map<SPL::rstring, SPL::rstring>& properties,
                                       SPL::rstring const& name)
{
    std::map<std::string, std::string> props;
    int32 rc = SPL::ProcessingElement::pe().getApplicationConfiguration(name, props);

    for (std::map<std::string, std::string>::const_iterator it = props.begin(); it != props.end();
         it++) {
        properties.add(it->first, it->second);
    }
    return rc;
}

SPL::rstring getApplicationConfigurationProperty(SPL::rstring const& configName,
                                                 SPL::rstring const& propertyName,
                                                 SPL::rstring const& defaultValue)
{
    SPL::int32 errorCode = 0;
    SPL::rstring value = SPL::ProcessingElement::pe().getApplicationConfigurationProperty(
      configName, propertyName, &errorCode);
    if (errorCode != 0) {
        value = defaultValue;
    }
    return value;
}
}

namespace Time {

#define THOUSAND 1000
#define MILLION 1000000
#define MS_TO_TS(x) SPL::timestamp(static_cast<int64_t>((x) / MILLION), 0u)
#define MSMS_TO_TS(x)                                                                              \
    SPL::timestamp(static_cast<int64_t>((x) / MILLION),                                            \
                   static_cast<uint32_t>(THOUSAND * ((x) % MILLION)))

SPL::decimal128 toDecimal128(const SPL::timestamp& ts)
{
    using namespace ibm::decimal;
    SPL::decimal128 result = ts.getSeconds();
    return result + make_decimal128((long long)ts.getNanoseconds(), -9);
}

SPL::timestamp fromDecimal128(const SPL::decimal128& t)
{
    using namespace ibm::decimal;
    SPL::decimal128 secs = trunc(t);
    SPL::decimal128 nanos = t - secs;
    return SPL::timestamp(llrint(secs), llrint(nanos * make_decimal128(1LL, 9)));
}

SPL::timestamp getTimestamp()
{
    timeval tval;
    if (0 != gettimeofday(&tval, NULL)) {
        THROW(SPL::SPLRuntime, "gettimeofday: " << SPL::RuntimeUtility::getErrorNoStr());
    }
    return SPL::timestamp(static_cast<long long>(tval.tv_sec),
                          static_cast<int32_t>(1000 * tval.tv_usec));
}

SPL::float64 getTimestampInSecs()
{
    timeval tval;
    if (0 != gettimeofday(&tval, NULL)) {
        THROW(SPL::SPLRuntime, "gettimeofday: " << SPL::RuntimeUtility::getErrorNoStr());
    }
    return tval.tv_sec + tval.tv_usec / static_cast<double>(MILLION);
}

SPL::int64 getCPUCounterInNanoSeconds()
{
    return Distillery::getTimeInNanosecs();
}

SPL::timestamp toTimestamp(const Enum& type, const char* str)
{
    uint64_t t;
    SPL::timestamp ts;
    try {
        switch (type.getIndex()) {
            case 0: // YYYYMMDDhhmmss
                t = Distillery::YYYYMMDDhhmmss_ToMicroseconds(str);
                ts = MS_TO_TS(t);
                break;
            case 1: // YYYY_MM_DD_hh_mm_ss
                t = Distillery::YYYY_MM_DD_hh_mm_ssToMicroseconds(str);
                ts = MS_TO_TS(t);
                break;
            case 2: // MM_DD_YYYY_hh_mm_ss
                t = Distillery::MM_DD_YYYY_hh_mm_ssToMicroseconds(str);
                ts = MS_TO_TS(t);
                break;
            case 3: // DD_MM_YYYY_hh_mm_ss
                t = Distillery::DD_MM_YYYY_hh_mm_ssToMicroseconds(str);
                ts = MSMS_TO_TS(t);
                break;
            case 4: // YYYY_MM_DD_hh_mm_ss_mmm
                t = Distillery::YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds(str);
                ts = MSMS_TO_TS(t);
                break;
            case 5: // MM_DD_YYYY_hh_mm_ss_mmm
                t = Distillery::MM_DD_YYYY_hh_mm_ss_mmmToMicroseconds(str);
                ts = MSMS_TO_TS(t);
                break;
            case 6: // DD_MM_YYYY_hh_mm_ss_mmm
                t = Distillery::DD_MM_YYYY_hh_mm_ss_mmmToMicroseconds(str);
                ts = MSMS_TO_TS(t);
                break;
            default:
                assert(false);
        }
    } catch (Distillery::DistilleryException const& e) {
        THROW(SPL::SPLRuntimeInvalidArgument, "invalid format: " << str);
    }
    return ts;
}

SPL::timestamp toTimestamp(const Enum& type, const char* str, const char* tmzone)
{
    int64_t t;
    SPL::timestamp ts;
    try {
        switch (type.getIndex()) {
            case 0: // YYYYMMDDhhmmss
                t = Distillery::YYYYMMDDhhmmss_ToMicroseconds(str, tmzone);
                ts = MS_TO_TS(t);
                break;
            case 1: // YYYY_MM_DD_hh_mm_ss
                t = Distillery::YYYY_MM_DD_hh_mm_ssToMicroseconds(str, tmzone);
                ts = MS_TO_TS(t);
                break;
            case 2: // MM_DD_YYYY_hh_mm_ss
                t = Distillery::MM_DD_YYYY_hh_mm_ssToMicroseconds(str, tmzone);
                ts = MS_TO_TS(t);
                break;
            case 3: // DD_MM_YYYY_hh_mm_ss
                t = Distillery::DD_MM_YYYY_hh_mm_ssToMicroseconds(str, tmzone);
                ts = MSMS_TO_TS(t);
                break;
            case 4: // YYYY_MM_DD_hh_mm_ss_mmm
                t = Distillery::YYYY_MM_DD_hh_mm_ss_mmmToMicroseconds(str, tmzone);
                ts = MSMS_TO_TS(t);
                break;
            case 5: // MM_DD_YYYY_hh_mm_ss_mmm
                t = Distillery::MM_DD_YYYY_hh_mm_ss_mmmToMicroseconds(str, tmzone);
                ts = MSMS_TO_TS(t);
                break;
            case 6: // DD_MM_YYYY_hh_mm_ss_mmm
                t = Distillery::DD_MM_YYYY_hh_mm_ss_mmmToMicroseconds(str, tmzone);
                ts = MSMS_TO_TS(t);
                break;
            default:
                assert(false);
        }
    } catch (Distillery::DistilleryException const& e) {
        THROW(SPL::SPLRuntimeInvalidArgument, "invalid format: " << str << " " << tmzone);
    }
    return ts;
}

static void retrieveAttribute(const SPL::Tuple& t,
                              uint32_t index,
                              const string& field,
                              int32_t& value)
{
    SPL::ConstTupleAttribute attr = t.getAttribute(index);
    assert(attr.getName() == field);
    SPL::ConstValueHandle handle = attr.getValue();
    assert(handle.getMetaType() == SPL::Meta::Type::INT32);
    const SPL::int32& f = handle;
    value = f;
}

static void retrieveAttribute(const SPL::Tuple& t,
                              uint32_t index,
                              const string& field,
                              long int& value)
{
    SPL::ConstTupleAttribute attr = t.getAttribute(index);
    assert(attr.getName() == field);
    SPL::ConstValueHandle handle = attr.getValue();
    if (handle.getMetaType() == SPL::Meta::Type::INT64) {
        const SPL::int64& f = handle;
        value = f;
    } else {
        assert(handle.getMetaType() == SPL::Meta::Type::INT32);
        const SPL::int32& f = handle;
        value = f;
    }
}

static void retrieveAttribute(const SPL::Tuple& t,
                              uint32_t index,
                              const string& field,
                              const char*& value)
{
    SPL::ConstTupleAttribute attr = t.getAttribute(index);
    assert(attr.getName() == field);
    SPL::ConstValueHandle handle = attr.getValue();
    assert(handle.getMetaType() == SPL::Meta::Type::RSTRING);
    const SPL::rstring& f = handle;
    value = f.c_str();
}

static void retrieveAttributes(const SPL::Tuple& t, struct tm& time)
{
    retrieveAttribute(t, 0, "sec", time.tm_sec);
    retrieveAttribute(t, 1, "min", time.tm_min);
    retrieveAttribute(t, 2, "hour", time.tm_hour);
    retrieveAttribute(t, 3, "mday", time.tm_mday);
    retrieveAttribute(t, 4, "mon", time.tm_mon);
    retrieveAttribute(t, 5, "year", time.tm_year);
    retrieveAttribute(t, 6, "wday", time.tm_wday);
    retrieveAttribute(t, 7, "yday", time.tm_yday);
    retrieveAttribute(t, 8, "isdst", time.tm_isdst);
    retrieveAttribute(t, 9, "gmtoff", time.tm_gmtoff);
    retrieveAttribute(t, 10, "zone", time.tm_zone);
}

static void updateAttribute(SPL::Tuple& t, uint32_t index, const string& field, int32_t value)
{
    SPL::TupleAttribute attr = t.getAttribute(index);
    assert(attr.getName() == field);
    SPL::ValueHandle handle = attr.getValue();
    assert(handle.getMetaType() == SPL::Meta::Type::INT32);
    SPL::int32& f = handle;
    f = value;
}

static void updateAttribute(SPL::Tuple& t,
                            uint32_t index,
                            const string& field,
                            const char* const& value)
{
    SPL::TupleAttribute attr = t.getAttribute(index);
    assert(attr.getName() == field);
    SPL::ValueHandle handle = attr.getValue();
    assert(handle.getMetaType() == SPL::Meta::Type::RSTRING);
    SPL::rstring& f = handle;
    if (value) {
        f = value;
    } else {
        f = "";
    }
}

static void updateAttribute(SPL::Tuple& t, const struct tm& time)
{
    updateAttribute(t, 0, "sec", time.tm_sec);
    updateAttribute(t, 1, "min", time.tm_min);
    updateAttribute(t, 2, "hour", time.tm_hour);
    updateAttribute(t, 3, "mday", time.tm_mday);
    updateAttribute(t, 4, "mon", time.tm_mon);
    updateAttribute(t, 5, "year", time.tm_year);
    updateAttribute(t, 6, "wday", time.tm_wday);
    updateAttribute(t, 7, "yday", time.tm_yday);
    updateAttribute(t, 8, "isdst", time.tm_isdst);
    updateAttribute(t, 9, "gmtoff", time.tm_gmtoff);
    updateAttribute(t, 10, "zone", time.tm_zone);
}

void time(const SPL::timestamp& time, SPL::Tuple& result)
{
    time_t timer = static_cast<time_t>(time.getSeconds());
    struct tm r = { 0 };
    Distillery::get_localtime(&timer, &r);
    updateAttribute(result, r);
}

void time(const SPL::timestamp& time, const SPL::rstring& tzone, SPL::Tuple& result)
{
    time_t t = static_cast<time_t>(time.getSeconds());
    struct tm r = { 0 };
    Distillery::get_localtime(&t, &r, tzone.c_str());
    updateAttribute(result, r);
}

void gmtime(const SPL::timestamp& time, SPL::Tuple& result)
{
    time_t timer = static_cast<time_t>(time.getSeconds());
    struct tm r;
    gmtime_r(&timer, &r);
    updateAttribute(result, r);
}

SPL::rstring ctime(const SPL::timestamp& time)
{
    char timetxt[128];
    time_t timer = static_cast<time_t>(time.getSeconds());
    ::ctime_r(&timer, timetxt);
    string result(timetxt);
    result.erase(result.size() - 1);
    return result;
}

SPL::rstring strftime(const SPL::Tuple& time, const SPL::rstring& format)
{
    char timetxt[1024];
    tm r = { 0 };
    retrieveAttributes(time, r);
    ::strftime(timetxt, sizeof(timetxt), format.c_str(), &r);
    return timetxt;
}

void strptime(const SPL::rstring& dateTime,
              const SPL::rstring& format,
              SPL::Tuple& result,
              SPL::uint32& numCharsProcessed)
{
    tm r = { 0 };
    char* p = strptime(dateTime.c_str(), format.c_str(), &r);
    if (p == NULL) {
        numCharsProcessed = 0;
    } else {
        r.tm_isdst = -1;
        updateAttribute(result, r);
        numCharsProcessed = p - dateTime.c_str();
    }
}

SPL::timestamp toTimestamp(const SPL::Tuple& time)
{
    tm r = { 0 };
    retrieveAttributes(time, r);
    time_t t = 0;
    if (*r.tm_zone == '\0' || strcmp(r.tm_zone, "UTC") == 0 || strcmp(r.tm_zone, "GMT") == 0) {
        int64_t offset = r.tm_gmtoff; // timegm below not only ignores this, but it also cleans this
        r.tm_zone = "UTC";
        t = ::timegm(&r); // was Distillery::get_mktime(&r);
        if (t == -1) {    // failed
            return SPL::timestamp();
        }
        t -= offset;
    } else {
        t = Distillery::get_mktime(&r);
        if (t == -1) { // failed
            return SPL::timestamp();
        }
    }
    return SPL::timestamp((int64_t)t, (uint32_t)0);
}

SPL::timestamp toTimestamp(const SPL::Tuple& time, bool defaultZone)
{
    tm r = { 0 };
    retrieveAttributes(time, r);
    time_t t = 0;
    if ((*r.tm_zone == '\0' && !defaultZone) || strcmp(r.tm_zone, "UTC") == 0 ||
        strcmp(r.tm_zone, "GMT") == 0) {
        int64_t offset = r.tm_gmtoff; // timegm below not only ignores this, but it also cleans this
        r.tm_zone = "UTC";
        t = ::timegm(&r); // was Distillery::get_mktime(&r);
        if (t == -1) {    // failed
            return SPL::timestamp();
        }
        t -= offset;
    } else {
        t = Distillery::get_mktime(&r);
        if (t == -1) { // failed
            return SPL::timestamp();
        }
    }
    return SPL::timestamp((int64_t)t, (uint32_t)0);
}
SPL::uint32 year(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return 1900 + result.tm_year;
}

SPL::uint32 month(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_mon;
}

SPL::uint32 day(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_mday;
}

SPL::uint32 hour(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_hour;
}

SPL::uint32 minute(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_min;
}

SPL::uint32 second(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_sec;
}

SPL::uint32 dayOfYear(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_yday;
}

SPL::uint32 dayOfWeek(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    Distillery::get_localtime(&timer, &result);
    return result.tm_wday;
}

SPL::uint32 gmyear(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return 1900 + result.tm_year;
}

SPL::uint32 gmmonth(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_mon;
}

SPL::uint32 gmday(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_mday;
}

SPL::uint32 gmhour(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_hour;
}

SPL::uint32 gmminute(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_min;
}

SPL::uint32 gmsecond(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_sec;
}

SPL::uint32 gmdayOfYear(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_yday;
}

SPL::uint32 gmdayOfWeek(const SPL::timestamp& time)
{
    tm result;
    time_t timer = static_cast<time_t>(time.getSeconds());
    gmtime_r(&timer, &result);
    return result.tm_wday;
}

/// DEPRECATED.  Will be removed eventually
SPL::timestamp timeStringtoTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     bool useLocaleMonths)
{
    return timeStringToTimestamp(dmy, hmsmilli, useLocaleMonths);
}

SPL::timestamp timeStringToTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     bool useLocaleMonths)
{
    if (!useLocaleMonths) {
        try {
            int64_t t = Distillery::timeStampToMicroseconds(dmy.c_str(), hmsmilli.c_str());
            return MSMS_TO_TS(t);
        } catch (Distillery::DistilleryException const& e) {
            THROW(SPL::SPLRuntimeInvalidArgument, e.getExplanation());
        }
    }
    struct tm tm;
    string v = dmy + ' ' + hmsmilli;
    const char* p = strptime(v.c_str(), "%d-%b-%Y %H:%M:%S.", &tm);
    if (p) {
        tm.tm_zone = NULL;
        time_t s = Distillery::get_mktime(&tm);
        return SPL::timestamp((int64)s, atoi(p) * 1000);
    }
    return SPL::timestamp(0LL, 0);
}

/// DEPRECATED.  Will be removed eventually
SPL::timestamp timeStringtoTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     bool useLocaleMonths)
{
    return timeStringToTimestamp(dmy, hmsmilli, useLocaleMonths);
}

SPL::timestamp timeStringToTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     bool useLocaleMonths)
{
    if (!useLocaleMonths) {
        try {
            int64_t t = Distillery::timeStampToMicroseconds(ustring::toUTF8(dmy).c_str(),
                                                            SPL::ustring::toUTF8(hmsmilli).c_str());
            return MSMS_TO_TS(t);
        } catch (Distillery::DistilleryException const& e) {
            THROW(SPL::SPLRuntimeInvalidArgument, e.getExplanation());
        }
    }
    string v = ustring::toUTF8(dmy) + ' ' + SPL::ustring::toUTF8(hmsmilli);
    struct tm tm;
    const char* p = strptime(v.c_str(), "%d-%b-%Y %H:%M:%S.", &tm);
    if (p) {
        tm.tm_zone = NULL;
        time_t s = Distillery::get_mktime(&tm);
        return SPL::timestamp((int64)s, atoi(p) * 1000);
    }
    return SPL::timestamp(0LL, 0);
}

SPL::timestamp timeStringToTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     const SPL::ustring& tmzone,
                                     bool useLocaleMonths)
{
    if (!useLocaleMonths) {
        try {
            int64_t t = Distillery::timeStampToMicroseconds(ustring::toUTF8(dmy).c_str(),
                                                            SPL::ustring::toUTF8(hmsmilli).c_str(),
                                                            SPL::ustring::toUTF8(tmzone).c_str());
            return MSMS_TO_TS(t);
        } catch (Distillery::DistilleryException const& e) {
            THROW(SPL::SPLRuntimeInvalidArgument, e.getExplanation());
        }
    }
    string v = ustring::toUTF8(dmy) + ' ' + SPL::ustring::toUTF8(hmsmilli);
    struct tm tm;
    const char* p = strptime(v.c_str(), "%d-%b-%Y %H:%M:%S.", &tm);
    if (p) {
        tm.tm_zone = SPL::ustring::toUTF8(tmzone).c_str();
        time_t s = Distillery::get_mktime(&tm);
        return SPL::timestamp((int64)s, atoi(p) * 1000);
    }
    return SPL::timestamp(0LL, 0);
}

/// DEPRECATED.  Will be removed eventually
SPL::timestamp timeStringtoTimestamp(const SPL::ustring& dmy,
                                     const SPL::ustring& hmsmilli,
                                     const SPL::ustring& tmzone,
                                     bool useLocaleMonths)
{
    return timeStringToTimestamp(dmy, hmsmilli, tmzone, useLocaleMonths);
}

/// DEPRECATED.  Will be removed eventually
SPL::timestamp timeStringtoTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     const SPL::rstring& tmzone,
                                     bool useLocaleMonths)
{
    return timeStringToTimestamp(dmy, hmsmilli, tmzone, useLocaleMonths);
}

SPL::timestamp timeStringToTimestamp(const SPL::rstring& dmy,
                                     const SPL::rstring& hmsmilli,
                                     const SPL::rstring& tmzone,
                                     bool useLocaleMonths)
{
    if (!useLocaleMonths) {
        try {
            int64_t t =
              Distillery::timeStampToMicroseconds(dmy.c_str(), hmsmilli.c_str(), tmzone.c_str());
            return MSMS_TO_TS(t);
        } catch (Distillery::DistilleryException const& e) {
            THROW(SPL::SPLRuntimeInvalidArgument, e.getExplanation());
        }
    }
    struct tm tm;
    string v = dmy + ' ' + hmsmilli;
    const char* p = strptime(v.c_str(), "%d-%b-%Y %H:%M:%S.", &tm);
    if (p) {
        tm.tm_zone = tmzone.c_str();
        time_t s = Distillery::get_mktime(&tm);
        return SPL::timestamp((int64)s, atoi(p) * 1000);
    }
    return SPL::timestamp(0LL, 0);
}
}

namespace EventTime {

namespace {
TriggerInfoProvider const* getTriggerInfoProvider()
{
    BaseWindow const* baseWindow = WindowTracker::getCurrentWindow();
    if (!baseWindow) {
        string const& opName =
          SPL::PEImpl::instance().getOperatorName(SPL::OperatorTracker::getCurrentOperator());
        // TODO better exception type
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_WINDOW_NOT_FOUND(opName), SPL_FUNC_DBG);
    }

    TriggerInfoProvider const* provider = dynamic_cast<TriggerInfoProvider const*>(baseWindow);

    if (!provider) {
        string const& opName =
          SPL::PEImpl::instance().getOperatorName(SPL::OperatorTracker::getCurrentOperator());
        // TODO better exception type
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_WINDOW_NOT_EVENT_TIME_WINDOW(opName), SPL_FUNC_DBG);
    }

    return provider;
}
}

SPL::timestamp intervalStart()
{
    return getTriggerInfoProvider()->getStartTime();
}

SPL::timestamp intervalEnd()
{
    return getTriggerInfoProvider()->getEndTime();
}

SPL::TriggerInfoProvider::PaneTiming paneTiming()
{
    return getTriggerInfoProvider()->getPaneTiming();
}

uint64 paneIndex()
{
    return getTriggerInfoProvider()->getPaneIndex();
}
/*
    SPL::timestamp getWatermark() {
        // If it is not a java operator, get the current window, get
        // its event-time context (if any), get the watermark from
        // event-time context.  return a default value is there is no
        // event-time context.

        // For a java operator, we have to get the event-time context
        // in java through JNI calls.

        SPL::timestamp wm = SPL::Functions::Utility::getCurrentOperator().getOperator().watermark();
        return wm;
    }
*/
// Convert a timestamp to either int64 or uint64, or convert from int64 or
// uint64 to a timestamp, based on the resolution of the integral type.
namespace {

template<typename T>
class TimestampConverter
{
  public:
    static SPL::timestamp convert(T ticks, T secondsToTicksMultiplier, T nanosToTicksDivisor)
    {
        return SPL::timestamp(
          static_cast<int64>(ticks / secondsToTicksMultiplier),
          static_cast<uint32>((ticks % secondsToTicksMultiplier) * nanosToTicksDivisor));
    }

    static T convert(SPL::timestamp const& ts, T secondsToTicksMultiplier, T nanosToTicksDivisor)
    {
        // Convert the second part to ticks
        T secs = static_cast<T>(ts.getSeconds());
        if (secs > std::numeric_limits<T>::max() / secondsToTicksMultiplier) {
            return std::numeric_limits<T>::max(); // would overflow
        }

        T secsToTicks = secs * secondsToTicksMultiplier;

        // Convert the nanosecond part to ticks
        T nanosToTicks = static_cast<T>(ts.getNanoseconds()) / (nanosToTicksDivisor);

        return (secsToTicks > std::numeric_limits<T>::max() - nanosToTicks)
                 ? std::numeric_limits<T>::max()
                 : // would overflow
                 secsToTicks + nanosToTicks;
    }
};
}

TimeResolution toTimeResolution(std::string const& resString)
{
    return resString == "Nanoseconds" ? Nanoseconds
                                      : resString == "Microseconds" ? Microseconds : Milliseconds;
}
SPL::timestamp toTimestamp(int64 ticks, Enum const& resolution)
{
    return toTimestamp(ticks, static_cast<TimeResolution>(resolution.getIndex()));
}

SPL::timestamp toTimestamp(int64 ticks, TimeResolution resolution)
{
    switch (resolution) {
        case Milliseconds:
            return TimestampConverter<int64>::convert(ticks, THOUSAND, MILLION);
        case Microseconds:
            return TimestampConverter<int64>::convert(ticks, MILLION, THOUSAND);
        case Nanoseconds:
            return TimestampConverter<int64>::convert(ticks, BILLION, 1);
    }
    return SPL::timestamp(); // unreachable
}

SPL::timestamp toTimestamp(uint64 ticks, Enum const& resolution)
{
    return toTimestamp(ticks, static_cast<TimeResolution>(resolution.getIndex()));
}
SPL::timestamp toTimestamp(uint64 ticks, TimeResolution resolution)
{
    switch (resolution) {
        case Milliseconds:
            return TimestampConverter<uint64>::convert(ticks, THOUSAND, MILLION);
        case Microseconds:
            return TimestampConverter<uint64>::convert(ticks, MILLION, THOUSAND);
        case Nanoseconds:
            return TimestampConverter<uint64>::convert(ticks, BILLION, 1);
    }
    return timestamp(); // unreachable.
}

int64 int64TicksFromTimestamp(SPL::timestamp const& ts, Enum const& resolution)
{
    return int64TicksFromTimestamp(ts, static_cast<TimeResolution>(resolution.getIndex()));
}
int64 int64TicksFromTimestamp(SPL::timestamp const& ts, TimeResolution resolution)
{
    switch (resolution) {
        case Milliseconds:
            return TimestampConverter<int64>::convert(ts, THOUSAND, MILLION);
        case Microseconds:
            return TimestampConverter<int64>::convert(ts, MILLION, THOUSAND);
        case Nanoseconds:
            return TimestampConverter<int64>::convert(ts, BILLION, 1);
    }
    return 0; // unreachable
}

uint64 uint64TicksFromTimestamp(SPL::timestamp const& ts, Enum const& resolution)
{
    return uint64TicksFromTimestamp(ts, static_cast<TimeResolution>(resolution.getIndex()));
}
uint64 uint64TicksFromTimestamp(SPL::timestamp const& ts, TimeResolution resolution)
{
    switch (resolution) {
        case Milliseconds:
            return TimestampConverter<uint64>::convert(ts, THOUSAND, MILLION);
        case Microseconds:
            return TimestampConverter<uint64>::convert(ts, MILLION, THOUSAND);
        case Nanoseconds:
            return TimestampConverter<uint64>::convert(ts, BILLION, 1);
    }
    return 0; // unreachable
}

void EventTimeBase::throwMissingEventTimeAttribute()
{
    SPLTRACEMSGANDTHROW(SPLRuntimeGetEventTimeMissingEventTimeAttribute, L_ERROR,
                        SPL_RUNTIME_GETEVENTTIME_MISSING_EVENT_TIME_ATTRIBUTE, SPL_FUNC_DBG);
}
}

namespace String {

SPL::rstring convertToUtf8(const SPL::list<SPL::uint8>& val, const SPL::rstring& enc)
{
    icu::UnicodeString ustr((char*)&val[0], val.size(), enc.c_str());
    unsigned bs = 4 * ustr.countChar32() + 1;
    boost::scoped_array<char> rc(new char[bs]);
    ustr.extract(0, ustr.length(), rc.get(), bs, "UTF-8");
    return SPL::rstring(rc.get());
}

SPL::rstring convertToUtf8(const SPL::rstring& val, const SPL::rstring& enc)
{
    icu::UnicodeString ustr(val.c_str(), val.size(), enc.c_str());
    unsigned bs = 4 * ustr.countChar32() + 1;
    boost::scoped_array<char> rc(new char[bs]);
    ustr.extract(0, ustr.length(), rc.get(), bs, "UTF-8");
    return SPL::rstring(rc.get());
}

SPL::list<SPL::uint8> convertFromUtf8(const SPL::rstring& str, const SPL::rstring& enc)
{
    icu::UnicodeString ustr(str.data(), str.size(), "UTF-8");
    unsigned bs = 4 * ustr.countChar32() + 1;
    boost::scoped_array<char> rc(new char[bs]);
    int32_t len = ustr.extract(0, ustr.length(), rc.get(), bs, enc.c_str());
    SPL::list<SPL::uint8> res;
    res.reserve(len);
    res.assign(rc.get(), rc.get() + len);
    return res;
}

SPL::rstring convertToUtf8(char* data, uint32_t size, const SPL::rstring& enc)
{
    icu::UnicodeString ustr(data, size, "UTF-8");
    unsigned bs = 4 * ustr.countChar32() + 1;
    boost::scoped_array<char> rc(new char[bs]);
    ustr.extract(0, ustr.length(), rc.get(), bs, enc.c_str());
    return SPL::rstring(rc.get());
}

SPL::rstring convertFromUtf8ToString(const SPL::rstring& str, const SPL::rstring& enc)
{
    icu::UnicodeString ustr(str.data(), str.size(), "UTF-8");
    unsigned bs = 4 * ustr.countChar32() + 1;
    boost::scoped_array<char> rc(new char[bs]);
    int32_t len = ustr.extract(0, ustr.length(), rc.get(), bs, enc.c_str());
    return SPL::rstring(rc.get(), len);
}

const SPL::list<SPL::rstring> tokenize(const SPL::rstring& text,
                                       const SPL::rstring& delim,
                                       const SPL::boolean keepEmptyTokens)
{
    SPL::list<SPL::rstring> res;
    tokenize(text, delim, keepEmptyTokens, true, res);
    return res;
}

const SPL::list<SPL::ustring> tokenize(const SPL::ustring& text,
                                       const SPL::ustring& delim,
                                       const SPL::boolean keepEmptyTokens)
{
    SPL::list<SPL::ustring> res;
    tokenize(text, delim, keepEmptyTokens, true, res);
    return res;
}

void tokenize(const SPL::rstring& str,
              const SPL::rstring& delimiter,
              SPL::boolean keepEmptyTokens,
              SPL::boolean multipleDelim,
              SPL::list<SPL::rstring>& tokens)
{
    tokens.clear();

    if (multipleDelim) {
        // Treat delimiter as a set of characters
        tokens.clear();

        string::size_type lastPos = 0;

        // Find first delimiter
        string::size_type pos = str.find_first_of(delimiter, lastPos);
        while (pos != string::npos) {
            // Found a token, add it to the vector
            if (pos != lastPos) {
                tokens.push_back(rstring(str, lastPos, pos - lastPos));
                lastPos = pos + 1;
            } else {
                if (keepEmptyTokens) {
                    tokens.push_back(rstring());
                }
                lastPos++;
            }
            // Find next "non-delimiter"
            pos = str.find_first_of(delimiter, lastPos);
        }
        if (lastPos != str.size()) {
            tokens.push_back(rstring(str, lastPos));
        } else {
            if (keepEmptyTokens) {
                tokens.push_back(rstring());
            }
        }
        return;
    }

    // Treat the delimiter as a string
    // The last position is required to extract the tokens.
    size_t last = 0;
    // Search the delimiter until the end of the string is reached.
    while (true) {
        // Find token.
        size_t found = str.find(delimiter, last);
        // If not found, then the string from last position to end must be stored
        // in tokens. Else, the string from last position to the current position
        // must be stored.
        if (str.npos == found) {
            if (keepEmptyTokens || (last != str.length())) {
                tokens.push_back(str.substr(last));
            }
            return; // no more
        }

        // Okay, we found the next delimter
        if (keepEmptyTokens || (last != found)) {
            tokens.push_back(str.substr(last, found - last));
        }

        // The new last position is the found position plus the token length.
        last = found + delimiter.length();
    }
}

void tokenize(const SPL::ustring& str,
              const SPL::ustring& delimiter,
              SPL::boolean keepEmptyTokens,
              SPL::boolean multipleDelim,
              SPL::list<SPL::ustring>& tokens)
{
    tokens.clear();
    if (multipleDelim) {
        // Treat delimiter as a set of characters
        int32_t lastPos = 0;
        int32_t lastCharPos = 0;
        int32_t pos = 0;
        int32_t charPos = 0;

        // Find first delimiter
        while (true) {
            // find the first delimiter
            UChar32 c = str.char32At(pos);
            while (pos < str.length() && delimiter.indexOf(c) == -1) {
                pos = str.moveIndex32(pos, 1);
                ++charPos;
                c = str.char32At(pos);
            }
            if (pos != str.length()) {
                if (pos != lastPos) {
                    tokens.push_back(SPL::Functions::String::subSlice(str, lastCharPos, charPos));
                } else {
                    if (keepEmptyTokens) {
                        tokens.push_back("");
                    }
                }
                lastPos = str.moveIndex32(pos, 1);
                lastCharPos = charPos + 1;
                pos = lastPos;
                charPos = lastCharPos;
            } else {
                // Hit the end
                if (lastPos != str.length()) {
                    tokens.push_back(SPL::Functions::String::subSliceU(str, lastCharPos));
                } else {
                    if (keepEmptyTokens) {
                        tokens.push_back("");
                    }
                }
                break;
            }
        }
        return;
    }

    // Treat delimiter as a string
    int32_t delimChars = delimiter.countChar32();
    int32_t strLen = str.length();
    // The last position is required to extract the tokens.
    int32_t last = 0;
    // Search the delimiter until the end of the string is reached.
    while (true) {
        // Find the token.
        int32 i = last;
        while (i != strLen) {
            if (str.compareCodePointOrder(i, delimChars, delimiter) == 0) {
                // They match
                if (keepEmptyTokens || (last != i)) {
                    tokens.push_back("");
                    tokens.back().append(str, last, i - last);
                }
                i = str.moveIndex32(i, delimChars);
                // The new last position is the found position plus the token length.
                last = i;
            } else { // move to next code point
                i = str.moveIndex32(i, 1);
            }
        }

        if (i == strLen) {
            // we are finished the string
            if (keepEmptyTokens || (last != strLen)) {
                tokens.push_back("");
                tokens.back().append(str, last, strLen - last);
            }
            return; // no more
        }
    }
}

const SPL::list<SPL::rstring> csvTokenize(const SPL::rstring& str)
{
    vector<string> tokens;
    try {
        Distillery::csvtokenize(str, tokens, false);
    } catch (Distillery::DistilleryException const& e) {
        THROW(SPL::SPLRuntimeInvalidArgument, e.getExplanation());
    }
    SPL::list<SPL::rstring> res;
    res.reserve(tokens.size());
    for (uint32_t i = 0; i < tokens.size(); i++) {
        res.push_back(tokens[i]);
    }
    return res;
}

SPL::rstring trim(const SPL::rstring& str, const SPL::rstring& t)
{
    return Distillery::trim(str, t);
}

SPL::rstring ltrim(const SPL::rstring& str, const SPL::rstring& t)
{
    return Distillery::ltrim(str, t);
}

SPL::rstring rtrim(const SPL::rstring& str, const SPL::rstring& t)
{
    return Distillery::rtrim(str, t);
}

SPL::ustring trim(const SPL::ustring& str, const SPL::ustring& t)
{
    SPL::ustring res = str;
    if (res.isEmpty()) {
        return res;
    }
    // trim the characters from the back
    int32_t i = res.moveIndex32(res.length(), -1);
    UChar32 c = res.char32At(i);
    while (res.length() > 0 && t.indexOf(c) != -1) {
        res.truncate(i);
        if (i == 0) {
            break; // nothing left!
        }
        i = res.moveIndex32(i, -1);
        c = res.char32At(i);
    }

    // Now the front
    if (res.isEmpty()) {
        return res;
    }
    c = res.char32At(0);
    while (res.length() > 0 && t.indexOf(c) != -1) {
        res.remove(0, res.moveIndex32(0, 1));
        if (res.isEmpty()) {
            break;
        }
        c = res.char32At(0);
    }
    return res;
}

SPL::ustring ltrim(const SPL::ustring& str, const SPL::ustring& t)
{
    SPL::ustring res = str;
    if (res.isEmpty()) {
        return res;
    }
    UChar32 c = res.char32At(0);
    while (res.length() > 0 && t.indexOf(c) != -1) {
        res.remove(0, res.moveIndex32(0, 1));
        if (res.isEmpty()) {
            break;
        }
        c = res.char32At(0);
    }
    return res;
}

SPL::ustring rtrim(const SPL::ustring& str, const SPL::ustring& t)
{
    SPL::ustring res = str;
    if (res.isEmpty()) {
        return res;
    }
    // trim the characters from the back
    int32_t i = res.moveIndex32(res.length(), -1);
    UChar32 c = res.char32At(i);
    while (res.length() > 0 && t.indexOf(c) != -1) {
        res.truncate(i);
        if (i == 0) {
            break; // nothing left!
        }
        i = res.moveIndex32(i, -1);
        c = res.char32At(i);
    }
    return res;
}

SPL::rstring subSlice(const SPL::rstring& s, uint32_t lower, uint32_t upper)
{
    if (lower > s.size()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_SLICE_LOWER_BOUND_LARGER_THAN_STRING_SIZE(lower, s.size()),
                            SPL_FUNC_DBG);
    }
    if (lower >= upper) {
        return "";
    }
    return s.substr(lower, upper - lower);
}

SPL::rstring subSliceL(const SPL::rstring& s, uint32_t upper)
{
    if (upper <= 1) {
        return "";
    }
    return s.substr(0, upper - 1);
}

SPL::rstring subSliceU(const SPL::rstring& s, uint32_t lower)
{
    if (lower > s.size()) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_SLICE_LOWER_BOUND_LARGER_THAN_STRING_SIZE(lower, s.size()),
                            SPL_FUNC_DBG);
    }
    return s.substr(lower);
}

SPL::ustring subSlice(const SPL::ustring& s, uint32_t lower, uint32_t upper)
{
    uint32_t size = s.countChar32();
    if (lower > size) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_SLICE_LOWER_BOUND_LARGER_THAN_STRING_SIZE(lower, size),
                            SPL_FUNC_DBG);
    }
    if (lower >= upper) {
        return "";
    }
    SPL::ustring res;
    // Need to use logical indexing
    int32_t l = s.moveIndex32(0, lower);
    int32_t u = s.moveIndex32(0, upper);
    s.extract(l, u - l, res);
    return res;
}

SPL::ustring subSliceL(const SPL::ustring& s, uint32_t upper)
{
    if (upper <= 1) {
        return "";
    }
    SPL::ustring res;
    int32_t u = s.moveIndex32(0, upper);
    s.extract(0, u, res);
    return res;
}

SPL::ustring subSliceU(const SPL::ustring& s, uint32_t lower)
{
    uint32_t size = s.countChar32();
    if (lower > size) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_SLICE_LOWER_BOUND_LARGER_THAN_STRING_SIZE(lower, size),
                            SPL_FUNC_DBG);
    }
    if (lower >= size) {
        return "";
    }
    SPL::ustring res;
    int32_t l = s.moveIndex32(0, lower);
    s.extract(l, s.length() - l, res);
    return res;
}
}

/*
 * File I/O
 */

namespace File {
void fwriteString(const char* val, SPL::uint64 len, const SPL::uint64 file, SPL::int32& err)
{
    FILE* f = reinterpret_cast<FILE*>(file);
    int e = fwrite(val, len, 1, f);
    if (e == -1) {
        err = errno;
    } else {
        err = 0;
    }
}

struct FILEInfo
{
    FILEInfo(FILE*& f, SPL::int32& err)
      : _f(f)
      , _err(err)
    {}
    FILE*& _f;
    SPL::int32& _err;
};

class FILEinterface : public boost::iostreams::device<boost::iostreams::bidirectional>
{
  public:
    FILEinterface(FILEInfo& f)
      : _f(f._f)
      , _err(f._err)
    {}
    streamsize write(const char* s, streamsize n)
    {
        size_t written = fwrite(s, 1, n, _f);
        if (written == 0 && n != 0) {
            _err = errno;
            return 0;
        }
        return written;
    }

    streamsize read(char* s, streamsize n)
    {
        // ignore n, and only read 1 byte
        size_t bytesRead = fread(s, 1, 1, _f);
        if (bytesRead == 0) {
            return -1;
        }
        if (bytesRead == (size_t)-1) {
            _err = errno;
            return 0;
        }
        return bytesRead;
    }

  private:
    FILE*& _f;
    SPL::int32& _err;
};

void fwriteTxt(const SPL::ConstValueHandle& val, const SPL::uint64 file, SPL::int32& err)
{
    FILE* f = reinterpret_cast<FILE*>(file);
    FILEInfo fi(f, err);
    err = 0;
    boost::iostreams::stream<FILEinterface> fs(fi);
    fs.imbue(locale::classic());
    try {
        val.serializeWithPrecision(fs);
    } catch (Distillery::DistilleryException& e) {
        APPTRC(L_ERROR, "Error during I/O: " << e.what(), SPL_FUNC_DBG);
        err = EIO;
    } catch (...) {
        APPTRC(L_ERROR, "Unknown I/O error", SPL_FUNC_DBG);
        err = EIO;
    }
}

void fwriteBin(const SPL::ConstValueHandle& val, const SPL::uint64 file, SPL::int32& err)
{
    FILE* f = reinterpret_cast<FILE*>(file);
    FILEInfo fi(f, err);
    err = 0;
    boost::iostreams::stream<FILEinterface> fs(fi);
    fs.imbue(locale::classic());
    SPL::StreamByteBuffer buf(fs);
    try {
        val.serialize(buf);
    } catch (Distillery::DistilleryException& e) {
        APPTRC(L_ERROR, "Error during I/O: " << e.what(), SPL_FUNC_DBG);
        err = EIO;
    } catch (...) {
        APPTRC(L_ERROR, "Unknown I/O error", SPL_FUNC_DBG);
        err = EIO;
    }
}

void fwrite(const SPL::list<SPL::uint8>& val,
            const SPL::uint64 file,
            const SPL::uint64 loffset,
            const SPL::uint64 length,
            SPL::int32& err)
{
    // Write a number of bytes to the file
    FILE* f = reinterpret_cast<FILE*>(file);
    uint64_t base = loffset;
    err = 0;
    for (uint64_t i = length; i > 0; i--) {
        int ret = fputc(val[base++], f);
        if (ret == EOF) {
            err = errno;
            break;
        }
    }
}

void freadfile(SPL::blob& res, const SPL::uint64 file, SPL::int32& err)
{
    // Read the entire file into a blob
    struct stat st;
    FILE* f = reinterpret_cast<FILE*>(file);
    if (fstat(fileno(f), &st) < 0) {
        err = errno;
        return;
    }
    uint64_t len = st.st_size;
    unsigned char* data = new unsigned char[len];
    fread(data, len, 1, f);
    if (feof(f) || ferror(f)) {
        delete[] data;
        err = errno;
        return;
    }
    res.adoptData(data, len);
    err = 0;
}

void fwriteblob(const SPL::blob& b, const SPL::uint64 file, SPL::int32& err)
{
    // Write a blob to the file
    FILE* f = reinterpret_cast<FILE*>(file);
    uint64_t blobSize;
    const unsigned char* blobData = b.getData(blobSize);
    int e = fwrite(blobData, blobSize, 1, f);
    if (e == -1) {
        err = errno;
    } else {
        err = 0;
    }
}

SPL::rstring freadToken(const SPL::uint64 file, SPL::int32& err)
{
    // read a file until a newline or EOF seen
    SPL::rstring res;
    FILE* f = reinterpret_cast<FILE*>(file);
    err = feof(f) ? -1 : 0;

    // Skip leading whitespace
    while (!feof(f)) {
        int ch = getc(f);
        if (ch == -1) {
            if (res.empty()) {
                err = -1; // EOF
            }
            break; // EOF
        }
        if (!isspace(ch)) {
            res += static_cast<char>(ch);
            break;
        }
    }

    // now read until a whitespace
    while (!feof(f)) {
        int ch = getc(f);
        if (ch == -1) {
            if (res.empty()) {
                err = -1; // EOF
            }
            break; // EOF
        }
        if (isspace(ch)) {
            // Too far -- done
            ungetc(ch, f);
            break;
        }
        res += static_cast<char>(ch);
    }
    return res;
}

SPL::rstring freadLine(const SPL::uint64 file, SPL::int32& err)
{
    // read a file until a newline or EOF seen
    SPL::rstring res;
    FILE* f = reinterpret_cast<FILE*>(file);
    err = feof(f) ? -1 : 0;
    while (!feof(f)) {
        int ch = getc(f);
        if (ch == -1) {
            if (res.empty()) {
                err = -1; // EOF
            }
            break; // EOF
        }
        if (ch == '\n') {
            break;
        }
        res += static_cast<char>(ch);
    }
    return res;
}

void freadTxt(SPL::ValueHandle& val, const SPL::uint64 file, SPL::int32& err)
{
    FILE* f = reinterpret_cast<FILE*>(file);
    err = feof(f) ? -1 : 0;
    FILEInfo fi(f, err);
    boost::iostreams::stream<FILEinterface> fs(fi);
    fs.imbue(locale::classic());
    try {
        val.deserialize(fs);
    } catch (Distillery::DistilleryException& e) {
        APPTRC(L_ERROR, "Error during I/O: " << e.what(), SPL_FUNC_DBG);
        err = EIO;
    } catch (...) {
        APPTRC(L_ERROR, "Unknown I/O error", SPL_FUNC_DBG);
        err = EIO;
    }
}

void freadBin(SPL::ValueHandle& val, const SPL::uint64 file, SPL::int32& err)
{
    FILE* f = reinterpret_cast<FILE*>(file);
    if (feof(f)) {
        err = -1;
        return;
    }
    err = 0;
    FILEInfo fi(f, err);
    boost::iostreams::stream<FILEinterface> fs(fi);
    fs.imbue(locale::classic());
    SPL::StreamByteBuffer buf(fs);
    try {
        val.deserialize(buf);
    } catch (Distillery::DistilleryException& e) {
        APPTRC(L_ERROR, "Error during I/O: " << e.what(), SPL_FUNC_DBG);
        err = EIO;
    } catch (...) {
        APPTRC(L_ERROR, "Unknown I/O error", SPL_FUNC_DBG);
        err = EIO;
    }
}

SPL::uint64 fread(SPL::list<SPL::uint8>& res,
                  const SPL::uint64 file,
                  const SPL::uint64 length,
                  SPL::int32& err)
{
    // read a length bytes from a file
    FILE* f = reinterpret_cast<FILE*>(file);
    if (feof(f)) {
        err = -1;
        return 0;
    }
    err = 0;
    res.clear();
    uint64_t len = length;
    uint64_t count = 0;
    while (!feof(f) && count < len) {
        int ch = getc(f);
        if (ch == -1) {
            if (res.empty()) {
                err = -1; // EOF
            }
            break; // EOF
        }
        count++;
        res.push_back(static_cast<SPL::uint8>(ch));
    }
    return count;
}

/*
     * stat routines
     */

static void fstat(const struct stat& st,
                  const SPL::rstring& what,
                  SPL::uint64& result,
                  SPL::int32& err)
{
    err = 0;
    if (what == "dev") {
        result = st.st_dev;
    } else if (what == "rdev") {
        result = st.st_rdev;
    } else if (what == "ino") {
        result = st.st_ino;
    } else if (what == "nlink") {
        result = st.st_nlink;
    } else if (what == "mode") {
        result = st.st_mode;
    } else if (what == "size") {
        result = st.st_size;
    } else if (what == "uid") {
        result = st.st_uid;
    } else if (what == "gid") {
        result = st.st_gid;
    } else if (what == "blksize") {
        result = st.st_blksize;
    } else if (what == "blocks") {
        result = st.st_blocks;
    } else {
        err = EINVAL;
    }
}

static void fstat(const struct stat& st,
                  const SPL::rstring& what,
                  SPL::timestamp& result,
                  SPL::int32& err)
{
    err = 0;
    if (what == "atime") {
        result = SPL::timestamp(static_cast<long long>(st.st_atim.tv_sec),
                                static_cast<int32_t>(st.st_atim.tv_nsec));
    } else if (what == "ctime") {
        result = SPL::timestamp(static_cast<long long>(st.st_ctim.tv_sec),
                                static_cast<int32_t>(st.st_ctim.tv_nsec));
    } else if (what == "mtime") {
        result = SPL::timestamp(static_cast<long long>(st.st_mtim.tv_sec),
                                static_cast<int32_t>(st.st_mtim.tv_nsec));
    } else {
        err = EINVAL;
    }
}

void fstat(const SPL::rstring& file, const SPL::rstring& what, SPL::uint64& result, SPL::int32& err)
{
    struct stat st;
    if (stat(file.c_str(), &st) < 0) {
        err = errno;
    } else {
        fstat(st, what, result, err);
    }
}

void fstat(const SPL::rstring& file,
           const SPL::rstring& what,
           SPL::timestamp& result,
           SPL::int32& err)
{
    struct stat st;
    if (stat(file.c_str(), &st) < 0) {
        err = errno;
    } else {
        fstat(st, what, result, err);
    }
}

void fstat(const SPL::uint64 file, const SPL::rstring& what, SPL::uint64& result, SPL::int32& err)
{
    struct stat st;
    FILE* f = reinterpret_cast<FILE*>(file);
    if (fstat(fileno(f), &st) < 0) {
        err = errno;
    } else {
        fstat(st, what, result, err);
    }
}

void fstat(const SPL::uint64 file,
           const SPL::rstring& what,
           SPL::timestamp& result,
           SPL::int32& err)
{
    struct stat st;
    FILE* f = reinterpret_cast<FILE*>(file);
    if (fstat(fileno(f), &st) < 0) {
        err = errno;
    } else {
        fstat(st, what, result, err);
    }
}
}

namespace Collections {
SPL::blob subSlice(const SPL::blob& b, uint32_t lower, uint32_t upper)
{
    uint64_t size;
    const unsigned char* data = b.getData(size);
    if (lower > size) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_INVALID_BLOB_SLICE(lower, size), SPL_FUNC_DBG);
    }
    if (lower >= upper) {
        return blob();
    }
    return SPL::blob(data + lower, upper - lower);
}

SPL::blob subSliceL(const SPL::blob& b, uint32_t upper)
{
    uint64_t size;
    const unsigned char* data = b.getData(size);
    if (upper > size) {
        upper = size;
    }
    return SPL::blob(data, upper);
}

SPL::blob subSliceU(const SPL::blob& b, uint32_t lower)
{
    uint64_t size;
    const unsigned char* data = b.getData(size);
    if (lower > size) {
        SPLTRACEMSGANDTHROW(SPLRuntimeInvalidArgument, L_ERROR,
                            SPL_RUNTIME_INVALID_BLOB_SLICE(lower, size), SPL_FUNC_DBG);
    }
    return SPL::blob(data + lower, size - lower);
}
}
namespace XML {
#define CONVERT_SUCCESS 0
#define CONVERT_BADXML 1
SPL::boolean convertToXML(SPL::xml& x, const string& str, SPL::int32& error)
{
    try {
        x = str;
        error = CONVERT_SUCCESS;
        return true;
    } catch (const SPLRuntimeInvalidXMLException& e) {
        error = CONVERT_BADXML;
    } catch (...) {
        error = CONVERT_BADXML;
    }
    return false;
}

SPL::boolean convertToXML(SPL::xml& x, const string& str)
{
    SPL::int32 error;
    return convertToXML(x, str, error);
}

#define WARNORTHROW(x, y, val)                                                                     \
    do {                                                                                           \
        if (error) {                                                                               \
            SPLAPPTRC(L_INFO, (string)y, SPL_FUNC_DBG);                                            \
            *error = val;                                                                          \
        } else {                                                                                   \
            SPLTRACEMSGANDTHROW(x, L_ERROR, y, SPL_FUNC_DBG);                                      \
        }                                                                                          \
    } while (0)

#define XQUERY_SUCCESS 0
#define XQUERY_BADXML 1
#define XQUERY_STATIC_FAIL 2
#define XQUERY_DYNAMIC_FAIL 3

class QueryCache
{
  public:
    ~QueryCache();
    XQQuery* getQuery(const ustring& query);

  private:
    typedef std::list<XQQuery*> UseOrderedXQQueries;
    typedef std::tr1::unordered_map<ustring, UseOrderedXQQueries::iterator> QueryMap;

    UseOrderedXQQueries _queryList;
    QueryMap _queries;
    Mutex _mutex;
} qCache;

QueryCache::~QueryCache()
{
    // Clean up queries
    for (UseOrderedXQQueries::iterator it = _queryList.begin(); it != _queryList.end(); it++) {
        delete *it;
    }
}

XQQuery* QueryCache::getQuery(const ustring& query)
{
    AutoMutex am(_mutex);

    QueryMap::iterator it = _queries.find(query);
    XQQuery* q;
    if (it != _queries.end()) {
        // Keep the list ordered with the most recent at the end
        q = *it->second;
        if (q != _queryList.back()) {
            // Move it to the end
            _queryList.erase(it->second);
            _queryList.insert(_queryList.end(), q);
        }
    } else {
        q = XMLHelper::instance().xqilla().parse(ustring(query).impl().getTerminatedBuffer());
        UseOrderedXQQueries::iterator it2 = _queryList.insert(_queryList.end(), q);
        _queries.insert(make_pair(query, it2));
    }

    // Do we have too many queries saved?
    if (_queryList.size() > 100) {
        // Clean up the oldest one
        delete _queryList.front();
        _queryList.pop_front();
    }

    // And now return the value
    return q;
}

static XQQuery* compileQuery(const ustring& q, int32* error)
{
    XQQuery* query = NULL;
    try {
        query = qCache.getQuery(q);
    } catch (const XQException& e) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY(makeUStringLiteral(q, true, false),
                                                           UTF8(e.getError())),
                    XQUERY_STATIC_FAIL);
    } catch (const std::exception& e) {
        WARNORTHROW(
          SPLRuntimeInvalidArgument,
          SPL_APPLICATION_RUNTIME_INVALID_XQUERY(makeUStringLiteral(q, true, false), e.what()),
          XQUERY_STATIC_FAIL);
    } catch (...) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_UNKNOWN_EXCEPTION, XQUERY_STATIC_FAIL);
    }
    return query;
}

static list<rstring> executeQuery(const string& q,
                                  const xercesc::DOMDocument& doc,
                                  XQQuery& query,
                                  const map<rstring, rstring>* xqueryVars,
                                  int32* error)
{
    // Create a context object
    auto_ptr<DynamicContext> context(query.createDynamicContext());
    context->setContextItem(XMLHelper::instance().xconfig().createNode(&doc, context.get()));

    if (xqueryVars) {
        map<rstring, rstring>::const_iterator it = xqueryVars->begin();
        ItemFactory& f = *context->getItemFactory();
        for (; it != xqueryVars->end(); it++) {
            Item::Ptr value = f.createUntypedAtomic(X(it->second.c_str()), context.get());
            context->setExternalVariable(X(it->first.c_str()), value);
        }
    }

    // Execute the query, using the context
    list<rstring> res;
    try {
        Result result = query.execute(context.get());
        // save the results
        Item::Ptr item;
        while (item = result->next(context.get())) {
            res.push_back(UTF8(item->asString(context.get())));
        }
    } catch (const XQException& e) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_EXECUTION(
                      makeRStringLiteral(q, true, false), UTF8(e.getError())),
                    XQUERY_DYNAMIC_FAIL);
    } catch (const std::exception& e) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_EXECUTION(
                      makeRStringLiteral(q, true, false), e.what()),
                    XQUERY_DYNAMIC_FAIL);
    } catch (...) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_UNKNOWN_EXCEPTION, XQUERY_DYNAMIC_FAIL);
    }
    return res;
}

static list<ustring> executeUstringQuery(const ustring& q,
                                         const xercesc::DOMDocument& doc,
                                         XQQuery& query,
                                         const map<ustring, ustring>* xqueryVars,
                                         int32* error)
{
    // Create a context object
    auto_ptr<DynamicContext> context(query.createDynamicContext());
    context->setContextItem(XMLHelper::instance().xconfig().createNode(&doc, context.get()));

    if (xqueryVars) {
        map<ustring, ustring>::const_iterator it = xqueryVars->begin();
        ItemFactory& f = *context->getItemFactory();
        for (; it != xqueryVars->end(); it++) {
            Item::Ptr value = f.createUntypedAtomic(
              ustring(it->second).impl().getTerminatedBuffer(), context.get());
            context->setExternalVariable(ustring(it->first).impl().getTerminatedBuffer(), value);
        }
    }

    // Execute the query, using the context
    list<ustring> res;
    try {
        Result result = query.execute(context.get());
        // save the results
        Item::Ptr item;
        while (item = result->next(context.get())) {
            res.push_back(item->asString(context.get()));
        }
    } catch (const XQException& e) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_EXECUTION(
                      makeUStringLiteral(q, true, false), UTF8(e.getError())),
                    XQUERY_DYNAMIC_FAIL);
    } catch (const std::exception& e) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_EXECUTION(
                      makeUStringLiteral(q, true, false), e.what()),
                    XQUERY_DYNAMIC_FAIL);
    } catch (...) {
        WARNORTHROW(SPLRuntimeInvalidArgument,
                    SPL_APPLICATION_RUNTIME_INVALID_XQUERY_UNKNOWN_EXCEPTION, XQUERY_DYNAMIC_FAIL);
    }
    return res;
}

list<rstring> xquery(const xml& value, const rstring& xqueryExpression)
{
    const xercesc::DOMDocument& doc = value.getInternal().getDocument();

    // Parse an XQuery expression
    XQQuery* query = compileQuery(ustring::fromUTF8(xqueryExpression), NULL);
    if (query) {
        return executeQuery(xqueryExpression, doc, *query, NULL, NULL);
    }
    return list<rstring>();
}

list<ustring> xquery(const xml& value, const ustring& xqueryExpression)
{
    const xercesc::DOMDocument& doc = value.getInternal().getDocument();

    // Parse an XQuery expression
    XQQuery* query = compileQuery(xqueryExpression, NULL);
    if (query) {
        return executeUstringQuery(xqueryExpression, doc, *query, NULL, NULL);
    }
    return list<ustring>();
}

list<rstring> xquery(const xml& value,
                     const rstring& xqueryExpression,
                     const map<rstring, rstring>& xqueryVars)
{
    const xercesc::DOMDocument& doc = value.getInternal().getDocument();

    // Parse an XQuery expression
    XQQuery* query = compileQuery(ustring::fromUTF8(xqueryExpression), NULL);
    if (query) {
        return executeQuery(xqueryExpression, doc, *query, &xqueryVars, NULL);
    }
    return list<rstring>();
}

list<ustring> xquery(const xml& value,
                     const ustring& xqueryExpression,
                     const map<ustring, ustring>& xqueryVars)
{
    const xercesc::DOMDocument& doc = value.getInternal().getDocument();

    // Parse an XQuery expression
    XQQuery* query = compileQuery(xqueryExpression, NULL);
    if (query) {
        return executeUstringQuery(xqueryExpression, doc, *query, &xqueryVars, NULL);
    }
    return list<ustring>();
}

list<rstring> xquery(const xml& value, const rstring& xqueryExpression, int32& error)
{
    error = XQUERY_SUCCESS;
    const xercesc::DOMDocument* doc = NULL;
    try {
        doc = &value.getInternal().getDocument();
    } catch (const SPLRuntimeInvalidXMLException& e) {
        SPLAPPTRC(L_INFO, e.getExplanation(), SPL_FUNC_DBG);
        error = XQUERY_DYNAMIC_FAIL;
    } catch (...) {
        error = XQUERY_DYNAMIC_FAIL;
    }

    if (!error) {
        // Parse an XQuery expression
        XQQuery* query = compileQuery(ustring::fromUTF8(xqueryExpression), &error);
        if (query) {
            return executeQuery(xqueryExpression, *doc, *query, NULL, &error);
        }
    }
    return list<rstring>();
}

list<ustring> xquery(const xml& value, const ustring& xqueryExpression, int32& error)
{
    error = XQUERY_SUCCESS;
    const xercesc::DOMDocument* doc = NULL;
    try {
        doc = &value.getInternal().getDocument();
    } catch (const SPLRuntimeInvalidXMLException& e) {
        SPLAPPTRC(L_INFO, e.getExplanation(), SPL_FUNC_DBG);
        error = XQUERY_DYNAMIC_FAIL;
    } catch (...) {
        error = XQUERY_DYNAMIC_FAIL;
    }

    // Parse an XQuery expression
    if (!error) {
        XQQuery* query = compileQuery(xqueryExpression, &error);
        if (query) {
            return executeUstringQuery(xqueryExpression, *doc, *query, NULL, &error);
        }
    }
    return list<ustring>();
}

list<rstring> xquery(const xml& value,
                     const rstring& xqueryExpression,
                     const map<rstring, rstring>& xqueryVars,
                     int32& error)
{
    error = XQUERY_SUCCESS;
    const xercesc::DOMDocument* doc = NULL;
    try {
        doc = &value.getInternal().getDocument();
    } catch (const SPLRuntimeInvalidXMLException& e) {
        SPLAPPTRC(L_INFO, e.getExplanation(), SPL_FUNC_DBG);
        error = XQUERY_DYNAMIC_FAIL;
    } catch (...) {
        error = XQUERY_DYNAMIC_FAIL;
    }

    if (!error) {
        // Parse an XQuery expression
        XQQuery* query = compileQuery(ustring::fromUTF8(xqueryExpression), &error);
        if (query) {
            return executeQuery(xqueryExpression, *doc, *query, &xqueryVars, &error);
        }
    }
    return list<rstring>();
}

list<ustring> xquery(const xml& value,
                     const ustring& xqueryExpression,
                     const map<ustring, ustring>& xqueryVars,
                     int32& error)
{
    error = XQUERY_SUCCESS;
    const xercesc::DOMDocument* doc = NULL;
    try {
        doc = &value.getInternal().getDocument();
    } catch (const SPLRuntimeInvalidXMLException& e) {
        SPLAPPTRC(L_INFO, e.getExplanation(), SPL_FUNC_DBG);
        error = XQUERY_DYNAMIC_FAIL;
    } catch (...) {
        error = XQUERY_DYNAMIC_FAIL;
    }

    // Parse an XQuery expression
    if (!error) {
        XQQuery* query = compileQuery(xqueryExpression, &error);
        if (query) {
            return executeUstringQuery(xqueryExpression, *doc, *query, &xqueryVars, &error);
        }
    }
    return list<ustring>();
}
}

}
}
