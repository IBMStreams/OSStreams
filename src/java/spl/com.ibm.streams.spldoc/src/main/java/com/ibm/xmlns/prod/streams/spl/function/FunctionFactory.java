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

import org.eclipse.emf.ecore.EFactory;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model. It provides a create method for each non-abstract class of the
 * model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage
 * @generated
 */
public interface FunctionFactory extends EFactory {
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  FunctionFactory eINSTANCE =
      com.ibm.xmlns.prod.streams.spl.function.impl.FunctionFactoryImpl.init();

  /**
   * Returns a new object of class '<em>Dependencies Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Dependencies Type</em>'.
   * @generated
   */
  DependenciesType createDependenciesType();

  /**
   * Returns a new object of class '<em>Document Root</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Document Root</em>'.
   * @generated
   */
  DocumentRoot createDocumentRoot();

  /**
   * Returns a new object of class '<em>Model Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Model Type</em>'.
   * @generated
   */
  FunctionModelType createFunctionModelType();

  /**
   * Returns a new object of class '<em>Set Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Set Type</em>'.
   * @generated
   */
  FunctionSetType createFunctionSetType();

  /**
   * Returns a new object of class '<em>Functions Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Functions Type</em>'.
   * @generated
   */
  FunctionsType createFunctionsType();

  /**
   * Returns a new object of class '<em>Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Type</em>'.
   * @generated
   */
  FunctionType createFunctionType();

  /**
   * Returns a new object of class '<em>Prototype Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Prototype Type</em>'.
   * @generated
   */
  PrototypeType createPrototypeType();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the package supported by this factory.
   * @generated
   */
  FunctionPackage getFunctionPackage();
} // FunctionFactory
