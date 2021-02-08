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

import java.util.HashMap;
import java.util.Map;
import org.apache.commons.codec.digest.DigestUtils;

public class KubernetesLabels {
  protected final Map<String, String> labels;
  protected final String keyBase;

  public KubernetesLabels(String kind) {
    this.labels = new HashMap<>();
    this.keyBase = "streams.ibm.com/" + kind;
  }

  /*
   * Because of Kubernetes label constraints, we must:
   *     - remove all instances of `[` and `]` from values
   *     - remove all instances of `::` from values
   *     - ensure that the label starts and ends with
   *       alpha-numeric characters; SPL allows identifiers
   *       to start and end with `_`
   *     - restrict the value to 63 characters
   * We will then truncate the value to size 63 and map:
   *     - `[`, `]` -> `-`
   *     - `::` -> `.`
   *     - leading `_` -> `u_`
   *     - trailing `_` -> `_u`
   *     - leading `.` -> `d.`
   * See:
   *     https://kubernetes.io/docs/concepts/overview/working-with-objects/labels/#syntax-and-character-set
   *     https://www.ibm.com/support/knowledgecenter/SSCRJU_4.3.0/com.ibm.streams.ref.doc/doc/lexicalsyntax.html
   */
  private String enforceKubernetesLabelConstraints(String value) {
    String str = value.replace('[', '-').replace(']', '-').replaceAll("::", ".");
    if (str.startsWith("_")) {
      str = "u" + str;
    }
    if (str.endsWith("_")) {
      str = str + "u";
    }
    if (str.startsWith(".")) {
      str = "d" + str;
    }
    return str.substring(0, Math.min(str.length(), 63));
  }

  public KubernetesLabels add(String value) {
    labels.put(
        keyBase + "." + DigestUtils.sha1Hex(value), enforceKubernetesLabelConstraints(value));
    return this;
  }

  public Map<String, String> getLabels() {
    return labels;
  }
}
