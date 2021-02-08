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

package com.ibm.streams.sch.composer;

import com.api.json.JSONArray;
import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.config.PropertyDefinition.JobResourceSharing;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.model.composer.ContainerSpecification;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.MembershipMode;
import com.ibm.streams.instance.sam.model.topology.Partitions;
import com.ibm.streams.instance.sam.model.topology.Resources;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.AdditionalExceptionDataType;
import com.ibm.streams.platform.services.MessageElementType;
import com.ibm.streams.sch.SCHLocationGroupManager;
import com.ibm.streams.sch.SCHPartitionColocationGroup;
import com.ibm.streams.sch.SupportFunctions;
import com.ibm.streams.sch.composer.fusingRules.ComposerContainerComparator;
import com.ibm.streams.sch.composer.fusingRules.FusableUnitComparator;
import com.ibm.streams.sch.composer.fusingRules.FusingCycleComparator;
import com.ibm.streams.sch.exceptions.IncompatibleFusingConstraintsException;
import com.ibm.streams.sch.exceptions.InvalidPlacementStrategyException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.SortedMap;
import java.util.TreeMap;

public class IEUComposer {

  /*
   * Constant definitions.
   */

  private static final String PARTITIONS = "P_CFG:";
  private static final String RESOURCES = "R_CFG:";
  private static final String FC_PREFIX = "$_$_FC: ";
  private static final int OPS_PER_RESOURCE = 8;

  /*
   * Static members.
   */

  private static Map<String /*groupType*/, Map<String /*groupId*/, String /*alias*/>>
      _groupAliasIds;

  /*
   * Members.
   */

  private TopologyApplication _topApp;
  private Set<ContainerSpecification> _precomposedContSpecs; // passed in
  private Set<ComposedContainer>
      _precomposedContainers; // derived from contSpecs, after model built.
  private List<FusableUnit> _fusUnits;
  private Map<BigInteger, ComposerCriteriaNode> _compCriteriaNodes;
  private SCHLocationGroupManager _locGrpMgr;
  private long _nextFuId;
  private long _nextContainerId;
  private long _nextCritNode;
  private SortedMap<TopologyNode, FusableUnit> _topNodeToFuMap;
  private long _fusionPhaseBeginTime_ms;
  private Map<BigInteger, Integer> _resToRequest;
  private List<ResourceRequest> _additionalResToRequest;
  private Set<String> _resToRemove;
  private Integer _peToCoreRatio;
  private Integer _fusionCurfew;
  private Integer _minOpsPerPe;
  private Integer _proposedOpsPerResource;
  private JobResourceSharing _jobResourceSharing;
  private boolean _jobSubmission;
  private Set<ResourceAcquistionMessage> _resAcquistionMessages;
  private int _numOfAugmentedContainers = 0;
  private JSONArray _resTruncatedMessages;

  /*
   * Inner class definitions.
   */

  static class ContainerSpecInfo {
    int _numOfContainers;
    int _minNumOfOpsPerContainer;

    ContainerSpecInfo(int numOfConts, int minNumOpsPerPe) {
      _numOfContainers = numOfConts;
      _minNumOfOpsPerContainer = minNumOpsPerPe;
    }

    public String toString() {
      return "HostSpecInfo: numOfCont["
          + _numOfContainers
          + "] minNumOpsCont["
          + _minNumOfOpsPerContainer
          + "]";
    }
  }

  public static class ComposerConfig {
    public Integer _peToCoreRatio;
    public Integer _minOpsPerPe;
    public Integer _fusionCurfew;
    //		public Integer _numOfTargetOperators;
    public Integer _proposedOpsPerResource;
    public JobResourceSharing _jobResourceSharing;
    public boolean _jobSubmission;

    public ComposerConfig() {
      _jobSubmission = true;
    }
  }

  public static class PreComposedInfo {
    Set<ComposedContainer> _nonTargetComposedContainers;
    Set<FusableUnit> _nonTargetFus;

    PreComposedInfo(
        Set<ComposedContainer> relevantPreComposedContainers,
        Set<FusableUnit> relevantPreComposedFus) {
      _nonTargetComposedContainers = relevantPreComposedContainers;
      _nonTargetFus = relevantPreComposedFus;
    }
  }

  public static class ResourceAcquistionMessage {
    public String _resAcqType;
    public String _acquiredNum;
    public String _requestedNum;
    public String _taggingRequirements;
    public String _opsPerRes;

    public ResourceAcquistionMessage(
        String resAcqType,
        String acquiredNum,
        String requestedNum,
        String tagRqmts,
        String opsPerRes) {
      _resAcqType = resAcqType;
      _acquiredNum = acquiredNum;
      _requestedNum = requestedNum;
      _taggingRequirements = tagRqmts;
      _opsPerRes = opsPerRes;
    }
  }

  public static class ResourceRequest {
    int _num;
    public Set<String> _tags;
    boolean _considerResourcesWithinJobForAddition;

    public ResourceRequest(
        int num, Set<String> tags, boolean considerResourcesWithinJobForAddition) {
      _num = num;
      _tags = tags;
      _considerResourcesWithinJobForAddition = considerResourcesWithinJobForAddition;
    }

    public String toString() {
      return "ResRqst["
          + _num
          + "] {"
          + _tags
          + "} consider("
          + _considerResourcesWithinJobForAddition
          + ")";
    }
  }

  /*
   * Duplicated STD functions.
   */

  private int roundUp(int num, int div) {
    return ((num + div - 1) / div);
  }

  private static String pad(String target, int numOfSpaces) {
    StringBuilder st = new StringBuilder();
    for (int x = 0; x < numOfSpaces; x++) {
      st.append(" ");
    }
    st.append(target);
    return st.toString();
  }

  /*
   * Constructor.
   */

  public IEUComposer(TopologyApplication topApp, Set<ContainerSpecification> precomposedContSpecs) {
    _fusUnits = new ArrayList<>();
    _compCriteriaNodes = new HashMap<BigInteger, ComposerCriteriaNode>();
    _nextFuId = 0;
    _nextContainerId = 0;
    _nextCritNode = 0;
    _topNodeToFuMap = new TreeMap<>();
    _topApp = topApp;
    if (precomposedContSpecs != null) {
      _precomposedContSpecs = precomposedContSpecs;
    } else {
      _precomposedContSpecs = new HashSet<>();
    }
    _precomposedContainers = new HashSet<>();
    _peToCoreRatio = null;
    _fusionCurfew = null;
    _minOpsPerPe = null;
    _locGrpMgr = new SCHLocationGroupManager();
    _resToRequest = new HashMap<>();
    _additionalResToRequest = new ArrayList<>();
    _resToRemove = new HashSet<>();
    _resAcquistionMessages = new HashSet<>();
    _resTruncatedMessages = new JSONArray();
    _groupAliasIds = new HashMap<>();
  }

  /*
   * Class methods.
   */

  private static List<String> alignNodeAbstracts(List<String> nodeAbstracts, String keyword) {
    int maxPartIndex = 0;
    for (String line : nodeAbstracts) {
      int partIndex = line.indexOf(keyword);
      if (partIndex > maxPartIndex) {
        maxPartIndex = partIndex;
      }
    }
    List<String> alignedNodeAbstracts = new ArrayList<>();
    for (String line : nodeAbstracts) {
      int partIndex = line.indexOf(keyword);
      alignedNodeAbstracts.add(line.replaceFirst(keyword, pad(keyword, maxPartIndex - partIndex)));
    }

    return alignedNodeAbstracts;
  }

  private static String traceTopologyNodeAbstract(TopologyNode node) {
    StringBuilder st = new StringBuilder();
    st.append(node.getName() + " (" + node.getIndex() + ") ");
    Partitions partConstr = node.getPartitions();
    if (partConstr != null) {
      st.append(PARTITIONS + "<");
      if ((partConstr.getCoLocations() != null) && (partConstr.getCoLocations().size() > 0)) {
        st.append("c[" + getAliases(partConstr.getCoLocations(), "c") + "] ");
      }
      if ((partConstr.getExLocations() != null) && (partConstr.getExLocations().size() > 0)) {
        st.append("x[" + getAliases(partConstr.getExLocations(), "x") + "] ");
      }
      if (partConstr.isIsolation()) {
        st.append("i[true] ");
      }
      if (partConstr.isSoftIsolation()) {
        st.append("j[true] ");
      }
      if ((partConstr.getProvisionalLocations() != null)
          && (partConstr.getProvisionalLocations().size() > 0)) {
        st.append("v[" + partConstr.getProvisionalLocations() + "] ");
      }
      if ((partConstr.getSoftProvisionalLocations() != null)
          && (partConstr.getSoftProvisionalLocations().size() > 0)) {
        st.append("u[" + partConstr.getSoftProvisionalLocations() + "] ");
      }
      if ((partConstr.getSegregatedLocations() != null)
          && (partConstr.getSegregatedLocations().size() > 0)) {
        st.append("g[" + partConstr.getSegregatedLocations() + "] ");
      }
      if ((partConstr.getSoftSegregatedLocations() != null)
          && (partConstr.getSoftSegregatedLocations().size() > 0)) {
        st.append("f[" + partConstr.getSoftSegregatedLocations() + "] ");
      }
      st.append("> ");
    }
    Resources resConstr = node.getResources();
    if (resConstr != null) {
      st.append(RESOURCES + "<");
      if ((resConstr.getCoLocations() != null) && (resConstr.getCoLocations().size() > 0)) {
        Set<String> scrubbedCoLoc = new HashSet<>();
        for (String cgId : resConstr.getCoLocations()) {
          if (!cgId.startsWith(ComposerCriteriaNode.INFERRED_COLOCATION)) {
            scrubbedCoLoc.add(cgId);
          }
        }
        if (scrubbedCoLoc.size() > 0) {
          st.append("C[" + getAliases(scrubbedCoLoc, "C") + "] ");
        }
      }
      if ((resConstr.getExLocations() != null) && (resConstr.getExLocations().size() > 0)) {
        st.append("X[" + getAliases(resConstr.getExLocations(), "X") + "] ");
      }
      if (resConstr.isIsolation()) {
        st.append("I[true] ");
      }
      if (resConstr.getHostLocation() != null) {
        st.append("H[" + resConstr.getHostLocation() + "] ");
      } else {
        if (resConstr.getPoolLocation() != null) {
          st.append("P[" + resConstr.getPoolLocation().getPoolIndex() + "");
          if (resConstr.getPoolLocation().getInpoolIndex() != null) {
            st.append("(" + resConstr.getPoolLocation().getInpoolIndex() + ")");
          }
          st.append("] ");
        }
      }
      st.append("> ");
    }
    if (!node.isRestartable()) {
      st.append("!R ");
    }
    if (!node.isRelocatable()) {
      st.append("!r ");
    }
    return st.toString();
  }

  private static Set<String> getAliases(Set<String> groupIds, String groupType) {
    Set<String> aliases = new HashSet<>();
    for (String groupId : groupIds) {
      aliases.add(getAlias(groupId, groupType));
    }
    return aliases;
  }

  private static String getAlias(String groupId, String groupType) {
    if (_groupAliasIds == null) {
      _groupAliasIds = new HashMap<>();
    }
    Map<String, String> aliasMap = _groupAliasIds.computeIfAbsent(groupType, k -> new HashMap<>());
    String alias = aliasMap.get(groupId);
    if (alias == null) {
      alias = groupType + aliasMap.size();
      aliasMap.put(groupId, alias);
    }
    return alias;
  }

  public static void traceTopologyModelAbstract(String header, TopologyApplication topApp) {
    StringBuilder st = new StringBuilder();
    st.append(header + "$@$@ TopologyModelAbstract:\n");
    st.append("Hostpools: \n");
    for (Hostpool hPool : topApp.getHostpools().values()) {
      st.append(
          "   pool["
              + hPool.getIndex()
              + ","
              + hPool.getName()
              + "] "
              + ((hPool.getSize() != null) ? "size[" + hPool.getSize() + "] " : " ")
              + ((hPool.getMembershipMode().equals(MembershipMode.exclusive))
                  ? "mMode[" + hPool.getMembershipMode().toString() + "]"
                  : " ")
              + " tags{"
              + hPool.getTags()
              + "}\n");
      for (String hostName : hPool.getHosts()) {
        st.append("      " + hostName + "\n");
      }
    }
    List<String> nodeAbstracts = new ArrayList<>();
    for (TopologyNode node : topApp.getNodes()) {
      nodeAbstracts.add(traceTopologyNodeAbstract(node));
    }
    for (String nodeAbst :
        alignNodeAbstracts(alignNodeAbstracts(nodeAbstracts, PARTITIONS), RESOURCES)) {
      st.append(nodeAbst + "\n");
    }
    st.append("Alias ID Map:" + "\n");
    st.append(_groupAliasIds);
    st.append("      \n");
    Trace.logDebug(st.toString());
    Trace.logDebug("abstract complete.");
  }

  /*
   * Methods.
   */

  // only return new container specs...
  //    - don't return precomposed ones.
  public Set<ContainerSpecification> composeContainerSpecs(ComposingInstructions inst)
      throws StreamsException {

    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("$_$_$_cmp_b4");
      Trace.logDebug(">>>>>>>>>>>>>> composeContainerSpecs[" + inst.toString() + "]");
      //			Trace.logDebug("Topology to compose ... \n" + _topApp.toString());
      Trace.logDebug(dumpConfig());
      Trace.logDebug("composer cfg: " + getConfigSettings());
      traceTopologyModelAbstract("decorated ", _topApp);
    }
    _fusionPhaseBeginTime_ms = System.currentTimeMillis();

    // best container collection after all fusing cycles done.
    List<ComposedContainer> bestContainerCollection = new ArrayList<>();

    Trace.logDebug("Fusing Cycles[" + inst.getFusingCycles().size() + "]");
    int fusCycleNum = 1;
    Integer fixedTargetNumOfContainers = null;

    // fusing cycle
    fusCycleLoop:
    for (FusingCycle fusCycle : inst.getFusingCycles()) {
      List<ComposedContainer> completedCompContainers = null;

      Trace.logDebug("*************************************************************");
      Trace.logDebug(
          "************** Fusing cycle ["
              + fusCycleNum
              + "] StrategyScript["
              + fusCycle.toString()
              + "] ***************");

      switch (inst.getComposingStyle()) {
        case FUSE_TO_ONE:
          {
            Trace.logDebug("$$$Comp: Fuse_to_one");
            validateFusableUnits();
            ComposedContainer cont = new ComposedContainer(this);
            cont.addFusableUnits(_fusUnits);
            bestContainerCollection.add(cont);
            Trace.logDebug("fuse to one - complete.");
            break fusCycleLoop;
          }
        case FUSE_LEGACY:
          {
            Trace.logDebug("$$$Comp: FUSE_LEGACY");
            validateFusableUnits();
            PreComposedInfo preCompInfo =
                processPreComposedContainers(_precomposedContainers, _fusUnits);
            bestContainerCollection.addAll(preCompInfo._nonTargetComposedContainers);
            Set<FusableUnit> remainingFus = new HashSet<>(_fusUnits);
            remainingFus.removeAll(preCompInfo._nonTargetFus);
            for (FusableUnit fu : remainingFus) {
              ComposedContainer cont = new ComposedContainer(this);
              cont.addFusableUnit(fu);
              bestContainerCollection.add(cont);
            }
            Trace.logDebug("FUSE_LEGACY done.");
            break fusCycleLoop;
          }
        case FUSE_MANUAL:
          {
            Trace.logDebug("$$$Comp: FUSE_MANUAL");
            validateFusableUnits();
            fixedTargetNumOfContainers = inst.getFixedNumOfContainers();
            completedCompContainers = performManualFusion(fixedTargetNumOfContainers, fusCycle);
            Trace.logDebug("fuse to avail - complete.");
            break;
          }
        default:
          {
            Trace.logError("Invalid comp style.");
            break fusCycleLoop;
          }
      }
      Trace.logDebug(
          "****** completed containers["
              + completedCompContainers.size()
              + "] after fusCycle["
              + fusCycleNum
              + "]");
      int numInterConns = 0;
      for (ComposedContainer cont : completedCompContainers) {
        Trace.logDebug("***** Container: " + cont.getAbstract());
        numInterConns += cont.numOfInterContainerConnections();
      }
      Trace.logDebug(
          "FusCycle_Results["
              + fusCycleNum
              + "] numConts["
              + completedCompContainers.size()
              + "] numInterConns["
              + numInterConns
              + "]");
      fusCycleNum++;
      bestContainerCollection =
          selectBestContainerAssignments(
              bestContainerCollection, completedCompContainers, inst, fixedTargetNumOfContainers);

      if (pastCurfew()) {
        break;
      }
    }

    // bestContainerCollection contains the results.

    // create container specs...
    int actualNumOfContainers = bestContainerCollection.size();
    Trace.logDebug("Total PE Count[" + actualNumOfContainers + "] ");
    Trace.logDebug("<<<<<<<<<<< numOfContainersReturned[" + actualNumOfContainers + "]");
    if ((fixedTargetNumOfContainers != null)
        && fixedTargetNumOfContainers != actualNumOfContainers) {
      Trace.logWarn(
          "Num of containers not able to be achieved.  Target["
              + fixedTargetNumOfContainers
              + "] Actual["
              + actualNumOfContainers
              + "]");
      SupportFunctions.productLog(
          StreamsRuntimeMessagesKey.Key.SCHUnattainableFusionCount,
          SupportFunctions.SCH_COMPUTE_PLACEMENT_FOR_JOB,
          fixedTargetNumOfContainers,
          actualNumOfContainers);
    }
    Set<ContainerSpecification> containerSpecs = new HashSet<>();
    for (ComposedContainer compCont : bestContainerCollection) {
      containerSpecs.add(new ContainerSpecification(compCont));
      Trace.logDebug("      Container: " + compCont.toString());
    }

    Trace.logDebug("$_$_$_cmp_af");
    for (ComposedContainer cont : bestContainerCollection) {
      Trace.logDebug("***** Container: " + cont.getAbstract());
    }

    Trace.logDebug("complete set of containerSpecs: " + containerSpecs);
    Trace.logDebug("remove pre-composed containers: " + _precomposedContSpecs);
    containerSpecs.removeAll(_precomposedContSpecs);
    Trace.logDebug("only new containerSpecs: " + containerSpecs);
    return containerSpecs;
  }

  private boolean pastCurfew() {
    if (_fusionCurfew == null) {
      return false;
    }
    long currentTime_ms = System.currentTimeMillis();
    long elapsedTime_ms = currentTime_ms - _fusionPhaseBeginTime_ms;
    return (elapsedTime_ms > (_fusionCurfew * 1000));
  }

  public JSONArray getResourceRequestTruncationMessage() {
    return _resTruncatedMessages;
  }

  private List<ComposedContainer> performManualFusion(
      Integer fixedNumOfContainers, FusingCycle fusCycle) {
    List<ComposedContainer> completedCompContainers = new ArrayList<>();

    // determine of containers to target.
    int numOfCcs = 1;
    Trace.logDebug("initialized num of containers[" + numOfCcs + "]");
    if (fixedNumOfContainers != null) {
      numOfCcs = fixedNumOfContainers;
      Trace.logDebug("Inst: fixed num of containers[" + numOfCcs + "]");
    } else {
      if (fusCycle._numOfContainers != null) {
        numOfCcs = fusCycle._numOfContainers;
        Trace.logDebug("fusCycle: numOfContainers[" + numOfCcs + "]");
      }
    }
    Trace.logDebug("num of containers[" + numOfCcs + "]");
    if (numOfCcs > _topApp.getNodes().size()) {
      Trace.logDebug(
          "Adjusting numOfContainers from specifed["
              + numOfCcs
              + "] to numOfNodes["
              + _topApp.getNodes().size()
              + "]");
      numOfCcs = _topApp.getNodes().size();
    }

    // move isolation FUs to their own completed containers
    List<FusableUnit> nonPlacedFus = new ArrayList<>(_fusUnits);

    Trace.logDebug(FC_PREFIX + "Manual: numOfCcs[" + numOfCcs + "]");

    // populate containers with FUs
    completedCompContainers.addAll(
        populateContainers(nonPlacedFus, fusCycle, numOfCcs, _precomposedContainers));

    return completedCompContainers;
  }

  // preprocess the precomposed containers
  //    - create initial containers
  //    - extract FUs included in these containers from the FUs remaining to be composed.
  private PreComposedInfo processPreComposedContainers(
      Set<ComposedContainer> precomposedContainers, List<FusableUnit> hSpec_fus) {
    Set<FusableUnit> preplacedFus = new HashSet<>();
    Set<ComposedContainer> preplacedCompContainers = new HashSet<>();
    for (FusableUnit fu : hSpec_fus) {
      for (ComposedContainer cont : precomposedContainers) {
        if (cont.contains(fu)) {
          preplacedFus.add(fu);
          preplacedCompContainers.add(cont);
          break;
        }
      }
    }
    return new PreComposedInfo(preplacedCompContainers, preplacedFus);
  }

  // input:
  //    - hSpec - (null = all resources considered)
  //    - num of containers  (null = no fixed number)
  //    - min num of ops per container
  //    - fus to be placed into containers
  //    - fusing rules
  //    - pre-composed containers, and related pre-composed fu's
  // output:
  //    - new composed containers (i.e. not including precomposed containers)
  private Set<ComposedContainer> populateContainers(
      List<FusableUnit> hSpec_fus,
      FusingCycle fusCycle,
      Integer fixedNumOfContainers,
      Set<ComposedContainer> nonTargetContainers) {
    Trace.logDebug(
        FC_PREFIX
            + "      populateContainers nonPlacedFus: \n"
            + hSpec_fus.toString()
            + " fusCycle: "
            + fusCycle.toString());
    Set<ComposedContainer> completedCompContainers;
    Set<String> tagSet = new HashSet<>();
    boolean numOfContainersFixed = (fixedNumOfContainers != null);
    PreComposedInfo hSpec_nonTargetCompInfo =
        processPreComposedContainers(nonTargetContainers, hSpec_fus);

    Set<FusableUnit> targetFus = new HashSet<>(hSpec_fus);
    targetFus.removeAll(hSpec_nonTargetCompInfo._nonTargetFus);

    ContainerSpecInfo peContInfo = determineNumContainersPerHostSpec(fixedNumOfContainers);
    int predictedNumOfContainersRemaining = peContInfo._numOfContainers;
    int minNumOfOpsPerContainer = peContInfo._minNumOfOpsPerContainer;
    //        $upo$ target ops & res's
    Trace.logDebug(
        "NumPreplacedConts[" + hSpec_nonTargetCompInfo._nonTargetComposedContainers.size() + "]");

    Trace.logDebug(
        "Preplaced containers{" + hSpec_nonTargetCompInfo._nonTargetComposedContainers + "}");
    Trace.logDebug("NumOfTargetFus[" + targetFus.size() + "]");
    Trace.logDebug(
        "Initial fus to be placed{"
            + hSpec_fus
            + "} \n Remaining fus to be placed{"
            + targetFus
            + "}");

    clearAugContCnt();

    // true initial value disables spillOver function.
    //    spillOver may not be functional anymore.  check b4 enabling.
    boolean spillOver = false;
    completedCompContainers = new HashSet<>(hSpec_nonTargetCompInfo._nonTargetComposedContainers);
    List<FusableUnit> remainingTargetFusToBePlaced = new ArrayList<>(targetFus);

    Set<FusableUnit> isolatedFuConts = new HashSet<>();
    int numIsolatedContainers = 0;
    Trace.logDebug("move isolation FUs to their own containers.");
    for (FusableUnit fu : remainingTargetFusToBePlaced) {
      if (fu.isPartitionIsolated() || fu.isPartitionSoftIsolated()) {
        Trace.logDebug("isolated FU found[" + fu.getAbstract() + "]");
        ComposedContainer targetCont = new ComposedContainer(this);
        targetCont.addFusableUnit(fu);
        completedCompContainers.add(targetCont);
        numIsolatedContainers++;
        isolatedFuConts.add(fu);
      }
    }
    remainingTargetFusToBePlaced.removeAll(isolatedFuConts);

    // setup available containers.
    Set<ComposedContainer> availCompContainers = new HashSet<>();
    predictedNumOfContainersRemaining -= numIsolatedContainers;
    if (predictedNumOfContainersRemaining < 0) {
      Trace.logDebug(
          "predictedNumOfConts["
              + predictedNumOfContainersRemaining
              + "] numIsolatedConts["
              + numIsolatedContainers
              + "]");
      predictedNumOfContainersRemaining = 0;
    }
    for (int i = 0; i < (predictedNumOfContainersRemaining); i++) {
      availCompContainers.add(new ComposedContainer(this));
    }

    // continue until all FUs have been placed into a container
    Trace.logDebug("place all remaining FUs to best containers.");
    FusableUnitComparator fuComp = new FusableUnitComparator(fusCycle);
    ComposedContainer targetCont = null;
    boolean chainInProgress = false;
    // minimize chance of empty containers in manual scheme, go into shortcut mode.
    while (remainingTargetFusToBePlaced.size() > 0) {
      if (numOfContainersFixed) {
        // detect if running out of operators to place in empty containers.
        // if so, break the chain.
        if (remainingTargetFusToBePlaced.size() <= availCompContainers.size()) {
          for (ComposedContainer cont : availCompContainers) {
            if (cont.isEmpty()) {
              // break the chain.
              targetCont = null;
              break;
            }
          }
          Trace.logDebug(
              "short cut mode. remainingFus["
                  + remainingTargetFusToBePlaced.size()
                  + "] availConts["
                  + availCompContainers.size()
                  + "]");
        }
      }
      Trace.logDebug("===== remaining FUs = [" + remainingTargetFusToBePlaced.size() + "]");
      if (targetCont != null && (fuComp.anySecondaryCompareRulesSpecified())) {
        Trace.logDebug("chain continuation");
        // secondary compare
        chainInProgress = true;
        fuComp.setPrimaryCompare(false);
      } else {
        // primary compare
        Trace.logDebug("head of chain.");
        targetCont = selectBestContainer(fusCycle, availCompContainers);
        fuComp.setPrimaryCompare(true);
      }
      Trace.logDebug(
          "      BestCont"
              + ((chainInProgress) ? "*CH" : "")
              + "["
              + targetCont.getDisplayName()
              + "] remainingFus["
              + remainingTargetFusToBePlaced.size()
              + "]");
      // disabled dynamic updating of target num of ops.  didn't behave well at end of cycle.
      int targetNumOfOpsPerContainer =
          determineNumOfOpsPerContainer(
              remainingTargetFusToBePlaced, availCompContainers, minNumOfOpsPerContainer);
      Trace.logDebug(
          "targetNumOfOpsCont["
              + targetNumOfOpsPerContainer
              + "] numNonPlacedFus["
              + remainingTargetFusToBePlaced.size()
              + "] availCompCont["
              + availCompContainers.size()
              + "] minNumOfOpsCont["
              + minNumOfOpsPerContainer
              + "]");
      targetCont.setNumberOfTargetOperators(targetNumOfOpsPerContainer);
      // find best FU for container
      FusableUnit fusUnit =
          selectBestFuForContainer(targetCont, remainingTargetFusToBePlaced, fuComp);
      if (fusUnit != null) {
        // FU found
        Trace.logDebug("acceptable FU[" + fusUnit.getAbstract() + "] found for container");
        Trace.logDebug("before adding FU to container => " + targetCont.getAbstract());
        targetCont.addFusableUnit(fusUnit);
        Trace.logDebug(
            "after adding Fu[" + fusUnit.getId() + "] to Container: " + targetCont.getAbstract());
        remainingTargetFusToBePlaced.remove(fusUnit);
        Trace.logDebug("      BestFu_found" + "[" + fusUnit.getId() + "] ");
        if (targetCont.full() /* || shortCut*/) {
          Trace.logDebug("     Container=FULL:" + targetCont.getDisplayName());
          if ((fixedNumOfContainers == null) /* || shortCut*/) {
            Trace.logDebug("full container[" + targetCont.getId() + "] detected.. completing it.");
            promoteContainerToCompleted(targetCont, availCompContainers, completedCompContainers);
          }
          chainInProgress = false;
          targetCont = null;
        }
      } else {
        // no FU found
        Trace.logDebug("no FU found.");
        if (!chainInProgress) {
          Trace.logDebug(
              "not in chain continuation.  so completing container[" + targetCont.getId() + "]");
          promoteContainerToCompleted(targetCont, availCompContainers, completedCompContainers);
        }
        Trace.logDebug("      BestFu_found[NONE]" + ((!chainInProgress) ? "=Completed" : ""));
        chainInProgress = false;
        targetCont = null;
      }
    }

    // promote remaining containers to completed
    int releasedContainers =
        promoteAllRemainingContainersToCompleted(availCompContainers, completedCompContainers);
    int numOfNewContainers =
        completedCompContainers.size()
            - hSpec_nonTargetCompInfo._nonTargetComposedContainers.size();

    Trace.logDebug(
        "ComposedPE Summary: HSpec["
            + null
            + "] TotalPEs["
            + completedCompContainers.size()
            + "] NewPEs["
            + numOfNewContainers
            + "] InitPeCnt["
            + peContInfo._numOfContainers
            + "] AugPeCont["
            + getAugContCnt()
            + "] RlsdPeCont["
            + releasedContainers
            + "] ");

    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("   containers: ");
      StringBuilder str = new StringBuilder();
      str.append("\n");
      for (ComposedContainer compCont : completedCompContainers) {
        str.append("  " + compCont.getAbstract());
      }
      Trace.logDebug(str.toString() + "\n");
    }

    Trace.logDebug("populate containers end.");
    return completedCompContainers;
  }

  // determine how many PEs per hostSpec  (derived from num of resources & FUs)
  //   - also, min num of procs per host
  // special handling
  //    - declared hosts
  //    - indexed w/in hostpool
  private ContainerSpecInfo determineNumContainersPerHostSpec(Integer fixedNumPes) {
    return new ContainerSpecInfo(fixedNumPes, 1);
  }

  private int determineNumOfOpsPerContainer(
      List<FusableUnit> nonPlacedFus,
      Set<ComposedContainer> availCompContainers,
      int minNumOfOpsPerContainer) {
    if (availCompContainers.size() == 0) {
      return minNumOfOpsPerContainer;
    }
    int numOfRemainingOps = 0;
    for (FusableUnit fu : nonPlacedFus) {
      numOfRemainingOps += fu.getCompositeCcNodes().size();
    }
    int numOfPendingCompletionOps = 0;
    int numOfNonFullContainers = 0;
    for (ComposedContainer cont : availCompContainers) {
      if (!cont.full()) {
        numOfPendingCompletionOps += cont.getNumOfFusedNodes();
        numOfNonFullContainers++;
      }
      ;
    }
    int numOfOpsPerCont =
        roundUp(
            (numOfRemainingOps + numOfPendingCompletionOps),
            ((numOfNonFullContainers != 0) ? numOfNonFullContainers : 1));
    if (minNumOfOpsPerContainer > numOfOpsPerCont) {
      numOfOpsPerCont = minNumOfOpsPerContainer;
    }
    return (numOfOpsPerCont);
  }

  private List<ComposedContainer> selectBestContainerAssignments(
      List<ComposedContainer> bestContainerCollection,
      List<ComposedContainer> challengerCompContainers,
      ComposingInstructions inst,
      Integer fixedTargetNumOfContainers)
      throws InvalidPlacementStrategyException {
    FusingCycleComparator comp =
        new FusingCycleComparator(inst.getFusingCycleCompareVector(), fixedTargetNumOfContainers);
    List<ComposedContainer> selectedColl = bestContainerCollection;
    boolean newChamp = false;
    if (bestContainerCollection.size() == 0) {
      selectedColl = challengerCompContainers;
      newChamp = true;
    } else {
      Integer result = comp.compare(bestContainerCollection, challengerCompContainers);
      if (result != null) {
        if (result > 0) {
          selectedColl = challengerCompContainers;
          newChamp = true;
        }
      }
    }
    Trace.logDebug("      " + (newChamp ? "Champ=new" : "Champ=same"));
    return selectedColl;
  }

  private void promoteContainerToCompleted(
      ComposedContainer cont,
      Set<ComposedContainer> availCompContainers,
      Set<ComposedContainer> completedCompContainers) {
    completedCompContainers.add(cont);
    availCompContainers.remove(cont);
  }

  private int promoteAllRemainingContainersToCompleted(
      Set<ComposedContainer> availCompContainers, Set<ComposedContainer> completedCompContainers) {
    int releasedContainers = 0;
    for (ComposedContainer cont : availCompContainers) {
      if (!cont.isEmpty()) {
        completedCompContainers.add(cont);
      } else {
        releasedContainers++;
      }
    }
    availCompContainers.clear();
    return releasedContainers;
  }

  private FusableUnit selectBestFuForContainer(
      ComposedContainer cont, List<FusableUnit> fus, FusableUnitComparator fuComp) {
    Trace.logDebug("select best FU for container.");
    FusableUnit champFu = null;
    List<MessageElementType> problemMessages = null;

    // init comparator to this container
    fuComp.setContainer(cont);
    fuComp.clearJrnlMsg();
    FusableUnitComparator.addJrnlMsg("FusJrnl:");

    // sort fus according to best match with container.
    Collections.sort(fus, fuComp);
    Trace.logDebug("======== Sorted FUs: ============ " + "\n" + fus.toString());

    for (FusableUnit fu : fus) {
      // through priority order, look for first one that is compatible.
      problemMessages = new ArrayList<>();
      if (FusingConstraintChecker._internalDebugMessages) {
        Trace.logDebug(
            "FCR_CC compare fu[" + fu.getAbstract() + "] against cc[" + cont.getAbstract() + "]");
        Trace.logDebug("--- FU operator constraints for: " + fu.getAbstract());
        for (ComposerCriteriaNode ccNode : fu.getCompositeCcNodes()) {
          Trace.logDebug(
              ComposerSupportHelper.convertMessageToText(
                  ccNode.getCurrentConstraintMessage(null, false)));
        }
        Trace.logDebug("--- container constraints for: " + cont.getAbstract());
        for (ComposerCriteriaNode ccNode : cont.getCompositeCcNodes()) {
          Trace.logDebug(
              ComposerSupportHelper.convertMessageToText(
                  ccNode.getCurrentConstraintMessage(null, false)));
        }
      }

      // check if compatible with chain
      if (fuComp.compatibleWithChain(fu, cont)) {
        // check if constraints are compatible
        Set<ComposerCriteriaNode> hostColocatedNodes = new HashSet<>();
        FusingConstraintChecker fcc = new FusingConstraintChecker(fu, cont, this);
        if (fcc.checkForIncompatibleConstraints(
                false, false, true, problemMessages, hostColocatedNodes)
            == null) {
          champFu = fu;
          break;
        } else {
          Trace.logDebug("         FU" + fu.getId() + " =Rejected, not constraint compatible");
          for (String textMsg : ComposerSupportHelper.convertMessagesToText(problemMessages)) {
            Trace.logDebug(textMsg);
          }
          continue;
        }
      } else {
        Trace.logDebug("   FU" + fu.getId() + " =Rejected, not chain compatible");
        continue;
      }
    }

    //		Trace.logDebug("BestFU[] from Journal= " + _fuComp.getJrnlMsg());
    return champFu;
  }

  private void clearAugContCnt() {
    _numOfAugmentedContainers = 0;
  }

  private void incAugContCnt() {
    _numOfAugmentedContainers++;
  }

  private int getAugContCnt() {
    return _numOfAugmentedContainers;
  }

  private ComposedContainer createNewContainer(Set<ComposedContainer> availCompContainers) {
    ComposedContainer newCont = new ComposedContainer(this);
    availCompContainers.add(newCont);
    incAugContCnt();
    return (newCont);
  }

  private ComposedContainer selectBestContainer(
      FusingCycle fusCycle, Set<ComposedContainer> out_availCompContainers) {
    //		Trace.logDebug("===== Select best container from availContainers["+availCompContainers+"]");
    List<ComposedContainer> orderedAvailCompContainers = new ArrayList<>(out_availCompContainers);
    ComposedContainer bestContainer = null;
    if (orderedAvailCompContainers.size() > 0) {
      ComposerContainerComparator ccComp =
          new ComposerContainerComparator(fusCycle._containerCompareVector);
      Collections.sort(orderedAvailCompContainers, ccComp);
      Trace.logDebug("Sorted: availCompContainers: " + orderedAvailCompContainers.toString());
      bestContainer = orderedAvailCompContainers.get(0);
    } else {
      Trace.logDebug("No avail containers, so create new one.");
      bestContainer = createNewContainer(out_availCompContainers);
    }
    // _availCompContainers.remove(bestContainer);
    Trace.logDebug("===== best container = " + bestContainer.toString() + "\n");
    return (bestContainer);
  }

  private void validateFusableUnits() throws StreamsException {
    AdditionalExceptionDataType jobAedt = new AdditionalExceptionDataType();
    boolean problemFound = false;
    for (FusableUnit fusUnit : _fusUnits) {
      AdditionalExceptionDataType peAedt = fusUnit.validate(true);
      if (peAedt != null) {
        jobAedt.getNestedAdditionalExceptionData().add(peAedt);
        problemFound = true;
      }
    }
    if (problemFound) {
      throw new IncompatibleFusingConstraintsException(jobAedt);
    }
  }

  FusableUnit getFuForTopNode(TopologyNode tNode) {
    return (_topNodeToFuMap.get(tNode));
  }

  private void buildTopNodeToFuMap() {
    for (FusableUnit fu : _fusUnits) {
      for (TopologyNode node : fu.getTopologyNodes()) {
        _topNodeToFuMap.put(node, fu);
      }
    }
  }

  private void buildComposerCriteriaNodeGraph() {
    _compCriteriaNodes = new HashMap<>();
    for (TopologyNode node : _topApp.getNodes()) {
      if (node == null) {
        Trace.logError("!!!!!!!!!!!!!!!!! null node found !!!!!!!!!!!!!!!!!!!");
      }
      ComposerCriteriaNode ccNode = new ComposerCriteriaNode(node, this);
      _compCriteriaNodes.put(ccNode.getNodeIndex(), ccNode);
      _locGrpMgr.addCcNodeToAssociatedOperatorHostLocationGroups(ccNode);
    }
    for (ComposerCriteriaNode ccn : _compCriteriaNodes.values()) {
      ccn.setCpuUsage(5);
    }
    int pCg_id = 0;
    // synthesize partition co-locates for hostIsol/co-locate nodes.
    for (ComposerCriteriaNode ccN : _compCriteriaNodes.values()) {
      if (ccN.isHostIsolation()) {
        for (ComposerCriteriaNode ccN_hcg : _locGrpMgr.getCgCoMembers(ccN)) {
          ccN_hcg.addPartitionColocationGroupId(
              ComposerCriteriaNode.INFERRED_COLOCATION_PARTITION + pCg_id);
        }
        pCg_id++;
      }
    }
    for (ComposerCriteriaNode ccNode : _compCriteriaNodes.values()) {
      _locGrpMgr.addNodeToAssociatedPartitionLocationGroups(ccNode);
    }
  }

  private ComposerCriteriaNode getCcNode(BigInteger index) {
    return _compCriteriaNodes.get(index);
  }

  private Collection<ComposerCriteriaNode> getCcNodes() {
    return (_compCriteriaNodes.values());
  }

  /// Build fusable Units from nodes in topology
  private void buildFusableUnits(boolean fuseToOne) {
    Set<BigInteger> alreadyAcctdFor = new HashSet<>();
    if (fuseToOne) {
      FusableUnit fu = new FusableUnit(this);
      fu.addCriteriaNodes(getCcNodes());
      _fusUnits.add(fu);
    } else {
      for (TopologyNode node : _topApp.getNodes()) {

        if (!alreadyAcctdFor.contains(node.getIndex())) {
          // not already accounted for, then ...
          FusableUnit newFu = new FusableUnit(this);
          _fusUnits.add(newFu);
          // add CCN to FU  (this will also add all co-located CCNs as well)
          Set<BigInteger> newAcctdNodes = newFu.addCriteriaNode(getCcNode(node.getIndex()));
          alreadyAcctdFor.addAll(newAcctdNodes);
        } else {
          Trace.logDebug(" node[" + node.getIndex() + "] already acctd for, skipping.");
        }
      }
    }
    buildTopNodeToFuMap();
    for (FusableUnit fu : _fusUnits) {
      fu.populateConnections();
      if (fu.speciallyFused()) {
        fu.setPartitionSoftIsolated(true);
      }
    }
    Trace.logDebug("freshly built fusable units: ");
    for (FusableUnit fu : _fusUnits) {
      Trace.logDebug(fu.toString());
    }
  }

  SCHPartitionColocationGroup getColocatedPartitionGroup(String id) {
    return (_locGrpMgr.getPartitionColocGroup(id));
  }

  long getNextFusableUnitId() {
    return (_nextFuId++);
  }

  long getNextContainerId() {
    return (_nextContainerId++);
  }

  long getNextCritNodeId() {
    return (_nextCritNode++);
  }

  Set<BigInteger> getExclusiveHostPools() {
    Set<BigInteger> hpIndexes = new HashSet<>();
    for (Hostpool hostPool : _topApp.getHostpools().values()) {
      if (hostPool.getMembershipMode() == MembershipMode.exclusive) {
        hpIndexes.add(hostPool.getIndex());
      }
    }
    return hpIndexes;
  }

  Hostpool getHostPool(BigInteger poolIndex) {
    return _topApp.getHostpools().get(poolIndex);
  }

  public TopologyApplication getApplication() {
    return (_topApp);
  }

  public void buildComposerModel(boolean standAloneMode) {
    buildComposerCriteriaNodeGraph();
    buildFusableUnits(standAloneMode);
    buildPrecomposedContainers();
  }

  private void buildPrecomposedContainers() {
    for (ContainerSpecification cSpec : _precomposedContSpecs) {
      _precomposedContainers.add(new ComposedContainer(this, cSpec));
    }
  }

  public void setComposerConfig(ComposerConfig compCfg) throws StreamsException {
    _peToCoreRatio = compCfg._peToCoreRatio;
    _minOpsPerPe = compCfg._minOpsPerPe;
    _fusionCurfew = compCfg._fusionCurfew;
    _proposedOpsPerResource = compCfg._proposedOpsPerResource;
    if (compCfg._jobResourceSharing != null) {
      _jobResourceSharing = compCfg._jobResourceSharing;
    } else {
      _jobResourceSharing = JobResourceSharing.SAME_INSTANCE;
    }
    _jobSubmission = compCfg._jobSubmission;
  }

  Set<ComposerCriteriaNode> expandHostColocatedNodes(Set<ComposerCriteriaNode> origNodes) {
    Set<ComposerCriteriaNode> expandedSet = new HashSet<>(origNodes);
    Set<Set<ComposerCriteriaNode>> cohabitationSets = new HashSet<>();
    for (Set<ComposerCriteriaNode> dGroup : getHostColocationGroups()) {
      for (ComposerCriteriaNode ccN : dGroup) {
        cohabitationSets.add(ccN.getFusingSets());
      }
    }
    Set<Set<ComposerCriteriaNode>> workingDefinedGroups = new HashSet<>(getHostColocationGroups());
    workingDefinedGroups.addAll(cohabitationSets);
    Set<Set<ComposerCriteriaNode>> intersectSets = new HashSet<>();
    do {
      intersectSets.clear();
      for (Set<ComposerCriteriaNode> definedGroup : workingDefinedGroups) {
        Set<ComposerCriteriaNode> intSet = (new HashSet<>(expandedSet));
        intSet.retainAll(definedGroup);
        if (intSet.size() > 0) {
          // intersection exists.
          intersectSets.add(definedGroup);
        }
      }
      for (Set<ComposerCriteriaNode> nSet : intersectSets) {
        expandedSet.addAll(nSet);
      }
      workingDefinedGroups.removeAll(intersectSets);
    } while (intersectSets.size() > 0);

    return expandedSet;
  }

  public void setResourcesToRequest(
      Map<BigInteger, Integer> resToRequest,
      List<ResourceRequest> additionalResToRequest,
      Set<String> resourceNamesToBeRemoved) {
    _resToRequest = resToRequest;
    _additionalResToRequest = additionalResToRequest;
    _resToRemove = resourceNamesToBeRemoved;
  }

  private Collection<Set<ComposerCriteriaNode>> getHostColocationGroups() {
    return _locGrpMgr.getColocOpGroups();
  }

  public Set<ResourceAcquistionMessage> getResourceAcquistionMessages() {
    return _resAcquistionMessages;
  }

  public Integer getProposedOpsPerResource() {
    if (_proposedOpsPerResource != null) {
      return _proposedOpsPerResource;
    } else {
      return OPS_PER_RESOURCE;
    }
  }

  private String getConfigSettings() {
    return "peToCore["
        + _peToCoreRatio
        + "] minOpsPerPe["
        + _minOpsPerPe
        + "] resToRequest["
        + _resToRequest
        + "] additionalResToRequest["
        + _additionalResToRequest
        + "] opsPerRes["
        + _proposedOpsPerResource
        + "] jResSharingEnabled["
        + _jobResourceSharing
        + "] jobSubmit["
        + _jobSubmission
        + "] precomposedContSpes{"
        + _precomposedContSpecs
        + "}";
  }

  private String dumpConfig() {
    return "Composer.config: minOpsPerPe["
        + _minOpsPerPe
        + "] peToCore["
        + _peToCoreRatio
        + "] proposedOpsPerRes["
        + _proposedOpsPerResource
        + "] jobSub["
        + _jobSubmission
        + "] resToRqst["
        + _resToRequest
        + "] adtlnRes["
        + _additionalResToRequest
        + "] removeRes["
        + _resToRemove
        + "] \npreComposed["
        + _precomposedContSpecs
        + "]\nfus["
        + _fusUnits
        + "]";
  }
}
