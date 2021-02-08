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

package com.ibm.streams.controller.utils;

import com.ibm.streams.controller.bundle.Bundle;
import com.ibm.streams.controller.bundle.IBundleLoader;
import com.ibm.streams.controller.bundle.KubernetesBundleLoader;
import com.ibm.streams.controller.instance.sam.ADLUtils;
import io.fabric8.kubernetes.client.KubernetesClient;
import java.util.Optional;

public class KubernetesEnvironment extends CommonEnvironment {

  private final KubernetesClient client;
  private final String name;
  private final String namespace;

  public KubernetesEnvironment(KubernetesClient client) {
    this.client = client;
    this.name =
        Optional.ofNullable(System.getenv("MY_POD_NAME")).orElseThrow(RuntimeException::new);
    this.namespace =
        Optional.ofNullable(System.getenv("MY_POD_NAMESPACE")).orElseThrow(RuntimeException::new);
  }

  @Override
  public Optional<String> getCheckpointPath() {
    return Optional.ofNullable(System.getenv("STREAMS_CHECKPOINT_PATH"));
  }

  @Override
  public Optional<String> getDataPath() {
    return Optional.ofNullable(System.getenv("STREAMS_DATA_PATH"));
  }

  @Override
  public String getHarnessDataPath() {
    return Optional.ofNullable(System.getenv("HARNESS_DATA_PATH")).orElse("/tmp");
  }

  @Override
  public String getTestRoot() {
    return Optional.ofNullable(System.getenv("STREAMS_TEST_DIR")).orElse("/test");
  }

  @Override
  public String getTestVolumeServer() {
    return "localhost";
  }

  @Override
  public String getTestVolumeRemoteDir() {
    return "/tmp";
  }

  @Override
  public IBundleLoader getBundleLoader() {
    return new KubernetesBundleLoader(client);
  }

  @Override
  public Optional<String> getADL(Bundle bundle) {
    return ADLUtils.getADLFromBundle(bundle);
  }

  @Override
  public int getDefaultNumberOfPEs() {
    return client.nodes().list().getItems().size();
  }

  @Override
  public boolean enableNotifications() {
    return true;
  }
}
