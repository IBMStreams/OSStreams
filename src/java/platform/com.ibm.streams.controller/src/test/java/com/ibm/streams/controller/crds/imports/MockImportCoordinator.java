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

package com.ibm.streams.controller.crds.imports;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.instance.broker.ImportExportCommons;
import com.ibm.streams.platform.services.ImportedStreamsType;
import lombok.var;
import org.microbean.kubernetes.controller.AbstractEvent;

public class MockImportCoordinator implements IImportCoordinator {

  @Override
  public void setImportFilter(Import imp, String filter) {
    imp.getSpec().getStreams().setFilter(filter);
  }

  @Override
  public void setImportImportedStreams(Import imp, ImportedStreamsType streams) {
    var ip = ImportExportCommons.getImportedStreams(streams);
    imp.getSpec().setStreams(ip);
  }

  @Override
  public void updateImport(Job job, Import imp) {
    imp.getMetadata()
        .getLabels()
        .put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
  }

  @Override
  public void onAddition(AbstractEvent<? extends Import> event) {}

  @Override
  public void onModification(AbstractEvent<? extends Import> event) {}

  @Override
  public void onDeletion(AbstractEvent<? extends Import> event) {}
}
