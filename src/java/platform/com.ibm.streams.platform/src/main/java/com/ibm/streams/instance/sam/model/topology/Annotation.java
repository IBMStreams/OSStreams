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
import com.ibm.streams.instance.sam.persistence.IXmlSerializer;
import com.ibm.streams.platform.services.AnnotationAttributeValueType;
import com.ibm.streams.platform.services.AnnotationTupleValueType;
import com.ibm.streams.platform.services.AnnotationType;
import java.util.List;
import org.apache.commons.lang.StringEscapeUtils;

public class Annotation implements IXmlSerializer<AnnotationType> {
  private final String tag;
  private final boolean isInternal;
  private final List<AnnotationKeyValue> keyValues;

  public Annotation(String tag, boolean isInternal, List<AnnotationKeyValue> keyValues) {
    super();
    this.tag = tag;
    this.isInternal = isInternal;
    this.keyValues = keyValues;
  }

  public List<AnnotationKeyValue> getKeyValues() {
    return keyValues;
  }

  public String getTag() {
    return tag;
  }

  public boolean isInternal() {
    return isInternal;
  }

  @Override
  public String toString() {
    ModelDumper dumper = new ModelDumper();
    StringBuilder sb = new StringBuilder();
    sb.append("{");

    boolean first = true;
    for (AnnotationKeyValue kv : keyValues) {
      if (!first) {
        sb.append(", ");
      }
      first = false;
      sb.append(kv.getKey() + "=");
      sb.append("\"" + kv.getValue().toString() + "\"");
    }
    sb.append("}");
    dumper.line(
        "<annotation tag=\""
            + tag
            + "\" value=\""
            + StringEscapeUtils.escapeXml(sb.toString())
            + "\"/>");
    return dumper.toString();
  }

  public AnnotationType toXml() {
    AnnotationType xml = new AnnotationType();
    xml.setTag(getTag());
    AnnotationTupleValueType atvt = new AnnotationTupleValueType();
    for (AnnotationKeyValue kv : keyValues) {
      AnnotationAttributeValueType aavt = new AnnotationAttributeValueType();
      aavt.setKey(kv.getKey());
      AnnotationValue val = kv.getValue();
      if (val.isAnnotationPrimitiveValue()) {
        aavt.setValue(val.asAnnotationPrimitiveValue().getValue());
      } else if (val.isAnnotationListValue()) {
        aavt.setList(val.asAnnotationListValue().toXml());
      } else if (val.isAnnotationTupleValue()) {
        aavt.setTuple(val.asAnnotationTupleValue().toXml());
      } else {
        assert (false);
      }
      atvt.getAttr().add(aavt);
    }
    xml.setTuple(atvt);
    return xml;
  }
}
