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
package com.ibm.xmlns.prod.streams.spl.toolkit.info;

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Lang Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#getValue <em>Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault <em>Default</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getLangType()
 * @model extendedMetaData="name='langType' kind='simple'"
 * @generated
 */
public interface LangType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getLangType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="name=':0'
   *     kind='simple'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#getValue
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
   * Returns the value of the '<em><b>Default</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Default</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Default</em>' attribute.
   * @see #isSetDefault()
   * @see #unsetDefault()
   * @see #setDefault(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getLangType_Default()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='default'"
   * @generated
   */
  boolean isDefault();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault
   * <em>Default</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Default</em>' attribute.
   * @see #isSetDefault()
   * @see #unsetDefault()
   * @see #isDefault()
   * @generated
   */
  void setDefault(boolean value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault
   * <em>Default</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetDefault()
   * @see #isDefault()
   * @see #setDefault(boolean)
   * @generated
   */
  void unsetDefault();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault <em>Default</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Default</em>' attribute is set.
   * @see #unsetDefault()
   * @see #isDefault()
   * @see #setDefault(boolean)
   * @generated
   */
  boolean isSetDefault();
} // LangType
