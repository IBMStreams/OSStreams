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

package com.ibm.streams.operator.samples.state;

import com.ibm.streams.operator.state.Checkpoint;
import com.ibm.streams.operator.state.StateHandler;
import java.io.IOException;
import java.io.Serializable;

/**
 * Generic typed {@link StateHandler} for
 *
 * @param <T> Type of the state, instances of the state must implement {@code java.io.Serializable}.
 * @since InfoSphere&reg; Streams Version 4.0
 */
public class OperatorState<T> implements StateHandler {

  private final Class<T> stateClass;
  private T state;

  public OperatorState(Class<T> stateClass) {
    this.stateClass = stateClass;
  }

  @Override
  public void close() throws IOException {}

  @Override
  public void drain() throws Exception {}

  @Override
  public void reset(Checkpoint checkpoint) throws Exception {
    setState(stateClass.cast(checkpoint.getInputStream().readObject()));
  }

  @Override
  public void checkpoint(Checkpoint checkpoint) throws Exception {
    checkpoint.getOutputStream().writeObject(getState());
  }

  @Override
  public void retireCheckpoint(long id) throws Exception {}

  @Override
  public void resetToInitialState() {
    setState(null);
  }

  public void setState(T state) {
    if (state != null && !(state instanceof Serializable)) throw new IllegalArgumentException();

    this.state = state;
  }

  public T getState() {
    return state;
  }
}
