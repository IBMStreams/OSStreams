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

import io.fabric8.kubernetes.api.model.KubernetesResource;
import io.fabric8.kubernetes.api.model.PodAffinityTerm;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.Stream;

public abstract class AsymmetricPodAffinityLabels {
  Map<String, LabeledPodAffinityTerms>
      symmetric; // this pod publishes these labels as well as its affinity terms
  Map<String, LabeledPodAffinityTerms>
      asymmetric; // this pod does NOT publish these labels, but does publish its affinity terms

  public AsymmetricPodAffinityLabels() {
    this.symmetric = new HashMap<>();
    this.asymmetric = new HashMap<>();
  }

  public AsymmetricPodAffinityLabels addSymmetric(String kind) {
    symmetric.put(kind, new LabeledPodAffinityTerms(kind));
    return this;
  }

  public AsymmetricPodAffinityLabels addAsymmetric(String kind) {
    asymmetric.put(kind, new LabeledPodAffinityTerms(kind));
    return this;
  }

  public void addSymmetricLabel(String kind, String label) {
    symmetric.get(kind).add(label);
  }

  public void addAsymmetricLabel(String kind, String label) {
    asymmetric.get(kind).add(label);
  }

  public Map<String, String> getLabels() {
    return symmetric.entrySet().stream()
        .map(e -> e.getValue().getLabels().entrySet())
        .flatMap(labels -> labels.stream())
        .collect(Collectors.toMap(entry -> entry.getKey(), entry -> entry.getValue()));
  }

  public List<PodAffinityTerm> getPodAffinityTerms() {
    return Stream.concat(symmetric.entrySet().stream(), asymmetric.entrySet().stream())
        .map(e -> e.getValue().getPodAffinityTerms())
        .flatMap(terms -> terms.stream())
        .collect(Collectors.toList());
  }

  public boolean isEmpty() {
    return symmetric.isEmpty() && asymmetric.isEmpty();
  }

  public abstract KubernetesResource build();
}
