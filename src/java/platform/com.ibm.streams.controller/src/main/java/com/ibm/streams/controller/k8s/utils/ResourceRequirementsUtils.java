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

package com.ibm.streams.controller.k8s.utils;

import io.fabric8.kubernetes.api.model.Quantity;
import java.text.DecimalFormat;
import java.util.AbstractMap;
import java.util.Arrays;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import lombok.var;

public class ResourceRequirementsUtils {
  /*
   * This formatter will drop digits past the decimal point if they
   * are zero. For example, "2.13" will stay "2.13", but "2.0" will
   * become "2".
   */
  private static final DecimalFormat dropZeros = new DecimalFormat("#.###");

  private static String divideAmount(String amount, long x) {
    List<String> validSuffixes =
        Arrays.asList(
            /*
             * The valid memory suffixes. See:
             *   https://kubernetes.io/docs/tasks/configure-pod-container/assign-memory-resource/
             */
            "E",
            "P",
            "T",
            "G",
            "M",
            "K",
            "Ei",
            "Pi",
            "Ti",
            "Gi",
            "Mi",
            "Ki",
            /*
             * The valid cpu suffixes. See:
             *   https://kubernetes.io/docs/tasks/configure-pod-container/assign-cpu-resource/
             */
            "m");
    /*
     * (amount, suffix) pair
     */
    var pair =
        validSuffixes.stream()
            .filter(suffix -> amount.endsWith(suffix))
            .map(suffix -> new AbstractMap.SimpleEntry<>(amount.replace(suffix, ""), suffix))
            .findAny()
            .orElse(new AbstractMap.SimpleEntry<>(amount, ""));
    /*
     * Perform the division on the amount, then turn it back into a string and
     * append the suffix.
     */
    return dropZeros.format(Double.parseDouble(pair.getKey()) / x) + pair.getValue();
  }

  public static Map<String, Quantity> divideQuantities(Map<String, Quantity> quantities, long x) {
    return quantities.entrySet().stream()
        .collect(
            Collectors.toMap(
                e -> e.getKey(),
                e ->
                    new Quantity(
                        divideAmount(e.getValue().getAmount(), x), e.getValue().getFormat())));
  }
}
