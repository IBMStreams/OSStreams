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
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Source Location Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getLine <em>Line</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceLocationType()
 * @model extendedMetaData="name='sourceLocationType' kind='empty'"
 * @generated
 */
public interface SourceLocationType extends EObject {
  /**
   * Returns the value of the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Column</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Column</em>' attribute.
   * @see #setColumn(BigInteger)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceLocationType_Column()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='column'"
   * @generated
   */
  BigInteger getColumn();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getColumn <em>Column</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Column</em>' attribute.
   * @see #getColumn()
   * @generated
   */
  void setColumn(BigInteger value);

  /**
   * Returns the value of the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Line</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Line</em>' attribute.
   * @see #setLine(BigInteger)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceLocationType_Line()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='line'"
   * @generated
   */
  BigInteger getLine();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getLine <em>Line</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Line</em>' attribute.
   * @see #getLine()
   * @generated
   */
  void setLine(BigInteger value);
} // SourceLocationType
