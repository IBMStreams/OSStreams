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

/**
 * <copyright> </copyright>
 *
 * <p>$Id$
 */
package com.ibm.xmlns.prod.streams.spl.source.code;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.eclipse.emf.common.util.Enumerator;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Window Policy Kind Enum
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowPolicyKindEnumType()
 * @model extendedMetaData="name='windowPolicyKindEnumType'"
 * @generated
 */
public enum WindowPolicyKindEnumType implements Enumerator {
  /**
   * The '<em><b>Count</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #COUNT_VALUE
   * @generated
   * @ordered
   */
  COUNT(0, "count", "count"),

  /**
   * The '<em><b>Time</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TIME_VALUE
   * @generated
   * @ordered
   */
  TIME(1, "time", "time"),

  /**
   * The '<em><b>Delta</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #DELTA_VALUE
   * @generated
   * @ordered
   */
  DELTA(2, "delta", "delta"),

  /**
   * The '<em><b>Punct</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #PUNCT_VALUE
   * @generated
   * @ordered
   */
  PUNCT(3, "punct", "punct");

  /**
   * The '<em><b>Count</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Count</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #COUNT
   * @model name="count"
   * @generated
   * @ordered
   */
  public static final int COUNT_VALUE = 0;

  /**
   * The '<em><b>Time</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Time</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TIME
   * @model name="time"
   * @generated
   * @ordered
   */
  public static final int TIME_VALUE = 1;

  /**
   * The '<em><b>Delta</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Delta</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #DELTA
   * @model name="delta"
   * @generated
   * @ordered
   */
  public static final int DELTA_VALUE = 2;

  /**
   * The '<em><b>Punct</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Punct</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #PUNCT
   * @model name="punct"
   * @generated
   * @ordered
   */
  public static final int PUNCT_VALUE = 3;

  /**
   * An array of all the '<em><b>Window Policy Kind Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final WindowPolicyKindEnumType[] VALUES_ARRAY =
      new WindowPolicyKindEnumType[] {
        COUNT, TIME, DELTA, PUNCT,
      };

  /**
   * A public read-only list of all the '<em><b>Window Policy Kind Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<WindowPolicyKindEnumType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Window Policy Kind Enum Type</b></em>' literal with the specified literal
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowPolicyKindEnumType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowPolicyKindEnumType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Policy Kind Enum Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowPolicyKindEnumType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowPolicyKindEnumType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Policy Kind Enum Type</b></em>' literal with the specified integer
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowPolicyKindEnumType get(int value) {
    switch (value) {
      case COUNT_VALUE:
        return COUNT;
      case TIME_VALUE:
        return TIME;
      case DELTA_VALUE:
        return DELTA;
      case PUNCT_VALUE:
        return PUNCT;
    }
    return null;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private final int value;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private final String name;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private final String literal;

  /**
   * Only this class can construct instances.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private WindowPolicyKindEnumType(int value, String name, String literal) {
    this.value = value;
    this.name = name;
    this.literal = literal;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getValue() {
    return value;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getName() {
    return name;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getLiteral() {
    return literal;
  }

  /**
   * Returns the literal value of the enumerator, which is its string representation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public String toString() {
    return literal;
  }
} // WindowPolicyKindEnumType
