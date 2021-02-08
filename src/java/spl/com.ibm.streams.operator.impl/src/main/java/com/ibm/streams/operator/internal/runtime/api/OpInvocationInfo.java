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

package com.ibm.streams.operator.internal.runtime.api;

import com.ibm.streams.operator.internal.pemodel.PEModel;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.streams.platform.services.PhysParallelRegionType;
import com.ibm.streams.platform.services.PhysParallelRegionsType;
import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/** Describes the basic information for an operator invocation within this PE. */
class OpInvocationInfo {

  /** Operator's name. */
  private final String operatorName;

  /** Operator's logical name. */
  private final String operatorLogicalName;

  /** SPL kind of the operator */
  private final String kind;

  /** Index of the Operator within the PE. */
  private final int index;

  /** Global indexes of the parallel region channels this operator is in. */
  private final List<Integer> globalChannels;

  /**
   * Total number of channels for the parallel regions that this operator is in. This includes
   * replication for nested regions.
   */
  private final List<Integer> globalMaxChannels;

  /** Indexes of the local parallel region channels this operator is in. */
  private final List<Integer> localChannels;

  /** Total number of channels for the parallel region that this operator is in. */
  private final List<Integer> localMaxChannels;

  /** The absolute path of the runtime location of the toolkit containing this operator */
  private final File toolkitDirectory;

  private static final List<Integer> NO_UDP_CHANNEL = Collections.singletonList(-1);
  private static final List<Integer> NO_UDP_MAX_CHANNEL = Collections.singletonList(0);

  OpInvocationInfo(final PEModel model, final int operatorIndex) {
    final AugmentedPEType peType = model.getPEType();
    final PhysOperInstanceType op = model.getOperatorInstance(operatorIndex);
    index = operatorIndex;
    operatorName = op.getName();
    operatorLogicalName = op.getLogicalName();
    PhysParallelRegionsType prs = op.getParallelRegions();
    if (null != prs) {
      List<PhysParallelRegionType> pr = prs.getParallelRegion();
      List<Integer> lc = new ArrayList<>(pr.size());
      List<Integer> lmc = new ArrayList<>(pr.size());
      List<Integer> gc = new ArrayList<>(pr.size());
      List<Integer> gmc = new ArrayList<>(pr.size());

      for (int i = 0; i < pr.size(); ++i) {
        PhysParallelRegionType p = pr.get(i);
        lc.add(p.getLocalChannelIndex());
        gc.add(p.getGlobalChannelIndex());
        lmc.add(p.getLocalMaxChannels().intValue());
        gmc.add(p.getGlobalMaxChannels().intValue());
      }

      // Ensure we return unmodifiable lists
      localChannels = Collections.unmodifiableList(lc);
      localMaxChannels = Collections.unmodifiableList(lmc);
      globalChannels = localChannels.size() == 1 ? localChannels : Collections.unmodifiableList(gc);
      globalMaxChannels =
          localMaxChannels.size() == 1 ? localMaxChannels : Collections.unmodifiableList(gmc);

    } else {
      localChannels = NO_UDP_CHANNEL;
      globalChannels = NO_UDP_CHANNEL;
      localMaxChannels = NO_UDP_MAX_CHANNEL;
      globalMaxChannels = NO_UDP_MAX_CHANNEL;
    }

    kind = model.getOperatorDefinition(op).getKind();
    toolkitDirectory = model.getToolkitDirectory(op);
  }

  // TODO: Fix this ctor
  OpInvocationInfo(int index, String name, String logicalName, String kind) {
    this.index = index;
    this.operatorName = name;
    this.operatorLogicalName = logicalName;
    this.kind = kind;
    this.localChannels = NO_UDP_CHANNEL;
    this.globalChannels = NO_UDP_CHANNEL;
    this.localMaxChannels = NO_UDP_MAX_CHANNEL;
    this.globalMaxChannels = NO_UDP_MAX_CHANNEL;
    this.toolkitDirectory = new File(System.getProperty("user.dir"));
  }

  public final String getName() {
    return operatorName;
  }

  public final String getLogicalName() {
    return operatorLogicalName;
  }

  public final String getKind() {
    return kind;
  }

  public final int getIndex() {
    return index;
  }

  public final int getChannel() {
    return globalChannels.get(0);
  }

  public final List<Integer> getAllChannels() {
    return globalChannels;
  }

  public final int getMaxChannels() {
    return globalMaxChannels.get(0);
  }

  public final List<Integer> getAllMaxChannels() {
    return globalMaxChannels;
  }

  public final int getLocalChannel() {
    return localChannels.get(0);
  }

  public final int getLocalMaxChannels() {
    return localMaxChannels.get(0);
  }

  public final File getToolkitDirectory() {
    return toolkitDirectory;
  }
}
