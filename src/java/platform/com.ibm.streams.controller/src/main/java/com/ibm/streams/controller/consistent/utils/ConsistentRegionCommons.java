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

package com.ibm.streams.controller.consistent.utils;

public interface ConsistentRegionCommons {
  /**
   * Type of a notification that indicates that trigger operators of a consistent region must start
   * draining its output ports.
   */
  String TRIGGER_DRAIN = "triggerDrain";
  /**
   * Type of a notification that indicates that trigger operators of a consistent region must reset
   * its state.
   */
  String TRIGGER_RESET = "triggerReset";
  /**
   * Type of a notification that indicates that an operator can resume tuple submission after
   * draining or resetting its output ports.
   */
  String RESUME_SUBMISSION = "resumeSubmission";
  /**
   * Type of a notification that indicates that all operators in a consistent region have drained
   * outstanding processing leaving the region in a consistent state.
   */
  String REGION_DRAINED = "drained";
  /**
   * Type of a notification that indicates that a reset has failed more than the specified number of
   * {@link #getMaxConsecutiveRestoreAttempts()}.
   */
  String RESET_FAILED = "resetFailed";
}
