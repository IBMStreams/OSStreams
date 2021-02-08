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
import java.math.BigInteger;
import java.util.concurrent.ConcurrentHashMap;
import lombok.var;

@JsonDeserialize
public class ExportSubscriptions extends ConcurrentHashMap<String, ExportSubscription> {

  public void add(Export exp, Import imp) {
    BigInteger portId = exp.getSpec().getPortId();
    computeIfAbsent(ImportExportCommons.getLabelForExport(exp), k -> new ExportSubscription(portId))
        .add(imp);
  }

  public boolean remove(Export exp) {
    return remove(ImportExportCommons.getLabelForExport(exp)) != null;
  }

  public boolean remove(Import imp) {
    var res = values().stream().map(s -> s.remove(imp)).reduce(Boolean::logicalAnd);
    entrySet().removeIf(e -> e.getValue().getEntries().isEmpty());
    return res.orElse(false);
  }
}
