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
import static org.junit.Assert.assertTrue;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import java.util.List;
import java.util.stream.Collectors;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;

public class HostPoolsTest {

  @Rule public MockResource mock = new MockResource();

  private final int defaultHostPool = 1;

  @Test
  public void addHostPoolsForJob() {
    String jobName = "hostpoolsTest";
    Job job = JobUtils.makeJob(jobName, "apps.hostpools.HostPools.sab", 3);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted(jobName, 60);
    /*
     * The sab has 2 explicit hostpools, and we always also have the 1 default hostpool.
     */
    assertEquals(2 + defaultHostPool, mock.getHostPoolClient().list().getItems().size());
    assertEquals(2 + defaultHostPool, mock.getHostPoolStore().size());
    /*
     * Make sure that the pods have the affinity expressions associated with a
     * hostpool.
     */
    mock.assertPeSize(3);
    mock.assertPodSize(3);
    job = mock.getJobStore().getJobWithName(jobName).get();
    var pes = mock.getProcessingElementStore().getPesWithJob(job);
    List<String> tags =
        pes.values().stream()
            .map(
                pe ->
                    mock.getPodStore().getPodForProcessingElement(pe).get().getSpec().getAffinity())
            .filter(affinity -> affinity != null)
            .map(affinity -> affinity.getNodeAffinity())
            .filter(nodeAffinity -> nodeAffinity != null)
            .map(
                nodeAffinity ->
                    nodeAffinity
                        .getRequiredDuringSchedulingIgnoredDuringExecution()
                        .getNodeSelectorTerms())
            .flatMap(List::stream)
            .map(term -> term.getMatchExpressions())
            .flatMap(List::stream)
            .filter(exp -> exp.getKey().equals("streams.ibm.com/hostpool"))
            .map(exp -> exp.getValues())
            .flatMap(List::stream)
            .collect(Collectors.toList());

    assertTrue(tags.contains("sink_hosts"));
    assertTrue(tags.contains("source_hosts"));
  }
}
