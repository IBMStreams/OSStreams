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
 * A representation of the literals of the enumeration '<em><b>Composite Param Enum Type</b></em>',
 * and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeParamEnumType()
 * @model extendedMetaData="name='compositeParamEnumType'"
 * @generated
 */
public enum CompositeParamEnumType implements Enumerator {
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
   * The '<em><b>Expression</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #EXPRESSION_VALUE
   * @generated
   * @ordered
   */
  EXPRESSION(1, "Expression", "Expression"),

  /**
   * The '<em><b>Function</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #FUNCTION_VALUE
   * @generated
   * @ordered
   */
  FUNCTION(2, "Function", "Function"),

  /**
   * The '<em><b>Operator</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #OPERATOR_VALUE
   * @generated
   * @ordered
   */
  OPERATOR(3, "Operator", "Operator"),

  /**
   * The '<em><b>Type</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TYPE_VALUE
   * @generated
   * @ordered
   */
  TYPE(4, "Type", "Type");

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
   * The '<em><b>Expression</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Expression</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #EXPRESSION
   * @model name="Expression"
   * @generated
   * @ordered
   */
  public static final int EXPRESSION_VALUE = 1;

  /**
   * The '<em><b>Function</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Function</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #FUNCTION
   * @model name="Function"
   * @generated
   * @ordered
   */
  public static final int FUNCTION_VALUE = 2;

  /**
   * The '<em><b>Operator</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Operator</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #OPERATOR
   * @model name="Operator"
   * @generated
   * @ordered
   */
  public static final int OPERATOR_VALUE = 3;

  /**
   * The '<em><b>Type</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Type</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TYPE
   * @model name="Type"
   * @generated
   * @ordered
   */
  public static final int TYPE_VALUE = 4;

  /**
   * An array of all the '<em><b>Composite Param Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final CompositeParamEnumType[] VALUES_ARRAY =
      new CompositeParamEnumType[] {
        ATTRIBUTE, EXPRESSION, FUNCTION, OPERATOR, TYPE,
      };

  /**
   * A public read-only list of all the '<em><b>Composite Param Enum Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<CompositeParamEnumType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Composite Param Enum Type</b></em>' literal with the specified literal
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParamEnumType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      CompositeParamEnumType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Composite Param Enum Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParamEnumType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      CompositeParamEnumType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Composite Param Enum Type</b></em>' literal with the specified integer
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CompositeParamEnumType get(int value) {
    switch (value) {
      case ATTRIBUTE_VALUE:
        return ATTRIBUTE;
      case EXPRESSION_VALUE:
        return EXPRESSION;
      case FUNCTION_VALUE:
        return FUNCTION;
      case OPERATOR_VALUE:
        return OPERATOR;
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
  private CompositeParamEnumType(int value, String name, String literal) {
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
} // CompositeParamEnumType
