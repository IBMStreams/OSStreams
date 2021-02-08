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

public class CompositeOutputPort extends CompositePort {

  protected CompositeOutputPort(BigInteger index, CompositeOperator owningComposite) {
    super(index, owningComposite);
  }

  public CompositeOutputPort(CompositeOutputPort other, CompositeOperator owningComposite) {
    super(other, owningComposite);
  }

  public void addOutgoingReverseConnections(BigInteger channelIndex, SPLLogicalModel model)
      throws SAMInternalErrorException {
    for (Connection conn : outgoingConnections) {
      conn.setPortIndex(channelIndex);
      Connection mc = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Output);
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

  public void createMergers(ParallelRegion parallelRegion, SPLLogicalModel model)
      throws SAMInternalErrorException, SubmissionParameterError {
    // To get the logical stream name we need to look at our incoming connection
    assert (getIncomingConnections().size() == 1);
    String logicalStreamName = getIncomingConnections().get(0).getLogicalStreamName(model);
    for (Connection conn : outgoingConnections) {
      MergerOperator newMerger =
          new MergerOperator(
              getOwningOperator().getOwningComposite(), logicalStreamName, parallelRegion, model);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("CompositeOutputPort.createMergers: create merger " + newMerger.getIndex());
      }
      Connection mc1 = new Connection(conn);
      newMerger.addOutputPortConnection(BigInteger.ZERO, mc1);

      Connection mc2 = new Connection(getOwningOperator().getIndex(), getIndex(), PortKind.Output);
      newMerger.addInputPortConnection(BigInteger.ZERO, mc2);

      conn.fixReverseConnection(
          getOwningOperator().getIndex(),
          getIndex(),
          newMerger.getIndex(),
          BigInteger.ZERO,
          PortKind.Output,
          model);

      conn.setOperIndex(newMerger.getIndex());
      conn.setPortIndex(BigInteger.ZERO);
      conn.setPortKind(PortKind.Input);
    } // end for
  }
}
