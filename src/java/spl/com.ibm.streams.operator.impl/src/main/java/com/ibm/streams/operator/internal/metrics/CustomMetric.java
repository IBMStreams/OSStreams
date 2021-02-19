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

import java.util.concurrent.atomic.AtomicLong;

public final class CustomMetric extends AbstractMetric {

  public CustomMetric(String name, Kind kind, String description) {
    super(name, kind, description);
    value = new AtomicLong();
  }

  protected AtomicLong value;

  @Override
  public final long getValue() {
    return value.get();
  }

  @Override
  public final void incrementValue(long delta) {
    if (delta != 0L) {
      value.addAndGet(delta);
    }
  }

  @Override
  public final void increment() {
    value.incrementAndGet();
  }

  @Override
  public final void setValue(long newValue) {
    value.set(newValue);
  }
}
