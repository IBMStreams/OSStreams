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
package com.ibm.xmlns.prod.streams.spl.toolkit.info;

import org.eclipse.emf.ecore.EFactory;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model. It provides a create method for each non-abstract class of the
 * model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage
 * @generated
 */
public interface InfoFactory extends EFactory {
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  InfoFactory eINSTANCE = com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoFactoryImpl.init();

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
   * Returns a new object of class '<em>Exclude Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Exclude Type</em>'.
   * @generated
   */
  ExcludeType createExcludeType();

  /**
   * Returns a new object of class '<em>Identity Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Identity Type</em>'.
   * @generated
   */
  IdentityType createIdentityType();

  /**
   * Returns a new object of class '<em>Include Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Include Type</em>'.
   * @generated
   */
  IncludeType createIncludeType();

  /**
   * Returns a new object of class '<em>Lang Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Lang Type</em>'.
   * @generated
   */
  LangType createLangType();

  /**
   * Returns a new object of class '<em>Message Set Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Message Set Type</em>'.
   * @generated
   */
  MessageSetType createMessageSetType();

  /**
   * Returns a new object of class '<em>Resources Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Resources Type</em>'.
   * @generated
   */
  ResourcesType createResourcesType();

  /**
   * Returns a new object of class '<em>Sab Files Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Sab Files Type</em>'.
   * @generated
   */
  SabFilesType createSabFilesType();

  /**
   * Returns a new object of class '<em>Toolkit Info Model Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Toolkit Info Model Type</em>'.
   * @generated
   */
  ToolkitInfoModelType createToolkitInfoModelType();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the package supported by this factory.
   * @generated
   */
  InfoPackage getInfoPackage();
} // InfoFactory
