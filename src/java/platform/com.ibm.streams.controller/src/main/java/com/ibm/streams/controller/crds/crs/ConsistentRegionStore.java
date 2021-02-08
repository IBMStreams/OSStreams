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

package com.ibm.streams.controller.crds.crs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;

public class ConsistentRegionStore extends ConcurrentHashMap<Object, ConsistentRegion> {

  public void put(ConsistentRegion cr) {
    put(cr.getMetadata().getName(), cr);
  }

  public Optional<ConsistentRegion> getWithJobNameAndRegionId(String jobName, Integer regionIndex) {
    String crName = jobName + "-cr" + regionIndex.toString();
    return values().stream().filter(c -> c.getMetadata().getName().equals(crName)).findAny();
  }

  public long countCrsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return values().stream()
        .filter(c -> c.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            c ->
                c.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }
}
