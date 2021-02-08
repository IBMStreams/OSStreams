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

import com.ibm.distillery.utils.DistilleryExceptionCode;
import com.ibm.streams.instance.sam.model.topology.StreamProperty;
import com.ibm.streams.sam.exc.SAMDuplicatePropertyException;
import com.ibm.streams.sam.exc.SAMException;
import com.ibm.streams.sam.exc.SAMPropertyNotFoundException;
import java.math.BigInteger;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Vector;

public class DynamicConnOpProps {

  private BigInteger _oportId;
  private String _scope;
  private Map<String, StreamProperty> _properties;

  public DynamicConnOpProps(BigInteger opId, String scope) {
    _oportId = opId;
    _scope = scope;
    _properties = new HashMap<String, StreamProperty>();
  }

  public DynamicConnOpProps(BigInteger opId, String scope, Map<String, StreamProperty> properties) {
    _oportId = opId;
    _scope = scope;
    _properties = properties;
  }

  public BigInteger getOportId() {
    return _oportId;
  }

  public String getScope() {
    return _scope;
  }

  public Map<String, StreamProperty> getProperties() {
    return _properties;
  }

  public List<StreamProperty> getProperties(List<String> propNames) {
    ArrayList<StreamProperty> props = new ArrayList<StreamProperty>();
    for (String name : propNames) {
      if (_properties.containsKey(name)) {
        props.add(_properties.get(name));
      }
    }
    return props;
  }

  public StreamProperty getProperty(String name) {
    if (_properties.containsKey(name)) {
      return _properties.get(name);
    } else {
      return null;
    }
  }

  public void addProperties(List<StreamProperty> properties) throws SAMException {
    // Check if any property already exist...
    for (StreamProperty prop : properties) {
      if (_properties.containsKey(prop.getName())) {
        String msg =
            "Unable to add stream properties: "
                + properties.toString()
                + ". Reason: attempting to add a property that is already present -- : "
                + "streamName: "
                + _oportId.toString()
                + " propertyName: "
                + prop.getName();
        throw new SAMDuplicatePropertyException(
            msg,
            new DistilleryExceptionCode("CDISR1049E"),
            new Vector<String>(Collections.singletonList(prop.getName())));
      }
    }
    // add them...
    for (StreamProperty prop : properties) {
      _properties.put(prop.getName(), prop);
    }
  }

  public void setProperties(List<StreamProperty> properties) throws SAMException {
    // Check if any property already exist...
    for (StreamProperty prop : properties) {
      if (!_properties.containsKey(prop.getName())) {
        String msg =
            "Unable to set stream properties: "
                + properties.toString()
                + ". Reason: Property '"
                + prop.getName()
                + "' does not exist.";
        throw new SAMPropertyNotFoundException(
            msg,
            new DistilleryExceptionCode("CDISR1014E"),
            new Vector<String>(Collections.singletonList(prop.getName())));
      }
    }
    // set them ...
    for (StreamProperty prop : properties) {
      _properties.put(prop.getName(), prop);
    }
  }

  public void removeProperties(List<String> propertyNames) {
    for (String name : propertyNames) {
      _properties.remove(name);
    }
  }

  public void addProperty(StreamProperty prop) {
    if (!_properties.containsKey(prop.getName())) {
      _properties.put(prop.getName(), prop);
    }
  }

  public void setProperty(StreamProperty prop) {
    if (_properties.containsKey(prop.getName())) {
      _properties.remove(prop.getName());
    }
    _properties.put(prop.getName(), prop);
  }

  public void removeProperty(String propName) {
    _properties.remove(propName);
  }

  public boolean doesPropExist(String propName) {
    return _properties.containsKey(propName);
  }

  public String getPropType(String propName) {
    if (_properties.containsKey(propName)) {
      return _properties.get(propName).getType();
    } else {
      return null;
    }
  }

  public List<String> getPropValues(String propName) {
    if (_properties.containsKey(propName)) {
      return _properties.get(propName).getValues();
    } else {
      return null;
    }
  }

  public String toString() {
    StringBuilder builder = new StringBuilder();
    builder.append("Output Port [").append(_oportId);
    builder.append("] Scope [").append(_scope);
    builder.append("] Properties: ").append(_properties.toString());
    return builder.toString();
  }
}
