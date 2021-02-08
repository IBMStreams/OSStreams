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

import com.ibm.streams.controller.crds.jobs.Job;
import com.ibm.streams.controller.crds.prs.ParallelRegion;
import com.ibm.streams.controller.crds.prs.ParallelRegionSpec;
import io.fabric8.kubernetes.api.model.ObjectMeta;
import java.math.BigInteger;
import java.util.HashMap;
import lombok.var;

public class ParallelRegionUtils {

  public static ParallelRegion makeParallelRegion(
      Job job, String name, String alias, BigInteger width) {
    /*
     * Create the dummy spec.
     */
    var spec = new ParallelRegionSpec();
    spec.setAlias(alias);
    spec.setWidth(width);
    spec.setIndex(BigInteger.ZERO);
    /*
     * Create the labels.
     */
    var labels =
        new HashMap<String, String>() {
          {
            put(STREAMS_JOB_LABEL_KEY, job.getMetadata().getName());
            put(STREAMS_GENERATION_LABEL_KEY, job.getSpec().getGenerationId().toString());
          }
        };
    /*
     * Create the metadata.
     */
    var meta = new ObjectMeta();
    meta.setLabels(labels);
    meta.setName(name);
    meta.setNamespace(job.getMetadata().getNamespace());
    /*
     * Create and return the dummy parallel region.
     */
    var pr = new ParallelRegion();
    pr.setMetadata(meta);
    pr.setSpec(spec);
    return pr;
  }
}
