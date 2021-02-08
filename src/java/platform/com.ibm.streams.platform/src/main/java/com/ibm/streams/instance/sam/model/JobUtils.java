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

import com.ibm.streams.instance.sam.model.fuser.FusedContainer;
import java.math.BigInteger;
import java.util.Collection;
import java.util.Map;
import java.util.Set;
import java.util.SortedMap;
import java.util.SortedSet;
import java.util.TreeMap;
import java.util.concurrent.atomic.AtomicLong;
import java.util.stream.Collectors;

public final class JobUtils {
  private JobUtils() {} // Private to disallow construction

  public static Map<BigInteger, BigInteger> createFusedContainerIndexToPeId(
      SortedSet<FusedContainer> containers, Map<BigInteger, Set<String>> peIdToOperators) {
    // we're going to reverse the peIdToOperators map by creating a deterministic "name" for a PE:
    // the
    // sorted concatenation of all of the names of the operators it containts
    Map<String, BigInteger> operatorsToPeId =
        peIdToOperators.entrySet().stream()
            .collect(
                Collectors.toMap(
                    e -> e.getValue().stream().sorted().reduce("", (s1, s2) -> s1 + s2),
                    e -> e.getKey()));
    // for each container, we will create the same concatted operator name as above, and use that to
    // determine its
    // PE id; if it has no prior PE id, then it gets a new one
    long maxPeId =
        peIdToOperators.keySet().stream()
            .max(BigInteger::compareTo)
            .orElse(BigInteger.valueOf(-1))
            .longValue();
    AtomicLong nextPeId = new AtomicLong(maxPeId + 1);
    return containers.stream()
        .collect(
            Collectors.toMap(
                c -> c.getIndex(),
                c -> {
                  String operators =
                      c.getContainerSpecification().getNodes().stream()
                          .map(n -> n.getName())
                          .sorted()
                          .reduce("", (s1, s2) -> s1 + s2);
                  return operatorsToPeId.containsKey(operators)
                      ? operatorsToPeId.get(operators)
                      : BigInteger.valueOf(nextPeId.getAndIncrement());
                }));
  }

  /**
   * Builds the entire set of PEs that corresponds to the set of static containers provided
   *
   * @param jobId - The ID of the Job object that owns this PE
   * @param containers - The fused containers that represents all of the static information these
   *     PEs
   * @param buildingJob - Indicates if we are conducting fusion/fission or submitting a Job
   * @param pesStopped - Used to set the PE state correctly based on recovery, fusion/fission, or
   *     job submission
   * @param isPreviewOnly - Used to indicate if this PE generation is a dry run and should not be
   *     persisted
   * @return The set of created PEs
   */
  public static SortedMap<BigInteger, Pe> createPes(
      BigInteger jobId,
      SortedSet<FusedContainer> containers,
      Map<BigInteger, BigInteger> fusedContainerIndexToPeId,
      BigInteger genId,
      boolean buildingJob,
      boolean pesStopped,
      boolean isPreviewOnly) {
    SortedMap<BigInteger, Pe> pes = new TreeMap<>();
    for (FusedContainer container : containers) {
      BigInteger peId = fusedContainerIndexToPeId.get(container.getIndex());
      pes.put(
          peId,
          createPe(
              jobId,
              peId,
              fusedContainerIndexToPeId,
              container,
              genId,
              buildingJob,
              pesStopped,
              isPreviewOnly));
    }

    return pes;
  }

  public static void buildPeConnections(Collection<Pe> pes, boolean buildingJob) {
    // Builds the connections, static and dynamic
    pes.stream()
        .forEach(
            (pe) -> {
              pe.getOutputPorts().stream().forEach((op) -> op.registerOutboundStaticConnections());
              pe.getInputPorts().stream()
                  .forEach((ip) -> ip.setInboundStaticConnectionIds(buildingJob));
              pe.setupDynConnections(false);
            });

    // If we're doing fusion/fission we may not have looked at an OutputPort that needs to create a
    // static connection
    // Take a second pass in that case to ensure we've checked that all InputPort static connections
    // have a matching
    // runtime connection object, and if needed, pull up the OutputPort and create one
    // DMK TODO
  }

  /**
   * Gets the Port IDs needed to construct a PE and then creates it
   *
   * @param jobId - The ID of the Job object that owns this PE
   * @param peId - The ID of the PE to build
   * @param container - The fused container that represents all of the static information about this
   *     PE
   * @param buildingJob - Indicates if we are conducting fusion/fission or submitting a Job
   * @param pesStopped - Used to set the PE state correctly based on recovery, fusion/fission, or
   *     job submission
   * @param isPreviewOnly - Used to indicate if this PE generation is a dry run and should not be
   *     persisted
   * @return The PE
   */
  private static Pe createPe(
      BigInteger jobId,
      BigInteger peId,
      Map<BigInteger, BigInteger> fusedContainerIndexToPeId,
      FusedContainer container,
      BigInteger genId,
      boolean buildingJob,
      boolean pesStopped,
      boolean isPreviewOnly) {
    return new Pe(
        jobId,
        container,
        container.getIndex(),
        peId,
        fusedContainerIndexToPeId,
        genId,
        isPreviewOnly,
        buildingJob,
        pesStopped);
  }
}
