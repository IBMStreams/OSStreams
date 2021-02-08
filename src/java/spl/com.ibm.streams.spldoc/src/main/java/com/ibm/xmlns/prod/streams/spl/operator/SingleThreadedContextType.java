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
 * A representation of the literals of the enumeration '<em><b>Single Threaded Context
 * Type</b></em>', and utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getSingleThreadedContextType()
 * @model extendedMetaData="name='singleThreadedContextType'"
 * @generated
 */
public enum SingleThreadedContextType implements Enumerator {
  /**
   * The '<em><b>Never</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #NEVER_VALUE
   * @generated
   * @ordered
   */
  NEVER(0, "Never", "Never"),

  /**
   * The '<em><b>Always</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #ALWAYS_VALUE
   * @generated
   * @ordered
   */
  ALWAYS(1, "Always", "Always"),

  /**
   * The '<em><b>Window Bound</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_BOUND_VALUE
   * @generated
   * @ordered
   */
  WINDOW_BOUND(2, "WindowBound", "WindowBound"),

  /**
   * The '<em><b>Window Trigger Bound</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_TRIGGER_BOUND_VALUE
   * @generated
   * @ordered
   */
  WINDOW_TRIGGER_BOUND(3, "WindowTriggerBound", "WindowTriggerBound"),

  /**
   * The '<em><b>Window Eviction Bound</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_EVICTION_BOUND_VALUE
   * @generated
   * @ordered
   */
  WINDOW_EVICTION_BOUND(4, "WindowEvictionBound", "WindowEvictionBound"),

  /**
   * The '<em><b>Window Partition Eviction Bound</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_PARTITION_EVICTION_BOUND_VALUE
   * @generated
   * @ordered
   */
  WINDOW_PARTITION_EVICTION_BOUND(
      5, "WindowPartitionEvictionBound", "WindowPartitionEvictionBound");

  /**
   * The '<em><b>Never</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Never</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #NEVER
   * @model name="Never"
   * @generated
   * @ordered
   */
  public static final int NEVER_VALUE = 0;

  /**
   * The '<em><b>Always</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Always</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #ALWAYS
   * @model name="Always"
   * @generated
   * @ordered
   */
  public static final int ALWAYS_VALUE = 1;

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
   * The '<em><b>Window Trigger Bound</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Window Trigger Bound</b></em>' literal object isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_TRIGGER_BOUND
   * @model name="WindowTriggerBound"
   * @generated
   * @ordered
   */
  public static final int WINDOW_TRIGGER_BOUND_VALUE = 3;

  /**
   * The '<em><b>Window Eviction Bound</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Window Eviction Bound</b></em>' literal object isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_EVICTION_BOUND
   * @model name="WindowEvictionBound"
   * @generated
   * @ordered
   */
  public static final int WINDOW_EVICTION_BOUND_VALUE = 4;

  /**
   * The '<em><b>Window Partition Eviction Bound</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Window Partition Eviction Bound</b></em>' literal object isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #WINDOW_PARTITION_EVICTION_BOUND
   * @model name="WindowPartitionEvictionBound"
   * @generated
   * @ordered
   */
  public static final int WINDOW_PARTITION_EVICTION_BOUND_VALUE = 5;

  /**
   * An array of all the '<em><b>Single Threaded Context Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final SingleThreadedContextType[] VALUES_ARRAY =
      new SingleThreadedContextType[] {
        NEVER,
        ALWAYS,
        WINDOW_BOUND,
        WINDOW_TRIGGER_BOUND,
        WINDOW_EVICTION_BOUND,
        WINDOW_PARTITION_EVICTION_BOUND,
      };

  /**
   * A public read-only list of all the '<em><b>Single Threaded Context Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<SingleThreadedContextType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Single Threaded Context Type</b></em>' literal with the specified literal
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static SingleThreadedContextType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      SingleThreadedContextType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Single Threaded Context Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static SingleThreadedContextType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      SingleThreadedContextType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Single Threaded Context Type</b></em>' literal with the specified integer
   * value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static SingleThreadedContextType get(int value) {
    switch (value) {
      case NEVER_VALUE:
        return NEVER;
      case ALWAYS_VALUE:
        return ALWAYS;
      case WINDOW_BOUND_VALUE:
        return WINDOW_BOUND;
      case WINDOW_TRIGGER_BOUND_VALUE:
        return WINDOW_TRIGGER_BOUND;
      case WINDOW_EVICTION_BOUND_VALUE:
        return WINDOW_EVICTION_BOUND;
      case WINDOW_PARTITION_EVICTION_BOUND_VALUE:
        return WINDOW_PARTITION_EVICTION_BOUND;
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
  private SingleThreadedContextType(int value, String name, String literal) {
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
} // SingleThreadedContextType
