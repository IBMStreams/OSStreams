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

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class CausalChainsTest {

  private static final int DELAY_IN_SEC = 60;
  private static final Logger LOGGER = LoggerFactory.getLogger(CausalChainsTest.class);

  @Rule public MockResource mock = new MockResource();

  @Test
  public void voluntaryPodDeletion() {
    /*
     * Create the new job.
     */
    var job = JobUtils.makeJob("job", JobUtils.parallelSab, 1);
    job.getSpec().getProcessingElement().setRestartDeletedPod(true);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("job", DELAY_IN_SEC);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
    /*
     * Get the one pod for the job.
     */
    var pod = mock.getPodClient().list().getItems().get(0);
    assertNotNull(pod);
    /*
     * Delete the PE.
     */
    mock.getPodClient().delete(pod);
    mock.assertPeLaunchCount("job-0", 2, DELAY_IN_SEC);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
  }

  @Test
  public void voluntaryProcessingElementDeletion() throws InterruptedException {
    /*
     * Create the new job.
     */
    var job = JobUtils.makeJob("job", JobUtils.parallelSab, 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("job", DELAY_IN_SEC);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
    /*
     * Get the one PE for the job.
     */
    var pe = mock.getPeClient().withName("job-0").get();
    assertNotNull(pe);
    assertTrue(pe.getMetadata().getLabels().containsKey(STREAMS_SVC_LABEL_KEY));
    /*
     * Delete the PE.
     */
    mock.getPeClient().delete(pe);
    mock.assertPeLaunchCount("job-0", 2, DELAY_IN_SEC);
    mock.assertPeSize(1);
    mock.assertPodSize(1);
  }
}
