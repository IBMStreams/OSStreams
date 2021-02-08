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

import com.ibm.streams.controller.bundle.Bundle;
import com.ibm.streams.controller.bundle.IBundleLoader;
import com.ibm.streams.controller.bundle.MockBundleLoader;
import com.ibm.streams.controller.instance.sam.ADLUtils;
import com.ibm.streams.controller.utils.CommonEnvironment;
import java.util.Optional;

public class MockEnvironment extends CommonEnvironment {

  private String dataPath;

  public MockEnvironment(String dataPath) {
    this.dataPath = dataPath;
  }

  @Override
  public Optional<String> getCheckpointPath() {
    return Optional.of(dataPath);
  }

  @Override
  public Optional<String> getDataPath() {
    return Optional.of(dataPath);
  }

  @Override
  public String getHarnessDataPath() {
    return "/tmp";
  }

  @Override
  public String getTestRoot() {
    return MockResourceBase.SOURCE_PATH + "/test";
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
    return new MockBundleLoader();
  }

  @Override
  public Optional<String> getADL(Bundle bundle) {
    return ADLUtils.getADLFromFile(getDataPath().get() + bundle.getName());
  }

  @Override
  public int getDefaultNumberOfPEs() {
    return 1;
  }

  @Override
  public boolean enableNotifications() {
    return false;
  }
}
