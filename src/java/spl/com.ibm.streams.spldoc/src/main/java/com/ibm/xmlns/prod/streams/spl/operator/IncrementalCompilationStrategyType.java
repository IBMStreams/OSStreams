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
 * A representation of the literals of the enumeration '<em><b>Incremental Compilation Strategy
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see
 *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getIncrementalCompilationStrategyType()
 * @model extendedMetaData="name='incrementalCompilationStrategyType'"
 * @generated
 */
public enum IncrementalCompilationStrategyType implements Enumerator {
  /**
   * The '<em><b>Source Dependent</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #SOURCE_DEPENDENT_VALUE
   * @generated
   * @ordered
   */
  SOURCE_DEPENDENT(0, "SourceDependent", "SourceDependent"),

  /**
   * The '<em><b>Result Dependent</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #RESULT_DEPENDENT_VALUE
   * @generated
   * @ordered
   */
  RESULT_DEPENDENT(1, "ResultDependent", "ResultDependent");

  /**
   * The '<em><b>Source Dependent</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Source Dependent</b></em>' literal object isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #SOURCE_DEPENDENT
   * @model name="SourceDependent"
   * @generated
   * @ordered
   */
  public static final int SOURCE_DEPENDENT_VALUE = 0;

  /**
   * The '<em><b>Result Dependent</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Result Dependent</b></em>' literal object isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #RESULT_DEPENDENT
   * @model name="ResultDependent"
   * @generated
   * @ordered
   */
  public static final int RESULT_DEPENDENT_VALUE = 1;

  /**
   * An array of all the '<em><b>Incremental Compilation Strategy Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final IncrementalCompilationStrategyType[] VALUES_ARRAY =
      new IncrementalCompilationStrategyType[] {
        SOURCE_DEPENDENT, RESULT_DEPENDENT,
      };

  /**
   * A public read-only list of all the '<em><b>Incremental Compilation Strategy Type</b></em>'
   * enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<IncrementalCompilationStrategyType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Incremental Compilation Strategy Type</b></em>' literal with the specified
   * literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static IncrementalCompilationStrategyType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      IncrementalCompilationStrategyType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Incremental Compilation Strategy Type</b></em>' literal with the specified
   * name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static IncrementalCompilationStrategyType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      IncrementalCompilationStrategyType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Incremental Compilation Strategy Type</b></em>' literal with the specified
   * integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static IncrementalCompilationStrategyType get(int value) {
    switch (value) {
      case SOURCE_DEPENDENT_VALUE:
        return SOURCE_DEPENDENT;
      case RESULT_DEPENDENT_VALUE:
        return RESULT_DEPENDENT;
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
  private IncrementalCompilationStrategyType(int value, String name, String literal) {
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
} // IncrementalCompilationStrategyType
