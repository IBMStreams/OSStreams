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

import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import java.util.Properties;
import lombok.var;
import org.junit.Ignore;
import org.junit.Rule;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class KubernetesServerStressTest {

  private static final int DELAY_IN_SEC = 120;
  private static final Logger LOGGER = LoggerFactory.getLogger(KubernetesServerStressTest.class);

  @Rule public MockResource mock = new MockResource();

  private void runStressTest(Integer w1, Integer w2, Integer w3, Integer reqPes) {
    final var props =
        new Properties() {
          {
            put("width1", w1.toString());
            put("width2", w2.toString());
            put("width3", w3.toString());
          }
        };
    var jobName = "stress_" + reqPes.toString();
    /*
     * Create the job.
     */
    var job = JobUtils.makeJob(jobName, JobUtils.parallelSab, reqPes, props);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, DELAY_IN_SEC);
    /*
     * Wait for the proper conditions.
     */
    mock.assertPeSize(reqPes);
    mock.assertPodSize(reqPes);
  }

  @Test
  public void stress_10() {
    runStressTest(6, 2, 2, 10);
  }

  @Test
  @Ignore
  public void stress_100() {
    runStressTest(50, 25, 25, 100);
  }

  @Test
  @Ignore
  public void stress_500() {
    runStressTest(250, 125, 125, 500);
  }
}
