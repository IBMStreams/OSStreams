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

#ifndef SRC_SPL_RUNTIME_OPERATOR_WATERMARKSENDER_H
#define SRC_SPL_RUNTIME_OPERATOR_WATERMARKSENDER_H

#include <SPL/Runtime/Type/Timestamp.h>
#include <SPL/Runtime/Utility/TimeUtils.h>

#include <limits>

namespace SPL {

/// One sender per operator output port.
class WatermarkSender
{
  public:
    /// Determines whether the sender is associated with event time.
    /// @return 'true' if not an event time sender, otherwise 'false'
    bool isInactive() const { return wm_ == std::numeric_limits<Watermark>::max(); }

    Watermark const& watermark() const { return wm_; }

    Watermark& watermark() { return wm_; }

    void setInactive() { wm_ = std::numeric_limits<Watermark>::max(); }

  private:
    Watermark wm_; // the latest submitted watermark
};
} // end namespace SPL
#endif /* SRC_SPL_RUNTIME_OPERATOR_WATERMARKSENDER_H_ */
