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

import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class RuntimeJobComposites {

  private String _compositeInfo;

  public RuntimeJobComposites(String compositeInfo) {
    _compositeInfo = compositeInfo;
  }

  public String getJobCompositeInformation() {
    return _compositeInfo;
  }

  public void setJobCompositeInformation(String compositeInfo) {
    _compositeInfo = compositeInfo;
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder().append(_compositeInfo).toHashCode();
  }

  @Override
  public boolean equals(Object obj) {
    if (obj == null) {
      return false;
    }
    if (obj == this) {
      return true;
    }
    if (obj.getClass() != getClass()) {
      return false;
    }
    RuntimeJobComposites rhs = (RuntimeJobComposites) obj;
    return new EqualsBuilder().append(_compositeInfo, rhs._compositeInfo).isEquals();
  }
}
