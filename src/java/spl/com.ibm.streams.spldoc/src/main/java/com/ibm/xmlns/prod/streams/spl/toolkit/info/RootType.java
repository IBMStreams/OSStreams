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
package com.ibm.xmlns.prod.streams.spl.toolkit.info;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import org.eclipse.emf.common.util.Enumerator;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the literals of the enumeration '<em><b>Root Type</b></em>', and utility
 * methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getRootType()
 * @model extendedMetaData="name='rootType'"
 * @generated
 */
public enum RootType implements Enumerator {
  /**
   * The '<em><b>Toolkit Dir</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TOOLKIT_DIR_VALUE
   * @generated
   * @ordered
   */
  TOOLKIT_DIR(0, "toolkitDir", "toolkitDir"),

  /**
   * The '<em><b>Application Dir</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #APPLICATION_DIR_VALUE
   * @generated
   * @ordered
   */
  APPLICATION_DIR(1, "applicationDir", "applicationDir"),

  /**
   * The '<em><b>Output Dir</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #OUTPUT_DIR_VALUE
   * @generated
   * @ordered
   */
  OUTPUT_DIR(2, "outputDir", "outputDir");

  /**
   * The '<em><b>Toolkit Dir</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Toolkit Dir</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TOOLKIT_DIR
   * @model name="toolkitDir"
   * @generated
   * @ordered
   */
  public static final int TOOLKIT_DIR_VALUE = 0;

  /**
   * The '<em><b>Application Dir</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Application Dir</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #APPLICATION_DIR
   * @model name="applicationDir"
   * @generated
   * @ordered
   */
  public static final int APPLICATION_DIR_VALUE = 1;

  /**
   * The '<em><b>Output Dir</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Output Dir</b></em>' literal object isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #OUTPUT_DIR
   * @model name="outputDir"
   * @generated
   * @ordered
   */
  public static final int OUTPUT_DIR_VALUE = 2;

  /**
   * An array of all the '<em><b>Root Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final RootType[] VALUES_ARRAY =
      new RootType[] {
        TOOLKIT_DIR, APPLICATION_DIR, OUTPUT_DIR,
      };

  /**
   * A public read-only list of all the '<em><b>Root Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<RootType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Root Type</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static RootType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      RootType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Root Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static RootType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      RootType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Root Type</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static RootType get(int value) {
    switch (value) {
      case TOOLKIT_DIR_VALUE:
        return TOOLKIT_DIR;
      case APPLICATION_DIR_VALUE:
        return APPLICATION_DIR;
      case OUTPUT_DIR_VALUE:
        return OUTPUT_DIR;
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
  private RootType(int value, String name, String literal) {
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
} // RootType
