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

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Op Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex <em>Port
 *       Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getType <em>Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpPortType()
 * @model extendedMetaData="name='compositeOpPortType' kind='elementOnly'"
 * @generated
 */
public interface CompositeOpPortType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Port Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Port Index</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Port Index</em>' attribute.
   * @see #isSetPortIndex()
   * @see #unsetPortIndex()
   * @see #setPortIndex(int)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpPortType_PortIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='portIndex'"
   * @generated
   */
  int getPortIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex <em>Port Index</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Port Index</em>' attribute.
   * @see #isSetPortIndex()
   * @see #unsetPortIndex()
   * @see #getPortIndex()
   * @generated
   */
  void setPortIndex(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex <em>Port Index</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPortIndex()
   * @see #getPortIndex()
   * @see #setPortIndex(int)
   * @generated
   */
  void unsetPortIndex();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex <em>Port Index</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Port Index</em>' attribute is set.
   * @see #unsetPortIndex()
   * @see #getPortIndex()
   * @see #setPortIndex(int)
   * @generated
   */
  boolean isSetPortIndex();

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpPortType_Type()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='type'"
   * @generated
   */
  String getType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getType <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Type</em>' attribute.
   * @see #getType()
   * @generated
   */
  void setType(String value);
} // CompositeOpPortType
