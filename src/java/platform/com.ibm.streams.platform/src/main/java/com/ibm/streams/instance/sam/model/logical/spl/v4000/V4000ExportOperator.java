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
import com.ibm.streams.instance.sam.model.logical.spl.ExportOperator;
import com.ibm.streams.instance.sam.model.logical.spl.PortKind;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveExportInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.platform.services.v4000.OperInstancePortConnectionType;
import com.ibm.streams.platform.services.v4000.OperInstancePortConnectionsType;
import com.ibm.streams.platform.services.v4000.PortKindType;
import com.ibm.streams.platform.services.v4000.PrimitiveOperInstancePortType;
import com.ibm.streams.platform.services.v4000.PrimitiveOperInstanceType;
import java.util.List;

public class V4000ExportOperator extends ExportOperator {

  public V4000ExportOperator(
      PrimitiveOperInstanceType primitiveOperInstanceType,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      SPLLogicalModel model)
      throws SAMInternalErrorException {

    super(
        primitiveOperInstanceType.getName(),
        primitiveOperInstanceType.getSourceIndex(),
        primitiveOperInstanceType.getIndex(),
        (primitiveOperInstanceType.getDescription() != null
            ? primitiveOperInstanceType.getDescription()
            : ""),
        annotations,
        owningComp,
        model.findOrCreatePrimitiveOperatorDefinitionIndex(
            model.getExportOperatorKind(),
            com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_CPlusPlus,
            model.getSplToolkitIndex()),
        model);

    if (primitiveOperInstanceType.getInputPorts() != null) {
      for (PrimitiveOperInstancePortType primitiveOperInstancePortType :
          primitiveOperInstanceType.getInputPorts().getPort()) {

        PrimitiveExportInputPort mpip =
            new PrimitiveExportInputPort(
                primitiveOperInstancePortType.getIndex(), /*operInstancePortConnectionsType,*/
                this,
                model);
        getInputPorts().add(mpip);

        OperInstancePortConnectionsType operInstancePortConnectionsType =
            primitiveOperInstancePortType.getConnections();
        if (operInstancePortConnectionsType != null) {
          List<OperInstancePortConnectionType> connection =
              operInstancePortConnectionsType.getConnection();
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
    model.getOperatorMap().put(getIndex(), this);
  }
}
