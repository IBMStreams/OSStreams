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

#include <SPL/Runtime/Operator/OperatorMetricsImpl.h>

#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/ApplicationRuntimeMessage.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Common/SystemMetricInfoFactory.h>
#include <SPL/Runtime/Operator/Operator.h>
#include <SPL/Runtime/Operator/OperatorContextImpl.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/Utility/LogTraceMessage.h>

#include <boost/filesystem/convenience.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <cassert>
#include <fstream>

namespace bf = boost::filesystem;

using namespace SPL;
using namespace Distillery;
using namespace Streams;
using namespace std;
using namespace std::tr1;

OperatorMetricsImpl::OperatorMetricsImpl(OperatorContextImpl& opctx, PEImpl& pe)
  : opName_(opctx.getName())
  , opIdx_(opctx.getIndex())
  , pe_(pe)
{
    isSingleThreadedOnInputs_ = opctx.isSingleThreadedOnInputs();
    isSingleThreadedOnOutputs_ = opctx.isSingleThreadedOnOutputs();

    uint32_t numIps = opctx.getNumberOfInputPorts();
    uint32_t numOps = opctx.getNumberOfOutputPorts();

    inputMetricsRaw_ = new InputPortMetricBlock[numIps];
    outputMetricsRaw_ = new OutputPortMetricBlock[numOps];

    // Check metric count
    assert(SystemMetricInfoFactory::getSystemMetricInfoCount(
             SystemMetricInfoFactory::OperatorInputPort) == nEnqueueWaits + 1);
    assert(SystemMetricInfoFactory::getSystemMetricInfoCount(
             SystemMetricInfoFactory::OperatorOutputPort) == nFinalPunctsSubmitted + 1);
    assert(SystemMetricInfoFactory::getSystemMetricInfoCount(SystemMetricInfoFactory::Operator) ==
           relativeOperatorCost + 1);

    for (uint32_t i = 0; i < numIps; ++i) {
        createInputPortMetric(i, nTuplesProcessed, "nTuplesProcessed");
        createInputPortMetric(i, nTuplesDropped, "nTuplesDropped");
        createInputPortMetric(i, nTuplesQueued, "nTuplesQueued");
        createInputPortMetric(i, nWindowPunctsProcessed, "nWindowPunctsProcessed");
        createInputPortMetric(i, nFinalPunctsProcessed, "nFinalPunctsProcessed");
        createInputPortMetric(i, nWindowPunctsQueued, "nWindowPunctsQueued");
        createInputPortMetric(i, nFinalPunctsQueued, "nFinalPunctsQueued");
        createInputPortMetric(i, queueSize, "queueSize");
        createInputPortMetric(i, maxItemsQueued, "maxItemsQueued");
        createInputPortMetric(i, recentMaxItemsQueued, "recentMaxItemsQueued");
        createInputPortMetric(i, recentMaxItemsQueuedInterval, "recentMaxItemsQueuedInterval");
        createInputPortMetric(i, nEnqueueWaits, "nEnqueueWaits");
    }

    for (uint32_t i = 0; i < numOps; ++i) {
        createOutputPortMetric(i, nTuplesSubmitted, "nTuplesSubmitted");
        createOutputPortMetric(i, nWindowPunctsSubmitted, "nWindowPunctsSubmitted");
        createOutputPortMetric(i, nFinalPunctsSubmitted, "nFinalPunctsSubmitted");
    }

    createSystemMetric(relativeOperatorCost, "relativeOperatorCost");
}

OperatorMetricsImpl::~OperatorMetricsImpl(void)
{
    APPTRC(L_DEBUG,
           "Deallocating metrics data structure for PE "
             << "'" << pe_.getPEId() << ", ' operator '" << opName_ << "'",
           SPL_METRIC_DBG);

    for (size_t i = 0, iu = systemMetrics_.size(); i < iu; ++i) {
        dumpMetricAtExit(false, *systemMetrics_[i]);
        delete systemMetrics_[i];
    }
    for (size_t i = 0, iu = inputMetrics_.size(); i < iu; ++i) {
        for (size_t j = 0, ju = inputMetrics_[i].size(); j < ju; ++j) {
            dumpMetricAtExit(true, *inputMetrics_[i][j]);
            delete inputMetrics_[i][j];
        }
    }
    for (size_t i = 0, iu = outputMetrics_.size(); i < iu; ++i) {
        for (size_t j = 0, ju = outputMetrics_[i].size(); j < ju; ++j) {
            dumpMetricAtExit(true, *outputMetrics_[i][j]);
            delete outputMetrics_[i][j];
        }
    }
    for (unordered_map<string, MetricImpl*>::const_iterator it = customMetrics_.begin();
         it != customMetrics_.end(); ++it) {
        dumpMetricAtExit(false, *it->second);
        delete it->second;
    }
    delete[] inputMetricsRaw_;
    delete[] outputMetricsRaw_;
}

void OperatorMetricsImpl::dumpMetricAtExit(bool isSystem, Metric& metric)
{
    int32_t dbgLevel = pe_.hasFailedOperators() ? L_ERROR : L_DEBUG;

    if (!pe_.isStandalone()) {
        APPTRC(dbgLevel,
               SPL_RUNTIME_OPERATOR_METRIC_FINAL_VALUE(
                 opName_, isSystem ? metric.getDescription() : metric.getName(),
                 metric.getValueNoLock()),
               SPL_METRIC_DBG);
    }
}

void OperatorMetricsImpl::createInputPortMetric(uint32_t port,
                                                InputPortMetricName name,
                                                string const& shortName)
{
    SystemMetricInfo const& mi = SystemMetricInfoFactory::getSystemMetricInfo(
      shortName, SystemMetricInfoFactory::OperatorInputPort);
    assert((InputPortMetricName)mi.getIndex() == name);
    const string& longName = mi.getDescription();
    Metric::Kind kind = mi.getKind();

    SystemMetricImpl* m = NULL;
    if (name == nTuplesQueued || name == nWindowPunctsQueued || name == nFinalPunctsQueued) {
        m = createItemsQueuedMetric(port, name, shortName, longName, kind);
    } else if (name == recentMaxItemsQueued) {
        m = createRecentMaxItemsQueuedMetric(port, name, shortName, longName, kind);
    } else {
        m = createPortMetric(port, shortName, longName, kind, &(inputMetricsRaw_[port][name]));
    }
    if (inputMetrics_.size() == port) {
        inputMetrics_.push_back(vector<SystemMetricImpl*>());
    }
    inputMetrics_[port].push_back(m);
    inputMetricsRaw_[port][name].store(0, boost::memory_order_relaxed);
}

#if 0
void OperatorMetricsImpl::createInputPortProfMetric(uint32_t port,
                                                    InputPortProfMetricName name,
                                                    string const & shortName,
                                                    string const & longName,
                                                    Metric::Kind kind)
{
    MultiSamplePerfCounter * m = createPortProfMetric(port, shortName, longName, kind);
    if(inputProfMetrics_.size()==port)
        inputProfMetrics_.push_back(vector<MultiSamplePerfCounter *>());
    inputProfMetrics_[port].push_back(m);
}
#endif

void OperatorMetricsImpl::createOutputPortMetric(uint32_t port,
                                                 OutputPortMetricName name,
                                                 string const& shortName)
{
    SystemMetricInfo const& mi = SystemMetricInfoFactory::getSystemMetricInfo(
      shortName, SystemMetricInfoFactory::OperatorOutputPort);
    assert((OutputPortMetricName)mi.getIndex() == name);
    const string& longName = mi.getDescription();
    Metric::Kind kind = mi.getKind();
    SystemMetricImpl* m =
      createPortMetric(port, shortName, longName, kind, &(outputMetricsRaw_[port][name]));
    if (outputMetrics_.size() == port) {
        outputMetrics_.push_back(vector<SystemMetricImpl*>());
    }
    outputMetrics_[port].push_back(m);
    outputMetricsRaw_[port][name].store(0, boost::memory_order_relaxed);
}

#if 0
void OperatorMetricsImpl::createOutputPortProfMetric(uint32_t port,
                                                     OutputPortProfMetricName name,
                                                     string const & shortName,
                                                     string const & longName,
                                                     Metric::Kind kind)
{
    MultiSamplePerfCounter * m = createPortProfMetric(port, shortName, longName, kind);
    if(outputProfMetrics_.size()==port)
        outputProfMetrics_.push_back(vector<MultiSamplePerfCounter *>());
    outputProfMetrics_[port].push_back(m);
}
#endif

void OperatorMetricsImpl::createSystemMetric(SystemMetricName name, string const& shortName)
{
    SystemMetricInfo const& mi =
      SystemMetricInfoFactory::getSystemMetricInfo(shortName, SystemMetricInfoFactory::Operator);
    assert((SystemMetricName)mi.getIndex() == name);
    const string& longName = mi.getDescription();
    Metric::Kind kind = mi.getKind();
    MetricImpl* m = new MetricImpl(shortName, longName, kind);
    systemMetrics_.push_back(m);
}

string OperatorMetricsImpl::makePortMetricDescription(uint32_t port, string const& longName)
{
    stringstream strLong;
    strLong.imbue(locale::classic());
    strLong << longName << " (port " << port << ")";
    return strLong.str();
}

SystemMetricImpl* OperatorMetricsImpl::createPortMetric(uint32_t port,
                                                        string const& shortName,
                                                        string const& longName,
                                                        Metric::Kind kind,
                                                        boost::atomic<int64_t>* value)
{
    SystemMetricImpl* np =
      new SystemMetricImpl(shortName, makePortMetricDescription(port, longName), kind, value);
    return np;
}

SystemMetricImpl* OperatorMetricsImpl::createItemsQueuedMetric(uint32_t port,
                                                               InputPortMetricName name,
                                                               string const& shortName,
                                                               string const& longName,
                                                               Metric::Kind kind)
{
    boost::atomic<int64_t>*enqueuePtr, *dequeuePtr, *maxPtr;
    if (name == nTuplesQueued) {
        enqueuePtr = &inputMetricsRaw_[port].nTuplesEnqueued_;
        dequeuePtr = &inputMetricsRaw_[port].nTuplesDequeued_;
    } else if (name == nWindowPunctsQueued) {
        enqueuePtr = &inputMetricsRaw_[port][name];
        dequeuePtr = &inputMetricsRaw_[port].nWindowPunctsDequeued_;
    } else if (name == nFinalPunctsQueued) {
        enqueuePtr = &inputMetricsRaw_[port][name];
        dequeuePtr = &inputMetricsRaw_[port].nFinalPunctsDequeued_;
    } else {
        assert(!"invalid metric name");
        enqueuePtr = NULL;
        dequeuePtr = NULL;
    }
    enqueuePtr->store(0, boost::memory_order_relaxed);
    dequeuePtr->store(0, boost::memory_order_relaxed);
    maxPtr = &inputMetricsRaw_[port][recentMaxItemsQueued];
    SystemMetricImpl* np = new ItemsQueuedSystemMetricImpl(
      shortName, makePortMetricDescription(port, longName), kind, enqueuePtr, dequeuePtr, maxPtr);
    return np;
}

SystemMetricImpl* OperatorMetricsImpl::createRecentMaxItemsQueuedMetric(uint32_t port,
                                                                        InputPortMetricName name,
                                                                        string const& shortName,
                                                                        string const& longName,
                                                                        Metric::Kind kind)
{
    boost::atomic<int64_t>* curMaxPtr = &inputMetricsRaw_[port][recentMaxItemsQueued];
    int64_t* prevMaxPtr = &inputMetricsRaw_[port].prevRecentMaxItemsQueued_;
    *prevMaxPtr = 0;
    SystemMetricImpl* np = new RecentMaxItemsQueuedSystemMetricImpl(
      shortName, makePortMetricDescription(port, longName), kind, curMaxPtr, prevMaxPtr);
    return np;
}

#if 0
SPL::MultiSamplePerfCounter * OperatorMetricsImpl::createPortProfMetric(uint32_t port,
                                                       string const & shortName,
                                                       string const & longName,
                                                       Metric::Kind kind)
{
    stringstream strShort, strLong;
    strShort.imbue(locale::classic());
    strLong.imbue(locale::classic());
    strShort << shortName << port;
    strLong << longName << " (port " << port << ")";
    SPL::MultiSamplePerfCounter * np =
        new SPL::MultiSamplePerfCounter(strShort.str(), strLong.str(), kind, reservoirSize_);
    return np;
}
#endif

Metric& OperatorMetricsImpl::createCustomMetric(string const& name,
                                                string const& longName,
                                                Metric::Kind kind)
{
    Distillery::AutoMutex am(mutex_);
    if (customMetrics_.count(name)) {
        // see defect 41226; we used to throw an exception, now we just log a warning
        APPTRC(L_WARN, SPL_RUNTIME_EXISTING_METRIC(name), SPL_OPER_DBG);
        return *customMetrics_[name];
    }
    MetricImpl* m = new MetricImpl(name, longName, kind);
    customMetrics_[name] = m;

    pe_.getPlatform().addCustomMetric(m->getName(), m->getKindName(), m->getDescription(),
                                      getOperatorName());
    return *m;
}

Metric& OperatorMetricsImpl::getCustomMetricByName(string const& name)
{
    Distillery::AutoMutex am(mutex_);
    std::tr1::unordered_map<std::string, MetricImpl*>::iterator it = customMetrics_.find(name);
    if (it != customMetrics_.end()) {
        return *it->second;
    }
    SPLTRACEMSGANDTHROW(SPLRuntimeInvalidMetric, L_ERROR, SPL_RUNTIME_UNKNOWN_METRIC(name),
                        SPL_OPER_DBG);
}

bool OperatorMetricsImpl::hasCustomMetric(string const& name)
{
    Distillery::AutoMutex am(mutex_);
    return customMetrics_.count(name);
}

vector<string> OperatorMetricsImpl::getCustomMetricNames() const
{
    vector<string> res;
    Distillery::AutoMutex am(mutex_);
    std::tr1::unordered_map<std::string, MetricImpl*>::const_iterator it;
    for (it = customMetrics_.begin(); it != customMetrics_.end(); it++) {
        res.push_back(it->first);
    }
    return res;
}

const Metric& OperatorMetricsImpl::getOperatorMetric(SystemMetricName metric) const
{
    systemMetrics_[metric]->setValue(pe_.getOperatorRelativeCost(opIdx_));
    return *systemMetrics_[metric];
}

Metric& OperatorMetricsImpl::getOperatorMetric(SystemMetricName metric)
{
    systemMetrics_[metric]->setValue(pe_.getOperatorRelativeCost(opIdx_));
    return *systemMetrics_[metric];
}

void OperatorMetricsImpl::flush() const
{
    return; // nothing to do
}

uint32_t OperatorMetricsImpl::getIntervalMetricsPeriod() const
{
    return pe_.getIntervalMetricsPeriod();
}

void OperatorMetricsImpl::processIntervalMetrics(OperatorImpl const& op) const
{
    OperatorContextImpl const& opctx = op.getContextImpl();
    uint32_t numIps = opctx.getNumberOfInputPorts();

    // input port metrics
    for (uint32_t i = 0; i < numIps; ++i) {
        boolean hasQueue = (inputMetricsRaw_[i][queueSize].load(boost::memory_order_relaxed) > 0);
        if (!hasQueue) {
            continue;
        }
        // save max items queued for last interval and reset to current length
        boost::atomic<int64_t>* curMaxPtr = &inputMetricsRaw_[i][recentMaxItemsQueued];
        int64_t curMax = curMaxPtr->load(boost::memory_order_relaxed);
        inputMetricsRaw_[i].prevRecentMaxItemsQueued_ = curMax;
        int64_t curLen = inputMetrics_[i][nTuplesQueued]->getValue() +
                         inputMetrics_[i][nWindowPunctsQueued]->getValue() +
                         inputMetrics_[i][nFinalPunctsQueued]->getValue();
        if (curLen < curMax) {
            // curLen is unreliable due to concurrency so could be too big
            int64_t newMax = curMaxPtr->exchange(curLen, boost::memory_order_relaxed);
            if (newMax > curMax) {
                inputMetricsRaw_[i].prevRecentMaxItemsQueued_ = newMax;
            }
        }
    }
}

void OperatorMetricsImpl::getMetrics(PEMetricsInfo& peMetrics,
                                     OperatorImpl const& op,
                                     int64_t const& cost) const
{
    OperatorContextImpl const& opctx = op.getContextImpl();
    uint32_t numIps = opctx.getNumberOfInputPorts();
    uint32_t numOps = opctx.getNumberOfOutputPorts();

    // Push an empty OperatorMetricsInfo, and then accessit directly
    peMetrics.addOperatorMetrics(
      OperatorMetricsInfo(opctx.getGlobalIndex(), opctx.getName(), opctx.getLogicalName()));
    std::vector<OperatorMetricsInfo>& operMetrics = peMetrics.getOperatorMetrics();
    OperatorMetricsInfo& omi = operMetrics[operMetrics.size() - 1];

    // input port metrics
    for (uint32_t i = 0; i < numIps; ++i) {
        omi.addInputPortMetrics(PortMetricsInfo(i));
        std::vector<PortMetricsInfo>& p = omi.getInputPortMetrics();
        PortMetricsInfo& pmi = p[p.size() - 1];
        for (uint32_t j = 0; j < numInputPortMetrics; j++) {
            pmi.addMetrics(inputMetrics_[i][j]->getValue());
        }
    }

    // output port metrics
    for (uint32_t i = 0; i < numOps; ++i) {
        omi.addOutputPortMetrics(PortMetricsInfo(i));
        std::vector<PortMetricsInfo>& p = omi.getOutputPortMetrics();
        PortMetricsInfo& pmi = p[p.size() - 1];
        for (uint32_t j = 0; j < numOutputPortMetrics; j++) {
            pmi.addMetrics(outputMetrics_[i][j]->getValue());
        }
    }

    // system metric: relative operator cost
    omi.addSysMetrics(cost);

    // custom metrics
    SPL::OperatorMetricsHandler* metricsHandler = op.getContextImpl().getOperatorMetricsHandler();
    if (metricsHandler == NULL) {
        Distillery::AutoMutex am(mutex_);
        for (unordered_map<string, MetricImpl*>::const_iterator it = customMetrics_.begin();
             it != customMetrics_.end(); ++it) {
            MetricImpl& m = *it->second;
            omi.addMetrics(m.getName(), m.getValue());
        }
    } else {
        vector<string> metricNames = metricsHandler->getCustomMetricNames();
        vector<string>::const_iterator it;
        for (it = metricNames.begin(); it != metricNames.end(); it++) {
            omi.addMetrics(*it, metricsHandler->getCustomMetricValue(*it));
        }
    }
}
