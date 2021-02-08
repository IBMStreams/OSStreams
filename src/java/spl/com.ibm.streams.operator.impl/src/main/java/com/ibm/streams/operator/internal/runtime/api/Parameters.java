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

package com.ibm.streams.operator.internal.runtime.api;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

/**
 * Sets up the set of parameters for the operator. An operator allows multiple values for a single
 * parameter.
 *
 * <p>Class is not thread-safe, expected to be used by a single thread during initialization of the
 * operator's environment from the C++ primitive operator JavaOp code.
 */
public final class Parameters {

  private final Map<String, List<String>> parameters = new HashMap<String, List<String>>();

  /**
   * Add a single parameter value to the current set.
   *
   * @param parameterName
   * @param value
   */
  public void addParameterValue(String parameterName, String value) {
    List<String> values = parameters.get(parameterName);
    if (values == null) {
      values = new ArrayList<String>();
      parameters.put(parameterName, values);
    }

    values.add(value);
  }

  /**
   * Get the list of values for a given parameter.
   *
   * @param parameterName
   * @return
   */
  public List<String> getParameterValues(String parameterName) {
    List<String> values = parameters.get(parameterName);
    if (values == null) return Collections.emptyList();
    return Collections.unmodifiableList(values);
  }

  /**
   * Get a single parameter value, assumes the parameter is always set.
   *
   * @param parameterName
   * @return
   */
  public String getParameterValue(String parameterName) {
    return getParameterValues(parameterName).get(0);
  }

  /**
   * Get a single parameter value, or the default value if the parameter is not set.
   *
   * @param parameterName the name of the parameter
   * @param defaultValue a default value
   * @return the string value of the parameter, or the default value if there is no parameter with
   *     the specified name.
   */
  public String getParameterValue(String parameterName, String defaultValue) {
    List<String> values = getParameterValues(parameterName);
    return (values.isEmpty()) ? defaultValue : values.get(0);
  }

  /**
   * Remove a parameter from the set.
   *
   * @param parameterName
   */
  public void remove(String parameterName) {
    parameters.remove(parameterName);
  }

  public Map<String, List<String>> getReadOnlyParams() {
    return Collections.unmodifiableMap(parameters);
  }

  /** Remove all internal parameters, which start with "[". */
  public void removeInternal() {
    for (Iterator<String> names = parameters.keySet().iterator(); names.hasNext(); )
      if (names.next().startsWith(OperatorAdapter.OP_PREFIX)) names.remove();
  }

  /**
   * Copy a set of parameters into this set.
   *
   * @param other
   */
  public void copy(Map<String, List<String>> other) {
    for (String parameterName : other.keySet()) {
      for (String value : other.get(parameterName)) addParameterValue(parameterName, value);
    }
  }
}
