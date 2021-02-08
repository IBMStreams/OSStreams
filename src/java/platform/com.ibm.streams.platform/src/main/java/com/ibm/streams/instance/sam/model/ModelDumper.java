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

package com.ibm.streams.instance.sam.model;

import java.util.List;
import java.util.Map;
import java.util.Set;

public class ModelDumper {

  StringBuilder sb = new StringBuilder();
  private static int _indent = 0;

  public ModelDumper indent() {
    _indent += 2;
    return this;
  }

  public ModelDumper outdent() {
    _indent -= 2;
    return this;
  }

  public ModelDumper line(String s) {
    startLine(s);
    end();
    return this;
  }

  public ModelDumper startLine(String s) {
    for (int i = 0; i < _indent; ++i) {
      sb.append(" ");
    }
    sb.append(s);
    return this;
  }

  public ModelDumper append(String s) {
    sb.append(s);
    return this;
  }

  public <T> ModelDumper append(List<T> list) {
    for (T t : list) {
      sb.append(t.toString());
    }
    return this;
  }

  public <T> ModelDumper append(Set<T> s) {
    for (T t : s) {
      sb.append(t.toString());
    }
    return this;
  }

  public <K, V> ModelDumper append(Map<K, V> m) {
    Set<K> keys = m.keySet();
    for (K k : keys) {
      sb.append(m.get(k).toString());
    }
    return this;
  }

  public ModelDumper end() {
    sb.append("\n");
    return this;
  }

  @Override
  public String toString() {
    return sb.toString();
  }
}
