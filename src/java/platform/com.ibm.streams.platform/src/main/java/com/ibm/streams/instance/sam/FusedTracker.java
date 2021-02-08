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

import com.ibm.streams.instance.sam.model.fuser.FusedApplication;
import com.ibm.streams.instance.sam.model.fuser.FusedContainer;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnection;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerConnectionId;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerInputPort;
import com.ibm.streams.instance.sam.model.fuser.FusedContainerOutputPort;
import java.math.BigInteger;
import java.util.Set;
import java.util.SortedSet;

public interface FusedTracker {

  public FusedApplication getFusedApplication(BigInteger appId);

  public void registerFusedApplication(BigInteger appId, FusedApplication app);

  public void unregisterFusedApplication(BigInteger appId);

  public SortedSet<FusedContainer> getFusedContainers(BigInteger appId);

  public FusedContainer getFusedContainer(BigInteger appId, BigInteger containerIndex);

  public FusedContainer unregisterFusedContainer(
      BigInteger appId, BigInteger containerIndex, boolean removingJob);

  public void registerFusedContainer(BigInteger appId, FusedContainer container);

  public SortedSet<FusedContainerInputPort> getFusedContainerInputPorts(
      BigInteger appId, BigInteger containerIndex);

  public FusedContainerInputPort getFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex);

  public FusedContainerInputPort unregisterFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex, boolean removingJob);

  public void registerFusedContainerInputPort(
      BigInteger appId, BigInteger containerIndex, FusedContainerInputPort port);

  public SortedSet<FusedContainerOutputPort> getFusedContainerOutputPorts(
      BigInteger appId, BigInteger containerIndex);

  public FusedContainerOutputPort getFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex);

  public FusedContainerOutputPort unregisterFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, BigInteger portIndex, boolean removingJob);

  public void registerFusedContainerOutputPort(
      BigInteger appId, BigInteger containerIndex, FusedContainerOutputPort port);

  public Set<FusedContainerConnection> getFusedContainerConnections(BigInteger appId);

  public FusedContainerConnection getFusedContainerConnection(
      BigInteger appId, FusedContainerConnectionId connId);

  public FusedContainerConnection unregisterFusedContainerConnection(
      BigInteger appId, FusedContainerConnectionId connId);

  public void registerFusedContainerConnection(BigInteger appId, FusedContainerConnection conn);
}
