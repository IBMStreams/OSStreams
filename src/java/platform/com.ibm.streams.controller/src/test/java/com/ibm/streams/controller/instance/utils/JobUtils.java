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

package com.ibm.streams.controller.instance.utils;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_NAME_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_APP_SCOPE_ANNOTATION_KEY;
import static org.junit.Assert.assertTrue;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.jobs.JobSpec;
import com.ibm.streams.controller.crds.prs.ParallelRegionSpec;
import com.ibm.streams.controller.k8s.utils.TimeUtils;
import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import java.math.BigInteger;
import java.time.Instant;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.stream.Collectors;
import lombok.var;

public class JobUtils {

  public static final String parallelSab = "apps.parallel.Parallel.sab";
  public static final String hostPlacementNodeSab = "apps.hostplacement.HostPlacement_nodename.sab";

  /*
   * Manual fusion is any positive integer. Since 0 and -1 are invalid values to specify for manual
   * fusion, we can use them to specify other kinds of fusion. Note that this convention is internal
   * to this utils class, we don't expose it.
   */
  private static final int LEGACY_FUSION = 0;
  private static final int AUTOMATIC_FUSION = -1;

  public static Job makeJob(String jobName) {
    Job job = makeJob(jobName, "", 1);
    HashMap<String, String> annotations =
        new HashMap<String, String>() {
          {
            put(STREAMS_APP_NAME_ANNOTATION_KEY, "Main-0");
            put(STREAMS_APP_SCOPE_ANNOTATION_KEY, "Default");
          }
        };
    job.getMetadata().setAnnotations(annotations);
    job.getSpec().setId(BigInteger.ONE);
    job.getSpec().setGenerationId(BigInteger.ZERO);
    return job;
  }

  public static Job makeJob(String jobName, String sabName, int requestedPes) {
    assertTrue(requestedPes > 0);
    return makeJobInternal(jobName, sabName, requestedPes, new Properties(), new HashMap<>());
  }

  public static Job makeJob(
      String jobName, String sabName, int requestedPes, Properties subValues) {
    assertTrue(requestedPes > 0);
    return makeJobInternal(jobName, sabName, requestedPes, subValues, new HashMap<>());
  }

  public static Job makeJob(
      String jobName,
      String sabName,
      int requestedPes,
      Properties subValues,
      Map<String, BigInteger> parWidths) {
    assertTrue(requestedPes > 0);
    return makeJobInternal(jobName, sabName, requestedPes, subValues, parWidths);
  }

  public static Job makeJobLegacy(String jobName, String sabName) {
    return makeJobInternal(jobName, sabName, LEGACY_FUSION, new Properties(), new HashMap<>());
  }

  public static Job makeJobLegacy(
      String jobName, String sabName, Map<String, BigInteger> parWidths) {
    return makeJobInternal(jobName, sabName, LEGACY_FUSION, new Properties(), parWidths);
  }

  private static Job makeJobInternal(
      String jobName,
      String sabName,
      int requestedPes,
      Properties subValues,
      Map<String, BigInteger> parWidths) {
    /*
     * Create the spec.
     */
    JobSpec spec = new JobSpec();
    spec.getBundle().setName(sabName);
    /*
     * Create the fusion spec.
     */
    assertTrue(
        requestedPes > 0 || requestedPes == AUTOMATIC_FUSION || requestedPes == LEGACY_FUSION);
    switch (requestedPes) {
      case AUTOMATIC_FUSION:
        spec.getFusion().setAutomatic(true);
        break;
      case LEGACY_FUSION:
        spec.getFusion().setLegacy(true);
        break;
      default:
        spec.getFusion().setManual(requestedPes);
        break;
    }
    spec.setSubmissionTimeValues(subValues);
    spec.setParallelRegions(makeParallelRegions(parWidths));
    /*
     * Create the metadata.
     */
    ObjectMeta meta = new ObjectMeta();
    meta.setName(jobName);
    meta.setNamespace(MockResource.KUBE_NAMESPACE);
    meta.setCreationTimestamp(TimeUtils.getInstance().toString(Instant.now()));
    /*
     * Create the job.
     */
    Job job = new Job();
    job.setSpec(spec);
    job.setMetadata(meta);
    return job;
  }

  private static List<ParallelRegionSpec> makeParallelRegions(Map<String, BigInteger> parWidths) {
    AtomicInteger index = new AtomicInteger(0);
    return parWidths.entrySet().stream()
        .map(
            e -> {
              var spec = new ParallelRegionSpec();
              spec.setIndex(BigInteger.valueOf(index.getAndIncrement()));
              spec.setAlias(e.getKey());
              spec.setWidth(e.getValue());
              return spec;
            })
        .collect(Collectors.toList());
  }
}
