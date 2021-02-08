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

public class EventTimeContext {

  private String attribute;
  private String type;
  private String resolution;

  public EventTimeContext(String attribute, String type, String resolution) {
    this.attribute = attribute;
    this.type = type;
    this.resolution = resolution;
  }

  public void setAttribute(String attribute) {
    this.attribute = attribute;
  }

  public String getAttribute() {
    return attribute;
  }

  public void setType(String type) {
    this.type = type;
  }

  public String getType() {
    return type;
  }

  public void setResolution(String resolution) {
    this.resolution = resolution;
  }

  public String getResolution() {
    return resolution;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + attribute.hashCode();
    result = prime * result + type.hashCode();
    result = prime * result + resolution.hashCode();
    return result;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<eventTimeContext attribute=\""
            + attribute
            + "\" type=\""
            + type
            + "\" resolution=\""
            + resolution
            + "\"/>");
    return dumper.toString();
  }
}
