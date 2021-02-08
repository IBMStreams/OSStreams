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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Java Op Library Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getManagedLibrary
 *       <em>Managed Library</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getJavaOpLibraryType()
 * @model extendedMetaData="name='javaOpLibraryType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpLibraryType extends EObject {
  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getJavaOpLibraryType_Description()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getDescription <em>Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Managed Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Managed Library</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Managed Library</em>' containment reference.
   * @see #setManagedLibrary(JavaOpManagedLibraryType)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getJavaOpLibraryType_ManagedLibrary()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='managedLibrary' namespace='##targetNamespace'"
   * @generated
   */
  JavaOpManagedLibraryType getManagedLibrary();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getManagedLibrary <em>Managed
   * Library</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Managed Library</em>' containment reference.
   * @see #getManagedLibrary()
   * @generated
   */
  void setManagedLibrary(JavaOpManagedLibraryType value);
} // JavaOpLibraryType
