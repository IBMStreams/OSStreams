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
import java.net.URL;
import java.util.Random;

/**
 * Meta-type for an attribute of type {@link com.ibm.streams.operator.Type.MetaType#XML XML}.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface XMLType extends Type {

  /**
   * Get the schema of the XML attribute.
   *
   * @return The URL for the declared XML schema or null if no schema was declared.
   */
  public URL getDeclaredSchema();

  /**
   * {@inheritDoc}
   *
   * <p>If {@code getDeclaredSchema()} returns null then a simple well-formed document is returned
   * with random encoding, element names, attribute names and contents.
   *
   * <p>If {@code getDeclaredSchema()} returns a URL then then the default value returned by {@link
   * Type#getDefaultValue()} is always returned.
   */
  @Override
  public Object randomValue(Random rand);
}
