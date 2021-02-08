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
 * A representation of the literals of the enumeration '<em><b>Composite Parameter Expression Mode
 * Enum Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see
 *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeParameterExpressionModeEnumType()
 * @model extendedMetaData="name='compositeParameterExpressionModeEnumType'"
 * @generated
 */
public enum CompositeParameterExpressionModeEnumType implements Enumerator {
  /**
   * The '<em><b>Operator</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #OPERATOR_VALUE
   * @generated
   * @ordered
   */
  OPERATOR(0, "operator", "operator"),

  /**
   * The '<em><b>Function</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #FUNCTION_VALUE
   * @generated
   * @ordered
   */
  FUNCTION(1, "function", "function"),

  /**
   * The '<em><b>Attribute</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE_VALUE
   * @generated
   * @ordered
   */
  ATTRIBUTE(2, "attribute", "attribute"),

  /**
   * The '<em><b>Expression</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #EXPRESSION_VALUE
   * @generated
   * @ordered
   */
  EXPRESSION(3, "expression", "expression"),

  /**
   * The '<em><b>Type</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TYPE_VALUE
   * @generated
   * @ordered
   */
  TYPE(4, "type", "type");

  /**
   * The '<em><b>Operator</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Operator</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #OPERATOR
   * @model name="operator"
   * @generated
   * @ordered
   */
  public static final int OPERATOR_VALUE = 0;

  /**
   * The '<em><b>Function</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Function</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #FUNCTION
   * @model name="function"
   * @generated
   * @ordered
   */
  public static final int FUNCTION_VALUE = 1;

  /**
   * The '<em><b>Attribute</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Attribute</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #ATTRIBUTE
   * @model name="attribute"
   * @generated
   * @ordered
   */
  public static final int ATTRIBUTE_VALUE = 2;

  /**
   * The '<em><b>Expression</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Expression</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #EXPRESSION
   * @model name="expression"
   * @generated
   * @ordered
   */
  public static final int EXPRESSION_VALUE = 3;

  /**
   * The '<em><b>Type</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Type</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TYPE
   * @model name="type"
   * @generated
   * @ordered
   */
  public static final int TYPE_VALUE = 4;

  /**
   * An array of all the '<em><b>Composite Parameter Expression Mode Enum Type</b></em>'
   * enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final CompositeParameterExpressionModeEnumType[] VALUES_ARRAY =
      new CompositeParameterExpressionModeEnumType[] {
        OPERATOR, FUNCTION, ATTRIBUTE, EXPRESSION, TYPE,
      };

  /**
   * A public read-only list of all the '<em><b>Composite Parameter Expression Mode Enum
   * Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<CompositeParameterExpressionModeEnumType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Composite Parameter Expression Mode Enum Type</b></em>' literal with the
   * specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParameterExpressionModeEnumType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      CompositeParameterExpressionModeEnumType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Composite Parameter Expression Mode Enum Type</b></em>' literal with the
   * specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParameterExpressionModeEnumType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      CompositeParameterExpressionModeEnumType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Composite Parameter Expression Mode Enum Type</b></em>' literal with the
   * specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParameterExpressionModeEnumType get(int value) {
    switch (value) {
      case OPERATOR_VALUE:
        return OPERATOR;
      case FUNCTION_VALUE:
        return FUNCTION;
      case ATTRIBUTE_VALUE:
        return ATTRIBUTE;
      case EXPRESSION_VALUE:
        return EXPRESSION;
      case TYPE_VALUE:
        return TYPE;
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
  private CompositeParameterExpressionModeEnumType(int value, String name, String literal) {
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
} // CompositeParameterExpressionModeEnumType
