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

import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.FeatureMap;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Document Root</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getMixed <em>Mixed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXMLNSPrefixMap <em>XMLNS
 *       Prefix Map</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXSISchemaLocation <em>XSI
 *       Schema Location</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getJavaOpLibrary <em>Java Op
 *       Library</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getLibrary <em>Library</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot()
 * @model extendedMetaData="name='' kind='mixed'"
 * @generated
 */
public interface DocumentRoot extends EObject {
  /**
   * Returns the value of the '<em><b>Mixed</b></em>' attribute list. The list contents are of type
   * {@link org.eclipse.emf.ecore.util.FeatureMap.Entry}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Mixed</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Mixed</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot_Mixed()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='elementWildcard' name=':mixed'"
   * @generated
   */
  FeatureMap getMixed();

  /**
   * Returns the value of the '<em><b>XMLNS Prefix Map</b></em>' map. The key is of type {@link
   * java.lang.String}, and the value is of type {@link java.lang.String},
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>XMLNS Prefix Map</em>' map isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>XMLNS Prefix Map</em>' map.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot_XMLNSPrefixMap()
   * @model mapType="org.eclipse.emf.ecore.EStringToStringMapEntry<org.eclipse.emf.ecore.EString,
   *     org.eclipse.emf.ecore.EString>" transient="true" extendedMetaData="kind='attribute'
   *     name='xmlns:prefix'"
   * @generated
   */
  EMap<String, String> getXMLNSPrefixMap();

  /**
   * Returns the value of the '<em><b>XSI Schema Location</b></em>' map. The key is of type {@link
   * java.lang.String}, and the value is of type {@link java.lang.String},
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>XSI Schema Location</em>' map isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>XSI Schema Location</em>' map.
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot_XSISchemaLocation()
   * @model mapType="org.eclipse.emf.ecore.EStringToStringMapEntry<org.eclipse.emf.ecore.EString,
   *     org.eclipse.emf.ecore.EString>" transient="true" extendedMetaData="kind='attribute'
   *     name='xsi:schemaLocation'"
   * @generated
   */
  EMap<String, String> getXSISchemaLocation();

  /**
   * Returns the value of the '<em><b>Java Op Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Java Op Library</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Java Op Library</em>' containment reference.
   * @see #setJavaOpLibrary(JavaOpLibraryType)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot_JavaOpLibrary()
   * @model containment="true" upper="-2" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='javaOpLibrary' namespace='##targetNamespace'"
   * @generated
   */
  JavaOpLibraryType getJavaOpLibrary();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getJavaOpLibrary <em>Java Op Library</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Java Op Library</em>' containment reference.
   * @see #getJavaOpLibrary()
   * @generated
   */
  void setJavaOpLibrary(JavaOpLibraryType value);

  /**
   * Returns the value of the '<em><b>Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Library</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Library</em>' containment reference.
   * @see #setLibrary(LibraryType)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDocumentRoot_Library()
   * @model containment="true" upper="-2" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='library' namespace='##targetNamespace'"
   * @generated
   */
  LibraryType getLibrary();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getLibrary
   * <em>Library</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Library</em>' containment reference.
   * @see #getLibrary()
   * @generated
   */
  void setLibrary(LibraryType value);
} // DocumentRoot
