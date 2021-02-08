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

package com.ibm.streams.instance.sam;

import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import com.ibm.streams.instance.sam.model.topology.TopologyNode;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnection;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeConnectionId;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeInputPort;
import com.ibm.streams.instance.sam.model.topology.TopologyNodeOutputPort;
import java.math.BigInteger;
import java.util.Set;
import java.util.SortedSet;

public interface TopologyTracker {

  public BigInteger getLastTopologyApplicationId();

  public Set<BigInteger> getTopologyApplications();

  public TopologyApplication getTopologyApplication(BigInteger appId);

  public void registerTopologyApplication(BigInteger appId, TopologyApplication app);

  public void unregisterTopologyApplication(BigInteger appId);

  public SortedSet<TopologyNode> getTopologyNodes(BigInteger appId);

  public TopologyNode getTopologyNode(BigInteger appId, BigInteger nodeIndex);

  public TopologyNode getTopologyNode(BigInteger appId, String nodeName);

  public void registerTopologyNode(BigInteger appId, TopologyNode node);

  public void unregisterTopologyNode(BigInteger appId, BigInteger nodeIndex);

  public SortedSet<TopologyNodeInputPort> getTopologyNodeInputPorts(
      BigInteger appId, BigInteger nodeIndex);

  public TopologyNodeInputPort getTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex);

  public void registerTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, TopologyNodeInputPort port);

  public void unregisterTopologyNodeInputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex);

  public SortedSet<TopologyNodeOutputPort> getTopologyNodeOutputPorts(
      BigInteger appId, BigInteger nodeIndex);

  public TopologyNodeOutputPort getTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex);

  public void registerTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, TopologyNodeOutputPort port);

  public void unregisterTopologyNodeOutputPort(
      BigInteger appId, BigInteger nodeIndex, BigInteger portIndex);

  public Set<TopologyNodeConnectionId> getTopologyNodeConnectionIds(BigInteger appId);

  public Set<TopologyNodeConnection> getTopologyNodeConnections(BigInteger appId);

  public TopologyNodeConnection getTopologyNodeConnection(
      BigInteger appId, TopologyNodeConnectionId connId);

  public void registerTopologyNodeConnection(BigInteger appId, TopologyNodeConnection conn);

  public TopologyNodeConnection unregisterTopologyNodeConnection(
      BigInteger appId, TopologyNodeConnectionId connId);
}
