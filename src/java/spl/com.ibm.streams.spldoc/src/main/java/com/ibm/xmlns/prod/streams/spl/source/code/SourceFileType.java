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
 * A representation of the model object '<em><b>Source File Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getCompilationUnit
 *       <em>Compilation Unit</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getUri <em>Uri</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceFileType()
 * @model extendedMetaData="name='sourceFileType' kind='elementOnly'"
 * @generated
 */
public interface SourceFileType extends EObject {
  /**
   * Returns the value of the '<em><b>Compilation Unit</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Compilation Unit</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Compilation Unit</em>' containment reference.
   * @see #setCompilationUnit(CompilationUnitType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceFileType_CompilationUnit()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='compilationUnit' namespace='##targetNamespace'"
   * @generated
   */
  CompilationUnitType getCompilationUnit();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getCompilationUnit <em>Compilation
   * Unit</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Compilation Unit</em>' containment reference.
   * @see #getCompilationUnit()
   * @generated
   */
  void setCompilationUnit(CompilationUnitType value);

  /**
   * Returns the value of the '<em><b>Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Uri</em>' attribute isn't clear, there really should be more of a
   * description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Uri</em>' attribute.
   * @see #setUri(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getSourceFileType_Uri()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='uri'"
   * @generated
   */
  String getUri();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getUri
   * <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Uri</em>' attribute.
   * @see #getUri()
   * @generated
   */
  void setUri(String value);
} // SourceFileType
