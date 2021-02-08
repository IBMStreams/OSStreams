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
 * A representation of the literals of the enumeration '<em><b>Window Type Enum Type</b></em>', and
 * utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowTypeEnumType()
 * @model extendedMetaData="name='windowTypeEnumType'"
 * @generated
 */
public enum WindowTypeEnumType implements Enumerator {
  /**
   * The '<em><b>Tumbling</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TUMBLING_VALUE
   * @generated
   * @ordered
   */
  TUMBLING(0, "tumbling", "tumbling"),

  /**
   * The '<em><b>Sliding</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #SLIDING_VALUE
   * @generated
   * @ordered
   */
  SLIDING(1, "sliding", "sliding");

  /**
   * The '<em><b>Tumbling</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Tumbling</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TUMBLING
   * @model name="tumbling"
   * @generated
   * @ordered
   */
  public static final int TUMBLING_VALUE = 0;

  /**
   * The '<em><b>Sliding</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Sliding</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #SLIDING
   * @model name="sliding"
   * @generated
   * @ordered
   */
  public static final int SLIDING_VALUE = 1;

  /**
   * An array of all the '<em><b>Window Type Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final WindowTypeEnumType[] VALUES_ARRAY =
      new WindowTypeEnumType[] {
        TUMBLING, SLIDING,
      };

  /**
   * A public read-only list of all the '<em><b>Window Type Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<WindowTypeEnumType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Window Type Enum Type</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowTypeEnumType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowTypeEnumType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Type Enum Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowTypeEnumType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowTypeEnumType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Type Enum Type</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static WindowTypeEnumType get(int value) {
    switch (value) {
      case TUMBLING_VALUE:
        return TUMBLING;
      case SLIDING_VALUE:
        return SLIDING;
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
  private WindowTypeEnumType(int value, String name, String literal) {
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
} // WindowTypeEnumType
