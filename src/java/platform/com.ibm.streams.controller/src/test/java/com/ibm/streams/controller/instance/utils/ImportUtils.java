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

package com.ibm.streams.controller.instance.utils;

import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_GENERATION_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_JOB_LABEL_KEY;

import com.ibm.streams.controller.crds.imports.Import;
import com.ibm.streams.controller.crds.imports.ImportSpec;
import com.ibm.streams.instance.sam.model.topology.ImportedStreams;
import com.ibm.streams.instance.sam.model.topology.NameBasedImport;
import com.ibm.streams.instance.sam.model.topology.PropertyBasedImport;
import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import java.math.BigInteger;
import java.util.HashMap;
import lombok.var;

public class ImportUtils {

  public static Import makeNamedImport(
      String appName,
      String operName,
      String streamName,
      String jobName,
      BigInteger peId,
      BigInteger portId) {
    /*
     * Build a named import.
     */
    var is = new ImportedStreams();
    is.getNameBasedImports()
        .put(BigInteger.ZERO, new NameBasedImport(appName, operName, streamName, "Default"));
    /*
     * Return the import.
     */
    return makeImport(is, jobName, peId, portId);
  }

  public static Import makeNamedImportWithFilter(
      String appName,
      String operName,
      String streamName,
      String filter,
      String jobName,
      BigInteger peId,
      BigInteger portId) {
    /*
     * Build a named import.
     */
    var is = new ImportedStreams();
    is.getNameBasedImports()
        .put(BigInteger.ZERO, new NameBasedImport(appName, operName, streamName, "Default"));
    is.setFilter(filter);
    /*
     * Return the import.
     */
    return makeImport(is, jobName, peId, portId);
  }

  public static Import makeSubscriptionImport(
      String subscription,
      String operName,
      BigInteger index,
      String jobName,
      BigInteger peId,
      BigInteger portId) {
    /*
     * Build a subscription import.
     */
    var is = new ImportedStreams();
    is.setPropertyBasedImport(new PropertyBasedImport(subscription, operName, index, "Default"));
    /*
     * Return the import.
     */
    return makeImport(is, jobName, peId, portId);
  }

  public static Import makeSubscriptionImportWithFilter(
      String subscription,
      String operName,
      BigInteger index,
      String filter,
      String jobName,
      BigInteger peId,
      BigInteger portId) {
    /*
     * Build a subscription import.
     */
    var is = new ImportedStreams();
    is.setPropertyBasedImport(new PropertyBasedImport(subscription, operName, index, "Default"));
    is.setFilter(filter);
    /*
     * Return the import.
     */
    return makeImport(is, jobName, peId, portId);
  }

  private static Import makeImport(
      ImportedStreams is, String jobName, BigInteger peId, BigInteger portId) {
    /*
     * Build the metadata.
     */
    var labels = new HashMap<String, String>();
    labels.put(STREAMS_JOB_LABEL_KEY, jobName);
    labels.put(STREAMS_GENERATION_LABEL_KEY, BigInteger.ZERO.toString());
    var meta = new ObjectMeta();
    meta.setLabels(labels);
    meta.setName(jobName + "-" + peId + "-" + portId);
    meta.setNamespace(MockResource.KUBE_NAMESPACE);
    /*
     * Build the spec.
     */
    var spec = new ImportSpec();
    spec.setJobId(BigInteger.ZERO);
    spec.setPeId(peId);
    spec.setPortId(portId);
    spec.setStreams(is);
    /*
     * Build and return the export.
     */
    var imp = new Import();
    imp.setMetadata(meta);
    imp.setSpec(spec);
    /*
     * Return the import.
     */
    return imp;
  }
}
