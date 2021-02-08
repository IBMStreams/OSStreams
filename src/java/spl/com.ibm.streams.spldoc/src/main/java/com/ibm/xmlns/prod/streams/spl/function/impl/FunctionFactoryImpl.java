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
package com.ibm.xmlns.prod.streams.spl.function.impl;

import com.ibm.xmlns.prod.streams.spl.function.*;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.impl.EFactoryImpl;
import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public class FunctionFactoryImpl extends EFactoryImpl implements FunctionFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static FunctionFactory init() {
    try {
      FunctionFactory theFunctionFactory =
          (FunctionFactory)
              EPackage.Registry.INSTANCE.getEFactory(
                  "http://www.ibm.com/xmlns/prod/streams/spl/function");
      if (theFunctionFactory != null) {
        return theFunctionFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new FunctionFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionFactoryImpl() {
    super();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public EObject create(EClass eClass) {
    switch (eClass.getClassifierID()) {
      case FunctionPackage.DEPENDENCIES_TYPE:
        return createDependenciesType();
      case FunctionPackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case FunctionPackage.FUNCTION_MODEL_TYPE:
        return createFunctionModelType();
      case FunctionPackage.FUNCTION_SET_TYPE:
        return createFunctionSetType();
      case FunctionPackage.FUNCTIONS_TYPE:
        return createFunctionsType();
      case FunctionPackage.FUNCTION_TYPE:
        return createFunctionType();
      case FunctionPackage.PROTOTYPE_TYPE:
        return createPrototypeType();
      default:
        throw new IllegalArgumentException(
            "The class '" + eClass.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DependenciesType createDependenciesType() {
    DependenciesTypeImpl dependenciesType = new DependenciesTypeImpl();
    return dependenciesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DocumentRoot createDocumentRoot() {
    DocumentRootImpl documentRoot = new DocumentRootImpl();
    return documentRoot;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionModelType createFunctionModelType() {
    FunctionModelTypeImpl functionModelType = new FunctionModelTypeImpl();
    return functionModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionSetType createFunctionSetType() {
    FunctionSetTypeImpl functionSetType = new FunctionSetTypeImpl();
    return functionSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionsType createFunctionsType() {
    FunctionsTypeImpl functionsType = new FunctionsTypeImpl();
    return functionsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionType createFunctionType() {
    FunctionTypeImpl functionType = new FunctionTypeImpl();
    return functionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PrototypeType createPrototypeType() {
    PrototypeTypeImpl prototypeType = new PrototypeTypeImpl();
    return prototypeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionPackage getFunctionPackage() {
    return (FunctionPackage) getEPackage();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @deprecated
   * @generated
   */
  @Deprecated
  public static FunctionPackage getPackage() {
    return FunctionPackage.eINSTANCE;
  }
} // FunctionFactoryImpl
