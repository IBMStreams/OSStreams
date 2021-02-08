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
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * TODO
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public class OrderedStateHandlers implements StateHandler {

  private final List<StateHandler> handlers =
      Collections.synchronizedList(new ArrayList<StateHandler>());

  public OrderedStateHandlers(StateHandler... handlers) {
    for (StateHandler handler : handlers) addHandler(handler);
  }

  public final void addHandler(StateHandler handler) {
    if (handler == null) throw new NullPointerException();
    this.handlers.add(handler);
  }

  public final List<StateHandler> getHandlers() {
    return handlers;
  }

  @Override
  public void close() throws IOException {
    for (StateHandler handler : handlers) handler.close();
  }

  @Override
  public void drain() throws Exception {
    for (StateHandler handler : handlers) handler.drain();
  }

  @Override
  public void reset(Checkpoint checkpoint) throws Exception {
    for (StateHandler handler : handlers) handler.reset(checkpoint);
  }

  @Override
  public void checkpoint(Checkpoint checkpoint) throws Exception {
    for (StateHandler handler : handlers) handler.checkpoint(checkpoint);
  }

  @Override
  public void retireCheckpoint(long id) throws Exception {
    for (StateHandler handler : handlers) handler.retireCheckpoint(id);
  }

  @Override
  public void resetToInitialState() throws Exception {
    for (StateHandler handler : handlers) handler.resetToInitialState();
  }

  @Override
  public void prepareForNonBlockingCheckpoint(long id) throws Exception {
    for (StateHandler handler : handlers) handler.prepareForNonBlockingCheckpoint(id);
  }

  @Override
  public void regionCheckpointed(long id) throws Exception {
    for (StateHandler handler : handlers) handler.regionCheckpointed(id);
  }
}
