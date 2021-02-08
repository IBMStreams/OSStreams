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
 * A representation of the model object '<em><b>Include Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getPath <em>Path</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot <em>Root</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIncludeType()
 * @model extendedMetaData="name='includeType' kind='empty'"
 * @generated
 */
public interface IncludeType extends EObject {
  /**
   * Returns the value of the '<em><b>Path</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Path</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Path</em>' attribute.
   * @see #setPath(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIncludeType_Path()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='path'"
   * @generated
   */
  String getPath();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getPath
   * <em>Path</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Path</em>' attribute.
   * @see #getPath()
   * @generated
   */
  void setPath(String value);

  /**
   * Returns the value of the '<em><b>Root</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Root</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Root</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @see #isSetRoot()
   * @see #unsetRoot()
   * @see #setRoot(RootType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIncludeType_Root()
   * @model unsettable="true" extendedMetaData="kind='attribute' name='root'"
   * @generated
   */
  RootType getRoot();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot
   * <em>Root</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Root</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @see #isSetRoot()
   * @see #unsetRoot()
   * @see #getRoot()
   * @generated
   */
  void setRoot(RootType value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot
   * <em>Root</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetRoot()
   * @see #getRoot()
   * @see #setRoot(RootType)
   * @generated
   */
  void unsetRoot();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot <em>Root</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Root</em>' attribute is set.
   * @see #unsetRoot()
   * @see #getRoot()
   * @see #setRoot(RootType)
   * @generated
   */
  boolean isSetRoot();
} // IncludeType
