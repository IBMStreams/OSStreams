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

public enum EMetric {
  /* average drain time */
  AVERAGE_DRAIN_TIME,
  /* average reset time */
  AVERAGE_RESET_TIME,
  /* last reset time */
  LAST_RESET_TIME,
  /* last time to reach consistent state */
  LAST_CONSISTENT_STATE_TIME,
  /* state of the consistent region */
  STATE,
  /* sequence id of the last completed drain */
  LAST_COMPLETED_DRAIN_SEQ_ID,
  /* sequence id of the last completed reset */
  LAST_COMPLETED_RESET_SEQ_ID,
  /* number of current reset attempts */
  CURRENT_RESET_ATTEMPT
}
