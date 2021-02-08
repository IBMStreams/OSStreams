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

package com.ibm.streams.controller.state.capture;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;

import com.ibm.streams.controller.crds.jobs.JobStore;
import com.ibm.streams.controller.crds.prs.ParallelRegionStore;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapCache;
import com.ibm.streams.controller.k8s.configmaps.ConfigMapStore;
import com.ibm.streams.platform.services.AugmentedPEType;
import com.ibm.streams.platform.services.PhysOperInstanceType;
import com.ibm.xmlns.prod.streams.api.state.InputPortType;
import com.ibm.xmlns.prod.streams.api.state.Instance;
import com.ibm.xmlns.prod.streams.api.state.JobType;
import com.ibm.xmlns.prod.streams.api.state.OperatorType;
import com.ibm.xmlns.prod.streams.api.state.OutputPortType;
import com.ibm.xmlns.prod.streams.api.state.ParallelChannelType;
import com.ibm.xmlns.prod.streams.api.state.PeConnectionType;
import com.ibm.xmlns.prod.streams.api.state.PeType;
import java.math.BigInteger;
import java.util.Comparator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;
import java.util.stream.Collectors;
import lombok.var;

public class CaptureConverter {

  private final JobStore jobStore;
  private final ConfigMapStore configMapStore;
  private final ConfigMapCache configMapCache;
  private final ParallelRegionStore parallelRegionStore;

  public CaptureConverter(
      JobStore jobStore,
      ConfigMapStore configMapStore,
      ConfigMapCache configMapCache,
      ParallelRegionStore parallelRegionStore) {
    this.jobStore = jobStore;
    this.configMapStore = configMapStore;
    this.configMapCache = configMapCache;
    this.parallelRegionStore = parallelRegionStore;
  }

  private String getParallelRegionNameForIndex(BigInteger index) {
    return parallelRegionStore.values().stream()
        .filter(pr -> pr.getSpec().getIndex().equals(index))
        .map(pr -> pr.getSpec().getAlias())
        .findFirst()
        .orElseThrow(RuntimeException::new);
  }

  private PeType convertAugmentedPeTypeToPeType(
      Map<BigInteger, BigInteger> operatorMap, AugmentedPEType pe) {
    /*
     * Collect all of the PE operators.
     */
    List<OperatorType> ops =
        pe.getOperInstances().getOperInstance().stream()
            .map(
                op -> {
                  var pop = (PhysOperInstanceType) op;
                  /*
                   * Build return the operator type.
                   */
                  var opType = new OperatorType();
                  opType.setName(pop.getName());
                  opType.setLogicalName(pop.getLogicalName());
                  /*
                   * Collect all the parallel channels.
                   */
                  if (pop.getParallelRegions() != null) {
                    List<ParallelChannelType> parChans =
                        pop.getParallelRegions().getParallelRegion().stream()
                            .map(
                                pr -> {
                                  var e = new ParallelChannelType();
                                  e.setIndex(BigInteger.valueOf(pr.getGlobalChannelIndex()));
                                  e.setLogicalName(
                                      getParallelRegionNameForIndex(pr.getRegionIndex()));
                                  return e;
                                })
                            .collect(Collectors.toList());
                    opType.getParallelChannels().addAll(parChans);
                  }
                  /*
                   * Collect the input ports.
                   */
                  if (pop.getInputPorts() != null) {
                    List<InputPortType> iPorts =
                        pop.getInputPorts().getInputPort().stream()
                            .map(
                                in -> {
                                  var e = new InputPortType();
                                  e.setIndex(in.getIndex());
                                  e.setName(in.getName());
                                  return e;
                                })
                            .sorted(Comparator.comparing(InputPortType::getIndex))
                            .collect(Collectors.toList());
                    opType.getInputPorts().addAll(iPorts);
                  }
                  /*
                   * Collect the output ports.
                   */
                  if (pop.getOutputPorts() != null) {
                    List<OutputPortType> oPorts =
                        pop.getOutputPorts().getOutputPort().stream()
                            .map(
                                out -> {
                                  /*
                                   * Build the connections.
                                   */
                                  List<PeConnectionType> peConns =
                                      out.getInterPeConnection().stream()
                                          .map(
                                              peConn -> {
                                                PeConnectionType e = new PeConnectionType();
                                                e.setInputPeId(
                                                    operatorMap.get(peConn.getOperInstanceIndex()));
                                                e.setInputPortIndex(peConn.getIportIndex());
                                                return e;
                                              })
                                          .collect(Collectors.toList());
                                  /*
                                   * Build the output port type.
                                   */
                                  var e = new OutputPortType();
                                  e.setIndex(out.getIndex());
                                  e.setName(out.getName());
                                  e.setStreamName(out.getStreamName());
                                  e.getConnections().addAll(peConns);
                                  return e;
                                })
                            .sorted(Comparator.comparing(OutputPortType::getIndex))
                            .collect(Collectors.toList());
                    opType.getOutputPorts().addAll(oPorts);
                  }
                  /*
                   * Return the operator.
                   */
                  return opType;
                })
            .collect(Collectors.toList());
    /*
     * Build and return the PE.
     */
    var peType = new PeType();
    peType.setId(pe.getId());
    peType.getOperators().addAll(ops);
    return peType;
  }

  public Instance captureInstanceState(String namespace) {
    /*
     * Build the job list.
     */
    var jobs =
        jobStore.values().stream()
            .map(
                job -> {
                  /*
                   * Build the AADL list for the job.
                   */
                  var uuids =
                      configMapStore.getConfigMapsForJob(job).stream()
                          .map(cm -> cm.getMetadata().getUid())
                          .collect(Collectors.toSet());
                  var aadls =
                      configMapCache.entrySet().stream()
                          .filter(e -> uuids.contains(e.getKey()))
                          .map(Map.Entry::getValue)
                          .collect(Collectors.toList());
                  /*
                   * Build the operator map.
                   */
                  Map<BigInteger, BigInteger> operatorMap = new TreeMap<>();
                  for (var pe : aadls) {
                    pe.getOperInstances()
                        .getOperInstance()
                        .forEach(op -> operatorMap.put(op.getIndex(), pe.getId()));
                  }
                  /*
                   * Convert the PE models.
                   */
                  List<PeType> pes =
                      aadls.stream()
                          .map(e -> convertAugmentedPeTypeToPeType(operatorMap, e))
                          .sorted(Comparator.comparing(PeType::getId))
                          .collect(Collectors.toList());
                  /*
                   * Build the job.
                   */
                  JobType e = new JobType();
                  e.setApplicationName(
                      job.getMetadata().getAnnotations().get(STREAMS_APP_NAME_ANNOTATION_KEY));
                  e.setId(job.getSpec().getId());
                  e.setName(job.getMetadata().getName());
                  e.setUser("streams");
                  e.getPes().addAll(pes);
                  return e;
                })
            .sorted(Comparator.comparing(JobType::getId))
            .collect(Collectors.toList());
    /*
     * Build and return the instance.
     */
    Instance instance = new Instance();
    instance.setId(namespace);
    instance.getJobs().addAll(jobs);
    return instance;
  }
}
