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

import com.google.common.base.Objects;
import com.google.gson.Gson;
import com.google.gson.GsonBuilder;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.SamConstants;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionTimeErrorException;
import com.ibm.streams.instance.sam.model.logical.spl.AnnotationSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperator;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperatorDefinition;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.CompositeOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.CompositePortSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.ExportOperator;
import com.ibm.streams.instance.sam.model.logical.spl.ExportOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.ImportOperator;
import com.ibm.streams.instance.sam.model.logical.spl.ImportOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.ListAnnotationValueSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.LogicalAnnotationPrimitiveValue;
import com.ibm.streams.instance.sam.model.logical.spl.LogicalExportedStream;
import com.ibm.streams.instance.sam.model.logical.spl.LogicalHostpool;
import com.ibm.streams.instance.sam.model.logical.spl.MergerOperator;
import com.ibm.streams.instance.sam.model.logical.spl.MergerOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.ParallelRegion;
import com.ibm.streams.instance.sam.model.logical.spl.ParallelRegionSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveAnnotationValueSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveExportInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveExportInputPortSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveImportOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveImportOutputPortSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveInputPortSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperator;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperatorDefinition;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveOutputPortSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitivePort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveSplitterInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveSplitterOutputPort;
import com.ibm.streams.instance.sam.model.logical.spl.RuntimeConstant;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModel;
import com.ibm.streams.instance.sam.model.logical.spl.SPLLogicalModelSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.SplitterOperator;
import com.ibm.streams.instance.sam.model.logical.spl.SplitterOperatorSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.SubmissionTimeValue;
import com.ibm.streams.instance.sam.model.logical.spl.SubmissionTimeValueKind;
import com.ibm.streams.instance.sam.model.logical.spl.TopologyTupleTypeSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.TopologyTypeSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.TupleAnnotationValueSerializer;
import com.ibm.streams.instance.sam.model.logical.spl.URI;
import com.ibm.streams.instance.sam.model.topology.Annotation;
import com.ibm.streams.instance.sam.model.topology.AnnotationListValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationPrimitiveValue;
import com.ibm.streams.instance.sam.model.topology.AnnotationTupleValue;
import com.ibm.streams.instance.sam.model.topology.CheckpointPolicy;
import com.ibm.streams.instance.sam.model.topology.Checkpointing;
import com.ibm.streams.instance.sam.model.topology.CongestionPolicy;
import com.ibm.streams.instance.sam.model.topology.CustomMetric;
import com.ibm.streams.instance.sam.model.topology.CustomMetricKind;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.MembershipMode;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import com.ibm.streams.instance.sam.model.topology.Resources;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.instance.sam.model.topology.ThreadedPort;
import com.ibm.streams.instance.sam.model.topology.Toolkit;
import com.ibm.streams.instance.sam.model.topology.TopologyListType;
import com.ibm.streams.instance.sam.model.topology.TopologyMapType;
import com.ibm.streams.instance.sam.model.topology.TopologyPrimitiveType;
import com.ibm.streams.instance.sam.model.topology.TopologySetType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import com.ibm.streams.instance.sam.model.topology.TraceLevel;
import com.ibm.streams.instance.sam.model.topology.Transport;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.v4000.ApplicationType;
import com.ibm.streams.platform.services.v4000.CheckpointingType;
import com.ibm.streams.platform.services.v4000.CoLocationType;
import com.ibm.streams.platform.services.v4000.CompositeOperInstanceType;
import com.ibm.streams.platform.services.v4000.CompositeOperType;
import com.ibm.streams.platform.services.v4000.CompositeOpersType;
import com.ibm.streams.platform.services.v4000.CompositesType;
import com.ibm.streams.platform.services.v4000.CongestionPolicyType;
import com.ibm.streams.platform.services.v4000.CustomMetricIndiciesType;
import com.ibm.streams.platform.services.v4000.CustomMetricKindType;
import com.ibm.streams.platform.services.v4000.CustomMetricType;
import com.ibm.streams.platform.services.v4000.CustomMetricsType;
import com.ibm.streams.platform.services.v4000.EncodingType;
import com.ibm.streams.platform.services.v4000.ExLocationType;
import com.ibm.streams.platform.services.v4000.ExecutableType;
import com.ibm.streams.platform.services.v4000.ExportedStreamType;
import com.ibm.streams.platform.services.v4000.HostLocationType;
import com.ibm.streams.platform.services.v4000.HostType;
import com.ibm.streams.platform.services.v4000.HostpoolType;
import com.ibm.streams.platform.services.v4000.ImportedStreamsType;
import com.ibm.streams.platform.services.v4000.NameBasedImportType;
import com.ibm.streams.platform.services.v4000.OperDefinitionType;
import com.ibm.streams.platform.services.v4000.OperDefinitionsType;
import com.ibm.streams.platform.services.v4000.OperInstanceInputPortType;
import com.ibm.streams.platform.services.v4000.OperInstanceInputPortsType;
import com.ibm.streams.platform.services.v4000.OperInstanceOutputPortType;
import com.ibm.streams.platform.services.v4000.OperInstanceOutputPortsType;
import com.ibm.streams.platform.services.v4000.OperInstancePEInputConnectionType;
import com.ibm.streams.platform.services.v4000.OperInstancePEOutputConnectionType;
import com.ibm.streams.platform.services.v4000.OperInstancePortType;
import com.ibm.streams.platform.services.v4000.OperInstanceType;
import com.ibm.streams.platform.services.v4000.OperInstancesType;
import com.ibm.streams.platform.services.v4000.ParallelRegionType;
import com.ibm.streams.platform.services.v4000.ParallelRegionsType;
import com.ibm.streams.platform.services.v4000.PeInputPortType;
import com.ibm.streams.platform.services.v4000.PeInputPortsType;
import com.ibm.streams.platform.services.v4000.PeOutputPortType;
import com.ibm.streams.platform.services.v4000.PeOutputPortsType;
import com.ibm.streams.platform.services.v4000.PeType;
import com.ibm.streams.platform.services.v4000.PesType;
import com.ibm.streams.platform.services.v4000.PoolLocationType;
import com.ibm.streams.platform.services.v4000.PropertyBasedImportType;
import com.ibm.streams.platform.services.v4000.ResourcesType;
import com.ibm.streams.platform.services.v4000.RuntimeConstantType;
import com.ibm.streams.platform.services.v4000.RuntimeConstantsType;
import com.ibm.streams.platform.services.v4000.StreamPropertiesType;
import com.ibm.streams.platform.services.v4000.StreamPropertyType;
import com.ibm.streams.platform.services.v4000.SubmissionTimeValueType;
import com.ibm.streams.platform.services.v4000.SubmissionTimeValuesType;
import com.ibm.streams.platform.services.v4000.TagType;
import com.ibm.streams.platform.services.v4000.ThreadedPortType;
import com.ibm.streams.platform.services.v4000.ToolkitType;
import com.ibm.streams.platform.services.v4000.TracingLevelType;
import com.ibm.streams.platform.services.v4000.TracingType;
import com.ibm.streams.platform.services.v4000.TransportType;
import com.ibm.streams.platform.services.v4000.TupleTypeType;
import com.ibm.streams.platform.services.v4000.UriType;
import com.ibm.streams.platform.services.v4000.UrisType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;

public class V4000SPLLogicalModel extends SPLLogicalModel {

  public V4000SPLLogicalModel(
      ApplicationType logicalApp, Properties submissionParms, Properties configParms)
      throws SAMInternalErrorException, StreamsException {

    name = logicalApp.getName();
    applicationScope = logicalApp.getApplicationScope();
    version = logicalApp.getVersion();
    dataDirectory = logicalApp.getDataDirectory();
    if (configParms != null && configParms.getProperty(SamConstants.data_directory) != null) {
      dataDirectory = configParms.getProperty(SamConstants.data_directory);
    }
    if (configParms != null && configParms.getProperty(SamConstants.tracing) != null) {
      setConfigTraceLevel(configParms.getProperty(SamConstants.tracing));
    }
    applicationDirectory = logicalApp.getApplicationDirectory();
    outputDirectory = logicalApp.getOutputDirectory();
    checkpointDirectory = logicalApp.getCheckpointDirectory();
    adlPath = logicalApp.getAdlPath();
    buildId = logicalApp.getBuildId();
    bundlePath = logicalApp.getBundle();
    productVersion = "4.1.0.0"; // Hard-coded here because we can't get to it. Should be sufficient.

    List<HostpoolType> hps = logicalApp.getHostpools().getHostpool();
    if (hps.size() > 0) {
      for (HostpoolType hp : hps) {
        List<String> hosts = new ArrayList<>();
        List<HostType> h = hp.getHost();
        for (HostType host : h) {
          hosts.add(host.getHostname());
        }
        Set<String> tags = new LinkedHashSet<>();
        List<TagType> t = hp.getTag();
        for (TagType tag : t) {
          tags.add(tag.getName());
        }
        BigInteger size = hp.getSize();
        MembershipMode mm =
            (hp.getMembershipMode().value().compareTo("shared") == 0)
                ? MembershipMode.shared
                : MembershipMode.exclusive;
        LogicalHostpool thp =
            new LogicalHostpool(hp.getName(), hp.getIndex(), mm, hosts, tags, size);
        getHostpoolMap().put(thp.getIndex(), thp);
      }
    }

    List<TupleTypeType> adlTuples = logicalApp.getTupleTypes().getTupleType();
    for (TupleTypeType adlTupleType : adlTuples) {
      TopologyTupleType topTupleType = TopologyTypeFactory.createType(adlTupleType.getTuple());
      getTupleTypes().put(adlTupleType.getIndex(), topTupleType);
    }

    // Build the map of toolkits. This needs to be done before we visit the PEs section
    List<ToolkitType> toolkits = logicalApp.getToolkits().getToolkit();
    for (ToolkitType toolkit : toolkits) {
      Toolkit newToolkit =
          new Toolkit(
              toolkit.getIndex(), toolkit.getName(), toolkit.getUri(), toolkit.getVersion());
      super.toolkits.put(toolkit.getIndex(), newToolkit);
      if (toolkit.getName().equals("spl")) {
        super.splToolkitIndex = toolkit.getIndex();
      }
    }

    // Visit operator definitions
    OperDefinitionsType operDefinitions = logicalApp.getOperDefinitions();
    List<OperDefinitionType> operDefs = operDefinitions.getOperDefinition();
    for (OperDefinitionType adlDef : operDefs) {
      List<BigInteger> customMetricIndices = new ArrayList<>();
      CustomMetricIndiciesType customIndices = adlDef.getCustomMetricIndicies();
      if (customIndices != null) {
        List<BigInteger> indices = customIndices.getCustomMetricIndex();
        for (BigInteger index : indices) {
          customMetricIndices.add(index);
        }
      }
      PrimitiveOperatorDefinition def =
          new PrimitiveOperatorDefinition(
              adlDef.getIndex(),
              adlDef.getKind(),
              com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_CPlusPlus,
              null,
              customMetricIndices,
              new ArrayList<String>());
      getPrimitiveOperDefs().put(def.getIndex(), def);
    }

    // Get the composite operator definitions
    CompositesType composites = logicalApp.getComposites();
    UrisType sourceUris = composites.getSourceUris();
    List<UriType> uris = sourceUris.getUri();
    for (UriType uri : uris) {
      URI newURI = new URI(uri.getIndex(), uri.getPath());
      sourceURIs.put(newURI.getIndex(), newURI);
    }
    CompositeOpersType compositeDefinitions = composites.getCompositeOpers();
    List<CompositeOperType> compositeOperDefs = compositeDefinitions.getCompositeOper();
    for (CompositeOperType comp : compositeOperDefs) {
      CompositeOperatorDefinition def =
          new CompositeOperatorDefinition(comp.getIndex(), comp.getKind(), comp.getToolkitIndex());
      this.compositeOperDefs.add(def);
    }

    // one composite in the sequence, the main composite
    List<CompositeOperInstanceType> compositeOperInstancesTypeList =
        composites.getCompositeOperInstances().getCompositeOperInstance();

    // build the logical model
    mainComposite = new V4000CompositeOperator(compositeOperInstancesTypeList.get(0), null, this);

    // walk the pes section and update the logical primitive operators
    PesType pesElem = logicalApp.getPes();
    for (PeType pe : pesElem.getPe()) {
      walkPE(pe);
    }

    // Build the custom metrics table
    CustomMetricsType customMetrics = logicalApp.getCustomMetrics();
    if (customMetrics != null) {
      List<CustomMetricType> customMetric = customMetrics.getCustomMetric();
      if (customMetric != null) {
        for (CustomMetricType adlMetric : customMetric) {
          CustomMetricKind kind = null;
          CustomMetricKindType adlMetricKind = adlMetric.getKind();
          if (adlMetricKind.value().compareTo("Gauge") == 0) {
            kind = CustomMetricKind.Gauge;
          } else if (adlMetricKind.value().compareTo("Counter") == 0) {
            kind = CustomMetricKind.Counter;
          } else {
            assert (adlMetricKind.value().compareTo("Time") == 0);
            kind = CustomMetricKind.Time;
          }
          CustomMetric metric =
              new CustomMetric(
                  adlMetric.getIndex(), adlMetric.getName(), kind, adlMetric.getDescription());
          getCustomMetrics().put(metric.getIndex(), metric);
        }
      }
    }

    ParallelRegionsType prt = logicalApp.getParallelRegions();
    if (prt != null) {
      for (ParallelRegionType parallelRegionType : prt.getParallelRegion()) {
        V4000ParallelRegion parallelRegion = new V4000ParallelRegion(parallelRegionType, this);
        parallelRegions.getParallelRegionMap().put(parallelRegion.getIndex(), parallelRegion);
        parallelRegions.getParallelOperatorMap().put(parallelRegion.getOperIndex(), parallelRegion);
      }
    }

    // Build the table of submission-time values
    SubmissionTimeValuesType subTimeValues = logicalApp.getSubmissionTimeValues();
    setAndVerifySubmissionParms(subTimeValues, submissionParms);
  }

  private TraceLevel mapTraceType(TracingType tt) {
    TracingLevelType tlt = tt.getLevel();
    if (tlt == TracingLevelType.ERROR) return TraceLevel.ERROR;
    if (tlt == TracingLevelType.WARN) return TraceLevel.WARN;
    if (tlt == TracingLevelType.INFO) return TraceLevel.INFO;
    if (tlt == TracingLevelType.DEBUG) return TraceLevel.DEBUG;
    if (tlt == TracingLevelType.TRACE) return TraceLevel.TRACE;

    return TraceLevel.OFF;
  }

  private void walkPE(PeType pe) {
    TraceLevel traceLevel = mapTraceType(pe.getTracing());
    OperInstancesType operInstances = pe.getOperInstances();
    List<OperInstanceType> operInstance = operInstances.getOperInstance();
    ExecutableType exeType = pe.getExecutable();
    String wrapper = exeType.getWrapper();
    String uri = exeType.getExecutableUri();

    int operCount = operInstance.size();

    for (OperInstanceType oper : operInstance) {
      BigInteger index = oper.getIndex();
      // Find the corresponding logical primitive operator
      PrimitiveOperator lPrim = getPrimitiveOperator(index);
      lPrim.setDefinitionIndex(oper.getDefinitionIndex());
      lPrim.setWrapper(wrapper);
      lPrim.setSharedObject(uri);
      lPrim.set_Class(oper.getClazz());
      lPrim.setTraceLevel(traceLevel);
      if (oper.isRelocatable() != null) {
        lPrim.setRelocatable(oper.isRelocatable());
      } else {
        lPrim.setRelocatable(pe.isRelocatable());
      }
      if (oper.isRestartable() != null) {
        lPrim.setRestartable(oper.isRestartable());
      } else {
        lPrim.setRestartable(pe.isRestartable());
      }
      lPrim.setOptimized(pe.isOptimized());
      lPrim.setSingleThreadedOnInputs(oper.isSingleThreadedOnInputs());
      lPrim.setSingleThreadedOnOutputs(oper.isSingleThreadedOnOutputs());
      PrimitiveOperatorDefinition operDef = primitiveOperatorDefs.get(oper.getDefinitionIndex());
      String realClass = lPrim.getClass().toString();
      assert (realClass.equals(
          "class com.ibm.streams.instance.sam.model.logical.spl.v4000.V4000PrimitiveOperator"));
      operDef.setToolkitIndex(((V4000PrimitiveOperator) lPrim).getToolkitIndex());

      // Handle resources
      ResourcesType resources = oper.getResources();

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
      Resources topResources =
          new Resources(
              hostLocation,
              poolLocation,
              resourceCoLocations,
              resourceExLocations,
              resourceIsolation);
      lPrim.setResources(topResources);

      // If there is more than one, then we mark each as partition colocated
      if (operCount > 1) {
        Set<String> coLocations = lPrim.getCoLocations();
        coLocations.add("pe" + String.valueOf(pe.getIndex()));
      }

      // Is this operator checkpointing?
      CheckpointingType checkpointing = oper.getCheckpointing();
      if (checkpointing != null) {
        Double period = null;
        if (checkpointing.getPeriod() != null) {
          period = new Double(checkpointing.getPeriod());
        }
        Checkpointing ckp =
            new Checkpointing(CheckpointPolicy.valueOf(checkpointing.getPolicy().name()), period);
        lPrim.setCheckpointing(ckp);
      }

      // Do we have any runtime constants?
      List<RuntimeConstant> list = new ArrayList<>();
      RuntimeConstantsType runtimeConstants = oper.getRuntimeConstants();
      for (RuntimeConstantType runtimeConstant : runtimeConstants.getRuntimeConstant()) {
        RuntimeConstant rc =
            new RuntimeConstant(
                runtimeConstant.getName(),
                runtimeConstant.getValue(),
                runtimeConstant.getDefaultValue(),
                runtimeConstant.getSubmissionTimeValueIndex(),
                this);
        list.add(rc);
      }
      lPrim.setRuntimeConstants(list);

      // Walk the operator input ports and update the logical counterpart
      OperInstanceInputPortsType pInputPorts = oper.getInputPorts();
      List<OperInstanceInputPortType> piPorts = pInputPorts.getInputPort();
      List<PrimitiveInputPort> liPorts = lPrim.getInputPorts();
      for (int i = 0; i < piPorts.size(); ++i) {
        OperInstanceInputPortType piPort = piPorts.get(i);
        PrimitiveInputPort liPort = liPorts.get(i);
        walkInputPort(piPort, liPort);
      }
      // Walk the operator output ports and update the logical counterpart
      OperInstanceOutputPortsType pOutputPorts = oper.getOutputPorts();
      List<OperInstanceOutputPortType> poPorts = pOutputPorts.getOutputPort();
      List<PrimitiveOutputPort> loPorts = lPrim.getOutputPorts();
      for (int i = 0; i < poPorts.size(); ++i) {
        OperInstanceOutputPortType poPort = poPorts.get(i);
        PrimitiveOutputPort loPort = loPorts.get(i);
        walkOutputPort(poPort, loPort);
      }
    }

    // Walk PE ports looking for import/export info, encoding, etc.
    PeInputPortsType peIPorts = pe.getInputPorts();
    List<PeInputPortType> peInputPorts = peIPorts.getInputPort();
    for (PeInputPortType peIPort : peInputPorts) {
      // Handle any imported streams
      ImportedStreamsType importedStreams = peIPort.getImportedStreams();
      if (importedStreams != null) {
        List<NameBasedImportType> nameBasedImports = importedStreams.getNameBasedImport();
        if (nameBasedImports != null) {
          for (NameBasedImportType nbit : nameBasedImports) {
            String importOperatorName = nbit.getImportOperName();
            BigInteger index = nbit.getIndex();
            ImportOperator imp = importOperators.get(importOperatorName);
            NameBasedImport nameBasedImport =
                new NameBasedImport(
                    nbit.getApplicationName(),
                    importOperatorName,
                    nbit.getStreamName(),
                    nbit.getApplicationScope());
            imp.getImportedStreams().getNameBasedImports().put(index, nameBasedImport);
            if (importedStreams.getFilter() != null) {
              imp.getImportedStreams().setFilter(importedStreams.getFilter());
            }
          }
        }
        PropertyBasedImportType pbit = importedStreams.getPropertyBasedImport();
        if (pbit != null) {
          String importOperatorName = pbit.getImportOperName();
          BigInteger index = pbit.getIndex();
          ImportOperator imp = importOperators.get(importOperatorName);
          // We only want to set the Import operator's properties once
          if (!imp.isInitialized()) {
            PropertyBasedImport propBasedImport =
                new PropertyBasedImport(
                    pbit.getSubscription(), importOperatorName, index, pbit.getApplicationScope());
            imp.getImportedStreams().setPropertyBasedImport(propBasedImport);
            if (importedStreams.getFilter() != null) {
              imp.getImportedStreams().setFilter(importedStreams.getFilter());
            }
            imp.setInitialized();
          }
        }
      }

      // Find the operator input port to which this corresponds
      OperInstancePEInputConnectionType iConn = peIPort.getOperInstanceConnection();
      PrimitiveOperator lPrim = getPrimitiveOperator(iConn.getOperInstanceIndex());
      PrimitiveInputPort liPort = lPrim.getInputPorts().get(iConn.getIportIndex().intValue());
      setEncoding(liPort, peIPort.getEncoding());
      setTransport(liPort, peIPort.getTransport());
    }

    PeOutputPortsType peOPorts = pe.getOutputPorts();
    List<PeOutputPortType> peOutputPorts = peOPorts.getOutputPort();
    for (PeOutputPortType peOPort : peOutputPorts) {
      // Handle any exported streams
      ExportedStreamType exportedStream = peOPort.getExportedStream();
      if (exportedStream != null) {
        String exportOperatorName = exportedStream.getExportOperName();
        ExportOperator export = exportOperators.get(exportOperatorName);
        assert (export != null);
        Boolean allowFilter = exportedStream.isAllowFilter();
        String name = exportedStream.getName();
        List<StreamProperty> streamProperties = null;
        if (exportedStream.getProperties() != null) {
          streamProperties = new ArrayList<>();
          StreamPropertiesType props = exportedStream.getProperties();
          for (StreamPropertyType prop : props.getProperty()) {
            String propName = prop.getName();
            String propType = prop.getType();
            List<String> values = prop.getValue();
            StreamProperty topProp = new StreamProperty(propName, propType, values);
            streamProperties.add(topProp);
          }
        }
        LogicalExportedStream topExportedStream =
            new LogicalExportedStream(export, name, allowFilter, null, streamProperties);
        export.setExportedStream(topExportedStream);
      }
      // Find the operator output port to which this corresponds
      OperInstancePEOutputConnectionType oConn = peOPort.getOperInstanceConnection();
      PrimitiveOperator lPrim = getPrimitiveOperator(oConn.getOperInstanceIndex());
      PrimitiveOutputPort loPort = lPrim.getOutputPorts().get(oConn.getOportIndex().intValue());
      setEncoding(loPort, peOPort.getEncoding());
      setTransport(loPort, peOPort.getTransport());
      loPort.setSingleThreadedOnOutput(peOPort.isSingleThreadedOnOutput());
    }
  }

  private void setTransport(PrimitivePort port, TransportType adlTransport) {
    if (adlTransport.value().compareTo("TCP") == 0) {
      port.setTransport(Transport.TCP);
    }
  }

  private void setEncoding(PrimitivePort port, EncodingType adlEncoding) {
    if (adlEncoding.value().matches("BE")) {
      port.setEncoding(Encoding.BE);
    } else if (adlEncoding.value().matches("LE")) {
      port.setEncoding(Encoding.LE);
    } else {
      assert (adlEncoding.value().matches("NBO"));
      port.setEncoding(Encoding.NBO);
    }
  }

  private void walkPort(OperInstancePortType pPort, PrimitivePort lPort) {
    lPort.setName(pPort.getName());
    lPort.setMutable(pPort.isIsMutable());
    lPort.setTupleTypeIndex(pPort.getTupleTypeIndex());
  }

  private void walkOutputPort(OperInstanceOutputPortType poPort, PrimitiveOutputPort loPort) {
    walkPort(poPort, loPort);
    loPort.setLogicalStreamName(poPort.getStreamName());
    if (poPort.isViewable() != null) loPort.setViewable(poPort.isViewable());
  }

  private CongestionPolicy mapCongestionPolicy(CongestionPolicyType policy) {
    if (policy == CongestionPolicyType.WAIT) return CongestionPolicy.wait;
    if (policy == CongestionPolicyType.DROP_FIRST) return CongestionPolicy.dropFirst;
    assert (policy == CongestionPolicyType.DROP_LAST);
    return CongestionPolicy.dropLast;
  }

  private void walkInputPort(OperInstanceInputPortType piPort, PrimitiveInputPort liPort) {
    walkPort(piPort, liPort);
    if (piPort.isIsControl() != null) {
      liPort.setIsControl(piPort.isIsControl());
    }
    if (piPort.getThreadedPort() != null) {
      ThreadedPortType tp = piPort.getThreadedPort();
      CongestionPolicyType cp = tp.getCongestionPolicy(); // mandatory
      Boolean stoi = tp.isSingleThreadedOnInput(); // mandatory
      BigInteger queueSize = tp.getQueueSize(); // can be null
      ThreadedPort newTp = new ThreadedPort(stoi, mapCongestionPolicy(cp), queueSize);
      liPort.setThreadedPort(newTp);
    }
  }

  @Override
  public String toJSON() {
    GsonBuilder gsonBuilder = new GsonBuilder();
    gsonBuilder.registerTypeAdapter(V4000ParallelRegion.class, new ParallelRegionSerializer());
    gsonBuilder.registerTypeAdapter(ParallelRegion.class, new ParallelRegionSerializer());
    gsonBuilder.registerTypeAdapter(V4000SPLLogicalModel.class, new SPLLogicalModelSerializer());
    gsonBuilder.registerTypeAdapter(
        V4000CompositeOperator.class, new CompositeOperatorSerializer());
    gsonBuilder.registerTypeAdapter(CompositeOperator.class, new CompositeOperatorSerializer());
    gsonBuilder.registerTypeAdapter(V4000CompositeInputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(CompositeInputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(V4000CompositeOutputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(CompositeOutputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(
        V4000PrimitiveOperator.class, new PrimitiveOperatorSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveOperator.class, new PrimitiveOperatorSerializer());
    gsonBuilder.registerTypeAdapter(
        PrimitiveInputPort.class, new PrimitiveInputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveSplitterInputPort.class, new PrimitiveInputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveOutputPort.class, new PrimitiveOutputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveSplitterOutputPort.class, new PrimitiveOutputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(V4000ImportOperator.class, new ImportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(ImportOperator.class, new ImportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(V4000ExportOperator.class, new ExportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(ExportOperator.class, new ExportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(
        PrimitiveImportOutputPort.class, new PrimitiveImportOutputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveExportInputPort.class, new PrimitiveExportInputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(SplitterOperator.class, new SplitterOperatorSerializer());
    gsonBuilder.registerTypeAdapter(MergerOperator.class, new MergerOperatorSerializer());
    gsonBuilder.registerTypeAdapter(TopologyTupleType.class, new TopologyTupleTypeSerializer());
    gsonBuilder.registerTypeAdapter(TopologyPrimitiveType.class, new TopologyTypeSerializer());
    gsonBuilder.registerTypeAdapter(TopologyListType.class, new TopologyTypeSerializer());
    gsonBuilder.registerTypeAdapter(TopologySetType.class, new TopologyTypeSerializer());
    gsonBuilder.registerTypeAdapter(TopologyMapType.class, new TopologyTypeSerializer());
    gsonBuilder.registerTypeAdapter(Annotation.class, new AnnotationSerializer());
    gsonBuilder.registerTypeAdapter(
        AnnotationPrimitiveValue.class, new PrimitiveAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(
        LogicalAnnotationPrimitiveValue.class, new PrimitiveAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(AnnotationListValue.class, new ListAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(
        AnnotationTupleValue.class, new TupleAnnotationValueSerializer());
    gsonBuilder.serializeNulls();
    //        gsonBuilder.setPrettyPrinting();
    Gson gson = gsonBuilder.create();
    return gson.toJson(this);
  }

  private void setAndVerifySubmissionParms(
      SubmissionTimeValuesType subTimeValues, Properties submissionParms) throws StreamsException {

    SubmissionTimeErrorException err = null;

    if (subTimeValues != null) {
      List<SubmissionTimeValueType> stValues = subTimeValues.getSubmissionTimeValue();
      for (SubmissionTimeValueType stValue : stValues) {
        boolean isRequired = stValue.isRequired();
        SubmissionTimeValue v =
            new SubmissionTimeValue(
                stValue.getCompositeName(),
                stValue.getIndex(),
                stValue.getKind().value().compareTo("named") == 0
                    ? SubmissionTimeValueKind.Named
                    : SubmissionTimeValueKind.NamedList,
                stValue.getName(),
                stValue.getDefaultValue(),
                isRequired);
        submissionTimeValues.put(stValue.getIndex(), v);

        boolean found = false, hasDuplicate = false;
        String name = v.getName();
        String qualName = v.getCompositeName() + "." + name;
        String partQualName = null;
        // verify Namespace is part of fully qualified name
        if (qualName.lastIndexOf(":") != -1) {
          partQualName = qualName.substring(qualName.lastIndexOf(":") + 1);
        }
        if (submissionParms != null && !submissionParms.isEmpty()) {
          // Look for a value with unqualified name
          if (submissionParms.getProperty(name) != null) {
            found = true;
            v.setValue(submissionParms.getProperty(name));
          }
          // Look for a value with qualified name
          if (submissionParms.getProperty(qualName) != null) {
            if (found) {
              hasDuplicate = true;
            } else {
              found = true;
              v.setValue(submissionParms.getProperty(qualName));
            }
          }
          // Look for a value with partially qualified name
          if ((partQualName != null) && (submissionParms.getProperty(partQualName) != null)) {
            if (found) {
              hasDuplicate = true;
            } else {
              found = true;
              v.setValue(submissionParms.getProperty(partQualName));
            }
          }
        }

        // check display name
        if (Objects.equal(name, SamConstants.DISPLAY_VAR_NAME)
            || Objects.equal(qualName, SamConstants.DISPLAY_VAR_NAME)
            || Objects.equal(partQualName, SamConstants.DISPLAY_VAR_NAME)) {
          if (v.getValue() == null) {
            // No display value given or set in the environment
            throw new StreamsException(StreamsRuntimeMessagesKey.Key.SubmissionDisplayMissing);
          }
        }

        if (found && hasDuplicate) {
          if (err == null) {
            err =
                new SubmissionTimeErrorException(
                    StreamsRuntimeMessagesKey.Key.SubmissionParameterDuplicateError, qualName);
          }
          err.getDuplicatesList().add(qualName);
        } else if (!found && isRequired) {
          if (err == null) {
            err =
                new SubmissionTimeErrorException(
                    StreamsRuntimeMessagesKey.Key.SubmissionParameterMissingError, qualName);
          }
          err.getMissingList().add(qualName);
        }
      }
    }

    if (submissionParms != null && !submissionParms.isEmpty()) {
      // check for named values which match more than one submission time values
      for (Object nameKey : submissionParms.keySet()) {
        String pname = (String) nameKey;
        if (pname.equals(SamConstants.DISPLAY_VAR_NAME)) { // this isn't a standard named value
          continue;
        }

        String matchName = null;
        boolean found = false;
        // Check if the application has any submission time values
        if (!submissionTimeValues.isEmpty()) {
          for (SubmissionTimeValue v : submissionTimeValues.values()) {
            String name = v.getName();
            String qualName = v.getCompositeName() + "." + name;
            String partQualName = qualName.substring(qualName.lastIndexOf(":") + 1);
            if (name.equals(pname) || qualName.equals(pname) || partQualName.equals(pname)) {
              if (found) {
                // was already found
                if (err == null) {
                  err =
                      new SubmissionTimeErrorException(
                          StreamsRuntimeMessagesKey.Key.SubmissionParameterAmbiguousError,
                          pname,
                          matchName);
                }
                err.getAmbiguousList().add(pname);
                break;
              } else {
                if (name.equals(pname)) {
                  matchName = name;
                }
                if (qualName.equals(pname)) {
                  matchName = qualName;
                }
                if (partQualName.equals(pname)) {
                  matchName = partQualName;
                }
              }
              found = true;
            }
          }
        }
        if (!found) {
          // was not used in any application
          if (_submissionTimeWarning == null) {
            _submissionTimeWarning =
                new SubmissionTimeErrorException(
                    StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning, pname);
          }
          _submissionTimeWarning.getExtraValues().add(pname);
          SAMStaticContext.productLog(
              StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning, pname);
        }
      }
    }

    if (err != null) {
      if (_submissionTimeWarning != null) {
        err.getExtraValues().addAll(_submissionTimeWarning.getExtraValues());
      }
      throw err;
    }
  }
}
