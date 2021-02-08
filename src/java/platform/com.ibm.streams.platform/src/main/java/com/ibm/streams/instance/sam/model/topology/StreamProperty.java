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

import static com.google.common.base.Preconditions.checkNotNull;
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNull;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.platform.services.StreamPropertyType;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class StreamProperty implements Validator {

  private List<String> values;
  private String type;
  private String name;

  public StreamProperty() {}

  public StreamProperty(String name, String type, List<String> values) {
    this.name = name;
    this.type = type;
    this.values = values;
  }

  public StreamProperty(StreamProperty other) {
    this.name = other.name;
    this.type = other.type;
    if (other.values != null) {
      this.values = new ArrayList<String>(other.values);
    } else {
      this.values = null;
    }
  }

  public List<String> getValues() {
    return values;
  }

  public void setValues(List<String> values) {
    this.values = values;
  }

  public String getType() {
    return type;
  }

  public void setType(String type) {
    this.type = type;
  }

  public String getName() {
    return name;
  }

  public void getName(String name) {
    this.name = name;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line("<streamProperty name=\"" + name + "\" type=\"" + type + "\">");
    dumper.indent();
    for (String s : values) {
      dumper.line("<value>" + s + "</value>");
    }
    dumper.outdent();
    dumper.line("</streamProperty>");
    return dumper.toString();
  }

  public void validate() throws Exception {
    checkNotNull(type);
    checkNotNull(name);
    checkContentsNotNull(values);
  }

  public StreamPropertyType toXml() {
    StreamPropertyType streamPropertyType = new StreamPropertyType();
    streamPropertyType.setName(getName());
    streamPropertyType.setType(getType());
    for (String value : getValues()) {
      streamPropertyType.getValue().add(value);
    }
    return streamPropertyType;
  }

  @Override
  public int hashCode() {
    return Objects.hash(name, type, values);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof StreamProperty) {
      StreamProperty t = (StreamProperty) obj;
      if (getName().equals(t.getName()) && getType().equals(t.getType())) {
        if (getValues().size() == t.getValues().size()) {
          for (String val : getValues()) {
            if (!t.getValues().contains(val)) {
              return false;
            }
          }
          return true;
        }
      }
    }
    return false;
  }
}
