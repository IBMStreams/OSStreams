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
import lombok.Getter;
import lombok.Setter;

@JsonDeserialize
public class Subscriptions {

  @Getter @Setter private ExportSubscriptions exports;
  @Getter @Setter private ImportSubscriptions imports;

  public Subscriptions() {
    this.exports = new ExportSubscriptions();
    this.imports = new ImportSubscriptions();
  }

  public void updateExportWithImport(Export exp, Import imp) {
    exports.add(exp, imp);
  }

  public void updateImportWithExport(Import imp, Export exp) {
    imports.add(imp, exp);
  }

  public boolean remove(Export exp) {
    return exports.remove(exp) || imports.remove(exp);
  }

  public boolean remove(Import imp) {
    return exports.remove(imp) || imports.remove(imp);
  }
}
