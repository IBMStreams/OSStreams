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

import java.math.BigInteger;
import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Operator Invocation Input Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIstream
 *       <em>Istream</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getAlias
 *       <em>Alias</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIndex
 *       <em>Index</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationInputType()
 * @model extendedMetaData="name='operatorInvocationInputType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationInputType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Istream</b></em>' containment reference list. The list
   * contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Istream</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Istream</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationInputType_Istream()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='istream'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<OperatorInvocationInputStreamType> getIstream();

  /**
   * Returns the value of the '<em><b>Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Alias</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Alias</em>' attribute.
   * @see #setAlias(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationInputType_Alias()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='alias'"
   * @generated
   */
  String getAlias();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getAlias
   * <em>Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Alias</em>' attribute.
   * @see #getAlias()
   * @generated
   */
  void setAlias(String value);

  /**
   * Returns the value of the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Index</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Index</em>' attribute.
   * @see #setIndex(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationInputType_Index()
   * @model dataType="org.eclipse.emf.ecore.xml.type.UnsignedLong" required="true"
   *     extendedMetaData="kind='attribute' name='index'"
   * @generated
   */
  BigInteger getIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIndex
   * <em>Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Index</em>' attribute.
   * @see #getIndex()
   * @generated
   */
  void setIndex(BigInteger value);
} // OperatorInvocationInputType
