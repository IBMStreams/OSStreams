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
 * A representation of the literals of the enumeration '<em><b>Metric Kind Type</b></em>', and
 * utility methods for working with them.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricKindType()
 * @model extendedMetaData="name='metricKindType'"
 * @generated
 */
public enum MetricKindType implements Enumerator {
  /**
   * The '<em><b>Gauge</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #GAUGE_VALUE
   * @generated
   * @ordered
   */
  GAUGE(0, "Gauge", "Gauge"),

  /**
   * The '<em><b>Counter</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #COUNTER_VALUE
   * @generated
   * @ordered
   */
  COUNTER(1, "Counter", "Counter"),

  /**
   * The '<em><b>Time</b></em>' literal object.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #TIME_VALUE
   * @generated
   * @ordered
   */
  TIME(2, "Time", "Time");

  /**
   * The '<em><b>Gauge</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Gauge</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #GAUGE
   * @model name="Gauge"
   * @generated
   * @ordered
   */
  public static final int GAUGE_VALUE = 0;

  /**
   * The '<em><b>Counter</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Counter</b></em>' literal object isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #COUNTER
   * @model name="Counter"
   * @generated
   * @ordered
   */
  public static final int COUNTER_VALUE = 1;

  /**
   * The '<em><b>Time</b></em>' literal value.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of '<em><b>Time</b></em>' literal object isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @see #TIME
   * @model name="Time"
   * @generated
   * @ordered
   */
  public static final int TIME_VALUE = 2;

  /**
   * An array of all the '<em><b>Metric Kind Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final MetricKindType[] VALUES_ARRAY =
      new MetricKindType[] {
        GAUGE, COUNTER, TIME,
      };

  /**
   * A public read-only list of all the '<em><b>Metric Kind Type</b></em>' enumerators.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final List<MetricKindType> VALUES =
      Collections.unmodifiableList(Arrays.asList(VALUES_ARRAY));

  /**
   * Returns the '<em><b>Metric Kind Type</b></em>' literal with the specified literal value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param literal the literal.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static MetricKindType get(String literal) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      MetricKindType result = VALUES_ARRAY[i];
      if (result.toString().equals(literal)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Metric Kind Type</b></em>' literal with the specified name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param name the name.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static MetricKindType getByName(String name) {
    for (int i = 0; i < VALUES_ARRAY.length; ++i) {
      MetricKindType result = VALUES_ARRAY[i];
      if (result.getName().equals(name)) {
        return result;
      }
    }
    return null;
  }

  /**
   * Returns the '<em><b>Metric Kind Type</b></em>' literal with the specified integer value.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the integer value.
   * @return the matching enumerator or <code>null</code>.
   * @generated
   */
  public static MetricKindType get(int value) {
    switch (value) {
      case GAUGE_VALUE:
        return GAUGE;
      case COUNTER_VALUE:
        return COUNTER;
      case TIME_VALUE:
        return TIME;
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
  private MetricKindType(int value, String name, String literal) {
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
} // MetricKindType
