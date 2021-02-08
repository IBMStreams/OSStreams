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

package com.ibm.streams.instance.sam.model.logical.spl;

import com.ibm.streams.instance.sam.model.ModelDumper;
import java.math.BigInteger;
import java.text.CharacterIterator;
import java.text.StringCharacterIterator;
import java.util.List;
import org.apache.commons.lang.text.StrTokenizer;

public class SubmissionTimeValue {
  private final String compositeName;
  private final BigInteger index;
  private final SubmissionTimeValueKind kind;
  private final String name;
  private final String defaultValue;
  private final boolean required;
  private String value = null;

  public SubmissionTimeValue(
      String compositeName,
      BigInteger index,
      SubmissionTimeValueKind kind,
      String name,
      String defaultValue,
      boolean required) {
    this.compositeName = compositeName;
    this.index = index;
    this.kind = kind;
    this.name = name;
    this.defaultValue = defaultValue;
    this.required = required;
  }

  public String getCompositeName() {
    return compositeName;
  }

  public BigInteger getIndex() {
    return index;
  }

  public SubmissionTimeValueKind getKind() {
    return kind;
  }

  public String getName() {
    return name;
  }

  public boolean isRequired() {
    return required;
  }

  public String toString() {
    ModelDumper dumper = new ModelDumper();
    dumper.startLine("<submissionTimeValue index=\"" + index + "\" name=\"" + name + "\"");
    if (defaultValue != null) dumper.append(" defaultValue=\"" + defaultValue + "\"");
    dumper.append("/>").end();
    return dumper.toString();
  }

  private String escapeString(String s) {
    StringBuilder sb = new StringBuilder();
    CharacterIterator it = new StringCharacterIterator(s);
    for (char c = it.first(); c != CharacterIterator.DONE; c = it.next()) {
      if (c == '"') {
        sb.append("\\\"");
      } else if (c == '\\') {
        sb.append("\\\\");
      } else {
        sb.append(c);
      }
    }
    return sb.toString();
  }

  public String getValue() {
    if (value != null) {
      StringBuilder sb = new StringBuilder();
      if (kind == SubmissionTimeValueKind.Named) {
        sb.append("\"");
        sb.append(escapeString(value));
        sb.append("\"");
      } else {
        sb.append("[");
        String[] list = value.split(",", -1);
        Boolean first = true;
        for (String s : list) {
          if (!first) {
            sb.append(",");
          }
          sb.append("\"" + escapeString(s) + "\"");
          first = false;
        }
        sb.append("]");
      }
      return sb.toString();
    }
    assert (defaultValue != null);
    return defaultValue;
  }

  public void setValue(String value) {
    this.value = value;
  }

  public String getDefaultValue() {
    return defaultValue;
  }

  public List<String> getValueList() {
    StrTokenizer tokenizer = StrTokenizer.getCSVInstance(value);
    return (List<String>) tokenizer.getTokenList();
  }
}
