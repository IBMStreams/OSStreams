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
 * A representation of the literals of the enumeration '<em><b>Java Op Expression Mode
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpExpressionModeType()
 * @model extendedMetaData="name='javaOpExpressionModeType'"
 * @generated
 */
public enum JavaOpExpressionModeType implements Enumerator {
  /**
   * The '<em><b>Attribute</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE_VALUE
   * @generated
   * @ordered
   */
  ATTRIBUTE(0, "Attribute", "Attribute"),

  /**
   * The '<em><b>Attribute Free</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE_FREE_VALUE
   * @generated
   * @ordered
   */
  ATTRIBUTE_FREE(1, "AttributeFree", "AttributeFree"),

  /**
   * The '<em><b>Custom Literal</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #CUSTOM_LITERAL_VALUE
   * @generated
   * @ordered
   */
  CUSTOM_LITERAL(2, "CustomLiteral", "CustomLiteral");

  /**
   * The '<em><b>Attribute</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Attribute</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE
   * @model name="Attribute"
   * @generated
   * @ordered
   */
  public static final int ATTRIBUTE_VALUE = 0;

  /**
   * The '<em><b>Attribute Free</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Attribute Free</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE_FREE
   * @model name="AttributeFree"
   * @generated
   * @ordered
   */
  public static final int ATTRIBUTE_FREE_VALUE = 1;

  /**
   * The '<em><b>Custom Literal</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Custom Literal</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #CUSTOM_LITERAL
   * @model name="CustomLiteral"
   * @generated
   * @ordered
   */
  public static final int CUSTOM_LITERAL_VALUE = 2;

  /**
   * An array of all the '<em><b>Java Op Expression Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final JavaOpExpressionModeType[] VALUES_ARRAY =
      new JavaOpExpressionModeType[] {
        ATTRIBUTE, ATTRIBUTE_FREE, CUSTOM_LITERAL,
      };

  /**
   * A public read-only list of all the '<em><b>Java Op Expression Mode Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<JavaOpExpressionModeType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Java Op Expression Mode Type</b></em>' literal with the specified literal
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static JavaOpExpressionModeType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      JavaOpExpressionModeType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Java Op Expression Mode Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static JavaOpExpressionModeType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      JavaOpExpressionModeType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Java Op Expression Mode Type</b></em>' literal with the specified integer
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static JavaOpExpressionModeType get(int value) {
    switch (value) {
      case ATTRIBUTE_VALUE:
        return ATTRIBUTE;
      case ATTRIBUTE_FREE_VALUE:
        return ATTRIBUTE_FREE;
      case CUSTOM_LITERAL_VALUE:
        return CUSTOM_LITERAL;
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
  private JavaOpExpressionModeType(int value, String name, String literal) {
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
} // JavaOpExpressionModeType
