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
package com.ibm.xmlns.prod.streams.spl.toolkit;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Base Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType#getUriTable <em>Uri
 *       Table</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitBaseType()
 * @model extendedMetaData="name='toolkitBaseType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitBaseType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Uri Table</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Uri Table</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Uri Table</em>' containment reference.
   * @see #setUriTable(UriTableType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitBaseType_UriTable()
   * @model containment="true" extendedMetaData="kind='element' name='uriTable'
   *     namespace='##targetNamespace'"
   * @generated
   */
  UriTableType getUriTable();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType#getUriTable <em>Uri Table</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Uri Table</em>' containment reference.
   * @see #getUriTable()
   * @generated
   */
  void setUriTable(UriTableType value);
} // ToolkitBaseType
