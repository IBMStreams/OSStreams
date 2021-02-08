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

package com.ibm.streams.controller.k8s.utils;

import io.fabric8.kubernetes.api.model.ConfigMap;
import java.util.Map;

public class ConfigMapUtils {

  /*
   * Note: this must match how we access the AADL in the SPL runtime. See:
   *     $INF/src/K8S/K8SApplication.cpp
   */
  private static final String AADL_KEY = "aadl.xml";

  public static String getAadl(ConfigMap cm) {
    return cm.getData().get(AADL_KEY);
  }

  public static void setAadl(Map<String, String> data, String aadl) {
    data.put(AADL_KEY, aadl);
  }
}
