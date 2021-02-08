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

package com.ibm.streams.operator.internal.ports.actions;

import com.ibm.streams.operator.internal.ports.StreamAction;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

/** Remove export stream properties from the set of properties for the output port. */
public class RemoveExportProperties implements StreamAction<Object> {

  private final Set<String> names;

  public RemoveExportProperties(String... names) {
    this.names = new HashSet<String>(Arrays.asList(names));
  }

  @Override
  public Type getType() {
    return Type.REMOVE_EXPORT_PROPERTIES;
  }

  public Set<String> getNames() {
    return names;
  }

  /**
   * No return value, Action is executed by the a RuntimeStream implementation extracting the
   * information and acting on it.
   */
  @Override
  public Object call() {
    return null;
  }
}
