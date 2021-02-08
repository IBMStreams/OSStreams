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

package com.ibm.streams.operator.internal.types;

import com.ibm.streams.operator.meta.EnumType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Random;
import java.util.StringTokenizer;

/**
 * SPL enum is serialized as a uint32 representing the index into the list of identifiers. This
 * numeric representation of the SPL enumeration is exposed in the C++ api. In Java we represent the
 * SPL enum as a String object but allow it to be converted to a Java enum using a registered
 * mapping to that Enum type. The String representation of the value is its identifier.
 */
final class EnumSpl extends StatelessType<String> implements EnumType {

  /** SPL enum definition. */
  private final String definition;
  /** List of the SPL enum identifiers in order. */
  private final List<String> identifiers;

  /** Map between identifiers and index. */
  private final Map<String, Integer> identifierLookup;

  static ImplementationType<?> getEnumType(String definition) {

    int start = definition.indexOf('{');
    int end = definition.indexOf('}');
    String types = definition.substring(start + 1, end);
    StringTokenizer st = new StringTokenizer(types, ",");
    ArrayList<String> ids = new ArrayList<String>();
    while (st.hasMoreTokens()) {
      String s = st.nextToken();
      ids.add(s.intern());
    }

    List<String> identifiers = Collections.unmodifiableList(ids);

    return new EnumSpl(definition, identifiers);
  }

  private EnumSpl(String definition, List<String> identifiers) {
    super(MetaType.ENUM, identifiers.get(0));
    this.definition = definition;
    this.identifiers = identifiers;

    identifierLookup = new HashMap<String, Integer>();
    for (int i = 0; i < identifiers.size(); i++) {
      identifierLookup.put(identifiers.get(i), i);
    }
  }

  /** Object type is String until an enum is registered and then the type is is the enum class. */
  @Override
  public Class<?> getObjectType() {
    return String.class;
  }

  /** Convert an index to a identifier. */
  private String indexToIdentifier(int index) {
    try {
      return getIdentifiers().get(index);
    } catch (IndexOutOfBoundsException e) {
      throw new IllegalArgumentException(
          new Message(
                  Key.SPL_RUNTIME_INDEX_OUT_OF_RANGE_FOR_LANGUAGE,
                  Integer.toString(index),
                  getLanguageType())
              .getLocalizedMessageAndPrefix());
    }
  }

  /** Convert an String to the SPL enum's index (ordinal). */
  private int identifierToIndex(final String identifier) {
    Integer index = identifierLookup.get(identifier);
    if (index == null)
      throw new IllegalArgumentException(
          new Message(
                  Key.SPL_RUNTIME_IDENTIFIER_NOT_VALID_FOR_LANGUAGE, identifier, getLanguageType())
              .getLocalizedMessageAndPrefix());
    return index;
  }

  @Override
  public int hashCode() {
    return getIdentifiers().hashCode();
  }

  @Override
  public boolean equals(Object other) {
    if (!super.equals(other)) return false;

    EnumSpl eso = (EnumSpl) other;
    return getIdentifiers().equals(eso.getIdentifiers());
  }

  @Override
  public String getLanguageType() {
    return definition;
  }

  @Override
  public Object readValue(ByteBuffer bytes) {
    return indexToIdentifier(bytes.getInt());
  }

  @Override
  public int getSerializedSize() {
    return 4;
  }

  @Override
  public void writeValue(ByteBuffer bytes, Object value) {
    bytes.putInt(identifierToIndex((String) value));
  }

  /** Create a random index and then return the object representing the identifier. */
  @Override
  synchronized String _randomValue(Random rand) {
    final List<String> identifiers = getIdentifiers();
    return identifiers.get(rand.nextInt(identifiers.size()));
  }

  @Override
  public String convertFrom(String value) {
    return indexToIdentifier(identifierToIndex(value));
  }

  @Override
  public String convertFromObject(Object value) {
    if (value instanceof String) return convertFrom((String) value);
    throw invalidConversion(value.getClass().getName());
  }

  @Override
  String safeCopy(String value) {
    return convertFrom(value);
  }

  @Override
  public <E extends Enum<E>> E getEnum(Class<E> enumType, Object value) {
    return Enum.valueOf(enumType, (String) value);
  }

  @Override
  public <E extends Enum<E>> String convertFrom(Class<E> enumType, E value) {
    return convertFrom(value.name());
  }

  /*
   ** Methods of EnumType
   **/

  @Override
  public List<String> getIdentifiers() {
    return identifiers;
  }

  @Override
  public synchronized <E extends Enum<E>> void verifyMapping(final Class<E> enumType) {

    // Verify all identifiers can be converted to
    // the enumType
    for (String identifier : getIdentifiers()) Enum.valueOf(enumType, identifier);
  }

  @Override
  public <E extends Enum<E>> void verifyOrdinals(Class<E> enumType) {
    for (int i = 0; i < getIdentifiers().size(); i++) {
      E constant = Enum.valueOf(enumType, getIdentifiers().get(i));

      if (constant.ordinal() != i) {
        throw new IllegalArgumentException(
            new Message(
                    Key.SPL_RUNTIME_INDEX_NOT_EQUAL_ORDINAL_FOR_LANGUAGE,
                    getLanguageType(),
                    constant.name(),
                    enumType.getName(),
                    constant.name())
                .getLocalizedMessageAndPrefix());
      }
    }
  }
}
