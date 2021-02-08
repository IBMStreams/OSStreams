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

#include <SPL/Runtime/Common/SystemMetricInfoFactory.h>

#include <SPL/Runtime/Common/RuntimeException.h>

using namespace std;
using namespace std::tr1;
using namespace SPL;

SystemMetricInfoFactory::SystemMetricInfoFactory()
{
    info_.resize(nTypes);
#include "SystemMetricInfoPopulator.h"
}

static auto_ptr<SystemMetricInfoFactory> instance(new SystemMetricInfoFactory());

SystemMetricInfo const& SystemMetricInfoFactory::getSystemMetricInfo(string const& name, Type type)
{
    return instance->getInfo(name, type);
}

SystemMetricInfo const& SystemMetricInfoFactory::getInfo(std::string const& name, Type type)
{
    unordered_map<string, SystemMetricInfo>::iterator it;
    it = info_[type].find(name);
    if (it == info_[type].end()) {
        THROW(SPLRuntime, "Invalid metric name '" << name << "' (should never happen)");
    }
    return it->second;
}

uint32_t SystemMetricInfoFactory::getSystemMetricInfoCount(Type type)
{
    return instance->getCount(type);
}

uint32_t SystemMetricInfoFactory::getCount(Type type)
{
    return info_[type].size();
}
