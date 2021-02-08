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
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;
import lombok.Getter;
import lombok.Setter;
import lombok.var;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

@JsonDeserialize
public class ImportSubscription {

  private static final Logger LOGGER = LoggerFactory.getLogger(ImportSubscription.class);

  @Getter @Setter private BigInteger portId;
  @Getter @Setter private Map<String, ExportSubscriptionEntry> entries;

  public ImportSubscription() {
    portId = BigInteger.ZERO;
    entries = new ConcurrentHashMap<>();
  }

  ImportSubscription(BigInteger portId) {
    this.portId = portId;
    entries = new HashMap<>();
  }

  public void add(Export exp) {
    var key = ImportExportCommons.getLabelForExport(exp);
    var sub = new ExportSubscriptionEntry(exp);
    entries.putIfAbsent(key, sub);
  }

  public boolean remove(Export exp) {
    var key = ImportExportCommons.getLabelForExport(exp);
    return entries.remove(key) != null;
  }
}
