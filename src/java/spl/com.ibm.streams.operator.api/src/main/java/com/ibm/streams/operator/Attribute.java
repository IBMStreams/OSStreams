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

package com.ibm.streams.operator;

/**
 * Meta-data for an attribute within a {@link com.ibm.streams.operator.StreamSchema StreamSchema}.
 */
public interface Attribute {

  /** Get the name of this attribute. */
  public String getName();

  /** Get the type information for this attribute. */
  public Type getType();

  /**
   * Get the logical index (zero based) of the attribute in the tuple.
   *
   * @return Index of attribute.
   */
  public int getIndex();

  /**
   * Are two attributes the same. Two attributes are the same if they have the same type and name.
   * Note that index is irrelevant for this comparison.
   *
   * @param other Attribute to compare against.
   * @return True if other is the same as this, false otherwise.
   */
  public boolean same(Attribute other);
}
