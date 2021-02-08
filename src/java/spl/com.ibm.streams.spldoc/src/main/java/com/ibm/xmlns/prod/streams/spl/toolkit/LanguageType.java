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
 * A representation of the literals of the enumeration '<em><b>Language Type</b></em>', and utility
 * methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getLanguageType()
 * @model extendedMetaData="name='languageType'"
 * @generated
 */
public enum LanguageType implements Enumerator {
  /**
   * The '<em><b>Cpp</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #CPP_VALUE
   * @generated
   * @ordered
   */
  CPP(0, "Cpp", "Cpp"),

  /**
   * The '<em><b>Java</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #JAVA_VALUE
   * @generated
   * @ordered
   */
  JAVA(1, "Java", "Java");

  /**
   * The '<em><b>Cpp</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Cpp</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #CPP
   * @model name="Cpp"
   * @generated
   * @ordered
   */
  public static final int CPP_VALUE = 0;

  /**
   * The '<em><b>Java</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Java</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #JAVA
   * @model name="Java"
   * @generated
   * @ordered
   */
  public static final int JAVA_VALUE = 1;

  /**
   * An array of all the '<em><b>Language Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final LanguageType[] VALUES_ARRAY =
      new LanguageType[] {
        CPP, JAVA,
      };

  /**
   * A public read-only list of all the '<em><b>Language Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<LanguageType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Language Type</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static LanguageType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      LanguageType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Language Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static LanguageType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      LanguageType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Language Type</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static LanguageType get(int value) {
    switch (value) {
      case CPP_VALUE:
        return CPP;
      case JAVA_VALUE:
        return JAVA;
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
  private LanguageType(int value, String name, String literal) {
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
} // LanguageType
