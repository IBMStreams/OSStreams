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

package com.ibm.streams.admin.internal.api.application.bundle;

import java.io.InputStream;

public interface Bundle {
  public String getVersion() throws ApplicationBundleException;

  public String getApplicationDirectory() throws ApplicationBundleException;

  public String getOutputDirectory() throws ApplicationBundleException;

  public String getBuildID() throws ApplicationBundleException;

  public String getBuildDate() throws ApplicationBundleException;

  public InputStream getBundleInfo() throws ApplicationBundleException;

  public String unpackAdlFileToXml() throws ApplicationBundleException;

  public String toolkitList(boolean filesToo) throws ApplicationBundleException;

  public void unpack(String alz) throws ApplicationBundleException;

  public void unpackAdlFile(String targetDir) throws ApplicationBundleException;

  public static final int EXECUTION_MODE_DISTRIBUTED = 1;
  public static final int EXECUTION_MODE_STANDALONE = 2;
  /**
   * Gets the execution mode for this bundle.
   *
   * <p>Test with one of the EXECUTION_MODE_* constants.
   *
   * @return the execution mode for the bundle
   */
  public int getExecutionMode() throws ApplicationBundleException;
}
