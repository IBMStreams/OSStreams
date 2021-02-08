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

#include <K8S/ConcurrentQueue.h>
#include <K8S/K8SMetrics.h>
#include <K8S/K8STypes.h>
#include <SPL/Runtime/ProcessingElement/BasePE.h>
#include <TRANS/ConnectionState.h>
#include <UTILS/Thread.h>

#include <boost/shared_ptr.hpp>

#include <atomic>
#include <condition_variable>

K8S_NAMESPACE_BEGIN

class K8SProbeThread : public UTILS_NAMESPACE::Thread
{
  public:
    using PE = boost::shared_ptr<SPL::BasePE>;

    K8SProbeThread(PE const& pe);

    void* run(void* arg);
    void stop();

    bool isLive() const;

  private:
    void touch(std::string const& path);

    PE m_pe;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic<bool> m_running;
};

K8S_NAMESPACE_END
