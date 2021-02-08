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
 * A representation of the model object '<em><b>Extended Source Location Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndColumn
 *       <em>End Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndLine
 *       <em>End Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartColumn
 *       <em>Start Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartLine
 *       <em>Start Line</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getExtendedSourceLocationType()
 * @model extendedMetaData="name='extendedSourceLocationType' kind='empty'"
 * @generated
 */
public interface ExtendedSourceLocationType extends EObject {
  /**
   * Returns the value of the '<em><b>End Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>End Column</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>End Column</em>' attribute.
   * @see #setEndColumn(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getExtendedSourceLocationType_EndColumn()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='endColumn'"
   * @generated
   */
  BigInteger getEndColumn();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndColumn <em>End
   * Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>End Column</em>' attribute.
   * @see #getEndColumn()
   * @generated
   */
  void setEndColumn(BigInteger value);

  /**
   * Returns the value of the '<em><b>End Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>End Line</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>End Line</em>' attribute.
   * @see #setEndLine(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getExtendedSourceLocationType_EndLine()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='endLine'"
   * @generated
   */
  BigInteger getEndLine();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndLine <em>End
   * Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>End Line</em>' attribute.
   * @see #getEndLine()
   * @generated
   */
  void setEndLine(BigInteger value);

  /**
   * Returns the value of the '<em><b>Start Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Start Column</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Start Column</em>' attribute.
   * @see #setStartColumn(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getExtendedSourceLocationType_StartColumn()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='startColumn'"
   * @generated
   */
  BigInteger getStartColumn();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartColumn <em>Start
   * Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Start Column</em>' attribute.
   * @see #getStartColumn()
   * @generated
   */
  void setStartColumn(BigInteger value);

  /**
   * Returns the value of the '<em><b>Start Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Start Line</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Start Line</em>' attribute.
   * @see #setStartLine(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getExtendedSourceLocationType_StartLine()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" required="true"
   *     extendedMetaData="kind='attribute' name='startLine'"
   * @generated
   */
  BigInteger getStartLine();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartLine <em>Start
   * Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Start Line</em>' attribute.
   * @see #getStartLine()
   * @generated
   */
  void setStartLine(BigInteger value);
} // ExtendedSourceLocationType
