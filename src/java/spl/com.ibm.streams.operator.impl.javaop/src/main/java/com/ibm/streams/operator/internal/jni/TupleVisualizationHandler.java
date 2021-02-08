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

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.internal.object.ObjectTupleEncoder;
import com.ibm.streams.operator.internal.ports.RuntimeStream;
import com.ibm.streams.operator.internal.ports.StreamAction;
import com.ibm.streams.operator.internal.runtime.Schema;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;

/**
 * This handler supports tuple visualization for Java operators that connect directly to other Java
 * operators.
 */
public class TupleVisualizationHandler implements RuntimeStream<Tuple> {

  private RuntimeStream<Tuple> innerHandler = null;
  private volatile int viewCount = 0;
  private ObjectTupleEncoder encoder;
  private ByteOrder tupleByteOrder = ByteOrder.nativeOrder();
  private ByteBuffer tupleBuffer = null;
  private int operatorIndex = 0;
  private int outputPortIndex = 0;

  public TupleVisualizationHandler(
      RuntimeStream<Tuple> inputHandler, Schema schema, int operatorIndex, int outputPortIndex) {
    innerHandler = inputHandler;
    encoder = new ObjectTupleEncoder(schema);
    this.operatorIndex = operatorIndex;
    this.outputPortIndex = outputPortIndex;
  }

  public void addView(long propertiesHandle) {
    ++viewCount;
  }

  public void deleteView(long propertiesHandle) {
    --viewCount;
  }

  @Override
  public void tuple(Tuple tuple) throws Exception {
    if (isVisualizing()) {
      visualize(tuple);
    }
    innerHandler.tuple(tuple);
  }

  @Override
  public void mark(Punctuation mark) throws Exception {
    innerHandler.mark(mark);
  }

  @Override
  public <A> A action(StreamAction<A> action) throws Exception {
    return innerHandler.action(action);
  }

  private final boolean isVisualizing() {
    return viewCount != 0;
  }

  private synchronized void visualize(Tuple tuple) {
    // Serialize tuple, then pass it, back out through JNI
    // to the visualizer associated with this output port.
    Tuple encodableTuple = tuple.asReadOnlyTuple();
    long size = encoder.getMaxSerializedSize(tuple);

    // Don't allocate a new buffer for every tuple
    if (tupleBuffer == null || this.tupleBuffer.capacity() < size) {
      tupleBuffer = ByteBuffer.allocateDirect((int) size).order(tupleByteOrder);
    } else {
      tupleBuffer.clear();
    }
    encoder.encode(encodableTuple, tupleBuffer);
    JNISPLRuntimeBridge.visualizeTuple(
        operatorIndex, outputPortIndex, tupleBuffer, tupleBuffer.limit());
  }
}
