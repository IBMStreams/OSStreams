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

package com.ibm.streams.admin.internal.core;

import java.net.URLDecoder;

public class InstallPathDetector {
  private String path;

  public InstallPathDetector() {
    try {
      String jarPath = getClass().getProtectionDomain().getCodeSource().getLocation().getPath();
      jarPath = URLDecoder.decode(jarPath, "UTF-8");
      if (jarPath != null) {
        int index = jarPath.lastIndexOf("/system/impl/lib");
        if (index >= 0) {
          path = jarPath.substring(0, index);
        } else {
          path = null; // if we don't find system/impl/lib, its not an install path
        }
      }
    } catch (Throwable t) {
      path = null;
    }
  }

  public String getPath() {
    return path;
  }
}
