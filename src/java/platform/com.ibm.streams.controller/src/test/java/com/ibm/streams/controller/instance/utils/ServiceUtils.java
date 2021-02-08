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
import static com.ibm.streams.controller.crds.ICustomResourceCommons.STREAMS_PE_LABEL_KEY;

import com.ibm.streams.mock.instance.MockResource;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import io.fabric8.kubernetes.api.model.Service;
import io.fabric8.kubernetes.api.model.ServicePort;
import io.fabric8.kubernetes.api.model.ServiceSpec;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import lombok.var;

public class ServiceUtils {

  public static Service makeService(String jobName, BigInteger peId, List<ServicePort> ports) {
    /*
     * Build the metadata.
     */
    Map<String, String> labels = new HashMap<>();
    labels.put(STREAMS_JOB_LABEL_KEY, jobName);
    labels.put(STREAMS_GENERATION_LABEL_KEY, "0");
    labels.put(STREAMS_PE_LABEL_KEY, "0");
    var meta = new ObjectMeta();
    meta.setLabels(labels);
    meta.setName(jobName + "-" + peId + "-");
    meta.setNamespace(MockResource.KUBE_NAMESPACE);
    /*
     * Build the spec.
     */
    var spec = new ServiceSpec();
    spec.setType("ClusterIP");
    spec.setPorts(ports);
    /*
     * Build and return the export.
     */
    var svc = new Service();
    svc.setMetadata(meta);
    svc.setSpec(spec);
    /*
     * Return the export.
     */
    return svc;
  }
}
