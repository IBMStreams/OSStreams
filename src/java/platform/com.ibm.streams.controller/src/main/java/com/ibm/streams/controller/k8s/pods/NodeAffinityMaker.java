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

import io.fabric8.kubernetes.api.model.NodeAffinity;
import io.fabric8.kubernetes.api.model.NodeAffinityBuilder;
import io.fabric8.kubernetes.api.model.NodeSelectorBuilder;
import io.fabric8.kubernetes.api.model.NodeSelectorRequirement;
import io.fabric8.kubernetes.api.model.NodeSelectorTerm;
import io.fabric8.kubernetes.api.model.NodeSelectorTermBuilder;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.stream.Collectors;

public class NodeAffinityMaker {
  private final String hostpoolKey = "streams.ibm.com/hostpool";
  private final List<String> values;

  public NodeAffinityMaker() {
    this.values = new ArrayList<>();
  }

  public NodeAffinityMaker add(String node) {
    values.add(node);
    return this;
  }

  private List<NodeSelectorTerm> getNodeSelectorTerms() {
    return values.stream()
        .map(
            v ->
                new NodeSelectorTermBuilder()
                    .withMatchExpressions(
                        new NodeSelectorRequirement(
                            hostpoolKey, "In", Collections.singletonList(v)))
                    .build())
        .collect(Collectors.toList());
  }

  public NodeAffinity build() {
    return new NodeAffinityBuilder()
        .withRequiredDuringSchedulingIgnoredDuringExecution(
            new NodeSelectorBuilder().withNodeSelectorTerms(getNodeSelectorTerms()).build())
        .build();
  }
}
