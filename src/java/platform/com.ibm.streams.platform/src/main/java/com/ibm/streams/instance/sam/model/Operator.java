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
import com.ibm.streams.instance.sam.model.runtime.RuntimeOperator;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import java.math.BigInteger;
import java.util.Objects;

public class Operator extends SamObject {

  private OperatorId _operatorId;
  private RuntimeOperator _runNode;

  public Operator(BigInteger appId, BigInteger topologyNodeIndex, boolean isPreviewOnly) {
    this(new RuntimeOperator(appId, topologyNodeIndex), isPreviewOnly);
  }

  // for recovery
  public Operator(RuntimeOperator runNode, boolean isPreviewOnly) {
    super(isPreviewOnly);
    BigInteger appId = runNode.getAppId();
    BigInteger topologyNodeIndex = runNode.getIndex();
    TopologyNode topologyNode =
        SAMStaticContext.getTopologyTracker().getTopologyNode(appId, topologyNodeIndex);
    if (topologyNode == null) {
      throw new IllegalStateException(
          "TopologyNode appId=" + appId + " topologyNodeIndex=" + topologyNodeIndex + " is null");
    }

    _operatorId = new OperatorId(appId, topologyNodeIndex);
    _runNode = runNode;
    SAMStaticContext.getObjectTracker().registerOperator(this);
  }

  public BigInteger getAppId() {
    return _runNode.getAppId();
  }

  public BigInteger getIndex() {
    return _runNode.getIndex();
  }

  public BigInteger getLaunchCount() {
    return _runNode.getLaunchCount();
  }

  public void setLaunchCount(BigInteger launchCount) {
    _runNode.setLaunchCount(launchCount);
  }

  public OperatorId getOperatorId() {
    return _operatorId;
  }

  @Override
  public int hashCode() {
    return Objects.hash(_operatorId, _runNode);
  }

  @Override
  public boolean isJobConstructed() {
    Job job = SAMStaticContext.getObjectTracker().getJob(getAppId());
    if (job == null) {
      return false;
    }
    return job.isConstructed();
  }
}
