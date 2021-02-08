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

package com.ibm.streams.controller.instance.jobs;

import com.ibm.streams.control.internal.server.consistent.ConsistentRegion;
import com.ibm.streams.controller.instance.sam.InputPortLabel;
import com.ibm.streams.controller.instance.sam.Pipeline;
import com.ibm.streams.instance.sam.model.InputPort;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.OutputPort;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.instance.sam.model.topology.Hostpool;
import com.ibm.streams.instance.sam.model.topology.ParallelRegion;
import com.ibm.streams.instance.sam.model.topology.PoolLocation;
import java.math.BigInteger;
import java.util.List;
import java.util.Map;
import java.util.Set;
import lombok.Getter;

/*
 * The JobBridge returns JobModelContext instances back to the JobLauncher, and the
 * JobLauncher stores the JobModelContext objects to maintain the context of the
 * current job submission. This is a read-only object for consumers. We call a
 * bunch of SAM pipeline methods to inspect the job model up-front so that when
 * the JobLauncher needs these various quantities, they already exist and we
 * don't need to refer to the SAM pipeline again.
 */
public class JobModelContext {
  @Getter private final String applicationName;
  @Getter private final String applicationScope;
  /*
   * Hostpool index -> hostpool.
   */
  @Getter private final Map<BigInteger, Hostpool> hostPools;
  /*
   * PE ID -> list of input ports that are imports.
   */
  @Getter private final Map<BigInteger, List<InputPort>> imports;
  /*
   * PE ID -> list of input port labels.
   */
  @Getter private final Map<BigInteger, List<InputPortLabel>> portLabels;
  /*
   * PE ID -> list of output ports that are exports.
   */
  @Getter private final Map<BigInteger, List<OutputPort>> exports;
  /*
   * Parallel region index -> parallel region.
   */
  @Getter private final Map<BigInteger, ParallelRegion> parallelRegions;
  /*
   * PE ID -> AADL.
   */
  @Getter private final Map<BigInteger, String> aadls;
  /*
   * Consistent region index -> consistent region.
   */
  @Getter private final Map<Integer, ConsistentRegion> consistentRegions;
  /*
   * Parallel region alias -> parallel region width.
   */
  @Getter private final Map<String, BigInteger> parallelRegionWidths;
  /*
   * PE ID -> if it is restartable.
   */
  @Getter private final Map<BigInteger, Boolean> pesIsRestartable;
  /*
   * PE ID -> host placement; PE ID is not present in map if PE has no
   * host placement.
   */
  @Getter private final Map<BigInteger, String> pesHostPlacement;
  /*
   * PE ID -> host colocations
   */
  @Getter private final Map<BigInteger, Set<String>> pesHostCoLocations;
  /*
   * PE ID -> host exlocations
   */
  @Getter private final Map<BigInteger, Set<String>> pesHostExLocations;
  /*
   * PE ID -> host isolation; PE ID is not present in map if PE has no
   * host isolation.
   */
  @Getter private final Map<BigInteger, String> pesHostIsolation;
  /*
   * PE ID -> host isolation labels of PEs that this PE must be isolated from.
   */
  @Getter private final Map<BigInteger, Set<String>> pesHostIsolationLabels;
  /*
   * PE ID -> operator names.
   */
  @Getter private final Map<BigInteger, Set<String>> pesOperatorNames;
  /*
   * PE ID -> host pool location; PE ID is not present in map if PE has no
   * host isolation.
   */
  @Getter private final Map<BigInteger, PoolLocation> pesHostPoolLocation;

  public JobModelContext(Pipeline sam, BigInteger jobId, LogicalModel logicalModel, Job jobModel)
      throws Exception {
    this.applicationName = jobModel.getApplicationName();
    this.applicationScope = jobModel.getApplicationScope();
    /*
     * Make all of the SAM pipeline calls that inspect the job model.
     */
    this.aadls = sam.getAADLs(jobModel);
    this.consistentRegions = sam.createConsistentRegions(jobId, logicalModel, jobModel);
    this.exports = sam.getExports(jobModel);
    this.hostPools = sam.getHostPools(jobModel);
    this.imports = sam.getImports(jobModel);
    this.parallelRegions = sam.getParallelRegions(jobModel);
    this.parallelRegionWidths = sam.getParallelRegionWidths(jobModel);
    this.portLabels = sam.getPortLabels(jobModel);
    this.pesIsRestartable = sam.getPesIsRestartable(jobModel);
    this.pesHostPlacement = sam.getPesHostPlacement(jobModel);
    this.pesHostCoLocations = sam.getPesHostCoLocations(jobModel);
    this.pesHostExLocations = sam.getPesHostExLocations(jobModel);
    this.pesHostIsolation = sam.getPesHostIsolation(jobModel);
    this.pesHostIsolationLabels = sam.getPesHostIsolationLabels(jobModel);
    this.pesOperatorNames = sam.getPesOperatorNames(jobModel);
    this.pesHostPoolLocation = sam.getPesHostPoolLocation(jobModel, this.hostPools);
  }
}
