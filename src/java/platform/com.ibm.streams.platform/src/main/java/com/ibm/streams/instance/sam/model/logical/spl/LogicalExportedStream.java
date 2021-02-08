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

import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.model.topology.ExportCongestionPolicyType;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.spl.expressions.SPLExpressionEval;
import com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import com.ibm.streams.spl.expressions.SPLMeta.SPLValue;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class LogicalExportedStream {

  private final List<StreamProperty> properties; // Optional: may be null
  private final ExportOperator exportOper; // Mandatory
  private String name; // Optional: may be null
  private final Boolean allowFilter; // Defaulted to true
  private final ExportCongestionPolicyType congestionPolicy;

  public LogicalExportedStream(
      ExportOperator exportOperator,
      String name,
      Boolean allowFilter,
      ExportCongestionPolicyType congestionPolicy,
      List<StreamProperty> properties) {
    this.exportOper = exportOperator;
    this.name = name;
    this.allowFilter = (allowFilter == null) ? true : allowFilter;
    this.congestionPolicy =
        (congestionPolicy == null) ? ExportCongestionPolicyType.getEnum("wait") : congestionPolicy;
    this.properties = properties;
  }

  public List<StreamProperty> getProperties() {
    return properties;
  }

  public String getName() {
    return name;
  }

  public void setName(String name) {
    this.name = name;
  }

  public Boolean isAllowFilter() {
    return allowFilter;
  }

  public ExportCongestionPolicyType getCongestionPolicy() {
    return congestionPolicy;
  }

  public String getExportOperName() throws SAMInternalErrorException {
    return exportOper.getFullyQualifiedName();
  }

  public void evaluate(
      BigInteger localChannelIndex,
      BigInteger localMaxChannels,
      BigInteger globalChannelIndex,
      BigInteger globalMaxChannels)
      throws SAMInternalErrorException {
    Map<String, SPLValue> intrinsics = new HashMap<String, SPLValue>();
    intrinsics.put("getChannel", new SPLInt64(globalChannelIndex.longValue()));
    intrinsics.put("getLocalChannel", new SPLInt64(localChannelIndex.longValue()));
    intrinsics.put("getMaxChannels", new SPLInt64(globalMaxChannels.longValue()));
    intrinsics.put("getLocalMaxChannels", new SPLInt64(localMaxChannels.longValue()));

    if (getProperties() != null) {
      for (StreamProperty prop : getProperties()) {
        List<String> values = prop.getValues();
        for (int i = 0; i < values.size(); i++) {
          values.set(i, SPLExpressionEval.intrinsicsSubstitution(values.get(i), intrinsics));
        }
      }
    }

    String name = getName();
    if (name != null) {
      setName(SPLExpressionEval.intrinsicsSubstitution(name, intrinsics));
    }
  }
}
