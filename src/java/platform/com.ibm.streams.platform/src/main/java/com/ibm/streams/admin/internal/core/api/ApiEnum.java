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

package com.ibm.streams.admin.internal.core.api;

import java.util.HashSet;
import java.util.Locale;
import java.util.Map;
import java.util.Set;

public class ApiEnum {

  public static <K, V> K toApiEnum(Map<K, V> enumMap, V internalValue) {
    if (internalValue == null) {
      return null;
    }

    for (Map.Entry<K, V> entry : enumMap.entrySet()) {
      if (internalValue.equals(entry.getValue())) {
        return entry.getKey();
      }
    }
    return null;
  }

  public static final String METRIC_KIND_GAUGE = "gauge";
  public static final String METRIC_KIND_COUNTER = "counter";
  public static final String METRIC_KIND_TIME = "time";
  public static final String METRIC_KIND_UNKNOWN = "unknown";

  public static final String SPL_TYPE_BSTRING = "bstring";
  public static final String SPL_TYPE_XML = "xml";
  public static final String SPL_TYPE_ENUM = "enum";
  public static final String SPL_TYPE_TUPLE = "tuple";
  public static final String SPL_TYPE_LIST = "list";
  public static final String SPL_TYPE_BLIST = "blist";
  public static final String SPL_TYPE_SET = "set";
  public static final String SPL_TYPE_BSET = "bset";
  public static final String SPL_TYPE_MAP = "map";
  public static final String SPL_TYPE_BMAP = "bmap";
  public static final String SPL_TYPE_BLOB = "blob";
  public static final String SPL_TYPE_OPTIONAL = "optional";

  private static final Set<String> _splFixedNamePrimitiveTypes = new HashSet<String>();

  static {
    _splFixedNamePrimitiveTypes.add("rstring");
    _splFixedNamePrimitiveTypes.add("boolean");
    _splFixedNamePrimitiveTypes.add("int8");
    _splFixedNamePrimitiveTypes.add("int16");
    _splFixedNamePrimitiveTypes.add("int32");
    _splFixedNamePrimitiveTypes.add("int64");
    _splFixedNamePrimitiveTypes.add("float32");
    _splFixedNamePrimitiveTypes.add("float64");
    _splFixedNamePrimitiveTypes.add("decimal32");
    _splFixedNamePrimitiveTypes.add("decimal64");
    _splFixedNamePrimitiveTypes.add("decimal128");
    _splFixedNamePrimitiveTypes.add("uint8");
    _splFixedNamePrimitiveTypes.add("uint16");
    _splFixedNamePrimitiveTypes.add("uint32");
    _splFixedNamePrimitiveTypes.add("uint64");
    _splFixedNamePrimitiveTypes.add("complex32");
    _splFixedNamePrimitiveTypes.add("complex64");
    _splFixedNamePrimitiveTypes.add("ustring");
    _splFixedNamePrimitiveTypes.add("timestamp");
    _splFixedNamePrimitiveTypes.add("blob");
  }

  public static String getSplTypeApiValue(String internalValue) {
    if (internalValue == null) {
      return "unknown";
    }
    String lookupValue = internalValue.trim().toLowerCase(Locale.ENGLISH);
    return _splFixedNamePrimitiveTypes.contains(lookupValue) ? lookupValue : "unknown";
  }
}
