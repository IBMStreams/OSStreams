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

package com.ibm.streams.controller.k8s.pods;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_CONTENT_ID_ANNOTATION_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_LAUNCH_COUNT_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.pes.ProcessingElement;
import io.fabric8.kubernetes.api.model.Pod;
import java.math.BigInteger;
import java.util.Optional;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;

public class PodStore extends ConcurrentHashMap<Object, Pod> {

  public Optional<Pod> getPodForProcessingElement(ProcessingElement pe) {
    var jobName = pe.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY);
    var id = pe.getSpec().getId();
    var hash = pe.getSpec().getContentId();
    return values().stream()
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY).equals(id.toString()))
        .filter(
            p ->
                p.getMetadata()
                    .getAnnotations()
                    .get(STREAMS_CONTENT_ID_ANNOTATION_KEY)
                    .equals(hash))
        .findAny();
  }

  public Optional<Pod> getPodWithJobNameAndId(
      String jobName, BigInteger id, Integer launchCount, String peUid) {
    return values().stream()
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_JOB_LABEL_KEY).equals(jobName))
        .filter(p -> p.getMetadata().getLabels().get(STREAMS_PE_LABEL_KEY).equals(id.toString()))
        .filter(
            p ->
                p.getMetadata()
                    .getLabels()
                    .get(STREAMS_LAUNCH_COUNT_LABEL_KEY)
                    .equals(launchCount.toString()))
        .filter(p -> p.getMetadata().getOwnerReferences().get(0).getUid().equals(peUid))
        .findAny();
  }

  public long countPodsWithJob(Job job) {
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
        .count();
  }
}
