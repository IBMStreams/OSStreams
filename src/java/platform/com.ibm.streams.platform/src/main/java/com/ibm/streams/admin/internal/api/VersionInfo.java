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

package com.ibm.streams.admin.internal.api;

import com.api.json.JSONObject;
import com.ibm.streams.admin.internal.api.trace.Trace;
import java.util.Properties;

public class VersionInfo implements Comparable<VersionInfo> {

  // constants
  public static final String VERSION_40 = "4.0.0.0";
  public static final String VERSION_41 = "4.1.0.0";
  public static final String VERSION_42 = "4.2.0.0";
  public static final String VERSION_421 = "4.2.1.0";
  public static final String VERSION_422 = "4.2.2.0";
  public static final String VERSION_424 = "4.2.4.0";
  public static final String VERSION_425 = "4.2.5.0";
  public static final String VERSION_430 = "4.3.0.0";
  public static final String VERSION_50 = "5.0.0.0";
  public static final String SPREE = VERSION_40;
  public static final String SOPRIS = VERSION_41;
  public static final String OLETA = VERSION_42;
  public static final String TIBER = VERSION_421;
  public static final String FIRST_CLOUD = VERSION_422;
  public static final String WDP_WAMS = VERSION_424;
  public static final String CHINOOK = VERSION_430;

  private static final String keyVersion = "Version";
  private static final String keyProductName = "ProductName";
  private static final String keyBuild = "Build";
  private static final String keyArchitecture = "Architecture";
  private static final String keyOS = "OS";
  private static final String keyOSVersion = "OS.VERSION";
  private static final String keyOSPatch = "OS.PATCH";
  private static final String keyEditionName = "EditionName";
  private static final String keyEdition = "Edition";

  // data members
  private String osStr = "";
  private String arch = "";
  private String version = "";
  private String name = "";
  private String build = "";
  private String editionName = "";
  private String edition = "";
  private String osVersion = "";
  private String osPatch = "";
  private boolean active;

  public VersionInfo(Properties versionProp) {
    osStr = versionProp.getProperty(keyOS, "");
    arch = versionProp.getProperty(keyArchitecture, "");
    version = versionProp.getProperty(keyVersion, "");
    name = versionProp.getProperty(keyProductName, "");
    build = versionProp.getProperty(keyBuild, "");
    editionName = versionProp.getProperty(keyEditionName, "");
    edition = versionProp.getProperty(keyEdition, "");
    osVersion = versionProp.getProperty(keyOSVersion, "");
    osPatch = versionProp.getProperty(keyOSPatch, "");
  }

  public VersionInfo(JSONObject versionJson) {
    osStr = (String) versionJson.get(keyOS);
    if (osStr == null) {
      osStr = "";
    }

    arch = (String) versionJson.get(keyArchitecture);
    if (arch == null) {
      arch = "";
    }

    version = (String) versionJson.get(keyVersion);
    if (version == null) {
      version = "";
    }

    name = (String) versionJson.get(keyProductName);
    if (name == null) {
      name = "";
    }

    build = (String) versionJson.get(keyBuild);
    if (build == null) {
      build = "";
    }

    editionName = (String) versionJson.get(keyEditionName);
    if (editionName == null) {
      editionName = "";
    }

    edition = (String) versionJson.get(keyEdition);
    if (edition == null) {
      edition = "";
    }

    osVersion = (String) versionJson.get(keyOSVersion);
    if (osVersion == null) {
      osVersion = "";
    }

    osPatch = (String) versionJson.get(keyOSPatch);
    if (osPatch == null) {
      osPatch = "";
    }
  }

  public String getVersion() {
    return version;
  }

  public void setVersion(String version) {
    this.version = version;
  }

  public String getOS() {
    return osStr;
  }

  public String getArchitecture() {
    return arch;
  }

  public String getProductName() {
    return name;
  }

  public String getBuild() {
    return build;
  }

  public void setBuild(String build) {
    this.build = build;
  }

  public String getEditionName() {
    return editionName;
  }

  public String getShortEditionName() {
    return edition;
  }

  public String getOSVersion() {
    return osVersion;
  }

  public String getOSPatch() {
    return osPatch;
  }

  public String getVersionAndBuild() {
    return version + " " + build;
  }

  public boolean isHotFix() {
    String[] values = version.split("\\.");
    return (values.length == 5);
  }

  public String getHotFixBaseVersion() {
    String baseVersion = null;
    if (isHotFix()) {
      int dot = version.lastIndexOf('.');
      baseVersion = version.substring(0, dot);
    }

    return baseVersion;
  }

  public String getHotFixVersion() {
    String hotFixVersion = null;
    if (isHotFix()) {
      int dot = version.lastIndexOf('.');
      hotFixVersion = version.substring(dot + 1);
    }

    return hotFixVersion;
  }

  public Properties getProperties() {
    Properties p = new Properties();

    p.put(keyOS, osStr);
    p.put(keyArchitecture, arch);
    p.put(keyVersion, version);
    p.put(keyProductName, name);
    p.put(keyBuild, build);
    p.put(keyEditionName, editionName);
    p.put(keyEdition, edition);
    p.put(keyOSVersion, osVersion);
    p.put(keyOSPatch, osPatch);

    return p;
  }

  public Properties getOsProperties() {
    Properties p = new Properties();

    p.put(keyOS, osStr);
    p.put(keyArchitecture, arch);
    p.put(keyOSVersion, osVersion);
    p.put(keyOSPatch, osPatch);

    return p;
  }

  public boolean isActive() {
    return active;
  }

  public void setActive(boolean isActive) {
    active = isActive;
  }

  public boolean isCompatible(VersionInfo target) {
    // version does NOT match
    if (!version.equals(target.getVersion())) {
      return false;
    }

    // compare architecture and os info
    return isExecutableCompatible(target);
  }

  public boolean isExecutableCompatible(VersionInfo target) {
    // Architecture does NOT match
    if (!arch.equals(target.getArchitecture())) {
      return false;
    }
    // Architecture matches
    else {
      String[] values = osStr.split(" ");
      String[] targetValues = target.getOS().split(" ");

      // first word in OS  does NOT match (i.e. RedHat vs SUSE)
      if (!values[0].equals(targetValues[0])) {
        return false;
      }

      // OS.VERSION has to match
      return osVersion.equals(target.getOSVersion());
    }
  }

  /**
   * Returns true if this VersionInfo is equal to requested version. Does not consider mod or
   * fixpack
   *
   * @param version - String
   * @return boolean
   */
  public boolean isEqual(String version) {
    String[] vrmf = version.split("\\.");
    String[] _vrmf = this.version.split("\\.");
    return _vrmf[0].equals(vrmf[0]) && _vrmf[1].equals(vrmf[1]);
  }

  /**
   * Returns true if this VersionInfo is from a release that comes prior to requested version. Does
   * not consider mod or fixpack
   *
   * @param version - String
   * @return boolean
   */
  public boolean isPre(String version) {
    try {
      String[] vrmf = version.split("\\.");
      int v = Integer.parseInt(vrmf[0]);
      int r = Integer.parseInt(vrmf[1]);

      String[] _vrmf = this.version.split("\\.");
      int _v = Integer.parseInt(_vrmf[0]);
      int _r = Integer.parseInt(_vrmf[1]);

      return (_v < v || (_v == v && _r < r));
    } catch (NumberFormatException nfe) {
      Trace.logError(nfe.getMessage(), nfe);
      return false;
    }
  }

  /**
   * Returns true if this VersionInfo is from a release that comes after the requested version. Does
   * not consider mod or fixpack
   *
   * @param version - String
   * @return boolean
   */
  public boolean isPost(String version) {
    try {
      String[] vrmf = version.split("\\.");
      int v = Integer.parseInt(vrmf[0]);
      int r = Integer.parseInt(vrmf[1]);

      String[] _vrmf = this.version.split("\\.");
      int _v = Integer.parseInt(_vrmf[0]);
      int _r = Integer.parseInt(_vrmf[1]);

      return (_v > v || (_v == v && _r > r));
    } catch (NumberFormatException nfe) {
      Trace.logError(nfe.getMessage(), nfe);
      return false;
    }
  }

  /* (non-Javadoc)
   * @see java.lang.Object#hashCode()
   */
  public int hashCode() {
    return version.hashCode();
  }

  /* (non-Javadoc)
   * @see java.lang.Object#equals(java.lang.Object)
   */
  public boolean equals(Object obj) {
    if (obj instanceof VersionInfo) {
      VersionInfo info = (VersionInfo) obj;
      return version.equals(info.version)
          && build.equals(info.build)
          && osStr.equals(info.osStr)
          && osVersion.equals(info.osVersion)
          && osPatch.equals(info.osPatch)
          && arch.equals(info.arch)
          && name.equals(info.name)
          && editionName.equals(info.editionName)
          && edition.equals(info.edition);
    }

    return false;
  }

  /* (non-Javadoc)
   * @see java.lang.Comparable#compareTo(java.lang.Object)
   */
  public int compareTo(VersionInfo info) {
    return compareVersions(version, info.version);
  }

  /**
   * Compare version values. Returns 0 (equal) -1 (us < version) 1 (us > version)
   *
   * @param version - String
   * @return int
   */
  public int compareTo(String version) {
    return compareVersions(this.version, version);
  }

  public static int compareVersions(String leftVersion, String rightVersion) {
    // versions are NOT equal
    if (!leftVersion.equals(rightVersion)) {
      // Trace.logTrace("left=" + leftVersion + " right=" + rightVersion);
      String[] leftVersions = leftVersion.split("\\.");
      String[] rightVersions = rightVersion.split("\\.");
      // Trace.logTrace("left length=" + leftVersions.length + " right length=" +
      // rightVersions.length);

      // compare vrmf digits first
      for (int index = 0; index < 4; ++index) {
        int leftValue = Integer.parseInt(leftVersions[index]);
        int rightValue = Integer.parseInt(rightVersions[index]);
        if (leftValue < rightValue) {
          return -1;
        } else if (leftValue > rightValue) {
          return 1;
        }
      }

      // left does NOT have a hotfix and right does
      if (leftVersions.length < rightVersions.length) {
        // Trace.logTrace("left=" + leftVersion + " does NOT have hotfix and right=" + rightVersion
        // + " does");
        return -1;
      }
      // left has a hotfix and right does NOT
      else if (leftVersions.length > rightVersions.length) {
        // Trace.logTrace("left=" + leftVersion + " has hotfix and right=" + rightVersion + " does
        // NOT");
        return 1;
      }
      // both left and right must be a hotfix
      else if (leftVersions.length > 4 && rightVersions.length > 4) {
        Long leftHot = Long.parseLong(leftVersions[4]);
        Long rightHot = Long.parseLong(rightVersions[4]);
        // Trace.logTrace("both left=" + leftVersion + " and right=" + rightVersion + " have
        // hotfix");
        if (leftHot < rightHot) {
          return -1;
        } else if (leftHot > rightHot) {
          return 1;
        }
      }
    }

    return 0;
  }
}
