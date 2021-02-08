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

#include <SPL/Runtime/Common/MetricImpl.h>

#include <SPL/Runtime/Common/RuntimeDebug.h>
#include <SPL/Runtime/operatorProfileModel.h>
#include <UTILS/SupportFunctions.h>

using namespace std;
using namespace SPL;
using namespace Distillery;

#if 0
MetricImpl::MetricImpl(string const & myName,
                       string const & myLongName,
                       Kind kind,
                       uint32_t reservoirSize)
    : name_(myName), longName_(myLongName), kind_(kind), value_(0)
{
    APPTRC(L_DEBUG, "Allocating metric '" << name_ << "', kind '" << getKindName() << '\'', SPL_METRIC_DBG);
}
#endif

MetricImpl::MetricImpl(string const& myName, string const& myLongName, Kind kind)
  : name_(myName)
  , longName_(myLongName)
  , kind_(kind)
{
    APPTRC(L_DEBUG, "Allocating metric '" << name_ << "', kind '" << getKindName() << '\'',
           SPL_METRIC_DBG);
    value_.store(0, boost::memory_order_relaxed);
}

MetricImpl::~MetricImpl(void)
{
    APPTRC(L_DEBUG, "Deallocating metric '" << name_ << "'", SPL_METRIC_DBG);
}

const std::string MetricImpl::kindMap_[] = { "Gauge", "Counter", "Time" };
