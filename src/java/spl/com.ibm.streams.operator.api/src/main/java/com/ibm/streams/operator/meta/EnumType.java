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

import com.ibm.streams.operator.OutputTuple;
import com.ibm.streams.operator.Tuple;
import com.ibm.streams.operator.Type;
import java.util.List;

/**
 * Meta-type for an attribute of type {@link com.ibm.streams.operator.Type.MetaType#ENUM ENUM}.
 *
 * @since InfoSphere&reg; Streams Version 3.0
 */
public interface EnumType extends Type {

  /**
   * Get the list of identifiers declared for this SPL enumeration.
   *
   * @return A read only list of this enumerations identifiers in declared order
   */
  public List<String> getIdentifiers();

  /**
   * Verify that the mapping between this SPL's {@code enum} identifiers and Java enumeration type
   * {@code E} is valid. An {@code enum} may be mapped to {@code E} if all of its identifiers can be
   * converted to an enumeration value using <br>
   * {@code Enum.valueOf(enumType, identifier)} <br>
   * {@code E} may contain constants that do not map to this {@code enum}. <br>
   * Verifying the mapping ensures that this SPL {@code enum} can be safely converted to values
   * {@code E} using {@link Tuple#getEnum(Class, int)} and {@link OutputTuple#setEnum(Class, int,
   * Enum)}
   *
   * @param <E> Enumeration type to verify against this SPL {@code enum}.
   * @param enumType Class of enumeration type to be mapped to.
   * @exception IllegalArgumentException SPL {@code enum} cannot be mapped to {@code E}
   */
  public <E extends Enum<E>> void verifyMapping(Class<E> enumType);

  /**
   * Verify that declared order of this SPL's {@code enum} identifiers matches order and position of
   * constants for {@code E}. <br>
   * {@code E} may contain constants that do not map to this {@code enum} but such constants must
   * have an ordinal value greater or equal to the number of identifiers.
   *
   * @param <E> Enumeration type to verify against this SPL {@code enum}.
   * @param enumType Class of enumeration type to be mapped to.
   */
  public <E extends Enum<E>> void verifyOrdinals(Class<E> enumType);
}
