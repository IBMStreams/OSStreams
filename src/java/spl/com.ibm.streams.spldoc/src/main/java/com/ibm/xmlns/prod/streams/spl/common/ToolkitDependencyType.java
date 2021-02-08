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
 * A representation of the model object '<em><b>Toolkit Dependency Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getVersion
 *       <em>Version</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getToolkitDependencyType()
 * @model extendedMetaData="name='toolkitDependencyType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitDependencyType extends EObject {
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Name</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getToolkitDependencyType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='name' namespace='##targetNamespace'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Version</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Version</em>' attribute.
   * @see #setVersion(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getToolkitDependencyType_Version()
   * @model dataType="com.ibm.xmlns.prod.streams.spl.common.VersionRangeType" required="true"
   *     extendedMetaData="kind='element' name='version' namespace='##targetNamespace'"
   * @generated
   */
  String getVersion();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getVersion <em>Version</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Version</em>' attribute.
   * @see #getVersion()
   * @generated
   */
  void setVersion(String value);
} // ToolkitDependencyType
