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
 * A representation of the model object '<em><b>Description Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getValue <em>Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getDocHref <em>Doc Href</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getSampleUri <em>Sample
 *       Uri</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDescriptionType()
 * @model extendedMetaData="name='descriptionType' kind='simple'"
 * @generated
 */
public interface DescriptionType extends EObject {
  /**
   * Returns the value of the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Value</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Value</em>' attribute.
   * @see #setValue(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDescriptionType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="name=':0'
   *     kind='simple'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getValue
   * <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);

  /**
   * Returns the value of the '<em><b>Doc Href</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Doc Href</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Doc Href</em>' attribute.
   * @see #setDocHref(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDescriptionType_DocHref()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='docHref'"
   * @generated
   */
  String getDocHref();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getDocHref
   * <em>Doc Href</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Doc Href</em>' attribute.
   * @see #getDocHref()
   * @generated
   */
  void setDocHref(String value);

  /**
   * Returns the value of the '<em><b>Sample Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Sample Uri</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Sample Uri</em>' attribute.
   * @see #setSampleUri(String)
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#getDescriptionType_SampleUri()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='sampleUri'"
   * @generated
   */
  String getSampleUri();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getSampleUri <em>Sample Uri</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Sample Uri</em>' attribute.
   * @see #getSampleUri()
   * @generated
   */
  void setSampleUri(String value);
} // DescriptionType
