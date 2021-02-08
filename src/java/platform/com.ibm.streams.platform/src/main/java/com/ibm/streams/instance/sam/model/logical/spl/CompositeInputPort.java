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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionParameterError;
import java.math.BigInteger;

public class CompositeInputPort extends CompositePort {

  protected CompositeInputPort(BigInteger index, CompositeOperator owningComposite) {
    super(index, owningComposite);
  }

  public CompositeInputPort(CompositeInputPort other, CompositeOperator owningComposite) {
    super(other, owningComposite);
  }

  public void addIncomingReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : incomingConnections) {
      conn.setPortIndex(channelIndex);
      Connection mc = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Input);
      conn.addConnection(mc, model);
    }
  }

  public void fixOutgoingReverseConnection(
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind newPortKind)
      throws SAMInternalErrorException {
    for (Connection conn : outgoingConnections) {
      if (conn.getOperIndex().equals(oldToOperIndex)
          && conn.getPortIndex().equals(oldToPortIndex)) {
        conn.setOperIndex(newToOperIndex);
        conn.setPortIndex(newToPortIndex);
        conn.setPortKind(newPortKind);
        return;
      }
    } // end for
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  public void fixIncomingReverseConnection(
      BigInteger oldToOperIndex,
      BigInteger oldToPortIndex,
      BigInteger newToOperIndex,
      BigInteger newToPortIndex,
      PortKind newPortKind)
      throws SAMInternalErrorException {
    for (Connection conn : incomingConnections) {
      if (conn.getOperIndex().equals(oldToOperIndex)
          && conn.getPortIndex().equals(oldToPortIndex)) {
        conn.setOperIndex(newToOperIndex);
        conn.setPortIndex(newToPortIndex);
        conn.setPortKind(newPortKind);
        return;
      }
    } // end for
    throw SPLLogicalModel.traceAndReturnException("Internal error: unexpected condition");
  }

  public void createSplitters(ParallelRegion parallelRegion, SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    for (Connection conn : incomingConnections) {
      String streamName = conn.getStreamName(model);
      SplitterOperator newSplitter =
          new SplitterOperator(
              getIndex(),
              getOwningOperator().getOwningComposite(),
              streamName,
              parallelRegion,
              model);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "CompositeInputPort.createSplitters: creating splitter: " + newSplitter.getIndex());
      }
      Connection mc1 = new Connection(conn);
      newSplitter.addInputPortConnection(BigInteger.ZERO, mc1);

      Connection mc2 = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Input);
      newSplitter.addOutputPortConnection(BigInteger.ZERO, mc2);

      conn.fixReverseConnection(
          getOwningOperator().getIndex(),
          getIndex(),
          newSplitter.getIndex(),
          BigInteger.ZERO,
          PortKind.Input,
          model);

      conn.setOperIndex(newSplitter.getIndex());
      conn.setPortIndex(BigInteger.ZERO);
      conn.setPortKind(PortKind.Output);
    } // end for
  }
}
