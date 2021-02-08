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

package com.ibm.streams.mock;

import java.util.Optional;
import org.junit.rules.ExternalResource;

public class MockResourceBase extends ExternalResource {

  public static final String SOURCE_PATH =
      Optional.ofNullable(System.getenv("SOURCE_PATH")).orElse("/source");

  public static final String PLATFORM_PATH = SOURCE_PATH + "/src/java/platform/";
  public static final String PACKAGE_PATH = PLATFORM_PATH + "com.ibm.streams.controller/";

  public static final String APPS_PATH = PACKAGE_PATH + "src/test/resources/apps/";
  public static final String CRDS_PATH = PACKAGE_PATH + "crds/";
  public static final String TESTCASES_PATH = PACKAGE_PATH + "src/test/resources/testcases/";

  public static final String KUBE_NAMESPACE = "test";
}
