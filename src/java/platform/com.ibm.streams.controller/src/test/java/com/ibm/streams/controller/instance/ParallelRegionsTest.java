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

package com.ibm.streams.controller.instance;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.prs.ParallelRegion;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import java.math.BigInteger;
import java.util.AbstractMap;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.Properties;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import lombok.var;
import org.junit.Assert;
import org.junit.Rule;
import org.junit.Test;

public class ParallelRegionsTest {

  static final String parallelSab = "apps.parallel.Parallel.sab";
  static final int TIMEOUT = 120;

  @Rule public MockResource mock = new MockResource();

  @Test
  public void parallelRegionSubmissionTimeValues() {
    final String jobName = "submissionTimeValues";
    final Properties props =
        new Properties() {
          {
            put("width1", "2");
            put("width2", "4");
            put("width3", "6");
          }
        };

    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 1, props);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, TIMEOUT);
    mock.assertParallelRegionSize(3);
    /*
     * The association between "RegionX" and "widthX" is parth of the application itself,
     * not any assumption in the design of the parallel regions.
     */
    mock.assertParallelRegionWidth(jobName, "Result.Region1", 2);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 4);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", 6);
  }

  @Test
  public void parallelRegionWidthOverride() {
    final String jobName = "widthOverride";
    final Map<String, BigInteger> widths =
        new HashMap<String, BigInteger>() {
          {
            put("Result.Region1", BigInteger.valueOf(9));
            put("Result.Region2", BigInteger.valueOf(9));
            put("Result.Region3", BigInteger.valueOf(9));
          }
        };
    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 20, new Properties(), widths);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, TIMEOUT);
    mock.assertPeSize(20);
    mock.assertPodSize(20);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", 9);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 9);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", 9);
  }

  @Test
  public void parallelRegionWildcard() {
    final String jobName = "wildcard";
    final Map<String, BigInteger> widths =
        new HashMap<String, BigInteger>() {
          {
            put("*Region1", BigInteger.valueOf(6));
            put("*Region2", BigInteger.valueOf(6));
            put("*Region3", BigInteger.valueOf(6));
          }
        };
    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 15, new Properties(), widths);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, TIMEOUT);
    mock.assertPeSize(15);
    mock.assertPodSize(15);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", 6);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 6);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", 6);
  }

  @Test
  public void parallelRegionWidthOverrideAndSubmissionTimeValues() {
    final String jobName = "widthOverrideAndSubmissionTimeValues";
    final Properties props =
        new Properties() {
          {
            put("width1", "3");
          }
        };
    final Map<String, BigInteger> widths =
        new HashMap<String, BigInteger>() {
          {
            put("*Region2", BigInteger.valueOf(7));
          }
        };
    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 1, props, widths);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, TIMEOUT);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", 3);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 7);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", 1); // should still be the default
  }

  @Test
  public void parallelRegionDynamicWidthIncreaseOnePe() {
    final String jobName = "dynamicWidthIncreaseOnePe";
    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, BigInteger.ZERO, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
    mock.assertConfigMapSize(2);
    mock.assertServiceSize(1);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", 1);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 1);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", 1);
    /*
     * Even though we created the job object above, we need to grab it from the job store. During the
     * job creation process, we add things (such as annotations), that are not present when we
     * synthesize a job object for test.
     */
    Assert.assertTrue(mock.getJobStore().getJobWithName(jobName).isPresent());

    ParallelRegion pr =
        mock.getParallelRegionStore().getParallelRegionWithAlias(jobName, "Result.Region2").get();
    mock.getParallelRegionCoordinator().setParallelRegionWidth(pr, BigInteger.valueOf(7));
    mock.assertParallelRegionWidth(jobName, "Result.Region2", 7);
    mock.assertJobSubmitted(jobName, BigInteger.ONE, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
    mock.assertConfigMapSize(2);
    mock.assertServiceSize(1);
  }

  private Map<BigInteger, String> getPesToPodName(Job job) {
    var pes = mock.getProcessingElementStore().getPesWithJob(job);
    var podStore = mock.getPodStore();
    return pes.entrySet().stream()
        .map(
            e ->
                podStore
                    .getPodForProcessingElement(e.getValue())
                    .map(v -> v.getMetadata().getName())
                    .map(n -> new AbstractMap.SimpleEntry<>(e.getKey(), n)))
        .filter(Optional::isPresent)
        .map(Optional::get)
        .collect(Collectors.toMap(Map.Entry::getKey, Map.Entry::getValue));
  }

  /*
   * We establish that a PE did not restart through two requirements:
   *   1. The pod name for that PE is the same before and after.
   *   2. The launch count for that PE is 1.
   */
  private void assertPeDidNotRestart(
      int pe,
      String jobName,
      Map<BigInteger, String> beforePesToPodName,
      Map<BigInteger, String> afterPesToPodName) {
    var beforeName = beforePesToPodName.get(BigInteger.valueOf(pe));
    var afterName = afterPesToPodName.get(BigInteger.valueOf(pe));
    assertEquals(beforeName, afterName);
    mock.assertPeLaunchCount(jobName + "-" + pe, 1, TIMEOUT);
  }

  /*
   * We establish that a PE did restart through two requirements:
   *   1. The pod name for that PE before is different after.
   *   2. The launch count for that PE is 2.
   */
  private void assertPeRestarted(
      int pe,
      String jobName,
      Map<BigInteger, String> beforePesToPodName,
      Map<BigInteger, String> afterPesToPodName) {
    var beforeName = beforePesToPodName.get(BigInteger.valueOf(pe));
    var afterName = afterPesToPodName.get(BigInteger.valueOf(pe));
    assertFalse(beforeName.equals(afterName));
    mock.assertPeLaunchCount(jobName + "-" + pe, 2, TIMEOUT);
  }

  @Test
  public void parallelRegionDynamicWidthIncreaseLegacy() {
    final String jobName = "dynamicWidthIncreaseLegacy";
    final int region1 = 2;
    final int region2 = 5;
    final int region3 = 4;
    final int source = 1;
    final int sink = 1;
    final Map<String, BigInteger> widths =
        new HashMap<String, BigInteger>() {
          {
            put("Result.Region1", BigInteger.valueOf(region1));
            put("Result.Region2", BigInteger.valueOf(region2));
            put("Result.Region3", BigInteger.valueOf(region3));
          }
        };
    /*
     * Initial job submission.
     */
    Job job = JobUtils.makeJobLegacy(jobName, JobUtils.parallelSab, widths);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, BigInteger.ZERO, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3 + source + sink);
    mock.assertPodSize(region1 + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3 + source + sink);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", region1);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", region2);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", region3);
    /*
     * All PEs should have a launch count of 1.
     */
    assertEquals(13, region1 + region2 + region3 + source + sink);
    IntStream.range(0, 13).forEach(i -> mock.assertPeLaunchCount(jobName + "-" + i, 1, TIMEOUT));
    /*
     * Get the job instance from the store - it has values the one created above
     * does not.
     */
    job = mock.getJobStore().getJobWithName(jobName).get();
    ParallelRegion pr =
        mock.getParallelRegionStore().getParallelRegionWithAlias(jobName, "Result.Region1").get();
    /*
     * Create the initial mapping of PE to pod name. We will use this to ensure the correct
     * pods do and do not restart.
     */
    var beforePesToPodName = getPesToPodName(job);
    /*
     * Perform the parallel region width increase.
     */
    final int region1Resize = 10;
    mock.getParallelRegionCoordinator()
        .setParallelRegionWidth(pr, BigInteger.valueOf(region1Resize));
    mock.assertParallelRegionWidth(jobName, "Result.Region1", region1Resize);
    mock.assertJobSubmitted(jobName, BigInteger.ONE, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(region1Resize + region2 + region3 + source + sink);
    mock.assertPodSize(region1Resize + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1Resize + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1Resize + region2 + region3 + source + sink);
    /*
     * Get the PE to pod name mapping for after the width change.
     */
    job = mock.getJobStore().getJobWithName(jobName).get();
    var afterPesToPodName = getPesToPodName(job);
    /*
     * All of the PEs which existed after the initial job submission, and were unaffected
     * by the width change, should not have restarted.
     */
    assertPeDidNotRestart(1, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(4, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(10, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(11, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(12, jobName, beforePesToPodName, afterPesToPodName);
    /*
     * All of the PEs which were added as a result of the width change should have
     * a launch count of 1. Since they did not exist before, there is no "before"
     * name to compare to.
     */
    mock.assertPeLaunchCount(jobName + "-13", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-14", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-15", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-16", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-17", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-18", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-19", 1, TIMEOUT);
    mock.assertPeLaunchCount(jobName + "-20", 1, TIMEOUT);
    /*
     * All of the PEs which existed after the initial job submission, but are in the
     * parallel region we changed, should have restarted.
     */
    assertPeRestarted(2, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(5, jobName, beforePesToPodName, afterPesToPodName);
    /*
     * All of the PEs which existed after the initial job submission, but communicate
     * with PEs in the target parallel region, should have also restarted.
     */
    assertPeRestarted(0, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(3, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(6, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(7, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(8, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(9, jobName, beforePesToPodName, afterPesToPodName);
  }

  @Test
  public void parallelRegionDynamicMultiWidthIncreaseLegacy() {
    final String jobName = "dynamicMultiWidthIncreaseLegacy";
    final int region1 = 1;
    int region2 = 1;
    final int region3 = 1;
    final int source = 1;
    final int sink = 1;
    Job job = JobUtils.makeJobLegacy(jobName, JobUtils.parallelSab);
    mock.getJobClient().createOrReplace(job);

    BigInteger jobId = BigInteger.ZERO;
    mock.assertJobSubmitted(jobName, jobId, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3 + source + sink);
    mock.assertPodSize(region1 + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3 + source + sink);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", region1);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", region2);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", region3);

    job = mock.getJobStore().getJobWithName(jobName).get();
    ParallelRegion pr =
        mock.getParallelRegionStore().getParallelRegionWithAlias(jobName, "Result.Region2").get();
    region2 = 5;
    mock.getParallelRegionCoordinator().setParallelRegionWidth(pr, BigInteger.valueOf(region2));

    jobId = jobId.add(BigInteger.ONE);
    mock.assertJobSubmitted(jobName, jobId, TIMEOUT);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", region2);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3 + source + sink);
    mock.assertPodSize(region1 + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3 + source + sink);

    pr = mock.getParallelRegionStore().getParallelRegionWithAlias(jobName, "Result.Region2").get();
    region2 = 10;
    mock.getParallelRegionCoordinator().setParallelRegionWidth(pr, BigInteger.valueOf(region2));

    jobId = jobId.add(BigInteger.ONE);
    mock.assertJobSubmitted(jobName, jobId, TIMEOUT);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", region2);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3 + source + sink);
    mock.assertPodSize(region1 + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3 + source + sink);
  }

  /*
   * We establish that a PE was deleted in two ways:
   *   1. It is not present in the map of pod names.
   *   2. It is not present in the PE store.
   */
  private void assertPeDeleted(int pe, String jobName, Map<BigInteger, String> afterPesToPodName) {
    assertFalse(afterPesToPodName.containsKey(BigInteger.valueOf(pe)));
    assertFalse(mock.getProcessingElementStore().getPeWithName(jobName + "-" + pe).isPresent());
  }

  @Test
  public void parallelRegionDynamicWidthDecreaseLegacy() {
    final String jobName = "dynamicWidthDecreaseLegacy";
    final int region1 = 2;
    final int region2 = 5;
    final int region3 = 4;
    final int source = 1;
    final int sink = 1;
    final Map<String, BigInteger> widths =
        new HashMap<String, BigInteger>() {
          {
            put("Result.Region1", BigInteger.valueOf(region1));
            put("Result.Region2", BigInteger.valueOf(region2));
            put("Result.Region3", BigInteger.valueOf(region3));
          }
        };
    /*
     * Initial job submission.
     */
    Job job = JobUtils.makeJobLegacy(jobName, JobUtils.parallelSab, widths);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, BigInteger.ZERO, TIMEOUT);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3 + source + sink);
    mock.assertPodSize(region1 + region2 + region3 + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3 + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3 + source + sink);
    mock.assertParallelRegionSize(3);
    mock.assertParallelRegionWidth(jobName, "Result.Region1", region1);
    mock.assertParallelRegionWidth(jobName, "Result.Region2", region2);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", region3);
    /*
     * All PEs should have a launch count of 1.
     */
    assertEquals(13, region1 + region2 + region3 + source + sink);
    IntStream.range(0, 13).forEach(i -> mock.assertPeLaunchCount(jobName + "-" + i, 1, TIMEOUT));
    /*
     * Get the job instance from the store - it has values the one created above
     * does not.
     */
    job = mock.getJobStore().getJobWithName(jobName).get();
    ParallelRegion pr =
        mock.getParallelRegionStore().getParallelRegionWithAlias(jobName, "Result.Region3").get();
    /*
     * Create the initial mapping of PE to pod name. We will use this to ensure the correct
     * pods do and do not restart.
     */
    var beforePesToPodName = getPesToPodName(job);
    /*
     * Perform the parallel width decrease.
     */
    final int region3Resize = 2;
    mock.getParallelRegionCoordinator()
        .setParallelRegionWidth(pr, BigInteger.valueOf(region3Resize));
    mock.assertJobSubmitted(jobName, BigInteger.ONE, TIMEOUT);
    mock.assertParallelRegionWidth(jobName, "Result.Region3", region3Resize);
    mock.assertJobSize(1);
    mock.assertPeSize(region1 + region2 + region3Resize + source + sink);
    mock.assertPodSize(region1 + region2 + region3Resize + source + sink);
    mock.assertConfigMapSize(region1 + region2 + region3Resize + source + sink + 1);
    mock.assertServiceSize(region1 + region2 + region3Resize + source + sink);
    /*
     * Get the PE to pod name mapping for after the width change.
     */
    job = mock.getJobStore().getJobWithName(jobName).get();
    var afterPesToPodName = getPesToPodName(job);
    /*
     * All of the PEs which existed after the initial job submission, and were unaffected
     * by the width change, should not have restarted.
     */
    assertPeDidNotRestart(0, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(2, jobName, beforePesToPodName, afterPesToPodName);
    assertPeDidNotRestart(5, jobName, beforePesToPodName, afterPesToPodName);
    /*
     * All of the PEs which existed after the initial job submission, but are in the
     * parallel region we changed, should have restarted.
     */
    assertPeRestarted(4, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(10, jobName, beforePesToPodName, afterPesToPodName);
    /*
     * All of the PEs which existed after the initial job submission, but communicate
     * with PEs in the target parallel region, should have also restarted.
     */
    assertPeRestarted(3, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(6, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(7, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(8, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(9, jobName, beforePesToPodName, afterPesToPodName);
    assertPeRestarted(1, jobName, beforePesToPodName, afterPesToPodName);
    /*
     * The PEs removed during the width change should not be present
     * any longer.
     */
    assertPeDeleted(11, jobName, afterPesToPodName);
    assertPeDeleted(12, jobName, afterPesToPodName);
  }
}
