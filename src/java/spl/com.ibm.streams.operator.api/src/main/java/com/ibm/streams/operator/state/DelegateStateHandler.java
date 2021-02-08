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

package com.ibm.streams.operator.state;

import java.io.IOException;

/**
 * TODO
 *
 * @param <T>
 * @since InfoSphere&reg; Streams Version 4.0
 */
public class DelegateStateHandler<T extends StateHandler> implements StateHandler {

  private final T handler;

  public DelegateStateHandler(T handler) {
    this.handler = handler;
  }

  public T getDelegateHandler() {
    return handler;
  }

  /** */
  @Override
  public void close() throws IOException {
    handler.close();
  }

  @Override
  public void drain() throws Exception {
    handler.drain();
  }

  @Override
  public void reset(Checkpoint checkpoint) throws Exception {
    handler.reset(checkpoint);
  }

  @Override
  public void checkpoint(Checkpoint checkpoint) throws Exception {
    handler.checkpoint(checkpoint);
  }

  @Override
  public void retireCheckpoint(long id) throws Exception {
    handler.retireCheckpoint(id);
  }

  @Override
  public void resetToInitialState() throws Exception {
    handler.resetToInitialState();
  }

  @Override
  public void prepareForNonBlockingCheckpoint(long id) throws Exception {
    handler.prepareForNonBlockingCheckpoint(id);
  }

  @Override
  public void regionCheckpointed(long id) throws Exception {
    handler.regionCheckpointed(id);
  }
}
