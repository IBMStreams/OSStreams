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

package com.ibm.streams.mock.server;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertTrue;

import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.controller.utils.ObjectUtils;
import com.ibm.streams.mock.instance.MockResource;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class KubernetesServerTest {

  private static final int DELAY_IN_SEC = 60;
  private static final Logger LOGGER = LoggerFactory.getLogger(KubernetesServerTest.class);

  @Rule public MockResource mock = new MockResource();

  @Test
  public void newServerAndClientListPods() {
    assertEquals(0, mock.getPodClient().list().getItems().size());
  }

  @Test
  public void labelPatch() {
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
    assertTrue(pe.getMetadata().getLabels().containsKey(STREAMS_GENERATION_LABEL_KEY));
    /*
     * Patch the SVC label.
     */
    var opt = ObjectUtils.deepCopy(pe, ProcessingElement.class);
    assertTrue(opt.isPresent());
    opt.ifPresent(
        cp -> {
          cp.setSpec(null);
          cp.getMetadata().getLabels().put(STREAMS_GENERATION_LABEL_KEY, "1");
          mock.getPeClient().withName("job-0").patch(cp);
        });
    /*
     * Check if the resource has been correctly updated.
     */
    pe = mock.getPeClient().withName("job-0").get();
    assertNotNull(pe);
    assertEquals("1", pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY));
  }

  @Test
  public void labelRemoval() {
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
     * Remove the SVC label.
     */
    var opt = ObjectUtils.deepCopy(pe, ProcessingElement.class);
    assertTrue(opt.isPresent());
    opt.ifPresent(
        cp -> {
          cp.getMetadata().getLabels().remove(STREAMS_SVC_LABEL_KEY);
          mock.getPeClient().createOrReplace(cp);
        });
    /*
     * Check if the resource has been correctly updated.
     */
    pe = mock.getPeClient().withName("job-0").get();
    assertNotNull(pe);
    assertFalse(pe.getMetadata().getLabels().containsKey(STREAMS_SVC_LABEL_KEY));
  }
}
