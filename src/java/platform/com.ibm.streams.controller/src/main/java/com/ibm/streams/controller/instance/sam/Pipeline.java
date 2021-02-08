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

package com.ibm.streams.controller.instance.sam;

import com.ibm.distillery.utils.DistilleryException;
import com.ibm.distillery.utils.exc.SerializationException;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.ConsoleLogger;
import com.ibm.streams.admin.internal.api.trace.LogManager;
import com.ibm.streams.admin.internal.api.trace.OutputStreamTraceLogger;
import com.ibm.streams.admin.internal.api.trace.StreamsLogger;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger;
import com.ibm.streams.admin.internal.api.trace.TraceManager;
import com.ibm.streams.control.internal.server.consistent.ConsistentRegion;
import com.ibm.streams.control.internal.server.consistent.ConsistentRegionSetup;
import com.ibm.streams.instance.sam.ObjectTracker;
import com.ibm.streams.instance.sam.SAMClientHelper;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.exception.SubmissionTimeErrorException;
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.OutputPort;
import com.ibm.streams.instance.sam.model.Pe;
import com.ibm.streams.instance.sam.model.fuser.FusedApplication;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.instance.sam.model.logical.LogicalModelFactory;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobComposites;
import com.ibm.streams.instance.sam.model.runtime.RuntimeJobConfig;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.ParallelRegion;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.messages.MessageUtilities;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.sch.composer.ComposingInstructionFactory;
import com.ibm.streams.sch.composer.ComposingInstructions;
import com.ibm.streams.sch.composer.FusingStrategy;
import com.ibm.streams.sch.composer.IEUComposer;
import java.math.BigInteger;
import java.util.AbstractMap;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Optional;
import java.util.Properties;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.stream.Collectors;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Pipeline {

  private static final Logger logger = LoggerFactory.getLogger(Pipeline.class);

  public static final boolean RESET_OBJECT_TRACKER = true;
  public static final boolean KEEP_OBJECT_TRACKER = false;

  public Pipeline(StreamsLogger.StreamsLogLevel logLevel, TraceLogger.Level traceLevel) {
    ConsoleLogger logger = new ConsoleLogger();
    logger.setLevel(logLevel);
    LogManager.setLogger(logger);
    OutputStreamTraceLogger tracer = new OutputStreamTraceLogger(System.out);
    TraceManager.setLevel(traceLevel);
    TraceManager.setTraceLogger(tracer);
    try {
      SAMStaticContext.init();
      SAMStaticContext.setStandaloneMode(false, false);
    } catch (Exception ex) {
      Trace.logError(ex.getMessage(), ex);
    }
  }

  public LogicalModel createLogicalModel(
      BigInteger jobId, String adlString, Properties submitParms, Properties cfgs)
      throws StreamsException {
    return createLogicalModel(jobId, adlString, submitParms, cfgs, RESET_OBJECT_TRACKER);
  }

  public LogicalModel createLogicalModel(
      BigInteger jobId,
      String adlString,
      Properties submitParms,
      Properties cfgs,
      boolean resetObjectTracker)
      throws StreamsException {
    Locale locale = Locale.getDefault();
    LogicalModel logicalModel;
    try {
      /*
       * Isolate each job creation. We want the job creation pipeline to be
       * stateless. Currently, it is not. The TopologyModel, Composer,
       * FusedApplication and the Job all rely on a shared ObjectTracker
       * for some shared state. We can, however, mitigate this reliance
       * by giving each new job creation its own ObjectTracker, implying
       * that we throw out the old state in the old ObjectTracker. We can
       * do this because the main purpose of the ObjectTracker was to
       * manage state persistence, but we now rely on Kubernetes for
       * that.
       */
      if (resetObjectTracker) {
        ObjectTracker tracker = new ObjectTracker();
        tracker.setTopologyApplicationId(jobId);
        SAMStaticContext.setObjectTracker(tracker);
      }
      /*
       * Log some arguments.
       */
      logger.info("Submission parameters: {}", submitParms);
      logger.info("Submission configuration: {}", cfgs);
      /*
       * Build the logical model.
       */
      logicalModel = LogicalModelFactory.createLogicalModel(adlString, submitParms, cfgs);
      if (logicalModel.getSubmissionTimeWarning() != null) {
        for (String extraValue : logicalModel.getSubmissionTimeWarning().getExtraValues()) {
          String msg =
              MessageUtilities.getMessageAndPrefix(
                  locale,
                  StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning,
                  extraValue);
          Trace.logWarn(msg);
        }
      }
    } catch (SubmissionTimeErrorException sse) {
      for (String ambiguousValue : sse.getAmbiguousList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SAMSubmissionParameterError, ambiguousValue);
        Trace.logError(msg);
      }
      for (String extraValue : sse.getExtraValues()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SubmissionParameterExtraWarning, extraValue);
        Trace.logError(msg);
      }
      for (String dupValue : sse.getDuplicatesList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale, StreamsRuntimeMessagesKey.Key.SubmissionParameterDuplicateError, dupValue);
        Trace.logError(msg);
      }
      for (String missingValue : sse.getMissingList()) {
        String msg =
            MessageUtilities.getMessageAndPrefix(
                locale,
                StreamsRuntimeMessagesKey.Key.SubmissionParameterMissingError,
                missingValue);
        Trace.logError(msg);
      }
      throw sse;
    }
    return logicalModel;
  }

  public static ComposingInstructions createManualFuse(Integer numPes) {
    String manual = FusingStrategy._defaultFusingScript_forManual;
    return ComposingInstructionFactory.createManualFuse(numPes, manual);
  }

  public static ComposingInstructions createLegacyFuse() {
    String legacy = FusingStrategy._defaultFusingScript_forLegacy;
    return ComposingInstructionFactory.createLegacyFuse(legacy);
  }

  public Job createJob(
      String jobName,
      String adlString,
      LogicalModel logicalModel,
      ComposingInstructions compInst,
      Properties cfgs,
      Map<String, BigInteger> parWidths)
      throws StreamsException {
    Job job;
    /*
     * The SAM pipeline to create job.
     */
    try {
      /*
       * Build the topology model.
       */
      var topologyModel = logicalModel.generateTopology(cfgs, parWidths);
      var composer = new IEUComposer(topologyModel, null);
      composer.buildComposerModel(false);
      /*
       * Build the fuser.
       */
      var containers = new TreeSet<>(composer.composeContainerSpecs(compInst));
      var fused = new FusedApplication(topologyModel, containers);
      /*
       * Build the job.
       */
      try {
        job =
            new Job(
                fused,
                "user",
                jobName,
                "default",
                new RuntimeJobConfig(adlString, null, null),
                new RuntimeJobComposites(logicalModel.toJSON()),
                false);
      } catch (DistilleryException e) {
        throw new StreamsException(e);
      }
    } catch (StreamsException e) {
      Optional.ofNullable(e.getNestedMessages())
          .ifPresent(ll -> ll.forEach(l -> l.forEach(msg -> Trace.logError(msg.getMessageText()))));
      throw e;
    }
    /*
     * Return the newly created job.
     */
    return job;
  }

  /*
   * Assumes that the given topologyApplication is for the given logicalModel, but does not enforce it. If that is
   * not the case, the returned job will be nonsense.
   */
  public Job createJob(
      String jobName,
      String adlString,
      LogicalModel logicalModel,
      TopologyApplication topologyModel,
      Map<BigInteger, Set<String>> peIdToOperators,
      ComposingInstructions compInst)
      throws StreamsException {
    Job job;
    /*
     * The SAM pipeline to create job.
     */
    try {
      /*
       * Build the topology model.
       */
      var composer = new IEUComposer(topologyModel, null);
      composer.buildComposerModel(false);
      /*
       * Build the fuser.
       */
      var containers = new TreeSet<>(composer.composeContainerSpecs(compInst));
      var fused = new FusedApplication(topologyModel, containers);
      /*
       * Build the job.
       */
      try {
        job =
            new Job(
                fused,
                "user",
                jobName,
                "default",
                peIdToOperators,
                new RuntimeJobConfig(adlString, null, null),
                new RuntimeJobComposites(logicalModel.toJSON()),
                false);
      } catch (DistilleryException e) {
        throw new StreamsException(e);
      }
    } catch (StreamsException e) {
      Optional.ofNullable(e.getNestedMessages())
          .ifPresent(ll -> ll.forEach(l -> l.forEach(msg -> Trace.logError(msg.getMessageText()))));
      throw e;
    }
    /*
     * Return the newly created job.
     */
    return job;
  }

  /*
   * TODO: can we narrow the kind of the exception thrown?
   */
  public Map<Integer, ConsistentRegion> createConsistentRegions(
      BigInteger jobId, LogicalModel logicalModel, Job job) throws Exception {
    String logicView = logicalModel.toJSON();
    final Set<Integer> consistentRegions =
        ConsistentRegionSetup.discoverConsistentRegions(jobId, logicView);

    /*
     * Create operator to PE mapping
     */
    Map<String, BigInteger> primOperatorToPeIdMap = new HashMap<>();
    for (Map.Entry<BigInteger, Pe> entry : job.getPes().entrySet()) {
      BigInteger peId = entry.getKey();
      Pe pe = entry.getValue();
      for (String opName : pe.getOperatorNames()) {
        primOperatorToPeIdMap.put(opName, peId);
      }
    }

    /*
     * Get the mapping between primitive operator definition index and its kind
     */
    Map<Integer, String> operDefIndexToOperKindMap =
        ConsistentRegionSetup.createPrimitiveOperatorDefinitions(logicView);

    /*
     * Parse the logical view to collect meta information for each consistent
     * region
     */
    Map<Integer, ConsistentRegion> consistentRegionMap = new HashMap<>();
    for (Integer regionIndex : consistentRegions) {
      ConsistentRegion cr = new ConsistentRegion();
      cr.constructRegion(regionIndex, primOperatorToPeIdMap, operDefIndexToOperKindMap, logicView);
      consistentRegionMap.put(regionIndex, cr);
    }
    return consistentRegionMap;
  }

  public void close() {
    SAMStaticContext.shutdown();
  }

  public SortedMap<BigInteger, String> getAADLs(Job job) throws StreamsException {
    SortedMap<BigInteger, String> aadlMap = new TreeMap<>();
    try {
      for (Map.Entry<BigInteger, Pe> entry : job.getPes().entrySet()) {
        BigInteger peId = entry.getKey();
        Pe pe = entry.getValue();
        AugmentedPEType xml = pe.toXml();
        String serializedXML = SAMClientHelper.serializeAugmentedPEType(xml);
        String cleanedXML = serializedXML.replaceAll("\\n+\\s*", "");
        aadlMap.put(peId, cleanedXML);
      }
    } catch (SerializationException ex) {
      throw new StreamsException(ex);
    }
    return aadlMap;
  }

  public static Set<String> getOperatorNames(AugmentedPEType aadl) {
    return aadl.getOperInstances().getOperInstance().stream()
        .map(op -> op.getName())
        .collect(Collectors.toSet());
  }

  public SortedMap<BigInteger, List<InputPortLabel>> getPortLabels(Job job) {
    SortedMap<BigInteger, List<InputPortLabel>> portLabelMap = new TreeMap<>();
    for (Map.Entry<BigInteger, Pe> entry : job.getPes().entrySet()) {
      BigInteger peId = entry.getKey();
      Pe pe = entry.getValue();
      List<InputPortLabel> portLabelList = new ArrayList<>();
      portLabelMap.put(peId, portLabelList);
      for (InputPort inputPort : pe.getInputPorts()) {
        /*
         * <receiver PE id, receiver PE port id>
         */
        String pePortLabel = peId + "." + inputPort.getId() + "@" + pe.getJobId();
        /*
         * <job id, receiver operator index, receiver operator port index>
         */
        String operatorPortLabel =
            pe.getJobId() + "." + inputPort.getNodeIndex() + "." + inputPort.getNodePortIndex();
        portLabelList.add(new InputPortLabel(pePortLabel, operatorPortLabel));
      }
    }
    return portLabelMap;
  }

  public SortedMap<BigInteger, List<OutputPort>> getExports(Job job) {
    SortedMap<BigInteger, List<OutputPort>> exportsMap = new TreeMap<>();
    for (Map.Entry<BigInteger, Pe> entry : job.getPes().entrySet()) {
      BigInteger peId = entry.getKey();
      Pe pe = entry.getValue();
      List<OutputPort> portList = new ArrayList<>();
      for (OutputPort op : pe.getOutputPorts()) {
        if (op.getExportedStream() != null) {
          portList.add(op);
        }
      }
      if (!portList.isEmpty()) {
        exportsMap.put(peId, portList);
      }
    }
    return exportsMap;
  }

  private boolean hasImportedStreams(InputPort ip) {
    /*
     * Check if the port has any imported streams.
     */
    if (ip.getImportedStreams() == null) {
      return false;
    }
    var is = ip.getImportedStreams();
    /*
     * Check if the imported streams contain anything as the model allow empty choices.
     */
    if (is.getNameBasedImports() != null && !is.getNameBasedImports().isEmpty()) {
      return true;
    }
    return is.getPropertyBasedImport() != null;
  }

  public SortedMap<BigInteger, List<InputPort>> getImports(Job job) {
    SortedMap<BigInteger, List<InputPort>> importsMap = new TreeMap<>();
    for (Map.Entry<BigInteger, Pe> entry : job.getPes().entrySet()) {
      BigInteger peId = entry.getKey();
      Pe pe = entry.getValue();
      List<InputPort> portList = new ArrayList<>();
      for (InputPort ip : pe.getInputPorts()) {
        if (hasImportedStreams(ip)) {
          portList.add(ip);
        }
      }
      if (!portList.isEmpty()) {
        importsMap.put(peId, portList);
      }
    }
    return importsMap;
  }

  public Map<BigInteger, Boolean> getPesIsRestartable(Job job) {
    return job.getPes().values().stream().collect(Collectors.toMap(Pe::getId, Pe::isRestartable));
  }

  public Map<BigInteger, String> getPesHostPlacement(Job job) {
    return job.getPes().values().stream()
        .flatMap(
            pe ->
                pe.getNodeIndexes().stream()
                    .map(
                        id ->
                            SAMStaticContext.getTopologyTracker().getTopologyNode(job.getId(), id))
                    .map(node -> node.getResources().getHostLocation())
                    .filter(Objects::nonNull)
                    .map(location -> new AbstractMap.SimpleImmutableEntry<>(pe.getId(), location)))
        .collect(
            Collectors.toMap(
                AbstractMap.SimpleImmutableEntry::getKey,
                AbstractMap.SimpleImmutableEntry::getValue));
  }

  public Map<BigInteger, Set<String>> getPesHostCoLocations(Job job) {
    return job.getPes().values().stream()
        .collect(
            Collectors.toMap(
                Pe::getId,
                pe ->
                    pe.getNodeIndexes().stream()
                        .map(
                            id ->
                                SAMStaticContext.getTopologyTracker()
                                    .getTopologyNode(job.getId(), id))
                        .map(node -> node.getResources().getCoLocations())
                        .flatMap(Collection::stream)
                        .collect(Collectors.toSet())));
  }

  public Map<BigInteger, Set<String>> getPesHostExLocations(Job job) {
    return job.getPes().values().stream()
        .collect(
            Collectors.toMap(
                Pe::getId,
                pe ->
                    pe.getNodeIndexes().stream()
                        .map(
                            id ->
                                SAMStaticContext.getTopologyTracker()
                                    .getTopologyNode(job.getId(), id))
                        .map(node -> node.getResources().getExLocations())
                        .flatMap(Collection::stream)
                        .collect(Collectors.toSet())));
  }

  public Map<BigInteger, String> getPesHostIsolation(Job job) {
    return job.getPes().values().stream()
        .flatMap(
            pe ->
                pe.getNodeIndexes().stream()
                    .map(
                        id ->
                            SAMStaticContext.getTopologyTracker().getTopologyNode(job.getId(), id))
                    .filter(node -> node.getResources().isIsolation())
                    .map(TopologyNode::getName)
                    .map(name -> new AbstractMap.SimpleImmutableEntry<>(pe.getId(), name)))
        .collect(
            Collectors.toMap(
                AbstractMap.SimpleImmutableEntry::getKey,
                AbstractMap.SimpleImmutableEntry::getValue));
  }

  public Map<BigInteger, Set<String>> getPesHostIsolationLabels(Job job) {
    return job.getPes().values().stream()
        .collect(Collectors.toMap(Pe::getId, pe -> getPeHostIsolationLabels(job, pe.getId())));
  }

  /*
   * This is perhaps counter-intuitive: in order to get the isolation labels for thisPpeId, we must
   * look for isolation labels on ALL OTHER PEs. We purposefully do NOT apply a label to thisPeId
   * if thisPeId happens to contain an isolation.
   */
  private Set<String> getPeHostIsolationLabels(Job job, BigInteger thisPeId) {
    return job.getPes().values().stream()
        .filter(pe -> !pe.getId().equals(thisPeId))
        .flatMap(pe -> pe.getNodeIndexes().stream())
        .map(nodeId -> SAMStaticContext.getTopologyTracker().getTopologyNode(job.getId(), nodeId))
        .filter(node -> node.getResources().isIsolation())
        .map(TopologyNode::getName)
        .collect(Collectors.toSet());
  }

  public Map<BigInteger, Set<String>> getPesOperatorNames(Job job) {
    return job.getPes().values().stream()
        .collect(Collectors.toMap(Pe::getId, Pe::getOperatorNames));
  }

  public Map<BigInteger, Hostpool> getHostPools(Job job) {
    return job.getHostpools();
  }

  /*
   * Note that by the point we ask for a PE's hostpool locations, some nodes in the PE have a
   * hostpool location. If that hostpool location did not come from a specific hostpool config
   * from the original source, then it is the default hostpool. The default hostpool is
   * originally created by the compiler, and always has the name "$default".
   *
   * This method excludes the default hostpool so that the only hostpool location it returns are
   * those from an actual hostpool config.
   *
   * TODO: figure out why some pool locations are null.
   */
  public Map<BigInteger, PoolLocation> getPesHostPoolLocation(
      Job job, Map<BigInteger, Hostpool> hostPools) {
    return job.getPes().values().stream()
        .flatMap(
            pe ->
                pe.getNodeIndexes().stream()
                    .map(
                        id ->
                            SAMStaticContext.getTopologyTracker().getTopologyNode(job.getId(), id))
                    .map(node -> node.getResources().getPoolLocation())
                    .filter(Objects::nonNull)
                    .filter(
                        pool -> !hostPools.get(pool.getPoolIndex()).getName().equals("$default"))
                    .distinct()
                    .map(pool -> new AbstractMap.SimpleImmutableEntry<>(pe.getId(), pool)))
        .collect(
            Collectors.toMap(
                AbstractMap.SimpleImmutableEntry::getKey,
                AbstractMap.SimpleImmutableEntry::getValue));
  }

  public Map<BigInteger, ParallelRegion> getParallelRegions(Job job) {
    var topoId = job.getTopologyApplicationId();
    return SAMStaticContext.getObjectTracker().getTopologyApplication(topoId).getParallelRegions();
  }

  public Map<String, BigInteger> getParallelRegionWidths(Job job) {
    var topoId = job.getTopologyApplicationId();
    return SAMStaticContext.getObjectTracker()
        .getTopologyApplication(topoId)
        .getParallelRegionWidths();
  }

  public void transferParallelRegionDifference(
      Job prior,
      Job curr,
      String regionName,
      Set<String> changedOperators,
      Set<String> addedOperators,
      Set<String> removedOperators)
      throws StreamsException {
    var changedNodes = new TreeSet<TopologyNode>();
    var addedNodes = new TreeSet<TopologyNode>();
    var removedNodes = new TreeSet<TopologyNode>();

    prior
        .getTopologyApplication()
        .transferParallelRegionDifference(
            curr.getTopologyApplication(), regionName, changedNodes, addedNodes, removedNodes);

    changedOperators.addAll(
        changedNodes.stream().map(TopologyNode::getName).collect(Collectors.toSet()));
    addedOperators.addAll(
        addedNodes.stream().map(TopologyNode::getName).collect(Collectors.toSet()));
    removedOperators.addAll(
        removedNodes.stream().map(TopologyNode::getName).collect(Collectors.toSet()));
  }
}
