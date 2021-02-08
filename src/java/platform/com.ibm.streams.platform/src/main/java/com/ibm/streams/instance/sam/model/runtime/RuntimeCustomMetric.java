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

package com.ibm.streams.instance.sam.model.runtime;

import com.ibm.streams.instance.sam.model.topology.CustomMetricKind;

public class RuntimeCustomMetric {

  private String _description;
  private CustomMetricKind _kind;
  private String _name;

  public RuntimeCustomMetric(String name, CustomMetricKind kind, String description) {
    _name = name;
    _kind = kind;
    _description = description;
  }

  public String getDescription() {
    return _description;
  }

  public CustomMetricKind getKind() {
    return _kind;
  }

  public String getName() {
    return _name;
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + ((_description == null) ? 0 : _description.hashCode());
    result = prime * result + ((_kind == null) ? 0 : _kind.hashCode());
    result = prime * result + ((_name == null) ? 0 : _name.hashCode());
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj) return true;
    if (obj == null) return false;
    if (getClass() != obj.getClass()) return false;
    RuntimeCustomMetric other = (RuntimeCustomMetric) obj;
    if (_description == null) {
      if (other._description != null) return false;
    } else if (!_description.equals(other._description)) return false;
    if (_kind != other._kind) return false;
    if (_name == null) {
      if (other._name != null) return false;
    } else if (!_name.equals(other._name)) return false;
    return true;
  }
}
