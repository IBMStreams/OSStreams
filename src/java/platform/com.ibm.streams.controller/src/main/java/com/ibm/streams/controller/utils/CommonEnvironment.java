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
import java.util.Optional;

public abstract class CommonEnvironment {

  public String getRegistry() {
    return Optional.ofNullable(System.getenv("DOCKER_REGISTRY")).orElse("us.icr.io");
  }

  public String getRegistryNameSpace() {
    return Optional.ofNullable(System.getenv("DOCKER_NAMESPACE")).orElse("spl_team");
  }

  public String getBuildType() {
    return Optional.ofNullable(System.getenv("STREAMS_BUILD_TYPE")).orElse("debug");
  }

  public String getStreamsInstall() {
    return "/usr/local/streams";
  }

  public String getStreamsLogLevel() {
    return Optional.ofNullable(System.getenv("STREAMS_LOG_LEVEL")).orElse("INFO");
  }

  public abstract Optional<String> getCheckpointPath();

  public abstract Optional<String> getDataPath();

  public abstract String getHarnessDataPath();

  public abstract String getTestRoot();

  public abstract String getTestVolumeServer();

  public abstract String getTestVolumeRemoteDir();

  public abstract IBundleLoader getBundleLoader();

  public abstract Optional<String> getADL(Bundle bundle);

  public abstract int getDefaultNumberOfPEs();

  public abstract boolean enableNotifications();
}
