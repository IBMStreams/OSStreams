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

package com.ibm.streams.controller.crds.prs;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import java.math.BigInteger;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;
import lombok.var;

public class ParallelRegionStore extends ConcurrentHashMap<Object, ParallelRegion> {

  public void put(ParallelRegion pr) {
    put(pr.getMetadata().getName(), pr);
  }

  public long countParallelRegionsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return values().stream()
        .filter(pr -> pr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            pr ->
                pr.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }

  public Optional<ParallelRegion> getParallelRegionWithAlias(String jobName, String alias) {
    return values().stream()
        .filter(pr -> pr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(pr -> pr.getSpec().getAlias().equals(alias))
        .findAny();
  }

  public Optional<ParallelRegion> getParallelRegionWithJobAndAlias(Job job, String alias) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return values().stream()
        .filter(pr -> pr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            pr ->
                pr.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(pr -> pr.getSpec().getAlias().equals(alias))
        .findFirst();
  }

  Optional<ParallelRegion> getParallelRegionWithName(String name) {
    return values().stream().filter(pr -> pr.getMetadata().getName().equals(name)).findFirst();
  }

  Map<String, BigInteger> getParallelRegionWidthsForJob(String jobName) {
    return values().stream()
        .filter(pr -> pr.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .map(ParallelRegion::getSpec)
        .collect(Collectors.toMap(ParallelRegionSpec::getAlias, ParallelRegionSpec::getWidth));
  }
}
