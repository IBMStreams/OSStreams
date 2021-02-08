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

package com.ibm.streams.operator.internal.ports;

import com.ibm.streams.operator.StreamingData.Punctuation;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.encoding.BinaryEncoding;
import com.ibm.streams.operator.internal.runtime.TupleProcessor;
import com.ibm.streams.operator.internal.runtime.api.InputPortsConduit;
import java.nio.ByteBuffer;

/** Process a tuple by serializing it and then delivering it to an InputPortsConduit. */
public class SerializeTuple2Conduit implements TupleProcessor<Integer, Tuple> {

  private final InputPortsConduit conduit;
  private final BinaryEncoding encoding;

  public SerializeTuple2Conduit(InputPortsConduit conduit, BinaryEncoding encoding) {
    this.conduit = conduit;
    this.encoding = encoding;
  }

  @Override
  public void processTuple(Integer port, Tuple tuple) throws Exception {
    ByteBuffer rawTuple = ByteBuffer.allocate((int) encoding.getEncodedSize(tuple));
    encoding.encodeTuple(tuple, rawTuple);
    rawTuple.flip();
    conduit.processRawTuple(port, rawTuple.asReadOnlyBuffer());
  }

  @Override
  public void processPunctuation(Integer port, Punctuation mark) throws Exception {
    conduit.processPunctuation(port, mark.ordinal());
  }
}
