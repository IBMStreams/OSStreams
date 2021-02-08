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

/**
 * Class representing a version number in the form of {@code version.release.mod.fix}. For example,
 * {@code '4.0.1.0'}.
 *
 * @since InfoSphere&reg; Streams Version 4.0.1
 */
public final class Version implements Comparable<Version> {

  private final int version;
  private final int release;
  private final int mod;
  private final int fix;

  /**
   * Returns the version level as an integer.
   *
   * @return Version level.
   */
  public int getVersion() {
    return version;
  }

  /**
   * Returns the release level as an integer.
   *
   * @return Release level.
   */
  public int getRelease() {
    return release;
  }

  /**
   * Returns the modification level as an integer.
   *
   * @return Modification level.
   */
  public int getMod() {
    return mod;
  }

  /**
   * Returns the fix level as an integer.
   *
   * @return Fix level.
   */
  public int getFix() {
    return fix;
  }

  /**
   * Create a version from its string form. String form is {@code version.release.mod.fix} or a
   * subset where trailing elements can be omitted, for example {@code '4.1'} . If a level is
   * omitted then its value is set to zero.
   *
   * @param version String representation of a version number.
   * @since IBM Streams 4.2
   */
  public Version(String version) {
    String[] parts = version.split("\\.");
    this.version = (parts.length > 0 ? Integer.parseInt(parts[0]) : 0);
    this.release = (parts.length > 1 ? Integer.parseInt(parts[1]) : 0);
    this.mod = (parts.length > 2 ? Integer.parseInt(parts[2]) : 0);
    this.fix = (parts.length > 3 ? Integer.parseInt(parts[3]) : 0);
  }

  /**
   * Create a version from its levels.
   *
   * @param version Version level.
   * @param release Release level.
   * @param mod Modification level.
   * @param fix Fix level.
   * @since IBM Streams 4.2
   */
  public Version(int version, int release, int mod, int fix) {
    this.version = version;
    this.release = release;
    this.mod = mod;
    this.fix = fix;
  }

  @Override
  public int compareTo(Version other) {
    if (version > other.version) return 1;
    if (version < other.version) return -1;
    if (release > other.release) return 1;
    if (release < other.release) return -1;
    if (mod > other.mod) return 1;
    if (mod < other.mod) return -1;
    if (fix > other.fix) return 1;
    if (fix < other.fix) return -1;
    return 0;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + fix;
    result = prime * result + mod;
    result = prime * result + release;
    result = prime * result + version;
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    Version other = (Version) obj;
    if (fix != other.fix) return false;
    if (mod != other.mod) return false;
    if (release != other.release) return false;
    if (version != other.version) return false;
    return true;
  }

  @Override
  public String toString() {
    return version + "." + release + "." + mod + "." + fix;
  }
}
