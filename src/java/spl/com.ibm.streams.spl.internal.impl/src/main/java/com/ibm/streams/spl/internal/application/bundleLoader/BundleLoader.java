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

package com.ibm.streams.spl.internal.application.bundleLoader;

import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleException;
import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;

public interface BundleLoader {
  public String getVersion() throws ApplicationBundleException;

  public String getBuildID() throws ApplicationBundleException;

  public String getApplicationDirectory() throws ApplicationBundleException;

  public String getBuildDate() throws ApplicationBundleException;

  public String getOutputDirectory() throws ApplicationBundleException;

  public InputStream getBundleInfo() throws ApplicationBundleException;

  public void extractADLFile(String targetDir) throws ApplicationBundleException;

  public void extractFiles(String targetAppDirName) throws ApplicationBundleException;

  public String extractADLFileToString() throws ApplicationBundleException;

  public String toolkitList(boolean filesToo) throws ApplicationBundleException;

  public int runStandalone(ArrayList<String> args) throws IOException;

  public String getExecutionMode() throws ApplicationBundleException;
}
