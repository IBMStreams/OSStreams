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

package com.ibm.streams.controller.instance;

import static org.junit.Assert.assertEquals;

import com.ibm.streams.controller.instance.utils.JobUtils;
import com.ibm.streams.mock.instance.MockResource;
import com.ibm.streams.utils.Probe;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;

public class ImportExportTest {

  private static final int DELAY_IN_SEC = 60;

  @Rule public MockResource mock = new MockResource();

  @Test
  public void addExportsForJob() {
    var job = JobUtils.makeJob("test", "apps.pubsub.Exporter.sab", 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("test", DELAY_IN_SEC);
    mock.assertExportSize(3);
  }

  @Test
  public void addImportsForJob() {
    var job = JobUtils.makeJob("test", "apps.pubsub.Importer.sab", 1);
    mock.getJobClient().createOrReplace(job);
    mock.assertJobSubmitted("test", DELAY_IN_SEC);
    mock.assertImportSize(4);
  }

  @Test
  public void checkImportMatchForExport() {
    /*
     * Create the exporting job.
     */
    var exp = JobUtils.makeJob("exporter", "apps.pubsub.Exporter.sab", 1);
    mock.getJobClient().createOrReplace(exp);
    mock.assertJobSubmitted("exporter", DELAY_IN_SEC);
    mock.assertExportSize(3);
    /*
     * Create the importing job.
     */
    var imp = JobUtils.makeJob("importer", "apps.pubsub.Importer.sab", 1);
    mock.getJobClient().createOrReplace(imp);
    mock.assertJobSubmitted("importer", DELAY_IN_SEC);
    mock.assertImportSize(4);
    /*
     * Check the subscription board.
     */
    var sb = mock.getSubscriptionBoard();
    Probe.watchUntil(DELAY_IN_SEC, sb::size, 2);
    Probe.watchUntil(DELAY_IN_SEC, () -> sb.get("exporter-0").getExports().size(), 3);
    Probe.watchUntil(DELAY_IN_SEC, () -> sb.get("importer-0").getImports().size(), 4);
    /*
     * Check the content of the exports.
     */
    var importCount =
        sb.get("exporter-0").getExports().values().stream()
            .map(e -> e.getEntries().size())
            .reduce(Integer::sum)
            .orElse(0);
    assertEquals(Integer.valueOf(6), importCount);
    /*
     * Check the content of the imports.
     */
    var exportCount =
        sb.get("importer-0").getImports().values().stream()
            .map(e -> e.getEntries().size())
            .reduce(Integer::sum)
            .orElse(0);
    assertEquals(Integer.valueOf(6), exportCount);
  }

  @Test
  public void checkExportMatchForImport() {
    /*
     * Create the importing job.
     */
    var imp = JobUtils.makeJob("importer", "apps.pubsub.Importer.sab", 1);
    mock.getJobClient().createOrReplace(imp);
    mock.assertJobSubmitted("importer", DELAY_IN_SEC);
    mock.assertImportSize(4);
    /*
     * Create the exporting job.
     */
    var exp = JobUtils.makeJob("exporter", "apps.pubsub.Exporter.sab", 1);
    mock.getJobClient().createOrReplace(exp);
    mock.assertJobSubmitted("exporter", DELAY_IN_SEC);
    mock.assertExportSize(3);
    /*
     * Check the subscription board.
     */
    var sb = mock.getSubscriptionBoard();
    Probe.watchUntil(DELAY_IN_SEC, sb::size, 2);
    Probe.watchUntil(DELAY_IN_SEC, () -> sb.get("exporter-0").getExports().size(), 3);
    Probe.watchUntil(DELAY_IN_SEC, () -> sb.get("importer-0").getImports().size(), 4);
    /*
     * Check the content of the exports.
     */
    var importCount =
        sb.get("exporter-0").getExports().values().stream()
            .map(e -> e.getEntries().size())
            .reduce(Integer::sum)
            .orElse(0);
    assertEquals(Integer.valueOf(6), importCount);
    /*
     * Check the content of the imports.
     */
    var exportCount =
        sb.get("importer-0").getImports().values().stream()
            .map(e -> e.getEntries().size())
            .reduce(Integer::sum)
            .orElse(0);
    assertEquals(Integer.valueOf(6), exportCount);
  }
}
