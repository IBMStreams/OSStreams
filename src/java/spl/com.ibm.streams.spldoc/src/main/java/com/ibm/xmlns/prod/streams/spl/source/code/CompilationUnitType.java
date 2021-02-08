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
 * A representation of the model object '<em><b>Compilation Unit Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplDoc <em>Spl
 *       Doc</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplNamespace
 *       <em>Spl Namespace</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getUseDirectives
 *       <em>Use Directives</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getDefinitions
 *       <em>Definitions</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompilationUnitType()
 * @model extendedMetaData="name='compilationUnitType' kind='elementOnly'"
 * @generated
 */
public interface CompilationUnitType extends EObject {
  /**
   * Returns the value of the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Spl Doc</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Spl Doc</em>' containment reference.
   * @see #setSplDoc(SplDocType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompilationUnitType_SplDoc()
   * @model containment="true" extendedMetaData="kind='element' name='splDoc'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplDocType getSplDoc();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplDoc <em>Spl Doc</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Doc</em>' containment reference.
   * @see #getSplDoc()
   * @generated
   */
  void setSplDoc(SplDocType value);

  /**
   * Returns the value of the '<em><b>Spl Namespace</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Spl Namespace</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Spl Namespace</em>' containment reference.
   * @see #setSplNamespace(SplNamespaceType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompilationUnitType_SplNamespace()
   * @model containment="true" extendedMetaData="kind='element' name='splNamespace'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplNamespaceType getSplNamespace();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplNamespace <em>Spl
   * Namespace</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Namespace</em>' containment reference.
   * @see #getSplNamespace()
   * @generated
   */
  void setSplNamespace(SplNamespaceType value);

  /**
   * Returns the value of the '<em><b>Use Directives</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Use Directives</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Use Directives</em>' containment reference.
   * @see #setUseDirectives(UseDirectivesType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompilationUnitType_UseDirectives()
   * @model containment="true" extendedMetaData="kind='element' name='useDirectives'
   *     namespace='##targetNamespace'"
   * @generated
   */
  UseDirectivesType getUseDirectives();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getUseDirectives <em>Use
   * Directives</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Use Directives</em>' containment reference.
   * @see #getUseDirectives()
   * @generated
   */
  void setUseDirectives(UseDirectivesType value);

  /**
   * Returns the value of the '<em><b>Definitions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Definitions</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Definitions</em>' containment reference.
   * @see #setDefinitions(DefinitionsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompilationUnitType_Definitions()
   * @model containment="true" extendedMetaData="kind='element' name='definitions'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DefinitionsType getDefinitions();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getDefinitions
   * <em>Definitions</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Definitions</em>' containment reference.
   * @see #getDefinitions()
   * @generated
   */
  void setDefinitions(DefinitionsType value);
} // CompilationUnitType
