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

import com.ibm.streams.operator.metrics.Metric;

/**
 * Abstract class providing base Metric functionality, namely name and description functionality.
 */
public abstract class AbstractMetric implements Metric {

  private final String name;
  private final String description;
  private final Kind kind;

  protected AbstractMetric(String name, Kind kind, String description) {
    this.name = name;
    this.description = description;
    this.kind = kind;
  }

  @Override
  public final String getDescription() {
    return description;
  }

  @Override
  public final String getName() {
    return name;
  }

  @Override
  public final Metric.Kind getKind() {
    return kind;
  }

  @Override
  public final String toString() {
    return "Metric{"
        + getName()
        + "="
        + getValue()
        + ", "
        + getKind()
        + ", description="
        + getDescription()
        + "}";
  }
}
