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

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Use Directive Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getNamespaceName
 *       <em>Namespace Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getTail <em>Tail</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getUseDirectiveType()
 * @model extendedMetaData="name='useDirectiveType' kind='empty'"
 * @generated
 */
public interface UseDirectiveType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Namespace Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Namespace Name</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Namespace Name</em>' attribute.
   * @see #setNamespaceName(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getUseDirectiveType_NamespaceName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='namespaceName'"
   * @generated
   */
  String getNamespaceName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getNamespaceName <em>Namespace
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Namespace Name</em>' attribute.
   * @see #getNamespaceName()
   * @generated
   */
  void setNamespaceName(String value);

  /**
   * Returns the value of the '<em><b>Tail</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Tail</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Tail</em>' attribute.
   * @see #setTail(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getUseDirectiveType_Tail()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='tail'"
   * @generated
   */
  String getTail();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getTail <em>Tail</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Tail</em>' attribute.
   * @see #getTail()
   * @generated
   */
  void setTail(String value);
} // UseDirectiveType
