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

import com.ibm.streams.controller.crds.exports.EExportedStreamType;
import com.ibm.streams.controller.crds.exports.Export;
import com.ibm.streams.controller.crds.exports.ExportSpec;
import com.ibm.streams.instance.sam.model.topology.ExportedStream;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import lombok.var;

public class ExportUtils {

  public static Export makeNamedExport(
      String operName, String streamName, String jobName, BigInteger peId, BigInteger portId) {
    /*
     * Build a dummy exporter stream property.
     */
    var prop = new StreamProperty("prop0", "someType", Arrays.asList("a", "b"));
    var props = new ArrayList<StreamProperty>();
    props.add(prop);
    var es = new ExportedStream(operName, streamName, true, null, null);
    /*
     * Return the export.
     */
    return makeExport(es, jobName, peId, portId);
  }

  public static Export makePropertyExport(
      String operName,
      List<StreamProperty> properties,
      String jobName,
      BigInteger peId,
      BigInteger portId) {
    /*
     * Build a dummy exporter stream property.
     */
    var prop = new StreamProperty("prop0", "someType", Arrays.asList("a", "b"));
    var props = new ArrayList<StreamProperty>();
    props.add(prop);
    var es = new ExportedStream(operName, null, true, null, props);
    /*
     * Return the export.
     */
    return makeExport(es, jobName, peId, portId);
  }

  private static Export makeExport(
      ExportedStream es, String jobName, BigInteger peId, BigInteger portId) {
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
    var spec = new ExportSpec();
    spec.setPeId(peId);
    spec.setPortId(portId);
    spec.setType(
        es.getName() == null ? EExportedStreamType.ByProperties : EExportedStreamType.ByName);
    spec.setStream(es);
    /*
     * Build and return the export.
     */
    var exp = new Export();
    exp.setMetadata(meta);
    exp.setSpec(spec);
    /*
     * Return the export.
     */
    return exp;
  }
}
