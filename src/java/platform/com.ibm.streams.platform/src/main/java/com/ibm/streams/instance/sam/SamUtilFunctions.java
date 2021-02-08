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

package com.ibm.streams.instance.sam;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.messages.StreamsRuntimeMessagesKey;
import com.ibm.streams.platform.services.StreamPropertiesType;
import com.ibm.streams.platform.services.StreamPropertyType;
import com.ibm.streams.spl.expressions.SPLMeta.SPLBoolean;
import com.ibm.streams.spl.expressions.SPLMeta.SPLBooleanList;
import com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64;
import com.ibm.streams.spl.expressions.SPLMeta.SPLFloat64List;
import com.ibm.streams.spl.expressions.SPLMeta.SPLInt64;
import com.ibm.streams.spl.expressions.SPLMeta.SPLInt64List;
import com.ibm.streams.spl.expressions.SPLMeta.SPLRstring;
import com.ibm.streams.spl.expressions.SPLMeta.SPLRstringList;
import com.ibm.streams.spl.expressions.SPLMeta.SPLType;
import com.ibm.streams.spl.expressions.SPLMeta.SPLValue;
import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class SamUtilFunctions {

  public static String getStreamName(String sname, String appname) {
    return sname + "@" + appname;
  }

  public static StreamProperty getStreamPropertyFromStreamPropertyType(
      StreamPropertyType property) {
    return new StreamProperty(property.getName(), property.getType(), property.getValue());
  }

  public static List<StreamProperty> getStreamPropertyFromStreamPropertiesType(
      StreamPropertiesType properties) {
    List<StreamProperty> props = new ArrayList<StreamProperty>();
    for (StreamPropertyType propType : properties.getProperty()) {
      StreamProperty prop =
          new StreamProperty(propType.getName(), propType.getType(), propType.getValue());
      props.add(prop);
    }
    return props;
  }

  public static StreamPropertyType getStreamPropertyTypeFromStreamProperty(
      StreamProperty property) {
    StreamPropertyType propType = new StreamPropertyType();
    propType.setName(property.getName());
    propType.setType(property.getType());
    propType.getValue().addAll(property.getValues());
    return propType;
  }

  public static StreamPropertiesType getStreamPropertiesTypeFromStreamProperty(
      Collection<StreamProperty> properties) {
    StreamPropertiesType propsType = new StreamPropertiesType();
    for (StreamProperty property : properties) {
      StreamPropertyType propType = new StreamPropertyType();
      propType.setName(property.getName());
      propType.setType(property.getType());
      propType.getValue().addAll(property.getValues());
      propsType.getProperty().add(propType);
    }
    return propsType;
  }
  /**
   * public static ImportedStreamsType getImportedStreamsTypeFromImportedStreams(ImportedStreams is)
   * { ImportedStreamsType importedStreams = new ImportedStreamsType(); if(is.getFilter() != null) {
   * importedStreams.setFilter(is.getFilter()); } if(!is.getNameBasedImports().isEmpty()) {
   * for(BigInteger index : is.getNameBasedImports().keySet()) { NameBasedImport nbi =
   * is.getNameBasedImports().get(index); NameBasedImportType namedImport = new
   * NameBasedImportType(); namedImport.setIndex(index);
   * namedImport.setApplicationName(nbi.getApplicationName());
   * namedImport.setApplicationScope(nbi.getApplicationScope());
   * namedImport.setStreamName(nbi.getStreamName());
   * namedImport.setImportOperName(nbi.getImportOperName());
   * importedStreams.getNameBasedImport().add(namedImport); } } if(is.getPropertyBasedImport() !=
   * null) { PropertyBasedImportType propImport = new PropertyBasedImportType();
   * propImport.setIndex(is.getPropertyBasedImport().getIndex());
   * propImport.setApplicationScope(is.getPropertyBasedImport().getApplicationScope());
   * propImport.setImportOperName(is.getPropertyBasedImport().getImportOperName());
   * propImport.setSubscription(is.getPropertyBasedImport().getSubscription());
   * importedStreams.setPropertyBasedImport(propImport); } return importedStreams; }
   */
  public static Map<String, SPLValue> getSPLValueMapFromStreamProperties(
      Collection<StreamProperty> props) throws StreamsException {
    Map<String, SPLValue> splVals = new HashMap<String, SPLValue>();
    for (StreamProperty prop : props) {
      SPLValue val = getSPLValue(prop.getType(), prop.getValues());
      splVals.put(prop.getName(), val);
      if (Trace.isEnabled(Level.DEBUG)) {
        Trace.logDebug(
            "Done getting SPLValue for "
                + prop.getName()
                + " of type "
                + prop.getType()
                + " with values "
                + prop.getValues().toString());
      }
    }
    return splVals;
  }

  private static SPLValue getSPLValue(String type, List<String> values) throws StreamsException {
    switch (SPLType.fromString(type)) {
      case BOOLEAN:
        return new SPLBoolean(Boolean.parseBoolean(values.get(0)));
      case INT64:
        return new SPLInt64(Long.parseLong(values.get(0)));
      case FLOAT64:
        return new SPLFloat64(Double.parseDouble(values.get(0)));
      case RSTRING:
        {
          // property strings have quotes; we must strip them off
          String str = values.get(0);
          return new SPLRstring(str.substring(1, str.length() - 1));
        }
      case LIST_BOOLEAN:
        {
          List<Boolean> vals = new ArrayList<Boolean>(values.size());
          for (String v : values) {
            vals.add(Boolean.valueOf(v));
          }
          return new SPLBooleanList(vals);
        }
      case LIST_INT64:
        {
          List<Long> vals = new ArrayList<Long>(values.size());
          for (String v : values) {
            vals.add(Long.valueOf(v));
          }
          return new SPLInt64List(vals);
        }
      case LIST_FLOAT64:
        {
          List<Double> vals = new ArrayList<Double>(values.size());
          for (String v : values) {
            vals.add(Double.valueOf(v));
          }
          return new SPLFloat64List(vals);
        }
      case LIST_RSTRING:
        {
          List<String> vals = new ArrayList<String>(values.size());
          for (String v : values) {
            vals.add(v.substring(1, v.length() - 1));
          }
          return new SPLRstringList(vals);
        }
      default:
        {
          throw new StreamsException(StreamsRuntimeMessagesKey.Key.SAMUnrecognizedSPLType, type);
        }
    }
  }

  public static boolean isValidWrapper(String wrapper) {
    boolean valid = false;
    if (wrapper != null
        && !wrapper.isEmpty()
        && (wrapper.equals("valgrind")
            || wrapper.equals("console")
            || wrapper.equals("sdb")
            || wrapper.equals("gdb")
            || wrapper.equals("nrgdb"))) {
      valid = true;
    }
    return valid;
  }
}
