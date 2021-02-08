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

import com.ibm.streams.operator.internal.state.InternalCheckpointContext;

public class JNICheckpointBridge implements InternalCheckpointContext {

  private JNIBridge bridge;

  private final Kind kind;

  public JNICheckpointBridge(JNIBridge bridge) {
    this.bridge = bridge;
    kind = getCheckpointingInterval() == 0.0 ? Kind.OPERATOR_DRIVEN : Kind.PERIODIC;
  }

  @Override
  public Kind getKind() {
    return kind;
  }

  public boolean createCheckpoint() {
    return createCheckpointDefault(bridge.getCppOperator());
  }

  public boolean createCheckpoint(long key) {
    return createCheckpoint(bridge.getCppOperator(), key);
  }

  public boolean restoreCheckpoint(long key) {
    return restoreCheckpoint(bridge.getCppOperator(), key);
  }

  public boolean deleteCheckpoint(long key) {
    return deleteCheckpoint(bridge.getCppOperator(), key);
  }

  @Override
  public double getCheckpointingInterval() {
    return getCheckpointingInterval(bridge.getCppOperator());
  }

  @Override
  public boolean isEnabled() {
    return isEnabled(bridge.getCppOperator());
  }

  public static native boolean isCheckpointingOn(long cppOperator);

  public static native double getCheckpointingInterval(long cppOperator);

  public static native boolean createCheckpoint(long opPointer, long key);

  public static native boolean createCheckpointDefault(long opPointer);

  public static native boolean restoreCheckpoint(long opPointer, long key);

  public static native boolean deleteCheckpoint(long opPointer, long key);

  public static native boolean isEnabled(long cppOperator);
}
