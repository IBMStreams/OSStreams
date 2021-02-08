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

import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Config Option Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getValue <em>Value</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getConfigOptionType()
 * @model extendedMetaData="name='configOptionType' kind='elementOnly'"
 * @generated
 */
public interface ConfigOptionType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Parameter</b></em>' containment reference list. The list
   * contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameter</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameter</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getConfigOptionType_Parameter()
   * @model containment="true" extendedMetaData="kind='element' name='parameter'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<ConfigValueParameterType> getParameter();

  /**
   * Returns the value of the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Value</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Value</em>' attribute.
   * @see #setValue(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getConfigOptionType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='value'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getValue <em>Value</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);
} // ConfigOptionType
