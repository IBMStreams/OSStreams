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

package com.ibm.streams.state;

import static org.junit.Assert.assertEquals;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;

public class CaptureConverterTest {

  @Rule public MockResource mock = new MockResource();

  @Test
  public void basicParallelJob() {
    /*
     * Create a new basic job.
     */
    var job = JobUtils.makeJob("basicParallel", JobUtils.parallelSab, 2);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("basicParallel", 600);
    /*
     * Capture the state.
     */
    var state = mock.getCaptureConverter().captureInstanceState(MockResource.KUBE_NAMESPACE);
    assertEquals(1, state.getJobs().size());
    assertEquals(2, state.getJobs().get(0).getPes().size());
  }

  @Test
  public void hostPoolJob() {
    /*
     * Create a new host pool job.
     */
    Job job = JobUtils.makeJob("test", "apps.hostpools.HostPools.sab", 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("test", 600);
    /*
     * Capture the state.
     */
    var state = mock.getCaptureConverter().captureInstanceState(MockResource.KUBE_NAMESPACE);
    assertEquals(1, state.getJobs().size());
    assertEquals(3, state.getJobs().get(0).getPes().size());
  }

  @Test
  public void exporterJob() {
    /*
     * Create the exporting job.
     */
    Job exp = JobUtils.makeJob("exporter", "apps.pubsub.Exporter.sab", 1);
    mock.getJobClient().createOrReplace(exp);
    mock.assertJobSubmitted("exporter", 600);
    /*
     * Capture the state.
     */
    var state = mock.getCaptureConverter().captureInstanceState(MockResource.KUBE_NAMESPACE);
    assertEquals(1, state.getJobs().size());
    assertEquals(1, state.getJobs().get(0).getPes().size());
  }
}
