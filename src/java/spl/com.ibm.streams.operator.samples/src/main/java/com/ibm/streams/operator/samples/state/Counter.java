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

/**
 * State handler that provides a {@code int} counter.
 *
 * @since InfoSphere&reg; Streams Version 4.0
 */
public class Counter implements StateHandler {

  private int count;

  @Override
  public void close() throws IOException {}

  @Override
  public void drain() throws Exception {}

  @Override
  public synchronized void reset(Checkpoint checkpoint) throws Exception {
    setCount(checkpoint.getInputStream().readInt());
  }

  @Override
  public synchronized void checkpoint(Checkpoint checkpoint) throws Exception {
    checkpoint.getOutputStream().writeInt(getCount());
  }

  @Override
  public void retireCheckpoint(long id) throws Exception {}

  @Override
  public synchronized void resetToInitialState() throws Exception {
    count = 0;
  }

  public synchronized int getCount() {
    return count;
  }

  public synchronized void setCount(int count) {
    this.count = count;
  }

  /**
   * Increment the counter by one.
   *
   * @return The value of the counter after incrementing by one.
   */
  public synchronized int increment() {
    return ++count;
  }

  /**
   * Decrement the count by one.
   *
   * @return The new value of the counter after deccrementing by one.
   */
  public synchronized int decrement() {
    return --count;
  }
}
