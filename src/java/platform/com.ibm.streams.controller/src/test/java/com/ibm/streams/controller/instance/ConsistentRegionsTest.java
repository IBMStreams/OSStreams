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
import static org.junit.Assert.assertNotNull;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import org.junit.Rule;
import org.junit.Test;

public class ConsistentRegionsTest {

  @Rule public MockResource mock = new MockResource();

  @Test
  public void parseConsistentRegionApplication() {
    Job job = JobUtils.makeJob("test", "apps.consistent.ConsistentRegion.sab", 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("test", 600);
    assertEquals(1, mock.getCRClient().list().getItems().size());
    assertEquals(1, mock.getCROClient().list().getItems().size());
    assertNotNull(mock.getDeployment("test-consistent-region-operator"));
  }
}
