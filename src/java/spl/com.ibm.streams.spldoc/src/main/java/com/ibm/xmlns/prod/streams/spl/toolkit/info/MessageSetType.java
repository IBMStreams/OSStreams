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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Message Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getLang <em>Lang</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getName <em>Name</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getMessageSetType()
 * @model extendedMetaData="name='messageSetType' kind='elementOnly'"
 * @generated
 */
public interface MessageSetType extends EObject {
  /**
   * Returns the value of the '<em><b>Lang</b></em>' containment reference list. The list contents
   * are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Lang</em>' containment reference list isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Lang</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getMessageSetType_Lang()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='lang'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<LangType> getLang();

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getMessageSetType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" required="true"
   *     extendedMetaData="kind='attribute' name='name'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);
} // MessageSetType
