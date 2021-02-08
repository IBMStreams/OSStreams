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
import com.ibm.streams.instance.sam.model.logical.spl.ImportOperator;
import com.ibm.streams.instance.sam.model.logical.spl.PortKind;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveImportOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import com.ibm.streams.platform.services.v4200.NameBasedImportType;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionType;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionsType;
import com.ibm.streams.platform.services.v4200.PortKindType;
import com.ibm.streams.platform.services.v4200.PrimitiveImportOperInstanceType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceOutputPortType;
import com.ibm.streams.platform.services.v4200.PropertyBasedImportType;
import java.math.BigInteger;
import java.util.List;

public class V4200ImportOperator extends ImportOperator {
  public V4200ImportOperator(
      PrimitiveImportOperInstanceType primitiveOperInstanceType,
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

    // build output port
    if (primitiveOperInstanceType.getOutputPorts() != null) {
      for (PrimitiveOperInstanceOutputPortType primitiveOperInstancePortType :
          primitiveOperInstanceType.getOutputPorts().getPort()) {

        PrimitiveImportOutputPort mpop =
            new PrimitiveImportOutputPort(primitiveOperInstancePortType.getIndex(), this, model);
        getOutputPorts().add(mpop);

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
            mpop.addConnection(mc);
          }
        }
      }
    }
    // Get the importedStream
    //        ImportedStreamsType importedStream = primitiveOperInstanceType.getImportedStream();
    //        assert (importedStream != null);
    NameBasedImportType nbit = primitiveOperInstanceType.getNameBasedImport();
    if (nbit != null) {
      // We only want to set the Import operator's properties once
      NameBasedImport nameBasedImport =
          new NameBasedImport(
              nbit.getApplicationName(),
              primitiveOperInstanceType.getName(),
              nbit.getStreamName(),
              nbit.getApplicationScope());
      getImportedStreams().getNameBasedImports().put(BigInteger.ZERO, nameBasedImport);
      if (primitiveOperInstanceType.getFilter() != null) {
        getImportedStreams().setFilter(primitiveOperInstanceType.getFilter());
      }
      setInitialized();
    }
    PropertyBasedImportType pbit = primitiveOperInstanceType.getPropertyBasedImport();
    if (pbit != null) {
      PropertyBasedImport propBasedImport =
          new PropertyBasedImport(
              pbit.getSubscription(),
              primitiveOperInstanceType.getName(),
              BigInteger.ZERO,
              pbit.getApplicationScope());
      getImportedStreams().setPropertyBasedImport(propBasedImport);
      if (primitiveOperInstanceType.getFilter() != null) {
        getImportedStreams().setFilter(primitiveOperInstanceType.getFilter());
      }
    }
  }
}
