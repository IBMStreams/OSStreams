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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Icon Uri Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getValue <em>Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize <em>Size</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getIconUriType()
 * @model extendedMetaData="name='iconUriType' kind='simple'"
 * @generated
 */
public interface IconUriType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getIconUriType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="name=':0'
   *     kind='simple'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getValue
   * <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);

  /**
   * Returns the value of the '<em><b>Size</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Size</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Size</em>' attribute.
   * @see #isSetSize()
   * @see #unsetSize()
   * @see #setSize(int)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getIconUriType_Size()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='size'"
   * @generated
   */
  int getSize();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize
   * <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Size</em>' attribute.
   * @see #isSetSize()
   * @see #unsetSize()
   * @see #getSize()
   * @generated
   */
  void setSize(int value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize
   * <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetSize()
   * @see #getSize()
   * @see #setSize(int)
   * @generated
   */
  void unsetSize();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize <em>Size</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Size</em>' attribute is set.
   * @see #unsetSize()
   * @see #getSize()
   * @see #setSize(int)
   * @generated
   */
  boolean isSetSize();
} // IconUriType
