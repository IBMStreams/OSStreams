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

package com.ibm.streams.controller.crds.pes;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.function.Function;
import java.util.stream.Collectors;
import java.util.stream.Stream;
import lombok.var;

public class ProcessingElementStore extends ConcurrentHashMap<Object, ProcessingElement> {

  public void put(ProcessingElement pe) {
    put(pe.getMetadata().getName(), pe);
  }

  public Optional<ProcessingElement> getPeWithNameAndUid(String name, String uid) {
    return values().stream()
        .filter(j -> j.getMetadata().getName().equals(name))
        .filter(j -> j.getMetadata().getUid().equals(uid))
        .findAny();
  }

  public Optional<ProcessingElement> getPeWithNameAndJobId(String name, String jobId) {
    return values().stream()
        .filter(j -> j.getMetadata().getName().equals(name))
        .filter(j -> j.getMetadata().getOwnerReferences().get(0).getUid().equals(jobId))
        .findAny();
  }

  public Optional<ProcessingElement> getPeWithNameAndGenerationId(
      String name, String jobGenerationId) {
    return values().stream()
        .filter(p -> p.getMetadata().getName().equals(name))
        .filter(
            p ->
                p.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .findAny();
  }

  public Optional<ProcessingElement> getPeWithName(String name) {
    return values().stream().filter(j -> j.getMetadata().getName().equals(name)).findAny();
  }

  public Optional<ProcessingElement> getPeWithJobAndId(Job job, BigInteger peId) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return values().stream()
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            p ->
                p.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(p -> p.getSpec().getId().equals(peId))
        .findFirst();
  }

  private Stream<ProcessingElement> streamPesWithJob(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            p ->
                p.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId));
  }

  public long countPesWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return streamPesWithJob(jobName, jobGenerationId).count();
  }

  public long countPesWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return 0;
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE);
    return streamPesWithJob(jobName, preGenerationId.toString()).count();
  }

  public Map<BigInteger, ProcessingElement> getPesWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return streamPesWithJob(jobName, jobGenerationId)
        .collect(Collectors.toMap(pe -> pe.getSpec().getId(), Function.identity()));
  }

  public Map<BigInteger, ProcessingElement> getPesWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return new HashMap<>();
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE);
    return streamPesWithJob(jobName, preGenerationId.toString())
        .collect(Collectors.toMap(pe -> pe.getSpec().getId(), Function.identity()));
  }

  public Map<BigInteger, ProcessingElement> getPreviousPesWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var prevJobGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE).toString();
    return streamPesWithJob(jobName, prevJobGenerationId)
        .collect(Collectors.toMap(pe -> pe.getSpec().getId(), e -> e));
  }
}
