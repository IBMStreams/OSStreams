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

package com.ibm.streams.operator.internal.metrics;

/** Interface between the SPL Java operator runtime and the mechanism for getting port metrics. */
public interface PortMetricsFactory<
    S extends ReadOnlyMetric, I extends Enum<I>, O extends Enum<O>> {

  /**
   * Get a system metric corresponding to an input port Will be called the first time the metric is
   * accessed and then will be cached by the SPL Java runtime.
   */
  S getInputMetric(int port, I name);

  /**
   * Get a system metric corresponding to an output port Will be called the first time the metric is
   * accessed and then will be cached by the SPL Java runtime.
   */
  S getOutputMetric(int port, O name);
}
