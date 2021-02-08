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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.state.CheckpointContext;
import com.ibm.streams.spl.model.CheckpointKindType;

/** Context is independent of operator state so just have fixed pre-allocated ones. */
final class CompileTimeCheckpointContext implements CheckpointContext {

  private static final CheckpointContext PERIODIC = new CompileTimeCheckpointContext(Kind.PERIODIC);

  private static final CheckpointContext OPERATOR_DRIVEN =
      new CompileTimeCheckpointContext(Kind.OPERATOR_DRIVEN);

  static CheckpointContext getContext(CheckpointKindType kind) {
    switch (kind) {
      case NONE:
        return null;
      case PERIODIC:
        return PERIODIC;
      case OPERATOR_DRIVEN:
        return OPERATOR_DRIVEN;
      default:
        assert false;
        return null;
    }
  }

  private final Kind kind;

  CompileTimeCheckpointContext(Kind kind) {
    this.kind = kind;
  }

  @Override
  public Kind getKind() {
    return kind;
  }

  @Override
  public double getCheckpointingInterval() {
    throw CompileTimeState.runtimeOnly();
  }

  @Override
  public boolean createCheckpoint() {
    throw CompileTimeState.runtimeOnly();
  }

  @Override
  public boolean isEnabled() {
    throw CompileTimeState.runtimeOnly();
  }
}
