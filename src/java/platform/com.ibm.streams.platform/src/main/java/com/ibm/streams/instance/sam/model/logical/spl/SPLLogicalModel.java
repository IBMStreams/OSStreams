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

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SamConstants;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionTimeErrorException;
import com.ibm.streams.instance.sam.exception.TooManyStartOperatorsException;
import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.instance.sam.model.topology.CustomMetric;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.OperatorRuntimeType;
import com.ibm.streams.instance.sam.model.topology.Toolkit;
import com.ibm.streams.instance.sam.model.topology.ToolkitMap;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleType;
import com.ibm.streams.instance.sam.model.topology.TopologyTupleTypeType;
import com.ibm.streams.instance.sam.model.topology.TraceLevel;
import com.ibm.streams.instance.sam.model.topology.YieldBehaviour;
import com.ibm.streams.sch.composer.Composer_helper;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Properties;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public abstract class SPLLogicalModel implements LogicalModel {

  private static Pattern getChannelPatt = Pattern.compile("getChannel\\(\\)");
  private static Pattern getLocalChannelPatt = Pattern.compile("getLocalChannel\\(\\)");
  private static Pattern getAllChannelsPatt = Pattern.compile("getAllChannels\\(\\)");
  private static Pattern getMaxChannelsPatt = Pattern.compile("getMaxChannels\\(\\)");
  private static Pattern getLocalMaxChannelsPatt = Pattern.compile("getLocalMaxChannels\\(\\)");
  private static Pattern getAllMaxChannelsPatt = Pattern.compile("getAllMaxChannels\\(\\)");

  protected List<CompositeOperatorDefinition> compositeOperDefs = new ArrayList<>();
  protected Map<BigInteger, PrimitiveOperatorDefinition> primitiveOperatorDefs = new HashMap<>();
  protected ParallelRegions parallelRegions = new ParallelRegions();

  private Map<BigInteger, LogicalHostpool> hostpoolMap = new HashMap<>();
  private Map<BigInteger, Operator> logicalOperatorMap = new HashMap<>();
  private Map<BigInteger, List<SplitterMapEntry>> splitterMap = new HashMap<>();
  private Map<BigInteger, PrimitiveOperator> primitiveOperatorMap = new HashMap<>();

  private List<PrimitiveOperatorBase> ccStartOperators = new ArrayList<>();
  private Set<PrimitiveOperatorBase> multiRegionOperators = new HashSet<>();
  private ConsistentCutRegions ccRegions = new ConsistentCutRegions();
  private BigInteger maxPrimitiveOperIndex = BigInteger.ZERO;
  private Map<BigInteger, CustomMetric> customMetrics = new HashMap<>();
  private Map<BigInteger, TopologyTupleType> tupleTypes = new HashMap<>();

  protected Map<BigInteger, SubmissionTimeValue> submissionTimeValues = new HashMap<>();
  protected Map<String, ExportOperator> exportOperators = new HashMap<>();
  protected Map<String, ImportOperator> importOperators = new HashMap<>();

  protected Map<BigInteger, Toolkit> toolkits = new HashMap<>();
  protected Map<BigInteger, URI> sourceURIs = new HashMap<>();

  protected CompositeOperator mainComposite;
  protected String name;
  protected String applicationScope;
  protected String version;
  protected String dataDirectory;
  protected String applicationDirectory;
  protected String outputDirectory;
  protected String checkpointDirectory;
  protected String adlPath;
  protected String buildId;
  protected String bundlePath;
  protected String productVersion;
  protected YieldBehaviour yieldBehaviour = YieldBehaviour.AUTO;

  // tracing setup at submission time. When this is set, it will override the
  // original tracing set from application compiling.
  protected TraceLevel cfgTraceLevel = null;
  private BigInteger splitterCount = BigInteger.ZERO;
  private BigInteger mergerCount = BigInteger.ZERO;

  protected BigInteger splToolkitIndex;

  protected SubmissionTimeErrorException _submissionTimeWarning;

  private boolean forFormalSubmitjob = true;

  public void setConfigTraceLevel(String tracing) {
    // tracing can be set as format of <level>:<size>:<style>, we just processing level for now.
    // no validation checking needed here, as we check at client side.
    String[] tokens = tracing.split(":");
    cfgTraceLevel = TraceLevel.fromValue(tokens[0]);
  }

  public TraceLevel getConfigTraceLevel() {
    return cfgTraceLevel;
  }

  public BigInteger getSplToolkitIndex() {
    return splToolkitIndex;
  }

  public Map<BigInteger, Toolkit> getToolkits() {
    return toolkits;
  }

  public Map<BigInteger, PrimitiveOperatorDefinition> getPrimitiveOperatorDefs() {
    return primitiveOperatorDefs;
  }

  public List<CompositeOperatorDefinition> getCompositeOperDefs() {
    return compositeOperDefs;
  }

  public Map<String, ImportOperator> getImportOperators() {
    return importOperators;
  }

  public Map<String, ExportOperator> getExportOperators() {
    return exportOperators;
  }

  public void setExportOperators(Map<String, ExportOperator> exportOperators) {
    this.exportOperators = exportOperators;
  }

  public Map<BigInteger, SubmissionTimeValue> getSubmissionTimeValues() {
    return submissionTimeValues;
  }

  public void setSubmissionTimeValues(Map<BigInteger, SubmissionTimeValue> submissionTimeValues) {
    this.submissionTimeValues = submissionTimeValues;
  }

  public Map<BigInteger, PrimitiveOperatorDefinition> getPrimitiveOperDefs() {
    return primitiveOperatorDefs;
  }

  public Map<BigInteger, URI> getSourceURIs() {
    return sourceURIs;
  }

  public Map<BigInteger, CustomMetric> getCustomMetrics() {
    return customMetrics;
  }

  public Map<BigInteger, TopologyTupleType> getTupleTypes() {
    return tupleTypes;
  }

  public CompositeOperator getMainComposite() {
    return mainComposite;
  }

  public String getAdlPath() {
    return adlPath;
  }

  public String getCheckpointDirectory() {
    return checkpointDirectory;
  }

  public String getOutputDirectory() {
    return outputDirectory;
  }

  public String getApplicationDirectory() {
    return applicationDirectory;
  }

  public String getDataDirectory() {
    return dataDirectory;
  }

  public String getVersion() {
    return version;
  }

  public String getApplicationScope() {
    return applicationScope;
  }

  public String getName() {
    return name;
  }

  public Map<BigInteger, LogicalHostpool> getHostpoolMap() {
    return hostpoolMap;
  }

  public Map<BigInteger, PrimitiveOperator> getPrimitiveOperatorMap() {
    return primitiveOperatorMap;
  }

  public PrimitiveOperator getPrimitiveOperator(BigInteger index) {
    return primitiveOperatorMap.get(index);
  }

  public Map<BigInteger, Operator> getOperatorMap() {
    return logicalOperatorMap;
  }

  public ConsistentCutRegions getCCRegions() {
    return ccRegions;
  }

  public Operator getModelOperator(BigInteger index) throws SAMInternalErrorException {
    if (!logicalOperatorMap.containsKey(index)) {
      throw new SAMInternalErrorException("Internal error: could not find model operator");
    }
    return logicalOperatorMap.get(index);
  }

  public void setMaxPrimitiveOperIndex(BigInteger index) {
    if (maxPrimitiveOperIndex.compareTo(index) < 0) {
      maxPrimitiveOperIndex = index;
    }
  }

  public ParallelRegions getParallelRegions() {
    return parallelRegions;
  }

  public List<PrimitiveOperatorBase> getCCStartOperators() {
    return ccStartOperators;
  }

  public Set<PrimitiveOperatorBase> getMultiRegionOperators() {
    return multiRegionOperators;
  }

  public void addSplitterMapping(
      BigInteger splitterOperIndex,
      BigInteger owningOperatorIndex,
      BigInteger owningPortIndex,
      BigInteger physicalSplitterIndex) {

    List<SplitterMapEntry> mappings;
    if (splitterMap.containsKey(splitterOperIndex)) {
      mappings = splitterMap.get(splitterOperIndex);
    } else {
      mappings = new ArrayList<>();
      splitterMap.put(splitterOperIndex, mappings);
    }
    mappings.add(new SplitterMapEntry(owningOperatorIndex, owningPortIndex, physicalSplitterIndex));
  }

  // SCOTT_HACK
  public void performTransforms() throws StreamsException {
    Map<String, BigInteger> parRegionNameToWidth = new TreeMap<>();
    performTransforms(parRegionNameToWidth);
  }

  // SCOTT_HACK
  public void performTransforms(Map<String, BigInteger> parRegionNameToWidth)
      throws StreamsException {
    if (!parallelRegions.isEmpty()) {
      Map<BigInteger, ParallelRegion> regions = getParallelRegions().getParallelRegionMap();
      for (Map.Entry<BigInteger, ParallelRegion> entry : regions.entrySet()) {
        ParallelRegion pr = entry.getValue();
        String logicalName = getOperatorMap().get(pr.getOperIndex()).getFullyQualifiedLogicalName();
        for (Map.Entry<String, BigInteger> nw : parRegionNameToWidth.entrySet()) {
          if (logicalName.matches(Composer_helper.prepareConditionString(nw.getKey()))) {
            pr.setOverlayWidth(nw.getValue());
          }
        }
      }

      mainComposite.performParallelTransform(this);
    }
  }

  public static CCRegion mergeRegions(
      CCRegion regionA, CCRegion regionB, Map<MergedPair, CCRegion> mergeMap)
      throws SAMInternalErrorException {
    if (regionA == regionB) {
      return regionA;
    }

    MergedPair mergedPair = new MergedPair(regionA, regionB);
    if (mergeMap.containsKey(mergedPair)) {
      return mergeMap.get(mergedPair);
    }

    if (regionA.wasMerged()) {
      return mergeRegions(regionA.mergedWith(), regionB, mergeMap);
    }
    if (regionB.wasMerged()) {
      return mergeRegions(regionA, regionB.mergedWith(), mergeMap);
    }

    mergeMap.put(new MergedPair(regionA, regionB), regionA);
    regionA.merge(regionB);
    return regionA;
  }

  @Override
  public void computeCCRegions() throws SAMInternalErrorException, TooManyStartOperatorsException {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace("Computing CC regions");
    }

    // Walk the logical model and discover any start operators
    mainComposite.findStartOperators(this);

    // for each start operator, compute the reachability graph
    for (PrimitiveOperatorBase oper : getCCStartOperators()) {
      BigInteger globalChannelIndex = BigInteger.ZERO;
      if (oper.isInParallelRegion()) {
        globalChannelIndex = oper.getGlobalChannelIndex();
      }
      CCRegion ccRegion =
          getCCRegions().findRegion(oper.getCCInfo().getIndex(), globalChannelIndex);
      Set<PrimitiveOperatorBase> operatorsSeen = new HashSet<>();

      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Computing CC region starting at operator " + oper.getFullyQualifiedName());
      }
      oper.computeCCReachabilityGraph(ccRegion, operatorsSeen, this);
    } // end for

    Set<PrimitiveOperatorBase> multiRegionOps = getMultiRegionOperators();
    // check for regions that intersect
    if (multiRegionOps.size() > 0) {
      Map<MergedPair, CCRegion> mergeMap = new HashMap<>();
      // regions that intersect exist, merge some regions
      for (PrimitiveOperatorBase op : multiRegionOps) {
        List<CCRegion> regionsToMerge = op.getCCInfo().getRegions();
        if (regionsToMerge.size() < 2) {
          throw new SAMInternalErrorException("Internal error: regionsToMerge is incorrect");
        }
        CCRegion regionA = regionsToMerge.get(0);
        CCRegion regionB = regionsToMerge.get(1);
        CCRegion mergedRegion = mergeRegions(regionA, regionB, mergeMap);
        for (int i = 2, size = regionsToMerge.size(); i < size; i++) {
          mergedRegion = mergeRegions(mergedRegion, regionsToMerge.get(i), mergeMap);
        }
        op.getCCInfo().setMergedRegion(mergedRegion);
      }
    }

    // see if any operatorDriven region has more than one start operator
    List<CCRegion> regions = getCCRegions().getRegions();
    for (CCRegion region : regions) {
      if (region.isOperatorDriven()) {
        Set<PrimitiveOperatorBase> operators = region.getOperators();
        int count = 0;
        for (PrimitiveOperatorBase oper : operators) {
          CCInfo ccInfo = oper.getCCInfo();
          if (ccInfo.isStartCutRegion()) {
            ++count;
          }
        }
        if (count > 1) {
          throw new TooManyStartOperatorsException();
        }
      }
    }
    // Finally, inject all the annotations that identify operators that are in CC regions
    mainComposite.injectCCAnnotations();
    mainComposite.performCCResourceTransforms();
  } // end computeCCRegions

  void applySubmissionConfigs(Properties cfgs) throws SAMInternalErrorException {
    String model = cfgs.getProperty(SamConstants.threadingModel);
    if (model == null || model.equals(SamConstants.threadingModel_notSpecified)) {
      return;
    }

    String threads = cfgs.getProperty(SamConstants.dynamicThreadingThreadCount);
    String elastic = cfgs.getProperty(SamConstants.dynamicThreadingElastic);
    if (mainComposite.getThreadingModel(this) == null) {
      mainComposite.setThreadingModel(ThreadingModel.create(model, threads, elastic));
    }
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<logicalModel>");
    dumper.indent();
    dumper.line("<hostPools>");
    dumper.indent();
    dumper.append(hostpoolMap);
    dumper.outdent();
    dumper.line("</hostPools>");
    dumper.line("<operDefinitions>");
    dumper.indent();
    dumper.append(primitiveOperatorDefs);
    dumper.outdent();
    dumper.line("</operDefinitions>");
    dumper.line("<compositeOperInstances>");
    dumper.indent();
    dumper.append(mainComposite.toString());
    dumper.outdent();
    dumper.line("</compositeOperInstances>");
    dumper.line("<toolkits>");
    dumper.indent();
    dumper.append(toolkits);
    dumper.outdent();
    dumper.line("</toolkits>");
    dumper.append(parallelRegions.toString());
    dumper.line("<submissionTimeValues>");
    dumper.indent();
    dumper.append(submissionTimeValues);
    dumper.outdent();
    dumper.line("</submissionTimeValues>");
    dumper.outdent();
    dumper.line("</logicalModel>");
    return dumper.toString();
  }

  public void populateHostpoolMap(TopologyApplication ta) {
    SortedMap<BigInteger, Hostpool> hostpools = new TreeMap<>();

    Map<BigInteger, LogicalHostpool> hostpoolMap = getHostpoolMap();
    for (BigInteger key : hostpoolMap.keySet()) {
      LogicalHostpool hp = hostpoolMap.get(key);
      Hostpool value = hp.createHostpool();
      hostpools.put(key, value);
    }
    ta.setHostpools(hostpools);
  }

  public void populateParallelRegionsMap(TopologyApplication ta) throws SAMInternalErrorException {
    SortedMap<BigInteger, com.ibm.streams.instance.sam.model.topology.ParallelRegion>
        topParallelRegions = new TreeMap<>();
    SortedMap<String, BigInteger> topParallelRegionNames = new TreeMap<>();

    Map<BigInteger, ParallelRegion> parallelRegionsMap =
        getParallelRegions().getParallelRegionMap();
    for (BigInteger key : parallelRegionsMap.keySet()) {
      ParallelRegion logicalPR = parallelRegionsMap.get(key);
      Operator oper = getOperatorMap().get(logicalPR.getOperIndex());
      com.ibm.streams.instance.sam.model.topology.ParallelRegion topPR =
          oper.createTopologyParallelRegion();
      topParallelRegions.put(key, topPR);
      topParallelRegionNames.put(oper.getFullyQualifiedLogicalName(), key);
    }
    ta.setParallelRegions(topParallelRegions);
    ta.setParallelRegionNames(topParallelRegionNames);
  }

  protected void populateTupleTypes(TopologyApplication ta) {
    SortedMap<BigInteger, TopologyTupleTypeType> topTupleTypes = new TreeMap<>();
    Map<BigInteger, TopologyTupleType> tupleTypes = getTupleTypes();
    for (BigInteger key : tupleTypes.keySet()) {
      TopologyTupleType tuple = tupleTypes.get(key);
      TopologyTupleType tupleTypeCopy = new TopologyTupleType(tuple);
      TopologyTupleTypeType tupleTypeType = new TopologyTupleTypeType(key, tupleTypeCopy);
      topTupleTypes.put(key, tupleTypeType);
    }
    ta.setTupleTypes(topTupleTypes);
  }

  private void populateCustomMetrics(TopologyApplication ta) {
    SortedMap<BigInteger, CustomMetric> topMetrics = new TreeMap<>();
    for (BigInteger key : customMetrics.keySet()) {
      topMetrics.put(key, customMetrics.get(key).copy());
    }
    ta.setCustomMetrics(topMetrics);
  }

  private void populateDisplay(TopologyApplication ta) {
    for (SubmissionTimeValue stv : submissionTimeValues.values()) {
      String name = stv.getName();
      String qualName = stv.getCompositeName() + "." + name;
      if (name.equals(SamConstants.DISPLAY_VAR_NAME)
          || qualName.equals(SamConstants.DISPLAY_VAR_NAME)) {
        com.ibm.streams.instance.sam.model.topology.RuntimeConstant topRC =
            new com.ibm.streams.instance.sam.model.topology.RuntimeConstant(
                SamConstants.DISPLAY_VAR_NAME, stv.getValue());
        ta.setDisplay(topRC);
        break;
      }
    }
  }

  @Override
  public TopologyApplication generateTopology(Properties cfgs) throws StreamsException {
    // Perform any transforms we need to do
    Map<String, BigInteger> parRegionNameToWidth = new TreeMap<>();
    return generateTopology(cfgs, parRegionNameToWidth);
  }

  // This method is a temporary hack so that Scott can work on transformer and topology diff logic
  // before we
  // figure out exactly how to pass this information through the configs. Once we do, we will go
  // back to
  // a single version of generateTopology.
  // SCOTT_HACK
  @Override
  public TopologyApplication generateTopology(
      Properties cfgs, Map<String, BigInteger> parRegionNameToWidth) throws StreamsException {
    // Perform any transforms we need to do
    performTransforms(parRegionNameToWidth);

    // Recompute the CC regions
    computeCCRegions();

    applySubmissionConfigs(cfgs);

    List<Toolkit> tkl = new ArrayList<>();
    for (Entry<java.math.BigInteger, Toolkit> tk : getToolkits().entrySet()) {
      tkl.add(tk.getValue());
    }
    ToolkitMap toolkitMap = new ToolkitMap(tkl);
    TopologyApplication ta =
        new TopologyApplication(
            getName(),
            getApplicationScope(),
            getVersion(),
            getDataDirectory(),
            getApplicationDirectory(),
            getOutputDirectory(),
            getCheckpointDirectory(),
            getAdlPath(),
            getBuildId(),
            getBundlePath(),
            toolkitMap,
            productVersion,
            yieldBehaviour);
    populateHostpoolMap(ta);
    populateParallelRegionsMap(ta);
    populateTupleTypes(ta);
    populateCustomMetrics(ta);
    List<TopologyNode> nodes = new ArrayList<>();
    Map<BigInteger, TopologyNode> nodeMap = new HashMap<>();
    getMainComposite().populateNodes(ta, nodes, nodeMap, this);
    getMainComposite().populateConnections(nodeMap, this);
    ta.setNodes(nodes);
    ta.setSubmissionTimeValues(submissionTimeValues);
    populateDisplay(ta);
    return ta;
  }

  @Override
  public void instanceVerificationForApp() throws StreamsException {}

  public String getBuildId() {
    return buildId;
  }

  public String getBundlePath() {
    return bundlePath;
  }

  public BigInteger findOrCreatePrimitiveOperatorDefinitionIndex(
      String kind, OperatorRuntimeType runtimeType, BigInteger toolkitIndex) {
    Collection<PrimitiveOperatorDefinition> values = primitiveOperatorDefs.values();
    for (PrimitiveOperatorDefinition def : values) {
      if (def.getKind().equals(kind)) {
        return def.getIndex();
      }
    }
    // If we are here we didn't find the def'n
    BigInteger newIndex = new BigInteger(Integer.toString(primitiveOperatorDefs.size()));
    PrimitiveOperatorDefinition newDef =
        new PrimitiveOperatorDefinition(
            newIndex,
            kind,
            runtimeType,
            toolkitIndex,
            new ArrayList<BigInteger>(),
            new ArrayList<String>());
    primitiveOperatorDefs.put(newIndex, newDef);
    // Special case to add a toolkit index to the Splitter/Merger operators
    if (kind.equals(getSplitterOperatorKind()) || kind.equals(getMergerOperatorKind())) {
      newDef.setToolkitIndex(getSPLToolkitIndex());
    }
    return newIndex;
  }

  private BigInteger getSPLToolkitIndex() {
    assert (splToolkitIndex != null);
    return splToolkitIndex;
  }

  public String getSplitterOperatorKind() {
    return "spl.utility::Splitter";
  }

  public String getMergerOperatorKind() {
    return "spl.utility::Merger";
  }

  public String getImportOperatorKind() {
    return "spl.adapter::Import";
  }

  public String getExportOperatorKind() {
    return "spl.adapter::Export";
  }

  public String allocateSplitterName() {
    splitterCount = splitterCount.add(BigInteger.ONE);
    return "splitter_" + splitterCount.toString();
  }

  public String allocateMergerName() {
    mergerCount = mergerCount.add(BigInteger.ONE);
    return "merger_" + mergerCount.toString();
  }

  @Override
  public SubmissionTimeErrorException getSubmissionTimeWarning() {
    return _submissionTimeWarning;
  }

  public static void trace(String msg) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(msg);
    }
  }

  public static SAMInternalErrorException traceAndReturnException(String msg) {
    if (Trace.isEnabled(Level.TRACE)) {
      Trace.logTrace(msg);
    }
    return new SAMInternalErrorException(msg);
  }

  private static BigInteger product(List<BigInteger> maxChannels, int limit) {
    BigInteger prod = maxChannels.get(0);
    for (int i = 1; i < limit; ++i) {
      prod = prod.multiply(maxChannels.get(i));
    }
    return prod;
  }

  // TODO: make sure this works with submission time widths
  public static BigInteger computeGlobalChannelIndex(List<ParallelRegionInfo> infos) {
    if (infos.isEmpty()) {
      return BigInteger.valueOf(-1);
    }

    List<BigInteger> channelIndexes = new ArrayList<>();
    List<BigInteger> maxChannels = new ArrayList<>();
    for (ParallelRegionInfo pr : infos) {
      channelIndexes.add(pr.getLocalChannelIndex());
      maxChannels.add(pr.getLocalMaxChannels());
    }
    return computeGlobalChannelIndex(channelIndexes, maxChannels);
  }

  // TODO: make sure this works with submission time widths
  private static BigInteger computeGlobalChannelIndex(
      List<BigInteger> channelIndexes, List<BigInteger> maxChannels) {
    BigInteger channel = channelIndexes.get(0);
    for (int i = 1; i < channelIndexes.size(); ++i) {
      channel = channel.add(product(maxChannels, i).multiply(channelIndexes.get(i)));
    }
    return channel;
  }

  // TODO: make sure this works with submission time widths
  public static BigInteger computeGlobalMaxChannels(List<ParallelRegionInfo> infos) {
    if (infos.isEmpty()) {
      return BigInteger.ZERO;
    }

    List<BigInteger> maxChannels = new ArrayList<>();
    for (ParallelRegionInfo pr : infos) {
      maxChannels.add(pr.getLocalMaxChannels());
    }

    BigInteger maxChan = maxChannels.get(0);
    for (int i = 1; i < maxChannels.size(); ++i) {
      maxChan = maxChan.multiply(maxChannels.get(i));
    }
    return maxChan;
  }

  public static String performIntrinsicFunctionSubstitutions(
      String str,
      List<com.ibm.streams.instance.sam.model.topology.ParallelRegion> parallelRegions) {

    StringBuilder getAllChannelsList = new StringBuilder();
    StringBuilder getAllMaxChannelsList = new StringBuilder();
    StringBuilder getLocalChannelInt = new StringBuilder();
    StringBuilder getLocalMaxChannelsInt = new StringBuilder();
    StringBuilder getChannelInt = new StringBuilder();
    StringBuilder getMaxChannelsInt = new StringBuilder();
    getAllChannelsList.append("[");
    getAllMaxChannelsList.append("[");
    List<BigInteger> globalChannels = new ArrayList<>();
    if (parallelRegions.isEmpty()) {
      getAllChannelsList.append("-1");
      getAllMaxChannelsList.append("0");
      getLocalChannelInt.append("-1");
      getLocalMaxChannelsInt.append(0);
      getChannelInt.append("-1");
      getMaxChannelsInt.append("0");
    } else {
      boolean first = true;
      for (com.ibm.streams.instance.sam.model.topology.ParallelRegion pr : parallelRegions) {
        if (!first) {
          getAllChannelsList.append(",");
          getAllMaxChannelsList.append(",");
        }
        getAllChannelsList.append(pr.getGlobalChannelIndex());
        getAllMaxChannelsList.append(pr.getGlobalMaxChannels());
        first = false;
        globalChannels.add(0, pr.getGlobalChannelIndex());
      }
      getLocalChannelInt.append(parallelRegions.get(0).getLocalChannelIndex());
      getLocalMaxChannelsInt.append(parallelRegions.get(0).getLocalMaxChannels());
      getChannelInt.append(parallelRegions.get(0).getGlobalChannelIndex().intValue());
      getMaxChannelsInt.append(parallelRegions.get(0).getGlobalMaxChannels().intValue());
    }
    getAllChannelsList.append("]");
    getAllMaxChannelsList.append("]");

    String result = str;
    result = performByChannelSubstitutions(result, globalChannels);
    result = getChannelPatt.matcher(result).replaceAll(getChannelInt.toString());
    result = getLocalChannelPatt.matcher(result).replaceAll(getLocalChannelInt.toString());
    result = getAllChannelsPatt.matcher(result).replaceAll(getAllChannelsList.toString());
    result = getMaxChannelsPatt.matcher(result).replaceAll(getMaxChannelsInt.toString());
    result = getLocalMaxChannelsPatt.matcher(result).replaceAll(getLocalMaxChannelsInt.toString());
    result = getAllMaxChannelsPatt.matcher(result).replaceAll(getAllMaxChannelsList.toString());
    return result;
  }

  private static String performByChannelSubstitutions(String str, List<BigInteger> globalChannels) {
    Pattern byChannelPatt = Pattern.compile("byChannel\\(([^,]*), ([^)]*)\\)");
    Matcher matcher = byChannelPatt.matcher(str);
    while (matcher.find()) {
      StringBuilder repText = new StringBuilder();
      repText.append(matcher.group(1));
      if (!globalChannels.isEmpty()) {
        repText.append("_");
        Integer index = Integer.valueOf(matcher.group(2));
        BigInteger channel = globalChannels.get(index);
        repText.append(channel.intValue());
      }
      str = matcher.replaceFirst(repText.toString());
      matcher = byChannelPatt.matcher(str);
    }

    return str;
  }

  public YieldBehaviour getYieldBehaviour() {
    return yieldBehaviour;
  }

  public boolean isForFormalSubmitjob() {
    return forFormalSubmitjob;
  }

  public void forPreviewSubmitjob() {
    forFormalSubmitjob = false;
  }
}
