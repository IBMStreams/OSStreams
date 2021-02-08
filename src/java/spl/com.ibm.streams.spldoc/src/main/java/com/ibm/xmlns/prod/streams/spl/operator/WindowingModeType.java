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

/** */
package com.ibm.xmlns.prod.streams.spl.operator;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.eclipse.emf.common.util.Enumerator;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Windowing Mode Type</b></em>', and
 * utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getWindowingModeType()
 * @model extendedMetaData="name='windowingModeType'"
 * @generated
 */
public enum WindowingModeType implements Enumerator {
  /**
   * The '<em><b>Non Windowed</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #NON_WINDOWED_VALUE
   * @generated
   * @ordered
   */
  NON_WINDOWED(0, "NonWindowed", "NonWindowed"),

  /**
   * The '<em><b>Windowed</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOWED_VALUE
   * @generated
   * @ordered
   */
  WINDOWED(1, "Windowed", "Windowed"),

  /**
   * The '<em><b>Optionally Windowed</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #OPTIONALLY_WINDOWED_VALUE
   * @generated
   * @ordered
   */
  OPTIONALLY_WINDOWED(2, "OptionallyWindowed", "OptionallyWindowed");

  /**
   * The '<em><b>Non Windowed</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Non Windowed</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #NON_WINDOWED
   * @model name="NonWindowed"
   * @generated
   * @ordered
   */
  public static final int NON_WINDOWED_VALUE = 0;

  /**
   * The '<em><b>Windowed</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Windowed</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #WINDOWED
   * @model name="Windowed"
   * @generated
   * @ordered
   */
  public static final int WINDOWED_VALUE = 1;

  /**
   * The '<em><b>Optionally Windowed</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Optionally Windowed</b></em>' literal object isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #OPTIONALLY_WINDOWED
   * @model name="OptionallyWindowed"
   * @generated
   * @ordered
   */
  public static final int OPTIONALLY_WINDOWED_VALUE = 2;

  /**
   * An array of all the '<em><b>Windowing Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final WindowingModeType[] VALUES_ARRAY =
      new WindowingModeType[] {
        NON_WINDOWED, WINDOWED, OPTIONALLY_WINDOWED,
      };

  /**
   * A public read-only list of all the '<em><b>Windowing Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<WindowingModeType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Windowing Mode Type</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowingModeType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowingModeType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Windowing Mode Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowingModeType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowingModeType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Windowing Mode Type</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowingModeType get(int value) {
    switch (value) {
      case NON_WINDOWED_VALUE:
        return NON_WINDOWED;
      case WINDOWED_VALUE:
        return WINDOWED;
      case OPTIONALLY_WINDOWED_VALUE:
        return OPTIONALLY_WINDOWED;
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
  private WindowingModeType(int value, String name, String literal) {
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
} // WindowingModeType
