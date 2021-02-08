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

package com.ibm.streams.controller.crds.imports;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;
import lombok.var;

public class ImportStore extends ConcurrentHashMap<Object, Import> {

  public void put(Import imp) {
    put(imp.getMetadata().getName(), imp);
  }

  public long countImportsWithJobAndGenerationId(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(i -> i.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            i ->
                i.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }

  public long countImportsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return countImportsWithJobAndGenerationId(jobName, jobGenerationId);
  }

  public long countImportsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return 0;
    }
    var preGenerationId = jobGenerationId.subtract(BigInteger.ONE).toString();
    return countImportsWithJobAndGenerationId(jobName, preGenerationId);
  }

  Optional<Import> getImportWithName(String name) {
    return values().stream().filter(e -> e.getMetadata().getName().equals(name)).findFirst();
  }

  Optional<Import> getImportWithNameAndGenerationId(String name, String jobGenerationId) {
    return values().stream()
        .filter(
            i ->
                i.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(e -> e.getMetadata().getName().equals(name))
        .findFirst();
  }

  public List<Import> getImportsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return getImportsWithJobAndGenerationId(jobName, jobGenerationId);
  }

  private List<Import> getImportsWithJobAndGenerationId(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(i -> i.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            i ->
                i.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .collect(Collectors.toList());
  }

  public List<Import> getImportsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return new ArrayList<>();
    }
    var preGenerationId = jobGenerationId.subtract(BigInteger.ONE).toString();
    return getImportsWithJobAndGenerationId(jobName, preGenerationId);
  }
}
