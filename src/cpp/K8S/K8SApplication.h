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

#include <K8S/K8SConnectivityThread.h>
#include <K8S/K8SConsistentRegionThread.h>
#include <K8S/K8SMetricsThread.h>
#include <K8S/K8SProbeThread.h>
#include <K8S/K8SSubscriptionsThread.h>
#include <K8S/K8STypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <UTILS/DistilleryApplication.h>
#include <boost/shared_ptr.hpp>

K8S_NAMESPACE_BEGIN

class K8SApplication : public UTILS_NAMESPACE::DistilleryApplication
{
  public:
    K8SApplication();

    int run(const UTILS_NAMESPACE::arg_vector_t& args);

  protected:
    std::ostream& printDescription(std::ostream& o) const;

  private:
    using PE = boost::shared_ptr<SPL::BasePE>;

    void termHandler(int sig);

    PE m_pe;
    std::shared_ptr<K8SMetricsThread> m_metrics;
    std::shared_ptr<K8SProbeThread> m_probe;
    std::shared_ptr<K8SConnectivityThread> m_connectivity;
    std::shared_ptr<K8SSubscriptionsThread> m_subscriptions;
    std::shared_ptr<K8SConsistentRegionThread> m_consistentregion;
    int m_retCode;
};

K8S_NAMESPACE_END
