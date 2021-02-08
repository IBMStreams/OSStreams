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

#pragma once

#include <K8S/K8SMetrics.h>
#include <K8S/K8STypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <SPL/Runtime/ProcessingElement/PEMetricsInfo.h>
#include <UTILS/Thread.h>

#include <boost/shared_ptr.hpp>
#include <prometheus/exposer.h>
#include <prometheus/registry.h>

#include <atomic>
#include <condition_variable>

K8S_NAMESPACE_BEGIN

class K8SMetricsThread : public UTILS_NAMESPACE::Thread
{
  public:
    using PE = boost::shared_ptr<SPL::BasePE>;
    using ConnectionMetrics = const std::map<SPL::IDPair, std::vector<int64_t> >;
    using PEMetricsInfo = SPL::PEMetricsInfo;
    using MetricNames = std::unordered_map<std::string, K8SMetric::Ref>;
    using OperatorNames = std::unordered_map<std::string, MetricNames>;

    K8SMetricsThread(std::string const& job_name, PE const& pe, uint64_t pe_id);
    void* run(void* arg);
    void stop();
    void addCustomMetric(const std::string& name,
                         const std::string& kind,
                         const std::string& desc,
                         const std::string& operName);

  private:
    using Registry = std::shared_ptr<prometheus::Registry>;

    void initializePEInputMetrics(const PEMetricsInfo& peMetrics);
    void initializePEOutputMetrics(const PEMetricsInfo& peMetrics);
    void initializeOperatorMetrics(const PEMetricsInfo& peMetrics);
    void collectPEInputMetrics(const PEMetricsInfo& peMetrics);
    void collectPEOutputMetrics(const PEMetricsInfo& peMetrics);
    void collectOperatorMetrics(const PEMetricsInfo& peMetrics);

    void initializePEOutputConnectionMetrics(const size_t i, const ConnectionMetrics& connMetrics);

    std::string m_job_name;
    PE m_pe;
    std::string m_pe_id;
    std::atomic<bool> m_running;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    prometheus::Exposer m_exposer;
    Registry m_registry;
    K8SMetric::Ref m_pe_output_congestion_factor_metric;
    K8SMetric::Ref m_pe_n_tuples_filtered_out;
    std::vector<K8SMetric::Ref> m_pe_in_metrics;
    std::vector<K8SMetric::Ref> m_pe_out_metrics;
    std::vector<K8SMetric::Ref> m_op_in_metrics;
    std::vector<K8SMetric::Ref> m_op_out_metrics;
    std::vector<K8SMetric::Ref> m_op_system_metrics;
    OperatorNames m_op_custom_metrics;
};

K8S_NAMESPACE_END
