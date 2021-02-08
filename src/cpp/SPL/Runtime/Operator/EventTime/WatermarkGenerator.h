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

#ifndef SRC_SPL_RUNTIME_OPERATOR_WATERMARKGENERATOR_H
#define SRC_SPL_RUNTIME_OPERATOR_WATERMARKGENERATOR_H

#include <SPL/Runtime/Operator/EventTime/EventTimeContext.h>
#include <SPL/Runtime/Type/Timestamp.h>
#include <SPL/Runtime/Utility/TimeUtils.h>

#include <boost/noncopyable.hpp>

namespace SPL {

/**
 * @brief Extracts time data from stream tuples and generates watermarks.
 */
class WatermarkGenerator : private boost::noncopyable
{
  public:
    WatermarkGenerator(EventTimeContext::impl& ctx, double lag, double minGap);

    ~WatermarkGenerator() {}

    Watermark getWatermarkFor(timestamp eventTime) const { return eventTime - lag_; }

    void setWatermark(Watermark const& wm);

  private:
    EventTimeContext::impl& ctx_;
    const Watermark lag_;    // watermark lag to the highest submitted tuple event time
    const Watermark minGap_; // minimum event time gap between successive watermarks
    Watermark nextWmOut_;    // threshold for the next Watermark value to submit
};
} // end namespace SPL
#endif /* SRC_SPL_RUNTIME_OPERATOR_WATERMARKGENERATOR_H */
