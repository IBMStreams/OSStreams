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

import com.ibm.streams.operator.metrics.PEMetrics;

public final class PEMetricsImpl<S extends ReadOnlyMetric>
    extends PortMetrics<S, PEMetrics.InputPortMetric, PEMetrics.OutputPortMetric>
    implements PEMetrics {

  public PEMetricsImpl(PEMetricsFactory<S> factory, int inputPortCount, int outputPortCount) {
    super(
        factory,
        inputPortCount,
        PEMetrics.InputPortMetric.class,
        outputPortCount,
        PEMetrics.OutputPortMetric.class);
  }
}
