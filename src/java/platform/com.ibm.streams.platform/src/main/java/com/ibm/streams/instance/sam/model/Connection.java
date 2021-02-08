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

import com.ibm.streams.instance.sam.SAMStaticContext;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.runtime.RuntimeConnection;
import java.math.BigInteger;
import java.util.Objects;

public class Connection extends SamObject {

  private RuntimeConnection run;
  private FusedContainerConnectionId fused;

  // caller must call objectAdded to persist and send notification
  public Connection(ConnectionId id, boolean isPreviewOnly) {
    this(id, null, isPreviewOnly);
  }

  // static
  public Connection(ConnectionId id, FusedContainerConnectionId fused, boolean isPreviewOnly) {
    this(fused, new RuntimeConnection(id, fused != null, false), isPreviewOnly);
  }

  public Connection(
      FusedContainerConnectionId fused, RuntimeConnection run, boolean isPreviewOnly) {
    super(isPreviewOnly);
    this.run = run;
    this.fused = fused;
    SAMStaticContext.getObjectTracker().registerConnection(this);
  }

  public FusedContainerConnectionId getFusedContainerConnectionId() {
    return fused;
  }

  public boolean isStatic() {
    return run.isStatic();
  }

  public ConnectionId getConnectionId() {
    return run.getId();
  }

  void setTimestamp(BigInteger timestamp) {
    run.setTimestamp(timestamp);
  }

  public BigInteger getTimestamp() {
    return run.getTimestamp();
  }

  @Override
  public boolean isJobConstructed() {
    ConnectionId connId = getConnectionId();
    BigInteger srcJobId =
        SAMStaticContext.getObjectTracker().getJobIdFromPeId(connId.getOutputPeId());
    BigInteger dstJobId =
        SAMStaticContext.getObjectTracker().getJobIdFromPeId(connId.getInputPeId());
    if (srcJobId == null || dstJobId == null) {
      return false;
    }
    Job srcJob = SAMStaticContext.getObjectTracker().getJob(srcJobId);
    Job dstJob = SAMStaticContext.getObjectTracker().getJob(dstJobId);
    if (srcJob == null || dstJob == null) {
      return false;
    }
    return srcJob.isConstructed() && dstJob.isConstructed();
  }

  @Override
  public int hashCode() {
    return Objects.hash(run, fused);
  }
}
