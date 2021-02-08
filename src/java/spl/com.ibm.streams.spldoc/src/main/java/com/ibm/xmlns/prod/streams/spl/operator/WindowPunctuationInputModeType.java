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
 * A representation of the literals of the enumeration '<em><b>Window Punctuation Input Mode
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getWindowPunctuationInputModeType()
 * @model extendedMetaData="name='windowPunctuationInputModeType'"
 * @generated
 */
public enum WindowPunctuationInputModeType implements Enumerator {
  /**
   * The '<em><b>Expecting</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #EXPECTING_VALUE
   * @generated
   * @ordered
   */
  EXPECTING(0, "Expecting", "Expecting"),

  /**
   * The '<em><b>Oblivious</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #OBLIVIOUS_VALUE
   * @generated
   * @ordered
   */
  OBLIVIOUS(1, "Oblivious", "Oblivious"),

  /**
   * The '<em><b>Window Bound</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_BOUND_VALUE
   * @generated
   * @ordered
   */
  WINDOW_BOUND(2, "WindowBound", "WindowBound");

  /**
   * The '<em><b>Expecting</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Expecting</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #EXPECTING
   * @model name="Expecting"
   * @generated
   * @ordered
   */
  public static final int EXPECTING_VALUE = 0;

  /**
   * The '<em><b>Oblivious</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Oblivious</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #OBLIVIOUS
   * @model name="Oblivious"
   * @generated
   * @ordered
   */
  public static final int OBLIVIOUS_VALUE = 1;

  /**
   * The '<em><b>Window Bound</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Window Bound</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_BOUND
   * @model name="WindowBound"
   * @generated
   * @ordered
   */
  public static final int WINDOW_BOUND_VALUE = 2;

  /**
   * An array of all the '<em><b>Window Punctuation Input Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final WindowPunctuationInputModeType[] VALUES_ARRAY =
      new WindowPunctuationInputModeType[] {
        EXPECTING, OBLIVIOUS, WINDOW_BOUND,
      };

  /**
   * A public read-only list of all the '<em><b>Window Punctuation Input Mode Type</b></em>'
   * enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<WindowPunctuationInputModeType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Window Punctuation Input Mode Type</b></em>' literal with the specified
   * literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowPunctuationInputModeType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowPunctuationInputModeType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Punctuation Input Mode Type</b></em>' literal with the specified
   * name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowPunctuationInputModeType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      WindowPunctuationInputModeType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Window Punctuation Input Mode Type</b></em>' literal with the specified
   * integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static WindowPunctuationInputModeType get(int value) {
    switch (value) {
      case EXPECTING_VALUE:
        return EXPECTING;
      case OBLIVIOUS_VALUE:
        return OBLIVIOUS;
      case WINDOW_BOUND_VALUE:
        return WINDOW_BOUND;
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
  private WindowPunctuationInputModeType(int value, String name, String literal) {
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
} // WindowPunctuationInputModeType
