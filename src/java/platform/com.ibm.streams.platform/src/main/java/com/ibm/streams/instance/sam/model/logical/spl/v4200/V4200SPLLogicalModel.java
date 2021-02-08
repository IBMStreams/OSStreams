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
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveSplitterInputPort;
import com.ibm.streams.instance.sam.model.logical.spl.PrimitiveSplitterOutputPort;
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
import com.ibm.streams.instance.sam.model.topology.CustomMetric;
import com.ibm.streams.instance.sam.model.topology.CustomMetricKind;
import com.ibm.streams.instance.sam.model.topology.MembershipMode;
import com.ibm.streams.instance.sam.model.topology.Toolkit;
import com.ibm.streams.instance.sam.model.topology.TopologyListType;
import com.ibm.streams.instance.sam.model.topology.TopologyMapType;
import com.ibm.streams.instance.sam.model.topology.TopologyOptionalType;
import com.ibm.streams.instance.sam.model.topology.TopologyPrimitiveType;
import com.ibm.streams.instance.sam.model.topology.TopologySetType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import com.ibm.streams.instance.sam.model.topology.YieldBehaviour;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.v4200.ApplicationType;
import com.ibm.streams.platform.services.v4200.CompositeOperDefinitionType;
import com.ibm.streams.platform.services.v4200.CompositeOperDefinitionsType;
import com.ibm.streams.platform.services.v4200.CompositeOperInstanceType;
import com.ibm.streams.platform.services.v4200.CompositesType;
import com.ibm.streams.platform.services.v4200.CustomMetricIndiciesType;
import com.ibm.streams.platform.services.v4200.CustomMetricKindType;
import com.ibm.streams.platform.services.v4200.CustomMetricType;
import com.ibm.streams.platform.services.v4200.CustomMetricsType;
import com.ibm.streams.platform.services.v4200.HostType;
import com.ibm.streams.platform.services.v4200.HostpoolType;
import com.ibm.streams.platform.services.v4200.OperatorRuntimeType;
import com.ibm.streams.platform.services.v4200.ParallelRegionType;
import com.ibm.streams.platform.services.v4200.ParallelRegionsType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperDefinitionType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperDefinitionsType;
import com.ibm.streams.platform.services.v4200.SplAnnotationType;
import com.ibm.streams.platform.services.v4200.SplAnnotationValueType;
import com.ibm.streams.platform.services.v4200.SubmissionTimeValueType;
import com.ibm.streams.platform.services.v4200.SubmissionTimeValuesType;
import com.ibm.streams.platform.services.v4200.TagType;
import com.ibm.streams.platform.services.v4200.ToolkitType;
import com.ibm.streams.platform.services.v4200.TupleTypeType;
import com.ibm.streams.platform.services.v4200.UriType;
import com.ibm.streams.platform.services.v4200.UrisType;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashSet;
import java.util.List;
import java.util.Properties;
import java.util.Set;

public class V4200SPLLogicalModel extends SPLLogicalModel {

  boolean emitUnsupportedImportExportWarning = false;
  Set<String> unsupportedAnnotationsWarning = new HashSet<>();
  Set<String> unsupportedConfigsWarning = new HashSet<>();

  public V4200SPLLogicalModel(
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
    productVersion = logicalApp.getProductVersion();
    yieldBehaviour = YieldBehaviour.valueOf(logicalApp.getAvoidRuntimeYield().name());

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

    // Visit operator definitions
    PrimitiveOperDefinitionsType operDefinitions = logicalApp.getPrimitiveOperDefinitions();
    List<PrimitiveOperDefinitionType> operDefs = operDefinitions.getPrimitiveOperDefinition();
    for (PrimitiveOperDefinitionType adlDef : operDefs) {
      com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType runtimeType =
          (adlDef.getRuntimeType().equals(OperatorRuntimeType.SPL_C_PLUS_PLUS))
              ? com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_CPlusPlus
              : com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType.SPL_Java;
      List<BigInteger> customMetricIndices = new ArrayList<>();
      CustomMetricIndiciesType customIndices = adlDef.getCustomMetricIndicies();
      if (customIndices != null) {
        List<BigInteger> indices = customIndices.getCustomMetricIndex();
        for (BigInteger index : indices) {
          customMetricIndices.add(index);
        }
      }
      List<String> capabilities = new ArrayList<>();
      if (adlDef.getCapability() != null) {
        capabilities.addAll(adlDef.getCapability());
      }
      PrimitiveOperatorDefinition def =
          new PrimitiveOperatorDefinition(
              adlDef.getIndex(),
              adlDef.getKind(),
              runtimeType,
              adlDef.getToolkitIndex(),
              customMetricIndices,
              capabilities);
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
    CompositeOperDefinitionsType compositeDefinitions = composites.getCompositeOperDefinitions();
    List<CompositeOperDefinitionType> compositeOperDefs =
        compositeDefinitions.getCompositeOperDefinition();
    for (CompositeOperDefinitionType comp : compositeOperDefs) {
      CompositeOperatorDefinition def =
          new CompositeOperatorDefinition(comp.getIndex(), comp.getKind(), comp.getToolkitIndex());
      this.compositeOperDefs.add(def);
      if (comp.getSplAnnotation() != null) {
        for (SplAnnotationType splAnnotation : comp.getSplAnnotation()) {
          String tag = splAnnotation.getTag();
          Boolean isInternal = splAnnotation.isInternal();
          List<SplAnnotationValueType> splValues = splAnnotation.getValue();
          def.addAnnotation(
              AnnotationFactory.createAnnotation(
                  tag, (isInternal != null) ? isInternal : false, splValues, this));
        }
      }
    }

    // one composite in the sequence, the main composite
    List<CompositeOperInstanceType> compositeOperInstancesTypeList =
        composites.getCompositeOperInstances().getCompositeOperInstance();

    // build the logical model
    mainComposite = new V4200CompositeOperator(compositeOperInstancesTypeList.get(0), null, this);

    // Build the map of toolkits
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
        V4200ParallelRegion parallelRegion = new V4200ParallelRegion(parallelRegionType, this);
        parallelRegions.getParallelRegionMap().put(parallelRegion.getIndex(), parallelRegion);
        parallelRegions.getParallelOperatorMap().put(parallelRegion.getOperIndex(), parallelRegion);
      }
    }

    // Build the table of submission-time values
    SubmissionTimeValuesType subTimeValues = logicalApp.getSubmissionTimeValues();
    setAndVerifySubmissionParms(subTimeValues, submissionParms);

    // Emit any messages we need to for standalone processing
    if (emitUnsupportedImportExportWarning) {
      SAMStaticContext.productLog(
          StreamsRuntimeMessagesKey.Key.SAMImportExportNotSupportedInStandalone);
    }

    if (!unsupportedAnnotationsWarning.isEmpty()) {
      for (String s : unsupportedAnnotationsWarning) {
        SAMStaticContext.productLog(
            StreamsRuntimeMessagesKey.Key.SAMAnnotationNotSupportedInStandalone, s);
      }
    }

    if (!unsupportedConfigsWarning.isEmpty()) {
      for (String s : unsupportedConfigsWarning) {
        SAMStaticContext.productLog(
            StreamsRuntimeMessagesKey.Key.SAMConfigNotSupportedInStandalone, s);
      }
    }
  }

  @Override
  public String toJSON() {
    GsonBuilder gsonBuilder = new GsonBuilder();
    gsonBuilder.registerTypeAdapter(V4200ParallelRegion.class, new ParallelRegionSerializer());
    gsonBuilder.registerTypeAdapter(ParallelRegion.class, new ParallelRegionSerializer());
    gsonBuilder.registerTypeAdapter(V4200SPLLogicalModel.class, new SPLLogicalModelSerializer());
    gsonBuilder.registerTypeAdapter(
        V4200CompositeOperator.class, new CompositeOperatorSerializer());
    gsonBuilder.registerTypeAdapter(CompositeOperator.class, new CompositeOperatorSerializer());
    gsonBuilder.registerTypeAdapter(V4200CompositeInputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(CompositeInputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(V4200CompositeOutputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(CompositeOutputPort.class, new CompositePortSerializer());
    gsonBuilder.registerTypeAdapter(
        V4200PrimitiveOperator.class, new PrimitiveOperatorSerializer());
    gsonBuilder.registerTypeAdapter(PrimitiveOperator.class, new PrimitiveOperatorSerializer());
    gsonBuilder.registerTypeAdapter(
        PrimitiveInputPort.class, new PrimitiveInputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveSplitterInputPort.class, new PrimitiveInputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveOutputPort.class, new PrimitiveOutputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(
        PrimitiveSplitterOutputPort.class, new PrimitiveOutputPortSerializer(this));
    gsonBuilder.registerTypeAdapter(V4200ImportOperator.class, new ImportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(ImportOperator.class, new ImportOperatorSerializer());
    gsonBuilder.registerTypeAdapter(V4200ExportOperator.class, new ExportOperatorSerializer());
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
    gsonBuilder.registerTypeAdapter(TopologyOptionalType.class, new TopologyTypeSerializer());
    gsonBuilder.registerTypeAdapter(Annotation.class, new AnnotationSerializer());
    gsonBuilder.registerTypeAdapter(
        LogicalAnnotationPrimitiveValue.class, new PrimitiveAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(
        AnnotationPrimitiveValue.class, new PrimitiveAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(AnnotationListValue.class, new ListAnnotationValueSerializer());
    gsonBuilder.registerTypeAdapter(
        AnnotationTupleValue.class, new TupleAnnotationValueSerializer());
    gsonBuilder.serializeNulls();
    gsonBuilder.setPrettyPrinting();
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
      } // end for
    } // end if

    if (submissionParms != null && !submissionParms.isEmpty()) {
      // check for named values which match more than one submission time
      // values
      for (Object nameKey : submissionParms.keySet()) {
        String pname = (String) nameKey;
        if (pname.equals(SamConstants.DISPLAY_VAR_NAME)) { // this isn't
          // a standard
          // named
          // value
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

  public void emitUnsupportedImportExport() {
    emitUnsupportedImportExportWarning = true;
  }

  public Set<String> getUnsupportedAnnotationsWarning() {
    return unsupportedAnnotationsWarning;
  }

  public Set<String> getUnsupportedConfigsWarning() {
    return unsupportedConfigsWarning;
  }
}
