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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Metric Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind <em>Kind</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic <em>Dynamic</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricType()
 * @model extendedMetaData="name='metricType' kind='elementOnly'"
 * @generated
 */
public interface MetricType extends EObject {
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Name</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='name' namespace='##targetNamespace'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getName
   * <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricType_Description()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getDescription
   * <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Kind</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.operator.MetricKindType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Kind</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Kind</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @see #isSetKind()
   * @see #unsetKind()
   * @see #setKind(MetricKindType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricType_Kind()
   * @model unsettable="true" required="true" extendedMetaData="kind='element' name='kind'
   *     namespace='##targetNamespace'"
   * @generated
   */
  MetricKindType getKind();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind
   * <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Kind</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @see #isSetKind()
   * @see #unsetKind()
   * @see #getKind()
   * @generated
   */
  void setKind(MetricKindType value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind
   * <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetKind()
   * @see #getKind()
   * @see #setKind(MetricKindType)
   * @generated
   */
  void unsetKind();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind <em>Kind</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Kind</em>' attribute is set.
   * @see #unsetKind()
   * @see #getKind()
   * @see #setKind(MetricKindType)
   * @generated
   */
  boolean isSetKind();

  /**
   * Returns the value of the '<em><b>Dynamic</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Dynamic</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Dynamic</em>' attribute.
   * @see #isSetDynamic()
   * @see #unsetDynamic()
   * @see #setDynamic(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getMetricType_Dynamic()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='element' name='dynamic' namespace='##targetNamespace'"
   * @generated
   */
  boolean isDynamic();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic
   * <em>Dynamic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Dynamic</em>' attribute.
   * @see #isSetDynamic()
   * @see #unsetDynamic()
   * @see #isDynamic()
   * @generated
   */
  void setDynamic(boolean value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic
   * <em>Dynamic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetDynamic()
   * @see #isDynamic()
   * @see #setDynamic(boolean)
   * @generated
   */
  void unsetDynamic();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic <em>Dynamic</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Dynamic</em>' attribute is set.
   * @see #unsetDynamic()
   * @see #isDynamic()
   * @see #setDynamic(boolean)
   * @generated
   */
  boolean isSetDynamic();
} // MetricType
