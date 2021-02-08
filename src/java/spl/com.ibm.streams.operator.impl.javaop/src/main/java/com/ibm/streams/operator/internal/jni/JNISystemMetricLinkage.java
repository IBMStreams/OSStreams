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

package com.ibm.streams.operator.internal.jni;

import com.ibm.streams.operator.internal.metrics.RuntimeMetric.MetricValue;

final class JNISystemMetricLinkage extends JNIMetricLinkage implements MetricValue {

  JNISystemMetricLinkage(JNIPointerState ptrs, long pointer) {
    super(ptrs, pointer);
  }

  @Override
  public void bump() {
    JNIBridge.bumpSystemMetricValue(getPointer());
  }

  @Override
  public void decrement() {
    JNIBridge.decrementSystemMetricValue(getPointer());
  }

  @Override
  public void setValue(long value) {
    JNIBridge.setSystemMetricValue(getPointer(), value);
  }

  @Override
  public void setMaxValue(long value) {
    JNIBridge.setMaxSystemMetricValue(getPointer(), value);
  }
}
