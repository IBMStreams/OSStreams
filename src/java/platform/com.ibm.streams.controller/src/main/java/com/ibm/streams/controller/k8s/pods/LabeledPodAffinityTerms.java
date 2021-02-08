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

import io.fabric8.kubernetes.api.model.LabelSelectorRequirement;
import io.fabric8.kubernetes.api.model.PodAffinityTerm;
import io.fabric8.kubernetes.api.model.PodAffinityTermBuilder;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

public class LabeledPodAffinityTerms extends KubernetesLabels {

  public LabeledPodAffinityTerms(String kind) {
    super(kind);
  }

  public List<PodAffinityTerm> getPodAffinityTerms() {
    return labels.entrySet().stream()
        .map(
            e ->
                new PodAffinityTermBuilder()
                    .withNewLabelSelector()
                    .withMatchExpressions(
                        new LabelSelectorRequirement(
                            e.getKey(), "In", Collections.singletonList(e.getValue())))
                    .endLabelSelector()
                    .withTopologyKey("kubernetes.io/hostname")
                    .build())
        .collect(Collectors.toList());
  }
}
