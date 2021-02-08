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

#include <K8S/K8SMetricsThread.h>
#include <SPL/Runtime/Operator/OperatorMetrics.h>
#include <SPL/Runtime/ProcessingElement/BasePEImpl.h>
#include <SPL/Runtime/ProcessingElement/PEMetrics.h>
#include <algorithm>
#include <chrono>
#include <thread>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE

K8SMetricsThread::K8SMetricsThread(std::string const& job_name, PE const& pe, uint64_t pe_id)
  : m_job_name(job_name)
  , m_pe(pe)
  , m_pe_id(std::to_string(pe_id))
  , m_running(true)
  , m_mutex()
  , m_cv()
  , m_exposer("0.0.0.0:9696")
  , m_registry(std::make_shared<prometheus::Registry>())
  ,

  // Note that I don't believe the index values are part of an enum anywhere like the other
  // metrics. I am basing their position off of the metrics gathering implementation in
  // PEMetricsImpl::getMetrics().
  m_pe_output_congestion_factor_metric(
    K8SGaugeMetric::build(m_registry,
                          "pe_output_congestion_factor",
                          { { "job", m_job_name }, { "pe", m_pe_id } },
                          "Congestion factor seen by the connection",
                          0))
  , m_pe_n_tuples_filtered_out(
      K8SCounterMetric::build(m_registry,
                              "pe_n_tuples_filtered_out",
                              { { "job", m_job_name }, { "pe", m_pe_id } },
                              "Number of tuples filtered out by connection",
                              1))
  ,

  // Note that the order of the metrics below matches the order established in the public
  // enum PEMetrics::InputPortMetricName.
  m_pe_in_metrics({ K8SCounterMetric::build(m_registry,
                                            "pe_input_n_tuples_processed",
                                            { { "job", m_job_name }, { "pe", m_pe_id } },
                                            "Processed input tuples count",
                                            SPL::PEMetrics::nTuplesProcessed),
                    K8SCounterMetric::build(m_registry,
                                            "pe_input_n_tuple_bytes_processed",
                                            { { "job", m_job_name }, { "pe", m_pe_id } },
                                            "Processed input tuple bytes count",
                                            SPL::PEMetrics::nTupleBytesProcessed),
                    K8SCounterMetric::build(m_registry,
                                            "pe_input_n_window_puncts_processed",
                                            { { "job", m_job_name }, { "pe", m_pe_id } },
                                            "Processed input window punctations count",
                                            SPL::PEMetrics::nWindowPunctsProcessed),
                    K8SCounterMetric::build(m_registry,
                                            "pe_input_n_final_puncts_processed",
                                            { { "job", m_job_name }, { "pe", m_pe_id } },
                                            "Processed input final punctations count",
                                            SPL::PEMetrics::nFinalPunctsProcessed) })
  ,

  // Note that the order of the metrics below matches the order established in the public
  // enum PEMetrics::OutputPortMetricName.
  m_pe_out_metrics({ K8SCounterMetric::build(m_registry,
                                             "pe_output_n_tuples_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Submitted output tuples count",
                                             SPL::PEMetrics::nTuplesSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_tuple_bytes_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Submitted output tuple bytes count",
                                             SPL::PEMetrics::nTupleBytesSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_window_puncts_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Submitted output window punctations count",
                                             SPL::PEMetrics::nWindowPunctsSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_final_puncts_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Submitted output final punctations count",
                                             SPL::PEMetrics::nFinalPunctsSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_broken_connections",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Broken connections count",
                                             SPL::PEMetrics::nBrokenConnections),
                     K8SGaugeMetric::build(m_registry,
                                           "pe_output_n_required_connecting",
                                           { { "job", m_job_name }, { "pe", m_pe_id } },
                                           "Required connections count",
                                           SPL::PEMetrics::nRequiredConnecting),
                     K8SGaugeMetric::build(m_registry,
                                           "pe_output_n_optional_connecting",
                                           { { "job", m_job_name }, { "pe", m_pe_id } },
                                           "Optional connections count",
                                           SPL::PEMetrics::nOptionalConnecting),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_tuples_transmitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Transmitted output tuples count",
                                             SPL::PEMetrics::nTuplesTransmitted),
                     K8SCounterMetric::build(m_registry,
                                             "pe_output_n_tuple_bytes_transmitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Transmitted output tuple bytes count",
                                             SPL::PEMetrics::nTupleBytesTransmitted),
                     K8SGaugeMetric::build(m_registry,
                                           "pe_output_n_connections",
                                           { { "job", m_job_name }, { "pe", m_pe_id } },
                                           "Connections count",
                                           SPL::PEMetrics::nOptionalConnecting) })
  ,

  // Note that this order matches the order in the enum
  // SPL::OperatorMetrics::InputPortMetricName.
  m_op_in_metrics(
    { K8SCounterMetric::build(m_registry,
                              "op_input_n_tuples_processed",
                              { { "job", m_job_name }, { "pe", m_pe_id } },
                              "Number of tuples processed by the port",
                              SPL::OperatorMetrics::nTuplesProcessed),
      K8SCounterMetric::build(m_registry,
                              "op_input_n_tuples_dropped",
                              { { "job", m_job_name }, { "pe", m_pe_id } },
                              "Number of tuples dropped by the port",
                              SPL::OperatorMetrics::nTuplesDropped),
      K8SGaugeMetric::build(m_registry,
                            "op_input_n_tuples_queued",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Number of tuples queued by the port",
                            SPL::OperatorMetrics::nTuplesQueued),
      K8SCounterMetric::build(m_registry,
                              "op_input_n_window_puncts_processed",
                              { { "job", m_job_name }, { "pe", m_pe_id } },
                              "Number of window punctuations processed by the port",
                              SPL::OperatorMetrics::nWindowPunctsProcessed),
      K8SCounterMetric::build(m_registry,
                              "op_input_n_final_puncts_processed",
                              { { "job", m_job_name }, { "pe", m_pe_id } },
                              "Number of final punctuations processed by the port",
                              SPL::OperatorMetrics::nFinalPunctsProcessed),
      K8SGaugeMetric::build(m_registry,
                            "op_input_n_window_puncts_queued",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Number of window punctuations queued by the port",
                            SPL::OperatorMetrics::nWindowPunctsQueued),
      K8SGaugeMetric::build(m_registry,
                            "op_input_n_final_puncts_queued",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Number of final punctuations queued by the port",
                            SPL::OperatorMetrics::nFinalPunctsQueued),
      K8SGaugeMetric::build(m_registry,
                            "op_input_queue_size",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Size of queue for the port or 0 if no queue",
                            SPL::OperatorMetrics::queueSize),
      K8SGaugeMetric::build(m_registry,
                            "op_input_max_items_queued",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Largest number of items queue to the port",
                            SPL::OperatorMetrics::maxItemsQueued),
      K8SGaugeMetric::build(m_registry,
                            "op_input_recent_max_items_queued",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Recent largest number of items queued to the port",
                            SPL::OperatorMetrics::recentMaxItemsQueued),
      K8SGaugeMetric::build(m_registry,
                            "op_input_recent_max_items_queued_interval",
                            { { "job", m_job_name }, { "pe", m_pe_id } },
                            "Interval in milliseconds used to determine max items queued",
                            SPL::OperatorMetrics::recentMaxItemsQueuedInterval),
      K8SCounterMetric::build(
        m_registry,
        "op_input_n_enqueue_waits",
        { { "job", m_job_name }, { "pe", m_pe_id } },
        "Number of waits due to a full queue when enqueueing times for the port",
        SPL::OperatorMetrics::nEnqueueWaits) })
  ,

  // Note that this order matches the order in the enum
  // SPL::OperatorMetrics::OutputPortMetricName.
  m_op_out_metrics({ K8SCounterMetric::build(m_registry,
                                             "op_output_n_tuples_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Number of tuples submitted by the port",
                                             SPL::OperatorMetrics::nTuplesSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "op_output_n_window_puncts_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Number of window punctuations submitted by the port",
                                             SPL::OperatorMetrics::nWindowPunctsSubmitted),
                     K8SCounterMetric::build(m_registry,
                                             "op_output_n_final_puncts_submitted",
                                             { { "job", m_job_name }, { "pe", m_pe_id } },
                                             "Number of final punctuations submitted by the port",
                                             SPL::OperatorMetrics::nFinalPunctsSubmitted) }

                   )
  ,

  // Note that this order matches the order in the enum
  // SPL::OperatorMetrics::SystemMetricName.
  m_op_system_metrics({ K8SGaugeMetric::build(m_registry,
                                              "op_system_relative_operator_cost",
                                              { { "job", m_job_name }, { "pe", m_pe_id } },
                                              "Relative cost of the operator within PE scope",
                                              SPL::OperatorMetrics::relativeOperatorCost) })
  , m_op_custom_metrics()
{}

void K8SMetricsThread::initializePEInputMetrics(const PEMetricsInfo& peMetrics)
{
    auto& peInputMetrics = peMetrics.getInputPortMetrics();
    for (size_t i = 0; i < peInputMetrics.size(); ++i) {
        for (auto& m : m_pe_in_metrics) {
            m->append({ { "pe_port", std::to_string(i) } });
        }
    }
}

void K8SMetricsThread::initializePEOutputMetrics(const PEMetricsInfo& peMetrics)
{
    auto& peOutputMetrics = peMetrics.getOutputPortMetrics();
    for (size_t i = 0; i < peOutputMetrics.size(); ++i) {
        for (auto& m : m_pe_out_metrics) {
            m->append({ { "pe_port", std::to_string(i) } });
        }
        auto& connectionMetrics = peOutputMetrics[i].getConnectionMetrics();
        initializePEOutputConnectionMetrics(i, connectionMetrics);
    }
}

void K8SMetricsThread::initializePEOutputConnectionMetrics(const size_t i,
                                                           const ConnectionMetrics& connMetrics)
{
    for (auto& kv : connMetrics) {
        std::string pe_port =
          "(" + std::to_string(kv.first.first) + ":" + std::to_string(kv.first.second) + ")";
        m_pe_output_congestion_factor_metric->append(
          { { "pe_port", std::to_string(i) }, { "connection", pe_port } });
        m_pe_n_tuples_filtered_out->append(
          { { "pe_port", std::to_string(i) }, { "connection", pe_port } });
    }
}

void K8SMetricsThread::initializeOperatorMetrics(const PEMetricsInfo& peMetrics)
{
    auto& opMetrics = peMetrics.getOperatorMetrics();
    for (size_t i = 0; i < opMetrics.size(); ++i) {
        const std::string& opName = opMetrics[i].getName();
        const std::string& opLogicalName = opMetrics[i].getLogicalName();
        uint64_t opIndex = opMetrics[i].getIndex();
        /*
         * Add input port metric entries.
         */
        auto& opInputMetrics = opMetrics[i].getInputPortMetrics();
        for (size_t j = 0; j < opInputMetrics.size(); ++j) {
            for (auto& m : m_op_in_metrics) {
                m->append({ { "op", std::to_string(opIndex) },
                            { "name", opName },
                            { "logical_name", opLogicalName },
                            { "op_port", std::to_string(j) } });
            }
        }
        /*
         * Add output port metric entries.
         */
        auto& opOutputMetrics = opMetrics[i].getOutputPortMetrics();
        for (size_t j = 0; j < opOutputMetrics.size(); ++j) {
            for (auto& m : m_op_out_metrics) {
                m->append({ { "op", std::to_string(opIndex) },
                            { "name", opName },
                            { "logical_name", opLogicalName },
                            { "op_port", std::to_string(j) } });
            }
        }
        /*
         * Add system metric entries.
         */
        for (auto& m : m_op_system_metrics) {
            m->append({ { "name", opMetrics[i].getName() } });
        }
    }
}

void K8SMetricsThread::collectPEInputMetrics(const PEMetricsInfo& peMetrics)
{
    auto& peInputMetrics = peMetrics.getInputPortMetrics();
    for (size_t i = 0; i < peInputMetrics.size(); ++i) {
        for (auto& m : m_pe_in_metrics) {
            m->update(i, peInputMetrics[i].getMetrics());
        }
    }
}

void K8SMetricsThread::collectPEOutputMetrics(const PEMetricsInfo& peMetrics)
{
    auto& peOutputMetrics = peMetrics.getOutputPortMetrics();
    for (size_t i = 0; i < peOutputMetrics.size(); ++i) {
        for (auto& m : m_pe_out_metrics) {
            m->update(i, peOutputMetrics[i].getMetrics());
        }
        /*
         * Connection metrics are tricky. Because of dynamic connections, the vector of metrics can
         * (of course) arbitrarily change size. So before processing these metrics, we need to make
         * sure that both value vector and metric vector are of a size. If there is a size change,
         * we need to reset and re-initialize the metric vector.
         */
        bool reset = false;
        auto& connectionMetrics = peOutputMetrics[i].getConnectionMetrics();
        if (m_pe_output_congestion_factor_metric->size() != connectionMetrics.size()) {
            m_pe_output_congestion_factor_metric->clear();
            reset = true;
        }
        if (m_pe_n_tuples_filtered_out->size() != connectionMetrics.size()) {
            m_pe_n_tuples_filtered_out->clear();
            reset = true;
        }
        /*
         * Re-initialize the connection metrics upon reset.
         */
        if (reset) {
            SPCDBG(L_DEBUG, "Resetting connection metrics", K8S_GENERAL);
            initializePEOutputConnectionMetrics(i, connectionMetrics);
        }
        /*
         * Update the values.
         */
        size_t j = 0;
        for (auto& kv : connectionMetrics) {
            m_pe_output_congestion_factor_metric->update(j, kv.second);
            m_pe_n_tuples_filtered_out->update(j, kv.second);
            j += 1;
        }
    }
}

void K8SMetricsThread::collectOperatorMetrics(const PEMetricsInfo& peMetrics)
{
    size_t inIdx = 0, outIdx = 0;
    auto& opMetrics = peMetrics.getOperatorMetrics();
    for (size_t i = 0; i < opMetrics.size(); ++i) {
        /*
         * Collect operator input port metrics.
         */
        auto& opInputMetrics = opMetrics[i].getInputPortMetrics();
        for (size_t j = 0; j < opInputMetrics.size(); ++j, ++inIdx) {
            for (auto& m : m_op_in_metrics) {
                m->update(inIdx, opInputMetrics[j].getMetrics());
            }
        }
        /*
         * Collect operator output port metrics.
         */
        auto& opOutputMetrics = opMetrics[i].getOutputPortMetrics();
        for (size_t j = 0; j < opOutputMetrics.size(); ++j, ++outIdx) {
            for (auto& m : m_op_out_metrics) {
                m->update(outIdx, opOutputMetrics[j].getMetrics());
            }
        }
        /*
         * Collect operator system metrics.
         */
        for (auto& m : m_op_system_metrics) {
            m->update(i, opMetrics[i].getSysMetrics());
        }
        /*
         * Collect operator custom metrics.
         */
        auto& customMetrics = opMetrics[i].getMetrics();
        const std::string& opName = opMetrics[i].getName();
        for (const SPL::BaseMetricInfo& b : customMetrics) {
            const std::string& metricName = b.getName();
            if (m_op_custom_metrics[opName].count(metricName) == 0) {
                SPCDBG(L_ERROR,
                       "Metric " << metricName << " for operator " << opName << " does not exist",
                       K8S_GENERAL);
                THROW(K8SMetricsUnknownMetric, metricName);
            }
            m_op_custom_metrics[opName][metricName]->update(0, b.getValue());
        }
    }
}

void K8SMetricsThread::addCustomMetric(const std::string& metricName,
                                       const std::string& kind,
                                       const std::string& desc,
                                       const std::string& operName)
{
    std::lock_guard<std::mutex> lk(m_mutex);

    SPCDBG(L_DEBUG, "Adding custom metric " << metricName << " for operator " << operName,
           K8S_GENERAL);

    // Note that this access to m_op_custom_metrics creates a default value if we have not
    // seen this operator name yet.
    MetricNames& metrics = m_op_custom_metrics[operName];
    size_t pos = metrics.size();
    const std::map<std::string, std::string> labels = { { "job", m_job_name }, { "pe", m_pe_id } };

    // Note that we're assuming all time metrics can be represented as counters. This is
    // not necessarily the case if the time represents an interval which should be a gauge
    // since intervals are not monotonically increasing. Not sure what else to do here
    // since we have no semantic knowledge of the metric.
    if (kind == "Counter" || kind == "Time") {
        metrics[metricName] = K8SCounterMetric::build(m_registry, metricName, labels, desc, pos);
    } else if (kind == "Gauge") {
        metrics[metricName] = K8SGaugeMetric::build(m_registry, metricName, labels, desc, pos);
    } else {
        SPCDBG(L_ERROR,
               "Invalid custom metric kind: " << kind << " for operator " << operName
                                              << "; taking no action",
               K8S_GENERAL);
        return;
    }

    metrics[metricName]->append({ { "name", operName } });
}

void* K8SMetricsThread::run(void* arg)
{
    std::unique_lock<std::mutex> lk(m_mutex);

    SPCDBG(L_TRACE, "Metrics thread started", K8S_GENERAL);
    setName("Metrics");

    /*
     * Grab an initial version of the metrics. PEImpl::getMetrics blocks on PE
     * startup, so once that call succeeds, all normal metrics should exist.
     */
    PEMetricsInfo peMetrics;
    while (!m_pe->getImpl().getMetrics(peMetrics) && m_running) {
        m_cv.wait_for(lk, std::chrono::seconds(1));
    }
    /*
     * Early abortion.
     */
    if (!m_running) {
        return NULL;
    }

    initializePEInputMetrics(peMetrics);
    initializePEOutputMetrics(peMetrics);
    initializeOperatorMetrics(peMetrics);

    collectPEInputMetrics(peMetrics);
    collectPEOutputMetrics(peMetrics);
    collectOperatorMetrics(peMetrics);

    m_exposer.RegisterCollectable(m_registry);

    SPCDBG(L_TRACE, "Metrics thread initialized", K8S_GENERAL);
    /*
     * Run.
     */
    while (m_running) {
        /*
         * Sleep for 5 second. If the condition was notified, skip checking the
         * state.
         */
        if (m_cv.wait_for(lk, std::chrono::seconds(5)) == std::cv_status::no_timeout) {
            continue;
        }
        /*
         * Check if the metrics are available.
         */
        peMetrics.reset();
        if (!m_pe->getImpl().getMetrics(peMetrics)) {
            continue;
        }

        collectPEInputMetrics(peMetrics);
        collectPEOutputMetrics(peMetrics);
        collectOperatorMetrics(peMetrics);
    }
    return NULL;
}

void K8SMetricsThread::stop()
{
    m_running = false;
    m_cv.notify_all();
}
