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

package com.ibm.streams.controller.crds.exports;

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

public class ExportStore extends ConcurrentHashMap<Object, Export> {

  public void put(Export exp) {
    put(exp.getMetadata().getName(), exp);
  }

  private long countExportsWithJobAndGenerationId(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(e -> e.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            e ->
                e.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }

  public long countExportsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return countExportsWithJobAndGenerationId(jobName, jobGenerationId);
  }

  public long countExportsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return 0;
    }
    var preGenerationId = jobGenerationId.subtract(BigInteger.ONE).toString();
    return countExportsWithJobAndGenerationId(jobName, preGenerationId);
  }

  Optional<Export> getExportWithName(String name) {
    return values().stream().filter(e -> e.getMetadata().getName().equals(name)).findFirst();
  }

  Optional<Export> getExportWithNameAndGenerationId(String name, String jobGenerationId) {
    return values().stream()
        .filter(
            e ->
                e.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(e -> e.getMetadata().getName().equals(name))
        .findFirst();
  }

  private List<Export> getExportsWithJobNameAndGenerationId(
      String jobName, String jobGenerationId) {
    return values().stream()
        .filter(e -> e.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            e ->
                e.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .collect(Collectors.toList());
  }

  public List<Export> getExportsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return getExportsWithJobNameAndGenerationId(jobName, jobGenerationId);
  }

  public List<Export> getExportsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return new ArrayList<>();
    }
    var preGenerationId = jobGenerationId.subtract(BigInteger.ONE).toString();
    return getExportsWithJobNameAndGenerationId(jobName, preGenerationId);
  }
}
