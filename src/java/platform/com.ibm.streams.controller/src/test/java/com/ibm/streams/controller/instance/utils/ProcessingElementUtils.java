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
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_LABEL_KEY;
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_SVC_PE_LABEL_VALUE;

import com.ibm.streams.controller.crds.pes.ProcessingElement;
import com.ibm.streams.controller.crds.pes.ProcessingElementSpec;
import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;

public class ProcessingElementUtils {

  public static ProcessingElement makeProcessingElement(String jobName, BigInteger peId) {
    /*
     * Build the metadata.
     */
    Map<String, String> labels = new HashMap<>();
    labels.put(STREAMS_JOB_LABEL_KEY, jobName);
    labels.put(STREAMS_SVC_LABEL_KEY, STREAMS_SVC_PE_LABEL_VALUE);
    labels.put(STREAMS_GENERATION_LABEL_KEY, "0");
    ObjectMeta meta = new ObjectMeta();
    meta.setLabels(labels);
    meta.setName(jobName + "-" + peId + "-");
    meta.setNamespace(MockResource.KUBE_NAMESPACE);
    /*
     * Build the spec.
     */
    ProcessingElementSpec spec = new ProcessingElementSpec();
    spec.setId(peId);
    /*
     * Build and return the export.
     */
    ProcessingElement exp = new ProcessingElement();
    exp.setMetadata(meta);
    exp.setSpec(spec);
    /*
     * Return the export.
     */
    return exp;
  }
}
