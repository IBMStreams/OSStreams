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
package com.ibm.xmlns.prod.streams.spl.common;

import org.eclipse.emf.ecore.EFactory;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model. It provides a create method for each non-abstract class of the
 * model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage
 * @generated
 */
public interface CommonFactory extends EFactory {
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  CommonFactory eINSTANCE = com.ibm.xmlns.prod.streams.spl.common.impl.CommonFactoryImpl.init();

  /**
   * Returns a new object of class '<em>Description Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Description Type</em>'.
   * @generated
   */
  DescriptionType createDescriptionType();

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
   * Returns a new object of class '<em>Java Op Library Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Java Op Library Type</em>'.
   * @generated
   */
  JavaOpLibraryType createJavaOpLibraryType();

  /**
   * Returns a new object of class '<em>Java Op Managed Library Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Java Op Managed Library Type</em>'.
   * @generated
   */
  JavaOpManagedLibraryType createJavaOpManagedLibraryType();

  /**
   * Returns a new object of class '<em>Library Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Library Type</em>'.
   * @generated
   */
  LibraryType createLibraryType();

  /**
   * Returns a new object of class '<em>Managed Library Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Managed Library Type</em>'.
   * @generated
   */
  ManagedLibraryType createManagedLibraryType();

  /**
   * Returns a new object of class '<em>Toolkit Dependency Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Toolkit Dependency Type</em>'.
   * @generated
   */
  ToolkitDependencyType createToolkitDependencyType();

  /**
   * Returns a new object of class '<em>Uri Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Uri Type</em>'.
   * @generated
   */
  UriType createUriType();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the package supported by this factory.
   * @generated
   */
  CommonPackage getCommonPackage();
} // CommonFactory
