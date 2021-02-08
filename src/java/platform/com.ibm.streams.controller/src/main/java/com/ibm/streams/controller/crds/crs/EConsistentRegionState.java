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

package com.ibm.streams.controller.crds.crs;

public enum EConsistentRegionState {
  /** The region has been started. */
  STARTED,

  /** The region is processing streams */
  PROCESSING,

  /**
   * The region is unhealthy, processing will resume once all processing elements within the region
   * become healthy.
   */
  UNHEALTHY,
  /**
   * The <em>establishing</em> operator has defined the <em>drain</em> and the region is draining
   * processing to become consistent.
   */
  DRAINING,

  /** The region is resetting to a previous consistent state. */
  RESETTING,

  /** The region has reached the maximum number of attempts to reset to a previous state. */
  MAXIMUM_RESET_ATTEMPTS_REACHED,

  /**
   * The region is processing streams, but there are still operators doing non-blocking
   * checkpointing that have not yet finished checkpointing.
   */
  CHECKPOINT_PENDING
}
