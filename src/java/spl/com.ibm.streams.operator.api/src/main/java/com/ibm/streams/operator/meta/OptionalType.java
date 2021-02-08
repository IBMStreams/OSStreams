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

package com.ibm.streams.operator.meta;

import com.ibm.streams.operator.Type;

/**
 * Meta-type for an attribute of type {@link com.ibm.streams.operator.Type.MetaType#OPTIONAL
 * OPTIONAL}.
 *
 * @since IBM&reg; Streams Version 4.3
 */
public interface OptionalType extends CompositeType {

  /**
   * Return the Type of the optional value.
   *
   * @return The Type of the optional value.
   */
  public Type getValueType();
}
