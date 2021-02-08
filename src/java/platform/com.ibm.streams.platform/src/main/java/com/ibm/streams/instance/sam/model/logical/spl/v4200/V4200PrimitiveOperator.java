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

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.Connection;
import com.ibm.streams.instance.sam.model.logical.spl.PortKind;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperator;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.RuntimeConstant;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.CheckpointPolicy;
import com.ibm.streams.instance.sam.model.topology.Checkpointing;
import com.ibm.streams.instance.sam.model.topology.CongestionPolicy;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.Resources;
import com.ibm.streams.instance.sam.model.topology.ThreadedPort;
import com.ibm.streams.instance.sam.model.topology.TraceLevel;
import com.ibm.streams.instance.sam.model.topology.Transport;
import com.ibm.streams.platform.services.v4200.CheckpointingType;
import com.ibm.streams.platform.services.v4200.CoLocationType;
import com.ibm.streams.platform.services.v4200.CongestionPolicyType;
import com.ibm.streams.platform.services.v4200.ExLocationType;
import com.ibm.streams.platform.services.v4200.HostLocationType;
import com.ibm.streams.platform.services.v4200.JvmArgumentsType;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionType;
import com.ibm.streams.platform.services.v4200.OperInstancePortConnectionsType;
import com.ibm.streams.platform.services.v4200.PartitionsType;
import com.ibm.streams.platform.services.v4200.PoolLocationType;
import com.ibm.streams.platform.services.v4200.PortKindType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceInputPortType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceOutputPortType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperInstanceType;
import com.ibm.streams.platform.services.v4200.ResourcesType;
import com.ibm.streams.platform.services.v4200.RuntimeConstantType;
import com.ibm.streams.platform.services.v4200.RuntimeConstantsType;
import com.ibm.streams.platform.services.v4200.ThreadedPortType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

public class V4200PrimitiveOperator extends PrimitiveOperator {

  private static Checkpointing getCheckpointing(
      PrimitiveOperInstanceType primitiveOperInstance, V4200SPLLogicalModel model) {
    CheckpointingType checkpointing = primitiveOperInstance.getCheckpointing();

    if (checkpointing != null) {
      boolean isStandalone = SAMStaticContext.isStandaloneMode();
      if (isStandalone) {
        model.getUnsupportedConfigsWarning().add("config");
      } else {
        Double period = null;
        if (checkpointing.getPeriod() != null) {
          period = new Double(checkpointing.getPeriod());
        }
        return new Checkpointing(
            CheckpointPolicy.valueOf(checkpointing.getPolicy().name()), period);
      }
    }
    return null;
  }

  private static Resources getResources(PrimitiveOperInstanceType primitiveOperInstance) {
    ResourcesType resources = primitiveOperInstance.getResources();

    HostLocationType hl = resources.getHostLocation();
    String hostLocation = null;
    if (hl != null) {
      hostLocation = new String(hl.getHostname());
    }

    PoolLocationType pl = resources.getPoolLocation();
    PoolLocation poolLocation = null;
    if (pl != null) {
      poolLocation = new PoolLocation(pl.getPoolIndex(), pl.getInpoolIndex());
    }

    Set<String> resourceCoLocations = new HashSet<>();
    List<CoLocationType> rcl = resources.getCoLocation();
    if (rcl != null) {
      for (CoLocationType coloc : rcl) {
        resourceCoLocations.add(coloc.getColocId());
      }
    }

    Set<String> resourceExLocations = new HashSet<>();
    List<ExLocationType> rxl = resources.getExLocation();
    if (rxl != null) {
      for (ExLocationType exloc : rxl) {
        resourceExLocations.add(exloc.getExlocId());
      }
    }
    boolean resourceIsolation = (resources.isIsolation() != null) ? true : false;
    return new Resources(
        hostLocation, poolLocation, resourceCoLocations, resourceExLocations, resourceIsolation);
  }

  private static List<RuntimeConstant> getRuntimeConstants(
      PrimitiveOperInstanceType primitiveOperInstance, SPLLogicalModel model) {
    RuntimeConstantsType runtimeConstants = primitiveOperInstance.getRuntimeConstants();
    List<RuntimeConstant> list = new ArrayList<RuntimeConstant>();
    for (RuntimeConstantType runtimeConstant : runtimeConstants.getRuntimeConstant()) {
      RuntimeConstant rc =
          new RuntimeConstant(
              runtimeConstant.getName(),
              runtimeConstant.getValue(),
              runtimeConstant.getDefaultValue(),
              runtimeConstant.getSubmissionTimeValueIndex(),
              model);
      list.add(rc);
    }
    return list;
  }

  public V4200PrimitiveOperator(
      PrimitiveOperInstanceType primitiveOperInstance,
      List<Annotation> annotations,
      CompositeOperator owningComp,
      V4200SPLLogicalModel model)
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
        primitiveOperInstance.getDefinitionIndex(),
        primitiveOperInstance.isOptimized(),
        primitiveOperInstance.isRestartable(),
        primitiveOperInstance.isRelocatable(),
        primitiveOperInstance.getSharedObject().getUri(),
        primitiveOperInstance.getSharedObject().getWrapper(),
        TraceLevel.valueOf(primitiveOperInstance.getTracing().getLevel().name()),
        true,
        primitiveOperInstance.isSingleThreadedOnOutputs(),
        primitiveOperInstance.getSharedObject().getClazz(),
        getResources(primitiveOperInstance),
        getCheckpointing(primitiveOperInstance, model),
        getRuntimeConstants(primitiveOperInstance, model),
        model);

    model.getPrimitiveOperatorMap().put(primitiveOperInstance.getIndex(), this);

    // Do we have an JVM arguments?
    JvmArgumentsType args = primitiveOperInstance.getJvmArguments();
    if (args != null) {
      List<String> jvmArguments = getJvmArguments();
      for (String arg : args.getJvmArgument()) {
        jvmArguments.add(arg.toString());
      }
    }

    PartitionsType adlPartitions = primitiveOperInstance.getPartitions();
    if (adlPartitions != null) {
      List<CoLocationType> adlCoLocations = adlPartitions.getCoLocation();
      for (CoLocationType adlCoLoc : adlCoLocations) {
        getCoLocations().add(adlCoLoc.getColocId());
      }
      List<ExLocationType> adlExLocations = adlPartitions.getExLocation();
      for (ExLocationType adlExLoc : adlExLocations) {
        getExLocations().add(adlExLoc.getExlocId());
      }

      if (adlPartitions.isIsolation() != null) {
        setIsolation(adlPartitions.isIsolation());
      }
    }

    // build input ports
    if (primitiveOperInstance.getInputPorts() != null) {
      for (PrimitiveOperInstanceInputPortType primitiveOperInstancePortType :
          primitiveOperInstance.getInputPorts().getPort()) {
        BigInteger index = primitiveOperInstancePortType.getIndex();
        Encoding encoding = Encoding.valueOf(primitiveOperInstancePortType.getEncoding().name());
        Transport transport =
            Transport.valueOf(primitiveOperInstancePortType.getTransport().name());
        BigInteger tupleTypeIndex = primitiveOperInstancePortType.getTupleTypeIndex();
        String portName = primitiveOperInstancePortType.getName();
        Boolean isMutable = primitiveOperInstancePortType.isIsMutable();
        Boolean isControl = false;
        if (primitiveOperInstancePortType.isIsControl() != null) {
          isControl = primitiveOperInstancePortType.isIsControl();
        }
        ThreadedPort threadedPort = null;
        ThreadedPortType tp = primitiveOperInstancePortType.getThreadedPort();
        if (tp != null) {
          CongestionPolicyType cp = tp.getCongestionPolicy();
          BigInteger queueSize = tp.getQueueSize();
          threadedPort = new ThreadedPort(true, CongestionPolicy.valueOf(cp.value()), queueSize);
        }

        PrimitiveInputPort mpip =
            new PrimitiveInputPort(
                index,
                this,
                portName,
                encoding,
                transport,
                tupleTypeIndex,
                isMutable,
                isControl,
                threadedPort,
                model);
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
      for (PrimitiveOperInstanceOutputPortType primitiveOperInstancePortType :
          primitiveOperInstance.getOutputPorts().getPort()) {
        BigInteger index = primitiveOperInstancePortType.getIndex();
        Encoding encoding = Encoding.valueOf(primitiveOperInstancePortType.getEncoding().name());
        Transport transport =
            Transport.valueOf(primitiveOperInstancePortType.getTransport().name());
        BigInteger tupleTypeIndex = primitiveOperInstancePortType.getTupleTypeIndex();
        String portName = primitiveOperInstancePortType.getName();
        Boolean isMutable = primitiveOperInstancePortType.isIsMutable();
        String streamName = primitiveOperInstancePortType.getStreamName();
        Boolean isViewable = true;
        if (primitiveOperInstancePortType.isViewable() != null) {
          isViewable = primitiveOperInstancePortType.isViewable();
        }
        PrimitiveOutputPort mpip =
            new PrimitiveOutputPort(
                index,
                streamName,
                this,
                portName,
                encoding,
                transport,
                tupleTypeIndex,
                isMutable,
                isViewable,
                model);
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
