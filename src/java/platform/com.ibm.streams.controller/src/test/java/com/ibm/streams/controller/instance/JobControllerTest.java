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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.Quantity;
import io.fabric8.kubernetes.api.model.ResourceRequirements;
import java.util.HashMap;
import java.util.Map;
import java.util.Properties;
import java.util.concurrent.ExecutionException;
import java.util.concurrent.ForkJoinPool;
import java.util.stream.IntStream;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;

public class JobControllerTest {

  private static final int DELAY_IN_SEC = 60;

  @Rule public MockResource mock = new MockResource();

  private void invokeBasicParallel(String name, int timeout) {
    Job job = JobUtils.makeJob("basicParallel_" + name, JobUtils.parallelSab, 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("basicParallel_" + name, timeout);
  }

  @Test
  public void basicParallel() {
    invokeBasicParallel("plain", DELAY_IN_SEC);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
  }

  @Test
  public void simultaneousBasicParallel() throws ExecutionException, InterruptedException {
    final int tests = 16;
    ForkJoinPool pool = new ForkJoinPool(2);
    pool.submit(
            () -> {
              IntStream.range(0, tests)
                  .parallel()
                  .forEach(i -> invokeBasicParallel(String.valueOf(i), DELAY_IN_SEC));
            })
        .get();
  }

  @Test
  public void severalPEParallel() {
    Job job = JobUtils.makeJob("severalPEParallel", JobUtils.parallelSab, 3);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("severalPEParallel", DELAY_IN_SEC);
    mock.assertPeSize(3);
    mock.assertPodSize(3);
  }

  @Test
  public void manyPEParallel() {
    final Properties props =
        new Properties() {
          {
            put("width1", "5");
            put("width2", "6");
            put("width3", "7");
          }
        };
    Job job = JobUtils.makeJob("manyPEParallel", JobUtils.parallelSab, 20, props);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("manyPEParallel", DELAY_IN_SEC);
    mock.assertPeSize(20);
    mock.assertPodSize(20);
  }

  @Test
  public void hostPlacementNodeName() {
    // Note that we are requesting 1 PE, but we expect more based on the host constraints
    // as represented in the sab.
    Job job = JobUtils.makeJob("hostPlacement", JobUtils.hostPlacementNodeSab, 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("hostPlacement", DELAY_IN_SEC);
    mock.assertPeSize(2);
    mock.assertPodSize(2);
  }

  /*
   * FIXME: would like to be able to unit test IP address based host placement. We currently can't
   * because the mocked node list is empty. Yes, we could make that part of our Environment object,
   * but that avoids the client calls which is what we want to test.
   */

  @Test
  public void resourceConstraints() {
    String jobName = "resources";
    Job job = JobUtils.makeJob(jobName, JobUtils.parallelSab, 4);
    final Map<String, Quantity> limits =
        new HashMap<String, Quantity>() {
          {
            put("cpu", new Quantity("16.0"));
            put("memory", new Quantity("64Gi"));
          }
        };
    final Map<String, Quantity> requests =
        new HashMap<String, Quantity>() {
          {
            put("cpu", new Quantity("0.1"));
            put("memory", new Quantity("100Mi"));
          }
        };
    var resources = new ResourceRequirements(limits, requests);
    job.getSpec().setResources(resources);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, DELAY_IN_SEC);
    mock.assertPeSize(4);
    mock.assertPodSize(4);
    job = mock.getJobStore().getJobWithName(jobName).get();
    var pes = mock.getProcessingElementStore().getPesWithJob(job);
    /*
     * We have 5 PEs, so we divide the originals by 4. We don't need a new map
     * for requests since they should remain the same.
     */
    final Map<String, Quantity> perPeLimits =
        new HashMap<String, Quantity>() {
          {
            put("cpu", new Quantity("4"));
            put("memory", new Quantity("16Gi"));
          }
        };
    /*
     * Assert that the PEs have the correct resources. Pods should inherit the
     * same requests as their PE.
     */
    pes.values().stream()
        .forEach(
            pe -> {
              var peResources = pe.getSpec().getResources();
              assertEquals(perPeLimits, peResources.getLimits());
              assertEquals(requests, peResources.getRequests());

              var podResources =
                  mock.getPodStore()
                      .getPodForProcessingElement(pe)
                      .get()
                      .getSpec()
                      .getContainers()
                      .get(0)
                      .getResources();
              assertEquals(perPeLimits, podResources.getLimits());
              assertEquals(requests, podResources.getRequests());
            });
  }
}
