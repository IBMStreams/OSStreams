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

package com.ibm.streams.controller.state.subscription;

import com.fasterxml.jackson.databind.annotation.JsonDeserialize;
import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import java.util.Objects;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.stream.Collectors;
import lombok.var;

@JsonDeserialize
public class SubscriptionBoard extends ConcurrentHashMap<String, Subscriptions> {

  /*
   * This is called by ExportController::onAddition and ImportController::onAddition.
   */
  public void add(Export exp, Import imp) {
    var expPeName = ImportExportCommons.getPeNameForExport(exp);
    computeIfAbsent(expPeName, k -> new Subscriptions()).updateExportWithImport(exp, imp);
    var impPeName = ImportExportCommons.getPeNameForImport(imp);
    computeIfAbsent(impPeName, k -> new Subscriptions()).updateImportWithExport(imp, exp);
  }

  /*
   * This is called by ExportController::onDeletion.
   */
  public Set<String> remove(Export exp) {
    var result =
        entrySet().stream()
            .map(e -> e.getValue().remove(exp) ? e.getKey() : null)
            .filter(Objects::nonNull)
            .collect(Collectors.toSet());
    entrySet()
        .removeIf(e -> e.getValue().getExports().isEmpty() && e.getValue().getImports().isEmpty());
    return result;
  }

  /*
   * This is called by ImportController::onDeletion.
   */
  public Set<String> remove(Import imp) {
    var result =
        entrySet().stream()
            .map(e -> e.getValue().remove(imp) ? e.getKey() : null)
            .filter(Objects::nonNull)
            .collect(Collectors.toSet());
    entrySet()
        .removeIf(e -> e.getValue().getExports().isEmpty() && e.getValue().getImports().isEmpty());
    return result;
  }
}
