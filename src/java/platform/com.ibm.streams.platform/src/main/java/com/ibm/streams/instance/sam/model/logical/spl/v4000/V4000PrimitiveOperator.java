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

package com.ibm.streams.instance.sam.model.logical.spl.v4000;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.Connection;
import com.ibm.streams.instance.sam.model.logical.spl.PortKind;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperator;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.TraceLevel;
import com.ibm.streams.platform.services.v4000.OperInstancePortConnectionType;
import com.ibm.streams.platform.services.v4000.OperInstancePortConnectionsType;
import com.ibm.streams.platform.services.v4000.PortKindType;
import com.ibm.streams.platform.services.v4000.PrimitiveOperInstancePortType;
import com.ibm.streams.platform.services.v4000.PrimitiveOperInstanceType;
import java.math.BigInteger;
import java.util.List;

public class V4000PrimitiveOperator extends PrimitiveOperator {
  private BigInteger toolkitIndex;

  public BigInteger getToolkitIndex() {
    return toolkitIndex;
  }

  public V4000PrimitiveOperator(
      PrimitiveOperInstanceType primitiveOperInstance,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      SPLLogicalModel model)
      throws SAMInternalErrorException {

    super(
        primitiveOperInstance.getName(),
        primitiveOperInstance.getSourceIndex(),
        primitiveOperInstance.getIndex(),
        (primitiveOperInstance.getDescription() != null
            ? primitiveOperInstance.getDescription()
            : ""),
        annotations,
        owningComp,
        BigInteger.ZERO,
        false,
        true,
        true,
        "",
        "none",
        TraceLevel.ERROR,
        true,
        true,
        "",
        null,
        null,
        null,
        model);
    model.getPrimitiveOperatorMap().put(primitiveOperInstance.getIndex(), this);
    toolkitIndex = primitiveOperInstance.getToolkitIndex();

    // build input ports
    if (primitiveOperInstance.getInputPorts() != null) {
      for (PrimitiveOperInstancePortType primitiveOperInstancePortType :
          primitiveOperInstance.getInputPorts().getPort()) {
        BigInteger index = primitiveOperInstancePortType.getIndex();
        PrimitiveInputPort mpip = new PrimitiveInputPort(index, /*connections,*/ this, model);
        inputPorts.add(mpip);
        OperInstancePortConnectionsType connections =
            primitiveOperInstancePortType.getConnections();
        if (connections != null) {
          List<OperInstancePortConnectionType> connection = connections.getConnection();
          for (OperInstancePortConnectionType operInstancePortConnectionType : connection) {
            Connection mc =
                new Connection(
                    operInstancePortConnectionType.getOperIndex(),
                    operInstancePortConnectionType.getPortIndex(),
                    operInstancePortConnectionType.getPortKind() == PortKindType.INPUT
                        ? PortKind.Input
                        : PortKind.Output);
            mpip.addConnection(mc);
          }
        }
      }
    }

    // Cleanup
    // build output ports
    if (primitiveOperInstance.getOutputPorts() != null) {
      for (PrimitiveOperInstancePortType primitiveOperInstancePortType :
          primitiveOperInstance.getOutputPorts().getPort()) {
        BigInteger index = primitiveOperInstancePortType.getIndex();
        PrimitiveOutputPort mpip = new PrimitiveOutputPort(index, "", this, model);
        outputPorts.add(mpip);
        OperInstancePortConnectionsType connections =
            primitiveOperInstancePortType.getConnections();
        if (connections != null) {
          List<OperInstancePortConnectionType> connection = connections.getConnection();
          for (OperInstancePortConnectionType operInstancePortConnectionType : connection) {
            Connection mc =
                new Connection(
                    operInstancePortConnectionType.getOperIndex(),
                    operInstancePortConnectionType.getPortIndex(),
                    operInstancePortConnectionType.getPortKind() == PortKindType.INPUT
                        ? PortKind.Input
                        : PortKind.Output);
            mpip.getConnections().add(mc);
          }
        }
      }
    }
  }
}
