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

package com.ibm.streams.controller.k8s.services;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import io.fabric8.kubernetes.api.model.Service;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;
import lombok.var;

public class ServiceStore extends ConcurrentHashMap<Object, Service> {

  public void put(Service svc) {
    put(svc.getMetadata().getName(), svc);
  }

  private long countServicesWithJobNameAndGenerationId(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(s -> s.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            s ->
                s.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }

  public long countServicesWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return countServicesWithJobNameAndGenerationId(jobName, jobGenerationId);
  }

  public long countServicesWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return 0;
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE).toString();
    return countServicesWithJobNameAndGenerationId(jobName, preGenerationId);
  }

  Optional<Service> getServiceWithName(String name) {
    return values().stream().filter(s -> s.getMetadata().getName().equals(name)).findAny();
  }

  public Optional<Service> getServiceWithJobPeIdAndPreviousGenerationId(Job job, BigInteger peId) {
    var oldServices = getServicesWithJobAndPreviousGenerationId(job);
    return oldServices.stream()
        .filter(s -> s.getMetadata().getLabels().containsKey(STREAMS_PE_LABEL_KEY))
        .filter(s -> s.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY).equals(peId.toString()))
        .findFirst();
  }

  public Optional<Service> getServiceForProcessingElement(ProcessingElement pe) {
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    return values().stream()
        .filter(s -> s.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            s ->
                s.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(s -> s.getMetadata().getLabels().containsKey(STREAMS_PE_LABEL_KEY))
        .filter(
            s ->
                s.getMetadata()
                    .getLabels()
                    .get(STREAMS_PE_LABEL_KEY)
                    .equals(pe.getSpec().getId().toString()))
        .findAny();
  }

  public List<Service> getServicesWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return getServicesWithJobNameAndGenerationId(jobName, jobGenerationId);
  }

  private List<Service> getServicesWithJobNameAndGenerationId(
      String jobName, String jobGenerationId) {
    return values().stream()
        .filter(s -> s.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            s ->
                s.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .collect(Collectors.toList());
  }

  public List<Service> getServicesWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return new ArrayList<>();
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE).toString();
    return getServicesWithJobNameAndGenerationId(jobName, preGenerationId);
  }
}
