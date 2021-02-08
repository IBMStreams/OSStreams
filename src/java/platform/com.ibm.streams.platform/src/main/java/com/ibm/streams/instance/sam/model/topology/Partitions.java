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

package com.ibm.streams.instance.sam.model.topology;

import com.ibm.streams.instance.sam.model.ModelDumper;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Objects;
import java.util.Set;

public class Partitions {

  private final Set<String> coLocations = new HashSet<>();
  private final Set<String> exLocations = new HashSet<>();
  private Map<String, Set<String>> provisionalLocations = new HashMap<>();
  private Map<String, Set<String>> softProvisionalLocations = new HashMap<>();
  private Map<String, Set<String>> segregatedLocations = new HashMap<>();
  private Map<String, Set<String>> softSegregatedLocations = new HashMap<>();
  private Boolean isolation = false;
  private Boolean softIsolation = false;

  public Partitions() {}

  public Map<String, Set<String>> getProvisionalLocations() {
    return provisionalLocations;
  }

  public Map<String, Set<String>> getSegregatedLocations() {
    return segregatedLocations;
  }

  public Map<String, Set<String>> getSoftProvisionalLocations() {
    return softProvisionalLocations;
  }

  public Map<String, Set<String>> getSoftSegregatedLocations() {
    return softSegregatedLocations;
  }

  public Set<String> getCoLocations() {
    return coLocations;
  }

  public Set<String> getExLocations() {
    return exLocations;
  }

  public void setIsolation(boolean isolation) {
    this.isolation = Boolean.valueOf(isolation);
  }

  public void setSoftIsolation(boolean isolation) {
    this.softIsolation = Boolean.valueOf(isolation);
  }

  public boolean isIsolation() {
    return isolation.booleanValue();
  }

  public boolean isSoftIsolation() {
    return softIsolation.booleanValue();
  }

  public String toDotGraph() {
    StringBuilder str = new StringBuilder();
    if (coLocations != null && coLocations.size() > 0) str.append(" c:" + coLocations);
    if (exLocations != null && exLocations.size() > 0) str.append(" x:" + exLocations);
    if (isolation != null && isolation) str.append("i");
    if (softIsolation != null && softIsolation) str.append("j");
    return str.toString();
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<partition");
    if (isolation) {
      dumper.append(" isolation=\"" + isolation + "\"");
    }
    if (softIsolation) {
      dumper.append(" softIsolation=\"" + softIsolation + "\"");
    }
    if (coLocations.size() == 0
        && exLocations.size() == 0
        && provisionalLocations.size() == 0
        && softProvisionalLocations.size() == 0
        && segregatedLocations.size() == 0
        && softSegregatedLocations.size() == 0) {
      dumper.append("/>").end();
    } else {
      dumper.append(">").end();
      dumper.indent();
      if (coLocations.size() > 0) {
        for (String coloc : coLocations) {
          dumper.line("<coLocation colocId=\"" + coloc + "\"/>");
        }
      }
      if (exLocations.size() > 0) {
        for (String exloc : exLocations) {
          dumper.line("<exLocation exlocId=\"" + exloc + "\"/>");
        }
      }
      if (provisionalLocations.size() > 0) {
        dumper.line("<provisionalLocations>");
        dumper.indent();
        for (Entry<String, Set<String>> e : provisionalLocations.entrySet()) {
          dumper.startLine("<provisionalLocation name=\"" + e.getKey() + "\" values=\"");
          boolean first = true;
          for (String val : e.getValue()) {
            if (first != true) {
              dumper.append(", ");
            }
            first = false;
            dumper.append(val);
          }
          dumper.append("\"/>");
          dumper.end();
        }
        dumper.outdent();
        dumper.line("</provisionalLocations>");
      }
      if (softProvisionalLocations.size() > 0) {
        dumper.line("<softProvisionalLocations>");
        dumper.indent();
        for (Entry<String, Set<String>> e : softProvisionalLocations.entrySet()) {
          dumper.startLine("<softProvisionalLocation name=\"" + e.getKey() + "\" values=\"");
          boolean first = true;
          for (String val : e.getValue()) {
            if (first != true) {
              dumper.append(", ");
            }
            first = false;
            dumper.append(val);
          }
          dumper.append("\"/>");
          dumper.end();
        }
        dumper.outdent();
        dumper.line("</softProvisionalLocations>");
      }
      if (segregatedLocations.size() > 0) {
        dumper.line("<segregatedLocations>");
        dumper.indent();
        for (Entry<String, Set<String>> e : segregatedLocations.entrySet()) {
          dumper.startLine("<segregatedLocation name=\"" + e.getKey() + "\" values=\"");
          boolean first = true;
          for (String val : e.getValue()) {
            if (first != true) {
              dumper.append(", ");
            }
            first = false;
            dumper.append(val);
          }
          dumper.append("\"/>");
          dumper.end();
        }
        dumper.outdent();
        dumper.line("</segregatedLocations>");
      }
      if (softSegregatedLocations.size() > 0) {
        dumper.line("<softSegregatedLocations>");
        dumper.indent();
        for (Entry<String, Set<String>> e : softSegregatedLocations.entrySet()) {
          dumper.startLine("<softSegregatedLocation name=\"" + e.getKey() + "\" values=\"");
          boolean first = true;
          for (String val : e.getValue()) {
            if (first != true) {
              dumper.append(", ");
            }
            first = false;
            dumper.append(val);
          }
          dumper.append("\"/>");
          dumper.end();
        }
        dumper.outdent();
        dumper.line("</softSegregatedLocations>");
      }
      dumper.outdent();
      dumper.line("</partition>");
    }
    return dumper.toString();
  }

  // for change detection
  @Override
  public int hashCode() {
    return Objects.hash(
        coLocations,
        exLocations,
        provisionalLocations,
        softProvisionalLocations,
        segregatedLocations,
        softSegregatedLocations,
        isolation,
        softIsolation);
  }
}
