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

import com.api.json.JSONArray;
import com.api.json.JSONObject;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.core.api.ApiEnum;
import com.ibm.streams.admin.internal.core.api.ApiProperties.PropertyType;
import java.math.BigInteger;
import java.util.Objects;
import java.util.regex.Pattern;

public class TopologyPrimitiveType extends TopologyType {

  protected static final String rstringRegExp =
      "\\s*rstring\\s*\\[\\s*[0-9]+\\s*\\]\\s*$"; // example: rstring[64]
  protected static final String enumRegExp =
      "\\s*enum\\s*\\{\\s*[^,]+(\\s*,\\s*[^,]+)*\\s*\\}\\s*$"; // example:
  // enum{error,info,debug,trace}
  protected static final String xmlRegExp =
      "\\s*xml\\s*(\\<\\s*\"[^\"]+\"\\s*\\>\\s*(\\([0-9]+\\)\\s*)?)?$"; // example:
  // xml<"bigger.xsd">(0)
  // syntax: ‘xml'
  // ('<' "schemaURI" '>')?  - extra (0) at the
  // end is an internal optional toolkit
  // designator

  private String typeName;

  public TopologyPrimitiveType(String typeName) {
    this.typeName = typeName;
  }

  public String getTypeName() {
    return typeName;
  }

  //    public void setTypeName(String typeName) {
  //        this.typeName = typeName;
  //    }
  @Override
  public boolean isTopologyPrimitiveType() {
    return true;
  }

  @Override
  public TopologyPrimitiveType asTopologyPrimitiveType() {
    return this;
  }

  @Override
  public TopologyPrimitiveType copy() {
    return new TopologyPrimitiveType(typeName);
  }

  @Override
  public String toString() {
    return typeName;
  }

  @Override
  public JSONObject toApi() {
    JSONObject typeObj = new JSONObject();
    String apiType = getApiType();
    typeObj.put(PropertyType.TYPE.toApiStr(), apiType);
    if (apiType.equals(ApiEnum.SPL_TYPE_XML) && typeName.trim().endsWith(")")) {
      typeObj.put(
          PropertyType.SPL_TYPE.toApiStr(),
          typeName.substring(0, typeName.lastIndexOf('('))); // Strip off (#) at end if present
    } else {
      typeObj.put(PropertyType.SPL_TYPE.toApiStr(), typeName);
    }
    if (apiType.equals(ApiEnum.SPL_TYPE_BSTRING)) {
      String boundedSizeStr = typeName.substring("rstring[".length(), typeName.length() - 1);
      try {
        BigInteger boundedSize = new BigInteger(boundedSizeStr.trim());
        typeObj.put(PropertyType.BOUNDED_SIZE.toApiStr(), boundedSize);
      } catch (Exception e) {
        Trace.logError(
            "Unexpected error trying to parse bounded size for: "
                + typeName
                + ". Parsed size: "
                + boundedSizeStr);
      }
    } else if (apiType.equals(ApiEnum.SPL_TYPE_ENUM)) {
      JSONArray possibleValues = new JSONArray();
      String possibleValuesStr = typeName.substring("enum{".length(), typeName.length() - 1);
      for (String possibleValue : possibleValuesStr.split("[,]")) {
        possibleValues.add(possibleValue.trim());
      }
      typeObj.put(PropertyType.POSSIBLE_VALUES.toApiStr(), possibleValues);
    } else if (apiType.equals(ApiEnum.SPL_TYPE_XML)) {
      int firstQuotePos = typeName.indexOf('\"');
      if (firstQuotePos != -1) {
        int lastQuotePos = typeName.lastIndexOf('\"');
        String schemaUri = typeName.substring(firstQuotePos + 1, lastQuotePos);
        typeObj.put(PropertyType.SCHEMA_URI.toApiStr(), schemaUri);
      }
    }
    return typeObj;
  }

  @Override
  public String getApiType() {
    StringBuffer typeNameBuffer = new StringBuffer(typeName);

    if (Pattern.matches(rstringRegExp, typeNameBuffer)) {
      return ApiEnum.SPL_TYPE_BSTRING;
    }
    if (Pattern.matches(enumRegExp, typeNameBuffer)) {
      return ApiEnum.SPL_TYPE_ENUM;
    }
    if (Pattern.matches(xmlRegExp, typeNameBuffer)) {
      return ApiEnum.SPL_TYPE_XML;
    }

    return ApiEnum.getSplTypeApiValue(typeName);
  }

  public int hashCode() {
    return Objects.hash(typeName);
  }

  public boolean equals(Object obj) {
    if (obj instanceof TopologyPrimitiveType) {
      TopologyPrimitiveType t = (TopologyPrimitiveType) obj;
      return (getTypeName().equals(t.getTypeName()));
    }
    return false;
  }
}
