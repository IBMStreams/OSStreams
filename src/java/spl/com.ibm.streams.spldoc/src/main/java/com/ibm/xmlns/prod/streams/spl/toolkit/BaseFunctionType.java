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

import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Base Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getReturnType <em>Return
 *       Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseFunctionType()
 * @model extendedMetaData="name='baseFunctionType' kind='elementOnly'"
 * @generated
 */
public interface BaseFunctionType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Prototype</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Prototype</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Prototype</em>' attribute.
   * @see #setPrototype(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseFunctionType_Prototype()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='prototype' namespace='##targetNamespace'"
   * @generated
   */
  String getPrototype();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getPrototype <em>Prototype</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Prototype</em>' attribute.
   * @see #getPrototype()
   * @generated
   */
  void setPrototype(String value);

  /**
   * Returns the value of the '<em><b>Parameter</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameter</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameter</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseFunctionType_Parameter()
   * @model containment="true" extendedMetaData="kind='element' name='parameter'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<ParameterType> getParameter();

  /**
   * Returns the value of the '<em><b>Return Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Return Type</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Return Type</em>' attribute.
   * @see #setReturnType(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseFunctionType_ReturnType()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='returnType'"
   * @generated
   */
  String getReturnType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getReturnType <em>Return Type</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Return Type</em>' attribute.
   * @see #getReturnType()
   * @generated
   */
  void setReturnType(String value);
} // BaseFunctionType
