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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Parameter Expression Mode Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode
 *       <em>Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getType
 *       <em>Type</em>}
 * </ul>
 *
 * @see
 *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeParameterExpressionModeType()
 * @model extendedMetaData="name='compositeParameterExpressionModeType' kind='empty'"
 * @generated
 */
public interface CompositeParameterExpressionModeType extends EObject {
  /**
   * Returns the value of the '<em><b>Mode</b></em>' attribute. The literals are from the
   * enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Mode</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @see #isSetMode()
   * @see #unsetMode()
   * @see #setMode(CompositeParameterExpressionModeEnumType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeParameterExpressionModeType_Mode()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute' name='mode'"
   * @generated
   */
  CompositeParameterExpressionModeEnumType getMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode
   * <em>Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @see #isSetMode()
   * @see #unsetMode()
   * @see #getMode()
   * @generated
   */
  void setMode(CompositeParameterExpressionModeEnumType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode
   * <em>Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetMode()
   * @see #getMode()
   * @see #setMode(CompositeParameterExpressionModeEnumType)
   * @generated
   */
  void unsetMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode
   * <em>Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Mode</em>' attribute is set.
   * @see #unsetMode()
   * @see #getMode()
   * @see #setMode(CompositeParameterExpressionModeEnumType)
   * @generated
   */
  boolean isSetMode();

  /**
   * Returns the value of the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type</em>' attribute.
   * @see #setType(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeParameterExpressionModeType_Type()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='type'"
   * @generated
   */
  String getType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getType
   * <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Type</em>' attribute.
   * @see #getType()
   * @generated
   */
  void setType(String value);
} // CompositeParameterExpressionModeType
