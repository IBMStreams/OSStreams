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

package com.ibm.streams.instance.sam.model;

import com.google.common.collect.HashMultimap;
import com.google.common.collect.ImmutableSet;
import com.google.common.collect.Multimap;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.ObjectTracker;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.SamUtilFunctions;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerInputPort;
import com.ibm.streams.instance.sam.model.runtime.RuntimePeInputPort;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.Transport;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Map;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.locks.ReentrantLock;

public class InputPort extends SamObject {

  private RuntimePeInputPort runPort;
  private FusedContainerInputPort fused;
  private Multimap<String, NameBasedImport> streamNameToNameBasedImportMap;
  private ReentrantLock lock;

  public InputPort(
      BigInteger jobId,
      BigInteger peId,
      BigInteger ipId,
      FusedContainerInputPort fused,
      boolean isPreviewOnly) {
    this(fused, new RuntimePeInputPort(jobId, peId, ipId, fused.getIndex()), false, isPreviewOnly);
  }

  public InputPort(
      FusedContainerInputPort fused,
      RuntimePeInputPort runPort,
      boolean recovery,
      boolean isPreviewOnly) {
    super(isPreviewOnly);
    this.runPort = runPort;
    this.fused = fused;
    this.lock = new ReentrantLock();
    if (!recovery) {
      ImportedStreams imports = fused.getTopologyNodeInputPort().getImports();
      if (imports != null) {
        ImportedStreams runImports = new ImportedStreams(imports);
        this.runPort.setImports(runImports);
      }
    }
    // else {
    //  Trace.logDebug("Topology ImportedStreams:" +
    // fused.getTopologyNodeInputPort().getImports().toString());
    //  Trace.logDebug("Runtime ImportedStreams:" + runPort.getImports().toString());
    // }
    SAMStaticContext.getObjectTracker().registerInputPort(this);
    streamNameToNameBasedImportMap = HashMultimap.create();
  }

  void setInboundStaticConnectionIds(boolean buildingJob) {
    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    Set<ConnectionId> fusedContainerConnectionIds = new HashSet<>();
    Job job = objectTracker.getJob(runPort.getJobId());
    for (FusedContainerConnectionId fccId : fused.getContainerConnections()) {
      BigInteger sourcePeId = job.getPeIdByFusedContainerIndex(fccId.getSourceContainerIndex());
      if (sourcePeId == null) {
        Trace.logError("sourcePeId is null for fccId=" + fccId);
        Trace.logError("setInboundStaticConnectionIds peIdToIndexMap: " + job.getPeIdToIndexMap());
      }
      Pe sourcePe = job.getPe(sourcePeId);
      BigInteger sourcePortId =
          sourcePe
              .getOutputPortByFusedContainerOutputPortIndex(fccId.getSourceContainerPortIndex())
              .getId();
      Pe thisPe = job.getPe(getPeId());
      ConnectionId connId = new ConnectionId(sourcePe, sourcePortId, thisPe, getId());
      // todo - delete buildingJob block once JobUtils.buildConnections() is complete
      // and ensures every InputPort static connection has a matching runtime connection
      if (!buildingJob) {
        if (!objectTracker.doesConnExist(connId)) {
          // create new pe connection
          new Connection(connId, fccId, isPreviewOnly());
        }
      }
      fusedContainerConnectionIds.add(connId);
    }
    runPort.setIncomingStaticConnectionIds(fusedContainerConnectionIds);
  }

  public ImmutableSet<ConnectionId> getIncomingStaticConnectionIds() {
    return runPort.getIncomingStaticConnectionIds();
  }

  public void setupDynConnections(BigInteger jobId, boolean isRecovery) {
    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Enter");
    }

    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    Job job = objectTracker.getJob(jobId);
    ImportedStreams importedStreams;
    if (isRecovery) {
      importedStreams = runPort.getImports();
    } else {
      TopologyNodeInputPort topNodeIP = fused.getTopologyNodeInputPort();
      importedStreams = topNodeIP.getImports();
      if (Trace.isEnabled(Level.TRACE)) {
        Trace.logTrace("TopologyNodeInputPort: " + topNodeIP.toString());
      }
    }

    if (importedStreams != null) {
      // property based import
      if (importedStreams.getPropertyBasedImport() != null) {
        if (Trace.isEnabled(Level.DEBUG)) {
          Trace.logDebug("Adding property based import for input port " + getId().toString());
        }
        String ascope = job.getApplicationScope();
        if (importedStreams.getPropertyBasedImport().getApplicationScope() != null) {
          ascope = importedStreams.getPropertyBasedImport().getApplicationScope();
        }
      }

      // name based import
      Map<BigInteger, NameBasedImport> nameBasedImports = importedStreams.getNameBasedImports();
      for (BigInteger index : nameBasedImports.keySet()) {
        NameBasedImport nbi = nameBasedImports.get(index);
        String sname =
            SamUtilFunctions.getStreamName(nbi.getStreamName(), nbi.getApplicationName());
        if (Trace.isEnabled(Level.DEBUG)) {
          Trace.logDebug(
              "Adding stream name based import for input port "
                  + getId().toString()
                  + ", stream= "
                  + sname);
        }
        streamNameToNameBasedImportMap.put(sname, nbi);
      }

      // filter
      if (importedStreams.getFilter() != null) {
        String filter = importedStreams.getFilter();
        if (Trace.isEnabled(Level.DEBUG)) {
          Trace.logDebug(
              "Added import stream filter for input port "
                  + getId().toString()
                  + ", filter= "
                  + filter);
        }
      }
    }

    if (Trace.isEnabled(Level.DEBUG)) {
      Trace.logDebug("Exit");
    }
  }

  public BigInteger getId() {
    return runPort.getId();
  }

  public BigInteger getPeId() {
    return runPort.getPeId();
  }

  public BigInteger getIndex() {
    return fused.getIndex();
  }

  public BigInteger getJobId() {
    return runPort.getJobId();
  }

  public ImportedStreams getImportedStreams() {
    // TopologyNodeInputPort topIP = fused.getTopologyNodeInputPort();
    // return topIP.getImports();
    return runPort.getImports();
  }

  public BigInteger getTupleTypeIndex() {
    return fused.getTopologyNodeInputPort().getTupleTypeIndex();
  }

  public Transport getTransport() {
    return fused.getTopologyNodeInputPort().getTransport();
  }

  public Encoding getEncoding() {
    return fused.getTopologyNodeInputPort().getEncoding();
  }

  public BigInteger getNodeIndex() {
    return fused.getTopologyNodeInputPort().getOwningOperator().getIndex();
  }

  public BigInteger getNodePortIndex() {
    return fused.getTopologyNodeInputPort().getIndex();
  }

  public String getApplicationScope() {
    Job job = SAMStaticContext.getObjectTracker().getJob(runPort.getJobId());
    return job.getApplicationScope();
  }

  @Override
  public boolean isJobConstructed() {
    Job job = SAMStaticContext.getObjectTracker().getJob(runPort.getJobId());
    if (job == null) {
      return false;
    }
    return job.isConstructed();
  }

  @Override
  public int hashCode() {
    return Objects.hash(runPort, fused);
  }
}
