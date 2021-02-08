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

package com.ibm.streams.controller.instance.jobs;

import com.ibm.streams.controller.crds.jobs.Job;
import java.math.BigInteger;

public class JobUtils {

  public static String getProcessingElementName(Job job, BigInteger id) {
    return getProcessingElementName(job.getMetadata().getName(), id.toString());
  }

  public static String getProcessingElementName(String job, String id) {
    return job + "-" + id;
  }
}
