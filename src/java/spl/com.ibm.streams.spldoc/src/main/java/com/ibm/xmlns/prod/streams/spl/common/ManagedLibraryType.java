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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Managed Library Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLib <em>Lib</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLibPath <em>Lib
 *       Path</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getIncludePath <em>Include
 *       Path</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getCommand
 *       <em>Command</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getManagedLibraryType()
 * @model extendedMetaData="name='managedLibraryType' kind='elementOnly'"
 * @generated
 */
public interface ManagedLibraryType extends EObject {
  /**
   * Returns the value of the '<em><b>Lib</b></em>' attribute list. The list contents are of type
   * {@link java.lang.String}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Lib</em>' attribute list isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Lib</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getManagedLibraryType_Lib()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
   *     extendedMetaData="kind='element' name='lib' namespace='##targetNamespace'"
   * @generated
   */
  EList<String> getLib();

  /**
   * Returns the value of the '<em><b>Lib Path</b></em>' attribute list. The list contents are of
   * type {@link java.lang.String}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Lib Path</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Lib Path</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getManagedLibraryType_LibPath()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
   *     extendedMetaData="kind='element' name='libPath' namespace='##targetNamespace'"
   * @generated
   */
  EList<String> getLibPath();

  /**
   * Returns the value of the '<em><b>Include Path</b></em>' attribute list. The list contents are
   * of type {@link java.lang.String}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Include Path</em>' attribute list isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Include Path</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getManagedLibraryType_IncludePath()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
   *     extendedMetaData="kind='element' name='includePath' namespace='##targetNamespace'"
   * @generated
   */
  EList<String> getIncludePath();

  /**
   * Returns the value of the '<em><b>Command</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Command</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Command</em>' attribute.
   * @see #setCommand(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getManagedLibraryType_Command()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='command' namespace='##targetNamespace'"
   * @generated
   */
  String getCommand();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getCommand <em>Command</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Command</em>' attribute.
   * @see #getCommand()
   * @generated
   */
  void setCommand(String value);
} // ManagedLibraryType
