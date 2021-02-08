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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getToolkit <em>Toolkit</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getProductVersion
 *       <em>Product Version</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitModelType()
 * @model extendedMetaData="name='toolkitModelType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitModelType extends EObject {
  /**
   * Returns the value of the '<em><b>Toolkit</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Toolkit</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Toolkit</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitModelType_Toolkit()
   * @model containment="true" extendedMetaData="kind='element' name='toolkit'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<ToolkitType> getToolkit();

  /**
   * Returns the value of the '<em><b>Product Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Product Version</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Product Version</em>' attribute.
   * @see #setProductVersion(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitModelType_ProductVersion()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" required="true"
   *     extendedMetaData="kind='attribute' name='productVersion'"
   * @generated
   */
  String getProductVersion();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getProductVersion <em>Product
   * Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Product Version</em>' attribute.
   * @see #getProductVersion()
   * @generated
   */
  void setProductVersion(String value);
} // ToolkitModelType
