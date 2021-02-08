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

package com.ibm.streams.operator.internal.window;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.RuntimeStreamForwarder;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.ports.StreamAction.Type;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.internal.window.sliding.SlidingWindow;
import com.ibm.streams.operator.internal.window.tumbling.TumblingWindow;
import com.ibm.streams.operator.state.StateHandler;
import com.ibm.streams.operator.window.StreamWindow;

/**
 * A tuple processor that creates events for a StreamWindowListener and sends the tuples and
 * punctuation off to the real processor. The real processor invokes the process methods on the
 * user's operator. Thus this class acts as an "intercepter" for a windowed input port.
 *
 * <p>Sub-classes need to synchronize on this object when performing window operations.
 *
 * @param <T> Tuple type.
 */
public final class WindowProcessor<T> extends RuntimeStreamForwarder<T> {

  private final WindowHandler<T> windowHandler;

  public WindowProcessor(
      OperatorAdapter adapter, RuntimeStream<T> opInput, StreamWindow<T> window) {
    super(opInput);

    PartitionFactory<T, ?> partitionFactory;
    if (window.getType() == StreamWindow.Type.TUMBLING)
      partitionFactory = TumblingWindow.<T>getTumbleFactory(window);
    else partitionFactory = SlidingWindow.<T>getSlideFactory(window);

    windowHandler =
        window.isPartitioned()
            ? PartitionedHandler.getPartitionedHandler(adapter, window, partitionFactory)
            : new NonPartitionedHandler<T>(adapter, window, partitionFactory);

    windowHandler.kickOffActivation();
  }

  public StateHandler getStateHandler() {
    return windowHandler;
  }

  /*
   * Methods that implement TupleProcessor
   */

  /**
   * First process the punctuation through the window and then through the operator.
   *
   * <p>synchronized to enforce order through the operator, so that
   */
  @Override
  public final synchronized void mark(Punctuation mark) throws Exception {
    if (mark == Punctuation.FINAL_MARKER) {
      windowHandler.drain();
      windowHandler.cancelAllBackgroundTasks();
    }

    windowHandler.lockWindow();
    try {
      windowHandler.markAllPartitions(mark);
      super.mark(mark);
    } finally {
      windowHandler.unlockWindow();
    }
  }

  /**
   * First process the tuple through the window and then through the operator.
   *
   * <p>Insert a tuple into the window through its partition ensuring the window is locked during
   * the operation.
   */
  @Override
  public final void tuple(T tuple) throws Exception {
    windowHandler.lockWindow();
    try {
      windowHandler.insertIntoPartition(tuple);
      super.tuple(tuple);
    } finally {
      windowHandler.unlockWindow();
    }
  }
  /** On a resume, reactivate any window handling. */
  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    if (action.getType() == Type.RESUME_SUBMISSION) windowHandler.activatePartitions();

    return super.action(action);
    /// throw new UnsupportedOperationException();
  }
}
