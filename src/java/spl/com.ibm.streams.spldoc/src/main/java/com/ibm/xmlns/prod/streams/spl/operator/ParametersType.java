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
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Parameters Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny <em>Allow
 *       Any</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getParameter
 *       <em>Parameter</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParametersType()
 * @model extendedMetaData="name='parametersType' kind='elementOnly'"
 * @generated
 */
public interface ParametersType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParametersType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getDescription <em>Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Allow Any</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Allow Any</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Allow Any</em>' attribute.
   * @see #isSetAllowAny()
   * @see #unsetAllowAny()
   * @see #setAllowAny(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParametersType_AllowAny()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='allowAny' namespace='##targetNamespace'"
   * @generated
   */
  boolean isAllowAny();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny
   * <em>Allow Any</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Allow Any</em>' attribute.
   * @see #isSetAllowAny()
   * @see #unsetAllowAny()
   * @see #isAllowAny()
   * @generated
   */
  void setAllowAny(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny <em>Allow Any</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetAllowAny()
   * @see #isAllowAny()
   * @see #setAllowAny(boolean)
   * @generated
   */
  void unsetAllowAny();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny <em>Allow Any</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Allow Any</em>' attribute is set.
   * @see #unsetAllowAny()
   * @see #isAllowAny()
   * @see #setAllowAny(boolean)
   * @generated
   */
  boolean isSetAllowAny();

  /**
   * Returns the value of the '<em><b>Parameter</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameter</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameter</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParametersType_Parameter()
   * @model containment="true" extendedMetaData="kind='element' name='parameter'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<ParameterType> getParameter();
} // ParametersType
