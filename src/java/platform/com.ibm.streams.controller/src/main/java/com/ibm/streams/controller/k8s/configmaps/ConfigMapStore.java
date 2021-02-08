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

package com.ibm.streams.controller.k8s.configmaps;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import io.fabric8.kubernetes.api.model.ConfigMap;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;
import lombok.var;

public class ConfigMapStore extends ConcurrentHashMap<Object, ConfigMap> {

  public List<ConfigMap> getConfigMapsForJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return values().stream()
        .filter(cm -> cm.getMetadata().getLabels().containsKey(STREAMS_PE_LABEL_KEY))
        .filter(cm -> cm.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            cm ->
                cm.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .collect(Collectors.toList());
  }

  public Optional<ConfigMap> getConfigMapForProcessingElement(ProcessingElement pe) {
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var jobGenerationId = pe.getMetadata().getLabels().get(STREAMS_GENERATION_LABEL_KEY);
    var peId = pe.getSpec().getId();
    var peHash = pe.getSpec().getContentId();
    return values().stream()
        .filter(cm -> cm.getMetadata().getLabels().containsKey(STREAMS_PE_LABEL_KEY))
        .filter(cm -> cm.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            cm ->
                cm.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .filter(
            cm -> cm.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY).equals(peId.toString()))
        .filter(
            cm ->
                cm.getMetadata()
                    .getAnnotations()
                    .get(STREAMS_CONTENT_ID_ANNOTATION_KEY)
                    .equals(peHash))
        .findAny();
  }

  private long countConfigMapWithJobAndGenerationId(String jobName, String jobGenerationId) {
    return values().stream()
        .filter(c -> c.getMetadata().getLabels().containsKey(STREAMS_JOB_LABEL_KEY))
        .filter(c -> c.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            c ->
                c.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .count();
  }

  public long countConfigMapsWithJob(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId().toString();
    return countConfigMapWithJobAndGenerationId(jobName, jobGenerationId);
  }

  public long countConfigMapsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return 0;
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE).toString();
    return countConfigMapWithJobAndGenerationId(jobName, preGenerationId);
  }

  private List<ConfigMap> getConfigMapWithJobAndGenerationId(
      String jobName, String jobGenerationId) {
    return values().stream()
        .filter(c -> c.getMetadata().getLabels().containsKey(STREAMS_JOB_LABEL_KEY))
        .filter(c -> c.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(
            c ->
                c.getMetadata()
                    .getLabels()
                    .get(STREAMS_GENERATION_LABEL_KEY)
                    .equals(jobGenerationId))
        .collect(Collectors.toList());
  }

  public List<ConfigMap> getConfigMapsWithJobAndPreviousGenerationId(Job job) {
    var jobName = job.getMetadata().getName();
    var jobGenerationId = job.getSpec().getGenerationId();
    if (jobGenerationId.equals(BigInteger.ZERO)) {
      return new ArrayList<>();
    }
    var preGenerationId = job.getSpec().getGenerationId().subtract(BigInteger.ONE).toString();
    return getConfigMapWithJobAndGenerationId(jobName, preGenerationId);
  }

  /*
   * Note that we assume here that the store has only one entry for a
   * hash value. This invariant is enforced in the ConfigMapFactory.
   */
  Optional<String> getConfigMapNameForContentId(String hash) {
    return values().stream()
        .filter(
            c ->
                c.getMetadata()
                    .getAnnotations()
                    .get(STREAMS_CONTENT_ID_ANNOTATION_KEY)
                    .equals(hash))
        .map(c -> c.getMetadata().getName())
        .findAny();
  }
}
