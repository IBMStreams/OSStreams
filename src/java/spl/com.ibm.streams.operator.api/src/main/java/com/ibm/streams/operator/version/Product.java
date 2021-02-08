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

package com.ibm.streams.operator.version;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.util.Properties;

/**
 * Class representing the Streams product information such as the product version number.
 *
 * @since InfoSphere&reg; Streams Version 4.0.1
 */
public class Product {

  private static Version currentVersion;

  /** Version {@code '3.2.0.0'}. */
  public static final Version STREAMS_3_2_0 = new Version(3, 2, 0, 0);
  /** Version {@code '3.2.1.0'}. */
  public static final Version STREAMS_3_2_1 = new Version(3, 2, 1, 0);
  /** Version {@code '4.0.0.0'}. */
  public static final Version STREAMS_4_0_0 = new Version(4, 0, 0, 0);
  /** Version {@code '4.0.1.0'}. */
  public static final Version STREAMS_4_0_1 = new Version(4, 0, 1, 0);
  /** Version {@code '4.1.0.0'}. */
  public static final Version STREAMS_4_1 = new Version(4, 1, 0, 0);
  /** Version {@code '4.2.0.0'}. */
  public static final Version STREAMS_4_2 = new Version(4, 2, 0, 0);

  /**
   * Returns the product's version number
   *
   * @return Version object instance representing Streams product version
   */
  public static Version getVersion() {
    if (currentVersion == null) {
      String streamsInstall = System.getenv("STREAMS_INSTALL");
      if (streamsInstall != null && streamsInstall.length() > 0) {
        File productFile = new File(streamsInstall, ".product");
        try (FileInputStream fis = new FileInputStream(productFile)) {
          Properties prop = new Properties();
          prop.load(fis);
          String version = (String) prop.getProperty("Version");
          if (version != null) {
            currentVersion = new Version(version);
          }
        } catch (FileNotFoundException e) {
        } catch (IOException e) {
        }
      }
    }
    return currentVersion;
  }
}
