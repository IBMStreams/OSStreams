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

import com.google.common.collect.ImmutableSet;
import com.ibm.streams.instance.sam.ObjectTracker;
import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerOutputPort;
import com.ibm.streams.instance.sam.model.runtime.RuntimePeOutputPort;
import com.ibm.streams.instance.sam.model.topology.Encoding;
import com.ibm.streams.instance.sam.model.topology.ExportedStream;
import com.ibm.streams.instance.sam.model.topology.Transport;
import java.math.BigInteger;
import java.util.HashSet;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.locks.ReentrantLock;

public class OutputPort extends SamObject {

  private RuntimePeOutputPort runPort;
  private FusedContainerOutputPort fused;
  private ReentrantLock lock;
  private String nameBasedExportStreamName;

  public OutputPort(
      BigInteger jobId,
      BigInteger peId,
      BigInteger opId,
      FusedContainerOutputPort fused,
      boolean isPreviewOnly) {
    this(fused, new RuntimePeOutputPort(jobId, peId, opId, fused.getIndex()), false, isPreviewOnly);
  }

  public OutputPort(
      FusedContainerOutputPort fused,
      RuntimePeOutputPort runPort,
      boolean recovery,
      boolean isPreviewOnly) {
    super(isPreviewOnly);
    this.fused = fused;
    this.runPort = runPort;
    this.lock = new ReentrantLock();
    if (!recovery) {
      ExportedStream export = fused.getTopologyNodeOutputPort().getExport();
      if (export != null) {
        ExportedStream runExport = new ExportedStream(export);
        this.runPort.setExport(runExport);
      }
    }
    // else {
    //  Trace.logDebug("Topology ExportedStream:" +
    // fused.getTopologyNodeOutputPort().getExport().toString());
    //  Trace.logDebug("Runtime ExportedStream:" + runPort.getExport().toString());
    // }
    SAMStaticContext.getObjectTracker().registerOutputPort(this);
  }

  void registerOutboundStaticConnections() {
    ObjectTracker objectTracker = SAMStaticContext.getObjectTracker();
    Set<ConnectionId> fusedContainerConnectionIds = new HashSet<>();
    Job job = objectTracker.getJob(runPort.getJobId());
    for (FusedContainerConnectionId fccId : fused.getContainerConnections()) {
      BigInteger targetPeId =
          job.getPeIdByFusedContainerIndex(fccId.getDestinationContainerIndex());
      Pe targetPe = job.getPe(targetPeId);
      BigInteger targetPortId =
          targetPe.getInputPortIdByIndex(fccId.getDestinationContainerPortIndex());
      Pe thisPe = job.getPe(getPeId());
      ConnectionId connId = new ConnectionId(thisPe, getId(), targetPe, targetPortId);
      Connection conn = new Connection(connId, fccId, isPreviewOnly());
      fusedContainerConnectionIds.add(conn.getConnectionId());
    }
    runPort.setOutgoingStaticConnectionIds(fusedContainerConnectionIds);
  }

  public ImmutableSet<ConnectionId> getOutgoingStaticConnectionIds() {
    return runPort.getOutgoingStaticConnectionIds();
  }

  public void setupDynConnections(BigInteger jobId, boolean isRecovery) {}

  public BigInteger getJobId() {
    return runPort.getJobId();
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

  public BigInteger getTupleTypeIndex() {
    return fused.getTopologyNodeOutputPort().getTupleTypeIndex();
  }

  public BigInteger getNodeIndex() {
    return fused.getTopologyNodeOutputPort().getOwningOperator().getIndex();
  }

  public BigInteger getNodePortIndex() {
    return fused.getTopologyNodeOutputPort().getIndex();
  }

  public BigInteger getSplitterIndex() {
    return fused.getSplitterIndex();
  }

  public BigInteger getChannelIndex() {
    return fused.getChannelIndex();
  }

  public boolean isSingleThreadedOnOutput() {
    return fused.getTopologyNodeOutputPort().isSingleThreadedOnOutput();
  }

  public Transport getTransport() {
    return fused.getTopologyNodeOutputPort().getTransport();
  }

  public Encoding getEncoding() {
    return fused.getTopologyNodeOutputPort().getEncoding();
  }

  public ExportedStream getExportedStream() {
    // return fused.getTopologyNodeOutputPort().getExport();
    return runPort.getExport();
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
