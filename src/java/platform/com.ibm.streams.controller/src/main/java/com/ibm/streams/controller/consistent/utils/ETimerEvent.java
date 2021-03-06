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

public enum ETimerEvent {
  /**
   * Issued at the start of processing for a periodic region. Will result in the start of drain
   * processing.
   */
  DrainPeriod,
  /** Issued when a drain starts to time it out if it gets blocked. */
  DrainTimeout,
  /** Issued when a reset starts to time it out if it gets blocked. */
  ResetTimeout,
}
