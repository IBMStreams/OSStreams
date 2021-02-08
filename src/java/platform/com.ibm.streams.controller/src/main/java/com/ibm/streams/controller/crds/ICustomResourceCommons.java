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

package com.ibm.streams.controller.crds;

public interface ICustomResourceCommons {

  String STREAMS_CRD_GROUP = "streams.ibm.com";
  String STREAMS_CRD_VERSION = "v1";

  static String GROUP(String name) {
    return STREAMS_CRD_GROUP + "/" + name;
  }

  String STREAMS_API_VERSION = GROUP(STREAMS_CRD_VERSION);

  String STREAMS_APP_LABEL_KEY = "app";
  String STREAMS_APP_LABEL_VALUE = "streams";

  /*
   * Docker UID/GID.
   */
  Long STREAMS_DOCKER_UID = 2098L;
  Long STREAMS_DOCKER_GID = 2015L;

  /*
   * We cannot scope SVC here because we use this label for service selectors, and
   * apparently service selectors don't support forward slashes.
   */

  String STREAMS_SVC_LABEL_KEY = "svc";
  String STREAMS_SVC_JOB_LABEL_VALUE = "job";
  String STREAMS_SVC_PE_LABEL_VALUE = "pe";
  String STREAMS_SVC_RUNNER_LABEL_VALUE = "runner";
  String STREAMS_SVC_ZOMBIE_LABEL_VALUE = "zombie";
  String STREAMS_SVC_CR_LABEL_VALUE = "consistent-region-operator";

  String STREAMS_PE_LABEL_KEY = GROUP("pe");
  String STREAMS_JOB_LABEL_KEY = GROUP("job");
  String STREAMS_CR_LABEL_KEY = GROUP("cr");
  String STREAMS_LAUNCH_COUNT_LABEL_KEY = GROUP("launch-count");
  String STREAMS_GENERATION_LABEL_KEY = GROUP("generation");

  String STREAMS_TESTSUITE_LABEL_KEY = GROUP("testsuite");
  String STREAMS_TEST_LABEL_KEY = GROUP("test");

  Integer STREAMS_PE_BASE_PORT = 10000;
  Integer STREAMS_PE_CONSISENT_REGION_PORT = 10001;

  Integer STREAMS_API_PORT = 10000;
  Integer STREAMS_METRIC_PORT = 9696;

  String STREAMS_APP_NAME_ANNOTATION_KEY = GROUP("applicationName");
  String STREAMS_APP_SCOPE_ANNOTATION_KEY = GROUP("applicationScope");
  String STREAMS_CONTENT_ID_ANNOTATION_KEY = GROUP("contentId");
  String STREAMS_REDIS_CONFIG_ANNOTATION_KEY = GROUP("redisConfig");
}
