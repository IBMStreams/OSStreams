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

package com.ibm.streams.controller.crds.jobs;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.ibm.streams.controller.crds.pes.ProcessingElementSpec;
import com.ibm.streams.controller.crds.prs.ParallelRegionSpec;
import io.fabric8.kubernetes.api.model.KubernetesResource;
import io.fabric8.kubernetes.api.model.ResourceRequirements;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.stream.Collectors;
import lombok.Getter;
import lombok.Setter;
import lombok.var;

@JsonDeserialize
public class JobSpec implements KubernetesResource {

  @Getter @Setter private BigInteger id;
  @Getter @Setter private BigInteger generationId;
  @Getter @Setter private BundleSpec bundle;
  @Getter @Setter private EJobSubmissionStatus status;
  @Getter @Setter private FusionSpec fusion;
  @Getter @Setter private Integer submissionTimeOutInSeconds;
  @Getter @Setter private Integer transportCertificateValidityInDays;
  @Getter @Setter private JobStatisticsSpec statistics;
  @Getter @Setter private List<ParallelRegionSpec> parallelRegions;
  @Getter @Setter private ProcessingElementSpec processingElement;
  @Getter @Setter private Properties submissionTimeValues;
  @Getter @Setter private String imagePullPolicy;
  @Getter @Setter private String imagePullSecret;
  @Getter @Setter private ThreadingModelSpec threadingModel;
  @Getter @Setter private ResourceRequirements resources;

  public JobSpec() {
    this.bundle = new BundleSpec();
    this.fusion = new FusionSpec();
    this.imagePullPolicy = "IfNotPresent";
    this.parallelRegions = new ArrayList<>();
    this.processingElement = new ProcessingElementSpec();
    this.statistics = new JobStatisticsSpec();
    this.status = EJobSubmissionStatus.Submitting;
    this.submissionTimeOutInSeconds = 300;
    this.submissionTimeValues = new Properties();
    this.threadingModel = new ThreadingModelSpec();
    this.transportCertificateValidityInDays = 30;
  }

  public Properties configurationProperties() {
    var result = new Properties();
    result.putAll(fusion.configurationProperties());
    result.putAll(threadingModel.configurationProperties());
    return result;
  }

  public Map<String, BigInteger> getParallelRegionWidths() {
    return parallelRegions.stream()
        .collect(Collectors.toMap(ParallelRegionSpec::getAlias, ParallelRegionSpec::getWidth));
  }

  public JobSpec(JobSpec spec) {
    this.id = spec.id;
    this.generationId = spec.generationId;
    this.bundle = new BundleSpec(spec.bundle);
    this.status = spec.status;
    this.fusion = new FusionSpec(spec.fusion);
    this.submissionTimeOutInSeconds = spec.submissionTimeOutInSeconds;
    this.transportCertificateValidityInDays = spec.transportCertificateValidityInDays;
    this.statistics = new JobStatisticsSpec(spec.statistics);
    this.parallelRegions = new ArrayList<>(spec.parallelRegions);
    this.processingElement = new ProcessingElementSpec(spec.processingElement);
    this.submissionTimeValues = spec.submissionTimeValues;
    this.imagePullPolicy = spec.imagePullPolicy;
    this.imagePullSecret = spec.imagePullSecret;
    this.threadingModel = new ThreadingModelSpec(spec.threadingModel);
    if (spec.resources != null) {
      this.resources =
          new ResourceRequirements(spec.resources.getLimits(), spec.resources.getRequests());
    }
  }
}
