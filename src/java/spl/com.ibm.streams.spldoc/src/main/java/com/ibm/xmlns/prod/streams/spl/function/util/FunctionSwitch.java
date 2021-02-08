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
package com.ibm.xmlns.prod.streams.spl.function.util;

import com.ibm.xmlns.prod.streams.spl.function.*;
import java.util.List;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy. It supports the call {@link
 * #doSwitch(EObject) doSwitch(object)} to invoke the <code>caseXXX</code> method for each class of
 * the model, starting with the actual class of the object and proceeding up the inheritance
 * hierarchy until a non-null result is returned, which is the result of the switch.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage
 * @generated
 */
public class FunctionSwitch<T> {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static FunctionPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionSwitch() {
    if (modelPackage == null) {
      modelPackage = FunctionPackage.eINSTANCE;
    }
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it
   * yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  public T doSwitch(EObject theEObject) {
    return doSwitch(theEObject.eClass(), theEObject);
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it
   * yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  protected T doSwitch(EClass theEClass, EObject theEObject) {
    if (theEClass.eContainer() == modelPackage) {
      return doSwitch(theEClass.getClassifierID(), theEObject);
    } else {
      List<EClass> eSuperTypes = theEClass.getESuperTypes();
      return eSuperTypes.isEmpty()
          ? defaultCase(theEObject)
          : doSwitch(eSuperTypes.get(0), theEObject);
    }
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it
   * yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  protected T doSwitch(int classifierID, EObject theEObject) {
    switch (classifierID) {
      case FunctionPackage.DEPENDENCIES_TYPE:
        {
          DependenciesType dependenciesType = (DependenciesType) theEObject;
          T result = caseDependenciesType(dependenciesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.DOCUMENT_ROOT:
        {
          DocumentRoot documentRoot = (DocumentRoot) theEObject;
          T result = caseDocumentRoot(documentRoot);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.FUNCTION_MODEL_TYPE:
        {
          FunctionModelType functionModelType = (FunctionModelType) theEObject;
          T result = caseFunctionModelType(functionModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.FUNCTION_SET_TYPE:
        {
          FunctionSetType functionSetType = (FunctionSetType) theEObject;
          T result = caseFunctionSetType(functionSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.FUNCTIONS_TYPE:
        {
          FunctionsType functionsType = (FunctionsType) theEObject;
          T result = caseFunctionsType(functionsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.FUNCTION_TYPE:
        {
          FunctionType functionType = (FunctionType) theEObject;
          T result = caseFunctionType(functionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case FunctionPackage.PROTOTYPE_TYPE:
        {
          PrototypeType prototypeType = (PrototypeType) theEObject;
          T result = casePrototypeType(prototypeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      default:
        return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Dependencies Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Dependencies Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseDependenciesType(DependenciesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Document Root</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Document Root</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseDocumentRoot(DocumentRoot object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Model Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Model Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionModelType(FunctionModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Set Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Set Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionSetType(FunctionSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Functions Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Functions Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionsType(FunctionsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionType(FunctionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Prototype Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Prototype Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrototypeType(PrototypeType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch, but
   * this is the last case anyway.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject)
   * @generated
   */
  public T defaultCase(EObject object) {
    return null;
  }
} // FunctionSwitch
