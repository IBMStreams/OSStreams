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
import static com.ibm.streams.instance.sam.model.ValidatorHelper.checkContentsNotNullAndValidate;

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.platform.services.ExportedStreamType;
import com.ibm.streams.platform.services.StreamPropertiesType;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class ExportedStream implements Validator {

  private List<StreamProperty> properties; // Optional: may be null
  private String exportOperName; // Mandatory
  private String name; // Optional: may be null
  private Boolean allowFilter; // Defaulted to true
  private ExportCongestionPolicyType congestionPolicy;

  public Boolean isAllowFilter() {
    return allowFilter;
  }

  public ExportCongestionPolicyType getCongestionPolicy() {
    return congestionPolicy;
  }

  public ExportedStream() {}

  public ExportedStream(
      String exportOperatorName,
      String name,
      Boolean allowFilter,
      ExportCongestionPolicyType congestionPolicy,
      List<StreamProperty> properties) {
    this.exportOperName = exportOperatorName;
    this.name = name;
    this.allowFilter = (allowFilter == null) ? true : allowFilter;
    this.congestionPolicy =
        (congestionPolicy == null) ? ExportCongestionPolicyType.getEnum("wait") : congestionPolicy;
    this.properties = properties;
  }

  public ExportedStream(ExportedStream other) {
    if (other.properties != null) {
      this.properties = new ArrayList<StreamProperty>();
      for (StreamProperty prop : other.properties) {
        this.properties.add(new StreamProperty(prop));
      }
    } else {
      this.properties = null;
    }
    this.exportOperName = other.exportOperName;
    this.name = other.name;
    this.allowFilter = other.allowFilter;
    this.congestionPolicy = other.congestionPolicy;
  }

  public List<StreamProperty> getProperties() {
    return properties;
  }

  public void setProperties(List<StreamProperty> properties) {
    this.properties = properties;
  }

  public String getExportOperName() {
    return exportOperName;
  }

  public void setExportOperName(String exportOperName) {
    this.exportOperName = exportOperName;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<exportedStream exportOperName=\"" + exportOperName + "\" allowFilter=\"");
    if (allowFilter != null) {
      dumper.append(allowFilter.toString());
    } else {
      dumper.append("true");
    }
    dumper.append("\"");
    dumper.append(" congestionPolicy=\"" + congestionPolicy.getValue() + "\"");
    if (name != null) {
      dumper.append(" name=\"" + name + "\"");
    }
    if (properties != null) {
      dumper.append(">").end();
      dumper.indent();
      dumper.line("<streamProperties>");
      dumper.indent();
      dumper.append(properties);
      dumper.outdent();
      dumper.line("</streamProperties>");
      dumper.outdent();
      dumper.line("</exportedStream>");
    } else {
      dumper.append("/>").end();
    }

    return dumper.toString();
  }

  public void validate() throws Exception {
    // Name is optional
    checkNotNull(exportOperName);
    // Allow Filter is optional. TODO: Is it really? or does it default?
    checkContentsNotNullAndValidate(properties);
  }

  public ExportedStreamType toXml() {
    ExportedStreamType exportedStreamType = new ExportedStreamType();
    exportedStreamType.setAllowFilter(isAllowFilter());
    exportedStreamType.setExportOperName(getExportOperName());
    exportedStreamType.setName(getName());
    if (congestionPolicy.compareTo(ExportCongestionPolicyType.WAIT) != 0) {
      exportedStreamType.setCongestionPolicy(
          com.ibm.streams.platform.services.ExportCongestionPolicyType.fromValue(
              congestionPolicy.getValue()));
    }
    if (getProperties() != null) {
      StreamPropertiesType streamPropertiesType = new StreamPropertiesType();
      for (StreamProperty streamProperty : getProperties()) {
        streamPropertiesType.getProperty().add(streamProperty.toXml());
      }
      exportedStreamType.setProperties(streamPropertiesType);
    }
    return exportedStreamType;
  }

  @Override
  public int hashCode() {
    return Objects.hash(name, exportOperName, allowFilter, properties);
  }

  @Override
  public boolean equals(Object obj) {
    if (obj instanceof ExportedStream) {
      ExportedStream t = (ExportedStream) obj;
      if (getExportOperName().equals(t.getExportOperName())
          && isAllowFilter().equals(t.isAllowFilter())) {
        if ((getName() == null && t.getName() == null)
            || (getName() != null && t.getName() != null && getName().equals(t.getName()))) {
          return hasSameStreamProperties(t);
        }
      }
    }
    return false;
  }

  public boolean hasSameStreamProperties(ExportedStream exp) {
    if (exp == null) {
      return false;
    }

    if (getProperties() == null && exp.getProperties() == null) {
      return true;
    } else if (getProperties() != null && exp.getProperties() != null) {
      if (getProperties().size() == exp.getProperties().size()) {
        for (StreamProperty s : getProperties()) {
          if (!exp.getProperties().contains(s)) {
            return false;
          }
        }
        return true;
      }
    }
    return false;
  }
}
