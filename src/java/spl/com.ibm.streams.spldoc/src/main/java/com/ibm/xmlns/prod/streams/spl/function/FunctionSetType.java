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
package com.ibm.xmlns.prod.streams.spl.function;

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getHeaderFileName <em>Header
 *       File Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getCppNamespaceName <em>Cpp
 *       Namespace Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getFunctions
 *       <em>Functions</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getDependencies
 *       <em>Dependencies</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionSetType()
 * @model extendedMetaData="name='functionSetType' kind='elementOnly'"
 * @generated
 */
public interface FunctionSetType extends EObject {
  /**
   * Returns the value of the '<em><b>Header File Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Header File Name</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Header File Name</em>' attribute.
   * @see #setHeaderFileName(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionSetType_HeaderFileName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='headerFileName' namespace='##targetNamespace'"
   * @generated
   */
  String getHeaderFileName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getHeaderFileName <em>Header File
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Header File Name</em>' attribute.
   * @see #getHeaderFileName()
   * @generated
   */
  void setHeaderFileName(String value);

  /**
   * Returns the value of the '<em><b>Cpp Namespace Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cpp Namespace Name</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cpp Namespace Name</em>' attribute.
   * @see #setCppNamespaceName(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionSetType_CppNamespaceName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='cppNamespaceName' namespace='##targetNamespace'"
   * @generated
   */
  String getCppNamespaceName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getCppNamespaceName <em>Cpp Namespace
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cpp Namespace Name</em>' attribute.
   * @see #getCppNamespaceName()
   * @generated
   */
  void setCppNamespaceName(String value);

  /**
   * Returns the value of the '<em><b>Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Functions</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Functions</em>' containment reference.
   * @see #setFunctions(FunctionsType)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionSetType_Functions()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='functions'
   *     namespace='##targetNamespace'"
   * @generated
   */
  FunctionsType getFunctions();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getFunctions <em>Functions</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Functions</em>' containment reference.
   * @see #getFunctions()
   * @generated
   */
  void setFunctions(FunctionsType value);

  /**
   * Returns the value of the '<em><b>Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Dependencies</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Dependencies</em>' containment reference.
   * @see #setDependencies(DependenciesType)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionSetType_Dependencies()
   * @model containment="true" extendedMetaData="kind='element' name='dependencies'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DependenciesType getDependencies();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getDependencies <em>Dependencies</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Dependencies</em>' containment reference.
   * @see #getDependencies()
   * @generated
   */
  void setDependencies(DependenciesType value);
} // FunctionSetType
