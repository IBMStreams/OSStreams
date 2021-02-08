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
package com.ibm.xmlns.prod.streams.spl.common;

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Uri Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex <em>Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getValue <em>Value</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getUriType()
 * @model extendedMetaData="name='uriType' kind='empty'"
 * @generated
 */
public interface UriType extends EObject {
  /**
   * Returns the value of the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Index</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Index</em>' attribute.
   * @see #isSetIndex()
   * @see #unsetIndex()
   * @see #setIndex(int)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getUriType_Index()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='index'"
   * @generated
   */
  int getIndex();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex
   * <em>Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Index</em>' attribute.
   * @see #isSetIndex()
   * @see #unsetIndex()
   * @see #getIndex()
   * @generated
   */
  void setIndex(int value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex
   * <em>Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetIndex()
   * @see #getIndex()
   * @see #setIndex(int)
   * @generated
   */
  void unsetIndex();

  /**
   * Returns whether the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex
   * <em>Index</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Index</em>' attribute is set.
   * @see #unsetIndex()
   * @see #getIndex()
   * @see #setIndex(int)
   * @generated
   */
  boolean isSetIndex();

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
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getUriType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='value'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.UriType#getValue
   * <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);
} // UriType
