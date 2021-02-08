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

/** */
package com.ibm.xmlns.prod.streams.spl.common.util;

import com.ibm.xmlns.prod.streams.spl.common.*;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.util.Switch;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy. It supports the call {@link
 * #doSwitch(EObject) doSwitch(object)} to invoke the <code>caseXXX</code> method for each class of
 * the model, starting with the actual class of the object and proceeding up the inheritance
 * hierarchy until a non-null result is returned, which is the result of the switch.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage
 * @generated
 */
public class CommonSwitch<T> extends Switch<T> {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static CommonPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonSwitch() {
    if (modelPackage == null) {
      modelPackage = CommonPackage.eINSTANCE;
    }
  }

  /**
   * Checks whether this is a switch for the given package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param ePackage the package in question.
   * @return whether this is a switch for the given package.
   * @generated
   */
  @Override
  protected boolean isSwitchFor(EPackage ePackage) {
    return ePackage == modelPackage;
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
  @Override
  protected T doSwitch(int classifierID, EObject theEObject) {
    switch (classifierID) {
      case CommonPackage.DESCRIPTION_TYPE:
        {
          DescriptionType descriptionType = (DescriptionType) theEObject;
          T result = caseDescriptionType(descriptionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.DOCUMENT_ROOT:
        {
          DocumentRoot documentRoot = (DocumentRoot) theEObject;
          T result = caseDocumentRoot(documentRoot);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.JAVA_OP_LIBRARY_TYPE:
        {
          JavaOpLibraryType javaOpLibraryType = (JavaOpLibraryType) theEObject;
          T result = caseJavaOpLibraryType(javaOpLibraryType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE:
        {
          JavaOpManagedLibraryType javaOpManagedLibraryType = (JavaOpManagedLibraryType) theEObject;
          T result = caseJavaOpManagedLibraryType(javaOpManagedLibraryType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.LIBRARY_TYPE:
        {
          LibraryType libraryType = (LibraryType) theEObject;
          T result = caseLibraryType(libraryType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.MANAGED_LIBRARY_TYPE:
        {
          ManagedLibraryType managedLibraryType = (ManagedLibraryType) theEObject;
          T result = caseManagedLibraryType(managedLibraryType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.TOOLKIT_DEPENDENCY_TYPE:
        {
          ToolkitDependencyType toolkitDependencyType = (ToolkitDependencyType) theEObject;
          T result = caseToolkitDependencyType(toolkitDependencyType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CommonPackage.URI_TYPE:
        {
          UriType uriType = (UriType) theEObject;
          T result = caseUriType(uriType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      default:
        return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Description Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Description Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseDescriptionType(DescriptionType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Java Op Library
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Library
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpLibraryType(JavaOpLibraryType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Managed Library
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Managed Library
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpManagedLibraryType(JavaOpManagedLibraryType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Library Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Library Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseLibraryType(LibraryType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Managed Library
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Managed Library
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseManagedLibraryType(ManagedLibraryType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Toolkit Dependency
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Toolkit Dependency
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitDependencyType(ToolkitDependencyType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Uri Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Uri Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseUriType(UriType object) {
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
  @Override
  public T defaultCase(EObject object) {
    return null;
  }
} // CommonSwitch
