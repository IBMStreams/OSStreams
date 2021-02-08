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

#include <SPL/Runtime/ProcessingElement/PEMetricsImpl.h>

#include <NAM/NAM_NameService.h>
#include <SAM/augmentedApplicationModel.h>
#include <SPL/Runtime/Common/RuntimeMessage.h>
#include <SPL/Runtime/Common/SystemMetricInfoFactory.h>
#include <SPL/Runtime/ProcessingElement/PEImpl.h>
#include <SPL/Runtime/ProcessingElement/PETransportPorts.h>
#include <UTILS/HostInfo.h>

#include <cassert>
#include <sstream>
#include <string>
#include <sys/resource.h>

using namespace SPL;
using namespace std;
using namespace Distillery;
using namespace Streams;

PEMetricsImpl::PEMetricsImpl(PEImpl& pe)
  : pe_(pe)
{
    uint32_t numIps = pe_.getPEModel().inputPorts().inputPort().size();
    uint32_t numOps = pe_.getPEModel().outputPorts().outputPort().size();

    inputMetricsRaw_ = new InputPortMetricBlock[numIps];
    outputMetricsRaw_ = new OutputPortMetricBlock[numOps];

    // Check metric count
    assert(SystemMetricInfoFactory::getSystemMetricInfoCount(
             SystemMetricInfoFactory::PEInputPort) == nFinalPunctsProcessed + 1);
    assert(SystemMetricInfoFactory::getSystemMetricInfoCount(
             SystemMetricInfoFactory::PEOutputPort) == nConnections + 1);
    // input port metrics
    for (uint32_t i = 0; i < numIps; ++i) {
        createInputPortMetric(i, nTuplesProcessed, "nTuplesProcessed");
        createInputPortMetric(i, nTupleBytesProcessed, "nTupleBytesProcessed");
        createInputPortMetric(i, nWindowPunctsProcessed, "nWindowPunctsProcessed");
        createInputPortMetric(i, nFinalPunctsProcessed, "nFinalPunctsProcessed");
    }

    // output port metrics
    for (uint32_t i = 0; i < numOps; ++i) {
        createOutputPortMetric(i, nTuplesSubmitted, "nTuplesSubmitted");
        createOutputPortMetric(i, nTupleBytesSubmitted, "nTupleBytesSubmitted");
        createOutputPortMetric(i, nWindowPunctsSubmitted, "nWindowPunctsSubmitted");
        createOutputPortMetric(i, nFinalPunctsSubmitted, "nFinalPunctsSubmitted");
        createOutputPortMetric(i, nBrokenConnections, "nBrokenConnections");
        createOutputPortMetric(i, nRequiredConnecting, "nRequiredConnecting");
        createOutputPortMetric(i, nOptionalConnecting, "nOptionalConnecting");
        createOutputPortMetric(i, nTuplesTransmitted, "nTuplesTransmitted");
        createOutputPortMetric(i, nTupleBytesTransmitted, "nTupleBytesTransmitted");
        createOutputPortMetric(i, nConnections, "nConnections");
    }
}

PEMetricsImpl::~PEMetricsImpl()
{
    APPTRC(L_DEBUG,
           "Deallocating metrics data structure for PE "
             << "'" << pe_.getPEId() << "'",
           SPL_METRIC_DBG);
    for (size_t i = 0, iu = inputMetrics_.size(); i < iu; ++i) {
        for (size_t j = 0, ju = inputMetrics_[i].size(); j < ju; ++j) {
            dumpMetricAtExit(*inputMetrics_[i][j]);
            delete inputMetrics_[i][j];
        }
    }
    for (size_t i = 0, iu = outputMetrics_.size(); i < iu; ++i) {
        for (size_t j = 0, ju = outputMetrics_[i].size(); j < ju; ++j) {
            dumpMetricAtExit(*outputMetrics_[i][j]);
            delete outputMetrics_[i][j];
        }
    }
    delete[] inputMetricsRaw_;
    delete[] outputMetricsRaw_;
}

void PEMetricsImpl::dumpMetricAtExit(Metric& metric)
{
    if (!pe_.isStandalone()) {
        APPTRC((pe_.hasFailedOperators() ? L_ERROR : L_DEBUG),
               SPL_RUNTIME_PE_METRIC_FINAL_VALUE(metric.getDescription(), metric.getValueNoLock()),
               SPL_METRIC_DBG);
    }
}

void PEMetricsImpl::createInputPortMetric(uint32_t port,
                                          InputPortMetricName name,
                                          string const& shortName)
{
    SystemMetricInfo const& mi =
      SystemMetricInfoFactory::getSystemMetricInfo(shortName, SystemMetricInfoFactory::PEInputPort);
    assert((InputPortMetricName)mi.getIndex() == name);
    const string& longName = mi.getDescription();
    Metric::Kind kind = mi.getKind();
    SystemMetricImpl* m =
      createPortMetric(port, shortName, longName, kind, &(inputMetricsRaw_[port][name]));
    if (inputMetrics_.size() == port) {
        inputMetrics_.push_back(vector<SystemMetricImpl*>());
    }
    inputMetrics_[port].push_back(m);
    inputMetricsRaw_[port][name].store(0, boost::memory_order_relaxed);
}

void PEMetricsImpl::createOutputPortMetric(uint32_t port,
                                           OutputPortMetricName name,
                                           string const& shortName)
{
    SystemMetricInfo const& mi = SystemMetricInfoFactory::getSystemMetricInfo(
      shortName, SystemMetricInfoFactory::PEOutputPort);
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

SystemMetricImpl* PEMetricsImpl::createPortMetric(uint32_t port,
                                                  string const& shortName,
                                                  string const& longName,
                                                  Metric::Kind kind,
                                                  boost::atomic<int64_t>* value)
{
    stringstream strLong;
    strLong.imbue(locale::classic());
    strLong << longName << " (port " << port << ")";
    SystemMetricImpl* np = new SystemMetricImpl(shortName, strLong.str(), kind, value);
    return np;
}

/// Fill in system PE metrics data (PE ports)
void PEMetricsImpl::getMetrics(PEMetricsInfo& peMetrics) const
{
    uint32_t numIps = pe_.getPEModel().inputPorts().inputPort().size();
    uint32_t numOps = pe_.getPEModel().outputPorts().outputPort().size();

    // input port metrics
    for (uint32_t i = 0; i < numIps; ++i) {
        APPTRC(L_TRACE, "Get input port metric" << i, SPL_METRIC_DBG);
        InputPortMetricBlock& m = inputMetricsRaw_[i];
        PortMetricsInfo pmi(i);
        for (uint32_t j = 0; j < numInputPortMetrics; j++) {
            pmi.addMetrics(m[j]);
        }
        peMetrics.addInputPortMetrics(pmi);
    }
    APPTRC(L_TRACE, "Finished input metrics", SPL_METRIC_DBG);

    // output port metrics
    for (uint32_t i = 0; i < numOps; ++i) {
        APPTRC(L_TRACE, "Get output port metric " << i, SPL_METRIC_DBG);
        OutputPortMetricBlock& m = outputMetricsRaw_[i];
        PEOutputPortMetricsInfo pmi(i);
        for (uint32_t j = 0; j < numOutputPortMetrics; j++) {
            pmi.addMetrics(m[j]);
        }
        // We need to add the congestion factor for the output port
        // Also put in a 0 for nTuplesFilteredOut at this time
        vector<DataSender::OutputCongestion> congestionfactors;
        vector<PETransportOPort::FilterMetric> metrics;
        APPTRC(L_TRACE, "Get congestion factors", SPL_METRIC_DBG);
        pe_.getCongestionFactorsAndMetrics(i, congestionfactors, metrics);
        for (uint32_t j = 0, uj = congestionfactors.size(); j < uj; j++) {
            APPTRC(L_TRACE, "Process congestion factor " << j, SPL_METRIC_DBG);
            const DataSender::OutputCongestion& cf = congestionfactors[j];
            vector<int64_t> opm;
            // Congestion factor
            opm.push_back(cf.congestionFactor);
            // nTuplesFilteredOut
            int64_t nTuplesFilteredOut = 0;
            for (uint32_t k = 0, uk = metrics.size(); k < uk; k++) {
                PETransportOPort::FilterMetric& fm = metrics[k];
                if (fm.match(cf.receiverPEId, cf.receiverPortId)) {
                    nTuplesFilteredOut = fm.nTuplesFilteredOut();
                    break;
                }
            }
            opm.push_back(nTuplesFilteredOut);
            pmi.addConnectionMetric(cf.receiverPEId, cf.receiverPortId, opm);
        }
        peMetrics.addOutputPortMetrics(pmi);
    }
    APPTRC(L_TRACE, "Finished output metrics", SPL_METRIC_DBG);
}
