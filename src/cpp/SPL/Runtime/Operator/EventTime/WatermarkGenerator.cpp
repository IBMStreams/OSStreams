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

#include <Runtime/Operator/EventTime/EventTimeContextImpl.h>
#include <Runtime/Operator/EventTime/WatermarkGenerator.h>
#include <SPL/Runtime/Function/BuiltinUtilFunctions.h>
#include <SPL/Runtime/Function/SPLCast.h>
#include <TRC/DistilleryDebug.h>
#include <UTILS/SupportFunctions.h>
#include <stdlib.h>

using namespace SPL;

WatermarkGenerator::WatermarkGenerator(EventTimeContext::impl& ctx, double lag, double minGap)
  : ctx_(ctx)
  , lag_(lag)
  , minGap_(minGap)
  , nextWmOut_(timestamp::min())
{
    SPLAPPTRC(L_INFO, "Format (secs,nanosecs,0): lag=" << lag_ << " minimum gap=" << minGap_,
              SPL_EVENTTIME_DBG);
}

void WatermarkGenerator::setWatermark(Watermark const& wm)
{
    if (wm >= nextWmOut_) {
        nextWmOut_ = wm + minGap_;
        SPLAPPTRC(L_TRACE, "Will submit watermark=" << wm << " nextWmOut_=" << nextWmOut_,
                  SPL_EVENTTIME_DBG);
        ctx_.submit(wm);
    }
}
