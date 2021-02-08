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

package com.ibm.streams.instance.sam.model.fuser;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.composer.ContainerSpecification;
import com.ibm.streams.instance.sam.model.topology.ParallelChannel;
import com.ibm.streams.instance.sam.model.topology.Splitter;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.Arrays;
import java.util.Collections;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.TreeSet;

public class FusedApplication {
  private SortedSet<BigInteger> containerIndexSet;

  private BigInteger appId;
  private BigInteger nextContainerIndex;
  private BigInteger id;

  public FusedApplication(TopologyApplication app, SortedSet<ContainerSpecification> specs) {
    appId = app.getId();
    // topology application id = fused application id = job id
    id = app.getId();
    SAMStaticContext.getFusedTracker().registerFusedApplication(id, this);
    nextContainerIndex = BigInteger.ZERO;
    containerIndexSet = new TreeSet<>();

    SortedSet<FusedContainer> containers = buildContainers(specs);
    addContainerIndexes(containers);
    buildContainerConnections(Collections.emptySet(), containers);
  }

  private SortedSet<FusedContainer> buildContainers(SortedSet<ContainerSpecification> specs) {
    SortedSet<FusedContainer> fusedContainers = new TreeSet<>();

    if (specs != null) {
      // build containers and ports
      for (ContainerSpecification contSpec : specs) {
        fusedContainers.add(new FusedContainer(id, contSpec, nextContainerIndex));
        nextContainerIndex = nextContainerIndex.add(BigInteger.ONE);
      }
    } // end if specs != null

    return fusedContainers;
  }

  private void buildContainerConnections(
      Set<FusedContainer> oldContainers, Set<FusedContainer> newContainers) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug(
          "ENTER FusedApplication.buildContainerConnections with "
              + Arrays.deepToString(new Object[] {oldContainers, newContainers}));
    }
    // Build FusedContainerConnections
    SortedMap<TopologyNode, FusedContainer> containerNodeMap =
        new TreeMap<TopologyNode, FusedContainer>();
    Set<TopologyNodeConnection> interContainerConns = new HashSet<>();

    for (FusedContainer fc : SAMStaticContext.getFusedTracker().getFusedContainers(id)) {
      // get registered containers
      // skip target containers to be removed during fission/fusion
      if (!oldContainers.contains(fc)) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("buildContainerConnections fc: " + fc);
        }
        for (BigInteger nodeIndex : fc.getNodeIndexes()) {
          TopologyNode tn = SAMStaticContext.getTopologyTracker().getTopologyNode(id, nodeIndex);
          containerNodeMap.put(tn, fc);
          if (Trace.isEnabled(Level.TRACE)) {
            Trace.logTrace("Added TopologyNode name=" + tn.getName() + ", index=" + nodeIndex);
          }
        }
      }
      if (newContainers.contains(fc)) {
        // get inter-container connections for new containers
        interContainerConns.addAll(fc.getInterContainerConnections());
      }
    }

    for (TopologyNodeConnection tnc : interContainerConns) {
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace(
            "Processing TopologyNodeConnection="
                + tnc.toString()
                + ", tracker="
                + tnc.getTopologyTracker());
      }

      FusedContainer srcContainer = containerNodeMap.get(tnc.getSource().getOwningOperator());
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Source container index=" + srcContainer.getIndex());
      }

      FusedContainer dstContainer = containerNodeMap.get(tnc.getDestination().getOwningOperator());
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Destination container index=" + dstContainer.getIndex());
      }

      FusedContainerInputPort fcip = dstContainer.getFusedContainerInputPort(tnc);
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Destination container input port index=" + fcip.getIndex());
      }

      // Is this connection associated with a splitter? If yes, find the output port through the
      // channel
      // this connection belongs to in that splitter. If no, then get the "normal" output port.
      FusedContainerOutputPort fcop = null;
      if (tnc.getSplitterIndex() != null) {
        TopologyNodeOutputPort tnop = tnc.getSource().getOutputPort();
        if (tnop == null) {
          Trace.logError("tnop is null for tnc=" + tnc);
        }
        Splitter splitter = tnop.getPortSplittersMap().get(tnc.getSplitterIndex());
        if (splitter == null) {
          Trace.logError(
              "splitter is null for splitterIndex=" + tnc.getSplitterIndex() + ", tnop=" + tnop);
        }
        ParallelChannel channel = splitter.getParallelChannels().get(tnc.getChannelIndex());
        if (channel == null) {
          Trace.logError(
              "channel is null for channelIndex=" + tnc.getChannelIndex() + ", tnop=" + tnop);
        }
        fcop = srcContainer.getFusedContainerOutputPort(channel);
      } else {
        fcop = srcContainer.getFusedContainerOutputPort(tnc.getSource());
      }
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("Source container output port index " + fcop.getIndex());
      }

      new FusedContainerConnection(tnc, fcop, fcip);
    } // end for each interContainerConn
  }

  private void addContainerIndexes(SortedSet<FusedContainer> containers) {
    for (FusedContainer container : containers) {
      addContainerIndex(container);
    }
  }

  public void addContainerIndex(FusedContainer container) {
    containerIndexSet.add(container.getIndex());
  }

  public void removeContainerIndex(FusedContainer fc) {
    containerIndexSet.remove(fc.getIndex());
  }

  public void clearContainerIndexes() {
    containerIndexSet.clear();
  }

  @Override
  public int hashCode() {
    return Objects.hash(containerIndexSet, appId, id);
  }

  public SortedSet<BigInteger> getContainers() {
    return Collections.unmodifiableSortedSet(containerIndexSet);
  }

  public BigInteger getApplicationId() {
    return id;
  }

  public TopologyApplication getTopologyApplication() {
    return SAMStaticContext.getTopologyTracker().getTopologyApplication(appId);
  }
}
