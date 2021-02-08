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

#include <K8S/K8SMetrics.h>

DEBUG_NAMESPACE_USE
K8S_NAMESPACE_USE

/*
 * Counter metric.
 */

K8SMetric::Ref K8SCounterMetric::build(Registry const& r,
                                       std::string const& name,
                                       std::map<std::string, std::string> const& labels,
                                       std::string const& help,
                                       const int type)
{
    return Ref(new K8SCounterMetric(r, name, labels, help, type));
}

K8SCounterMetric::K8SCounterMetric(Registry const& r,
                                   std::string const& name,
                                   std::map<std::string, std::string> const& labels,
                                   std::string const& help,
                                   const int type)
  : m_type(type)
  , m_family(prometheus::BuildCounter().Name(name).Labels(labels).Help(help).Register(*r))
  , m_counters()
{}

void K8SCounterMetric::append(std::map<std::string, std::string> const& labels)
{
    m_counters.push_back(m_family.Add(labels));
}

void K8SCounterMetric::update(const size_t index, std::vector<int64_t> const& values)
{
    double prev = m_counters[index].get().Value();
    double next = values[m_type] - prev;
    m_counters[index].get().Increment(next);
}

void K8SCounterMetric::update(const size_t index, int64_t value)
{
    double prev = m_counters[index].get().Value();
    double next = value - prev;
    m_counters[index].get().Increment(next);
}

void K8SCounterMetric::clear()
{
    m_counters.clear();
}

size_t K8SCounterMetric::size() const
{
    return m_counters.size();
}

/*
 * Gauge metric.
 */

K8SMetric::Ref K8SGaugeMetric::build(Registry const& r,
                                     std::string const& name,
                                     std::map<std::string, std::string> const& labels,
                                     std::string const& help,
                                     const int type)
{
    return Ref(new K8SGaugeMetric(r, name, labels, help, type));
}

K8SGaugeMetric::K8SGaugeMetric(Registry const& r,
                               std::string const& name,
                               std::map<std::string, std::string> const& labels,
                               std::string const& help,
                               const int type)
  : m_type(type)
  , m_family(prometheus::BuildGauge().Name(name).Labels(labels).Help(help).Register(*r))
  , m_counters()
{}

void K8SGaugeMetric::append(std::map<std::string, std::string> const& labels)
{
    m_counters.push_back(m_family.Add(labels));
}

void K8SGaugeMetric::update(const size_t index, std::vector<int64_t> const& values)
{
    double next = values[m_type];
    m_counters[index].get().Set(next);
}

void K8SGaugeMetric::update(const size_t index, int64_t value)
{
    m_counters[index].get().Set(static_cast<double>(value));
}

void K8SGaugeMetric::clear()
{
    m_counters.clear();
}

size_t K8SGaugeMetric::size() const
{
    return m_counters.size();
}
