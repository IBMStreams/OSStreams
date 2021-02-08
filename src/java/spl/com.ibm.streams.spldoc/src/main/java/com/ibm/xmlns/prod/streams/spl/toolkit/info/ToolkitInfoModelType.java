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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Toolkit Info Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getIdentity
 *       <em>Identity</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getDependencies
 *       <em>Dependencies</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getResources
 *       <em>Resources</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getSabFiles <em>Sab
 *       Files</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getToolkitInfoModelType()
 * @model extendedMetaData="name='toolkitInfoModelType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitInfoModelType extends EObject {
  /**
   * Returns the value of the '<em><b>Identity</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Identity</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Identity</em>' containment reference.
   * @see #setIdentity(IdentityType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getToolkitInfoModelType_Identity()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='identity'
   *     namespace='##targetNamespace'"
   * @generated
   */
  IdentityType getIdentity();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getIdentity
   * <em>Identity</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Identity</em>' containment reference.
   * @see #getIdentity()
   * @generated
   */
  void setIdentity(IdentityType value);

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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getToolkitInfoModelType_Dependencies()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='dependencies'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DependenciesType getDependencies();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getDependencies
   * <em>Dependencies</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Dependencies</em>' containment reference.
   * @see #getDependencies()
   * @generated
   */
  void setDependencies(DependenciesType value);

  /**
   * Returns the value of the '<em><b>Resources</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Resources</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Resources</em>' containment reference.
   * @see #setResources(ResourcesType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getToolkitInfoModelType_Resources()
   * @model containment="true" extendedMetaData="kind='element' name='resources'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ResourcesType getResources();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getResources
   * <em>Resources</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Resources</em>' containment reference.
   * @see #getResources()
   * @generated
   */
  void setResources(ResourcesType value);

  /**
   * Returns the value of the '<em><b>Sab Files</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Sab Files</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Sab Files</em>' containment reference.
   * @see #setSabFiles(SabFilesType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getToolkitInfoModelType_SabFiles()
   * @model containment="true" extendedMetaData="kind='element' name='sabFiles'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SabFilesType getSabFiles();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getSabFiles <em>Sab
   * Files</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Sab Files</em>' containment reference.
   * @see #getSabFiles()
   * @generated
   */
  void setSabFiles(SabFilesType value);
} // ToolkitInfoModelType
