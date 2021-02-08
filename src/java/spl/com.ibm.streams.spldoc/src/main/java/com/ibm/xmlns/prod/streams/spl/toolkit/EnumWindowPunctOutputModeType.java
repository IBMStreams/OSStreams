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
package com.ibm.xmlns.prod.streams.spl.toolkit;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.eclipse.emf.common.util.Enumerator;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Enum Window Punct Output Mode
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumWindowPunctOutputModeType()
 * @model extendedMetaData="name='enumWindowPunctOutputModeType'"
 * @generated
 */
public enum EnumWindowPunctOutputModeType implements Enumerator {
  /**
   * The '<em><b>Generating</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #GENERATING_VALUE
   * @generated
   * @ordered
   */
  GENERATING(0, "Generating", "Generating"),

  /**
   * The '<em><b>Free</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #FREE_VALUE
   * @generated
   * @ordered
   */
  FREE(1, "Free", "Free"),

  /**
   * The '<em><b>Preserving</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #PRESERVING_VALUE
   * @generated
   * @ordered
   */
  PRESERVING(2, "Preserving", "Preserving");

  /**
   * The '<em><b>Generating</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Generating</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #GENERATING
   * @model name="Generating"
   * @generated
   * @ordered
   */
  public static final int GENERATING_VALUE = 0;

  /**
   * The '<em><b>Free</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Free</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #FREE
   * @model name="Free"
   * @generated
   * @ordered
   */
  public static final int FREE_VALUE = 1;

  /**
   * The '<em><b>Preserving</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Preserving</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #PRESERVING
   * @model name="Preserving"
   * @generated
   * @ordered
   */
  public static final int PRESERVING_VALUE = 2;

  /**
   * An array of all the '<em><b>Enum Window Punct Output Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final EnumWindowPunctOutputModeType[] VALUES_ARRAY =
      new EnumWindowPunctOutputModeType[] {
        GENERATING, FREE, PRESERVING,
      };

  /**
   * A public read-only list of all the '<em><b>Enum Window Punct Output Mode Type</b></em>'
   * enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<EnumWindowPunctOutputModeType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Enum Window Punct Output Mode Type</b></em>' literal with the specified
   * literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static EnumWindowPunctOutputModeType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      EnumWindowPunctOutputModeType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Enum Window Punct Output Mode Type</b></em>' literal with the specified
   * name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static EnumWindowPunctOutputModeType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      EnumWindowPunctOutputModeType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Enum Window Punct Output Mode Type</b></em>' literal with the specified
   * integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static EnumWindowPunctOutputModeType get(int value) {
    switch (value) {
      case GENERATING_VALUE:
        return GENERATING;
      case FREE_VALUE:
        return FREE;
      case PRESERVING_VALUE:
        return PRESERVING;
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
  private EnumWindowPunctOutputModeType(int value, String name, String literal) {
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
} // EnumWindowPunctOutputModeType
