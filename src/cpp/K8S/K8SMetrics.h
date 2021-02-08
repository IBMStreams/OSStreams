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

#include <K8S/K8STypes.h>
#include <prometheus/counter.h>
#include <prometheus/registry.h>

K8S_NAMESPACE_BEGIN

class K8SMetric
{
  public:
    using Ref = std::shared_ptr<K8SMetric>;

    virtual ~K8SMetric() {}

    virtual void append(std::map<std::string, std::string> const& labels) = 0;
    virtual void update(const size_t index, std::vector<int64_t> const& values) = 0;
    virtual void update(const size_t index, int64_t value) = 0;

    virtual void clear() = 0;
    virtual size_t size() const = 0;
};

class K8SCounterMetric : public K8SMetric
{
  public:
    using Family = prometheus::Family<prometheus::Counter>;
    using Counters = std::vector<std::reference_wrapper<prometheus::Counter> >;
    using Registry = std::shared_ptr<prometheus::Registry>;

    static Ref build(Registry const& r,
                     std::string const& name,
                     std::map<std::string, std::string> const& labels,
                     std::string const& help,
                     const int type);

    void append(std::map<std::string, std::string> const& labels);
    void update(const size_t index, std::vector<int64_t> const& values);
    void update(const size_t index, int64_t value);

    void clear();
    size_t size() const;

  private:
    K8SCounterMetric(Registry const& r,
                     std::string const& name,
                     std::map<std::string, std::string> const& labels,
                     std::string const& help,
                     const int type);

    int m_type;
    Family& m_family;
    Counters m_counters;
};

class K8SGaugeMetric : public K8SMetric
{
  public:
    using Family = prometheus::Family<prometheus::Gauge>;
    using Counters = std::vector<std::reference_wrapper<prometheus::Gauge> >;
    using Registry = std::shared_ptr<prometheus::Registry>;

    static Ref build(Registry const& r,
                     std::string const& name,
                     std::map<std::string, std::string> const& labels,
                     std::string const& help,
                     const int type);

    void append(std::map<std::string, std::string> const& labels);
    void update(const size_t index, std::vector<int64_t> const& values);
    void update(const size_t index, int64_t value);

    void clear();
    size_t size() const;

  private:
    K8SGaugeMetric(Registry const& r,
                   std::string const& name,
                   std::map<std::string, std::string> const& labels,
                   std::string const& help,
                   const int type);

    int m_type;
    Family& m_family;
    Counters m_counters;
};

K8S_NAMESPACE_END
