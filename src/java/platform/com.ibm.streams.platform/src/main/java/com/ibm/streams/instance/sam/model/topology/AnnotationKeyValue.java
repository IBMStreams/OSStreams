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

import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.SplAnnotationValueType;

public class AnnotationKeyValue implements IXmlSerializer<SplAnnotationValueType> {
  private final String key;
  private final AnnotationValue value;

  public AnnotationKeyValue(String key, AnnotationValue value) {
    super();
    this.key = key;
    this.value = value;
  }

  public String getKey() {
    return key;
  }

  public AnnotationValue getValue() {
    return value;
  }

  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder();
    sb.append(key + "=");
    sb.append("\"" + value.toString() + "\"");
    return sb.toString();
  }

  public SplAnnotationValueType toXml() {
    /*
    SplAnnotationValueType xml = new SplAnnotationValueType();
    xml.setValue(getValue());
    xml.setKey(getKey());
    xml.setType(getType());
    // No submission time value index
    return xml;
    */
    return null;
  }
}
