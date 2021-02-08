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

package com.ibm.streams.admin.internal.api.trace;

public class TraceContextType {

  // static data members
  public static final TraceContextType CONTROLLER = new TraceContextType("controller");
  public static final TraceContextType AAS = new TraceContextType("aas");
  public static final TraceContextType AUDITLOG = new TraceContextType("auditlog");
  public static final TraceContextType JMX = new TraceContextType("jmx");
  public static final TraceContextType SWS = new TraceContextType("sws");
  public static final TraceContextType BUILD = new TraceContextType("build");
  public static final TraceContextType SAM = new TraceContextType("sam");
  public static final TraceContextType SCH = new TraceContextType("sch");
  public static final TraceContextType SRM = new TraceContextType("srm");
  public static final TraceContextType APP = new TraceContextType("app");
  public static final TraceContextType HC = new TraceContextType("hc");
  public static final TraceContextType VIEW = new TraceContextType("view");
  public static final TraceContextType STREAMTOOL = new TraceContextType("streamtool");
  public static final TraceContextType NONE = new TraceContextType("none");

  // data members
  private String _type;

  /**
   * Constructor
   *
   * @param type - String
   */
  public TraceContextType(String type) {
    _type = type;
  }

  /* (non-Javadoc)
   * @see java.lang.Object#hashCode()
   */
  public int hashCode() {
    return _type.hashCode();
  }

  /* (non-Javadoc)
   * @see java.lang.Object#equals(java.lang.Object)
   */
  public boolean equals(Object obj) {
    if (obj instanceof TraceContextType) {
      return ((TraceContextType) obj)._type.equals(_type);
    }

    return false;
  }

  /* (non-Javadoc)
   * @see java.lang.Object#toString()
   */
  public String toString() {
    return _type;
  }
}
