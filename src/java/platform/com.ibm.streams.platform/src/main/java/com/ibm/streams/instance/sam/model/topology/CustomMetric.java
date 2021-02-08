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

import com.ibm.streams.instance.sam.model.ModelDumper;
import com.ibm.streams.instance.sam.model.Validator;
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.CustomMetricKindType;
import com.ibm.streams.platform.services.CustomMetricType;
import java.math.BigInteger;
import java.util.Objects;

public class CustomMetric implements Validator, IXmlSerializer<CustomMetricType> {

  private String description;
  private CustomMetricKind kind;
  private String name;
  private BigInteger index;

  public CustomMetric(BigInteger index, String name, CustomMetricKind kind, String description) {
    this.index = index;
    this.name = name;
    this.kind = kind;
    this.description = description;
  }

  public String getDescription() {
    return description;
  }

  public CustomMetricKind getKind() {
    return kind;
  }

  public String getName() {
    return name;
  }

  public BigInteger getIndex() {
    return index;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.line(
        "<customMetric index=\""
            + index
            + "\" name=\""
            + name
            + "\" kind=\""
            + kind
            + "\" description=\""
            + description
            + "\"/>");
    return dumper.toString();
  }

  public CustomMetric copy() {
    return new CustomMetric(index, name, kind, description);
  }

  public int hashCode() {
    return Objects.hash(index, name, kind, description);
  }

  public CustomMetricType toXml() {
    CustomMetricType xml = new CustomMetricType();
    xml.setDescription(getDescription());
    xml.setIndex(getIndex());
    xml.setKind(CustomMetricKindType.fromValue(getKind().toString()));

    xml.setName(getName());
    return xml;
  }

  public void validate() throws Exception {
    checkNotNull(description);
    checkNotNull(kind); // Enum is either null or valid
    checkNotNull(name);
    checkNotNull(index);
  }
}
