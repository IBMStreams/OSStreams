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
 * Type specific information for SPL bounded types, {@link
 * com.ibm.streams.operator.Type.MetaType#BLIST BLIST}, {@link
 * com.ibm.streams.operator.Type.MetaType#BMAP BMAP}, {@link
 * com.ibm.streams.operator.Type.MetaType#BSET BSET} and {@link
 * com.ibm.streams.operator.Type.MetaType#BSTRING BSTRING}.
 *
 * <p>Bounded types are limited to <code>getBoundedSize()</code> number of elements. On assignment
 * of a value to a bounded type excess elements are silently dropped, when dropping elements the
 * behavior is specific to the type.
 *
 * <UL>
 *   <LI>For bounded lists elements beyond the bound are dropped so that the first <code>
 *       getBoundedSize()</code> elements will be kept.
 *   <LI>For bounded rstrings bytes beyond the bound are dropped so that the first <code>
 *       getBoundedSize()</code> bytes will be kept. Note that the truncation is regardless of the
 *       encoding of the characters and thus with some encoding the truncation may result in an
 *       invalid encoded sequence.
 *   <LI>For bounded maps and sets there is no guarantee as to which elements will be discarded,
 *       only that the assignment of a value with too many elements will result in a map or set of
 *       size <code>getBoundedSize()</code>.
 * </UL>
 */
public interface BoundedType extends Type {

  /**
   * Get the bound size of the type.
   *
   * @return The bound size.
   */
  public int getBoundedSize();
}
