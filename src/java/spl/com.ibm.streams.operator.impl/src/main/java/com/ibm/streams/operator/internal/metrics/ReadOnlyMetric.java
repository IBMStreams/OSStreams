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

/**
 * Read-only metric used for system metrics. Any method that modifies the metric throws an
 * UnsupportedOperationException.
 */
public abstract class ReadOnlyMetric extends AbstractMetric {

  protected ReadOnlyMetric(String name, Kind kind, String description) {
    super(name, kind, description);
  }

  @Override
  public final void incrementValue(long value) {
    throw readOnly();
  }

  @Override
  public final void setValue(long value) {
    throw readOnly();
  }

  @Override
  public void increment() {
    throw readOnly();
  }

  private RuntimeException readOnly() {
    return new UnsupportedOperationException();
  }
}
