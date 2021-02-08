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

package com.ibm.streams.instance.sam.model.logical.spl.v4200;

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.Connection;
import com.ibm.streams.instance.sam.model.logical.spl.ExportOperator;
import com.ibm.streams.instance.sam.model.logical.spl.LogicalExportedStream;
import com.ibm.streams.instance.sam.model.logical.spl.PortKind;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveExportInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.ExportCongestionPolicyType;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionType;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionsType;
import com.ibm.streams.platform.services.v4200.PortKindType;
import com.ibm.streams.platform.services.v4200.PrimitiveExportOperInstanceType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceInputPortType;
import com.ibm.streams.platform.services.v4200.StreamPropertiesType;
import com.ibm.streams.platform.services.v4200.StreamPropertyType;
import java.util.ArrayList;
import java.util.List;

public class V4200ExportOperator extends ExportOperator {

  public V4200ExportOperator(
      PrimitiveExportOperInstanceType primitiveOperInstanceType,
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
        primitiveOperInstanceType.getDefinitionIndex(),
        model);

    if (primitiveOperInstanceType.getInputPorts() != null) {
      for (PrimitiveOperInstanceInputPortType primitiveOperInstancePortType :
          primitiveOperInstanceType.getInputPorts().getPort()) {

        PrimitiveExportInputPort mpip =
            new PrimitiveExportInputPort(primitiveOperInstancePortType.getIndex(), this, model);
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
    //        ExportedStreamType exportedStream = primitiveOperInstanceType.getExportedStream();
    //        String exportOperatorName = exportedStream.getExportOperName();
    Boolean allowFilter = primitiveOperInstanceType.isAllowFilter();
    String name = primitiveOperInstanceType.getStreamName();
    List<StreamProperty> streamProperties = null;
    if (primitiveOperInstanceType.getProperties() != null) {
      streamProperties = new ArrayList<>();
      StreamPropertiesType props = primitiveOperInstanceType.getProperties();
      for (StreamPropertyType prop : props.getProperty()) {
        String propName = prop.getName();
        String propType = prop.getType();
        List<String> values = prop.getValue();
        StreamProperty topProp = new StreamProperty(propName, propType, values);
        streamProperties.add(topProp);
      }
    }
    ExportCongestionPolicyType congestionPolicy = null;
    if (primitiveOperInstanceType.getCongestionPolicy() != null) {
      congestionPolicy =
          ExportCongestionPolicyType.getEnum(
              primitiveOperInstanceType.getCongestionPolicy().value());
    }
    LogicalExportedStream topExportedStream =
        new LogicalExportedStream(this, name, allowFilter, congestionPolicy, streamProperties);
    setExportedStream(topExportedStream);

    model.getOperatorMap().put(getIndex(), this);
  }
}
