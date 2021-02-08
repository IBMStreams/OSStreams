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

import java.util.Properties;
import org.apache.commons.lang.builder.EqualsBuilder;
import org.apache.commons.lang.builder.HashCodeBuilder;

public class RuntimeJobConfig {

  private Properties _submissionParms;
  private Properties _configParms;
  private transient String _adlString;

  public RuntimeJobConfig(String adlString, Properties submissionParms, Properties configParms) {
    _adlString = adlString;
    _submissionParms = submissionParms;
    _configParms = configParms;
  }

  public String getAdlString() {
    return _adlString;
  }

  @Override
  public int hashCode() {
    return new HashCodeBuilder().append(_submissionParms).append(_configParms).toHashCode();
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
    RuntimeJobConfig rhs = (RuntimeJobConfig) obj;
    return new EqualsBuilder()
        .append(_submissionParms, rhs._submissionParms)
        .append(_configParms, rhs._configParms)
        .isEquals();
  }

  @Override
  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("submissionParms=" + _submissionParms);
    builder.append(" configParms=" + _configParms);
    return builder.toString();
  }
}
