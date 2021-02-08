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
 * A representation of the model object '<em><b>Base Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#getImages
 *       <em>Images</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic
 *       <em>Public</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseToolkitOperatorType()
 * @model extendedMetaData="name='baseToolkitOperatorType' kind='elementOnly'"
 * @generated
 */
public interface BaseToolkitOperatorType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Images</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Images</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Images</em>' containment reference.
   * @see #setImages(ImagesType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseToolkitOperatorType_Images()
   * @model containment="true" extendedMetaData="kind='element' name='images'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ImagesType getImages();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#getImages <em>Images</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Images</em>' containment reference.
   * @see #getImages()
   * @generated
   */
  void setImages(ImagesType value);

  /**
   * Returns the value of the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Public</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Public</em>' attribute.
   * @see #isSetPublic()
   * @see #unsetPublic()
   * @see #setPublic(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getBaseToolkitOperatorType_Public()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='public'"
   * @generated
   */
  boolean isPublic();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic <em>Public</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Public</em>' attribute.
   * @see #isSetPublic()
   * @see #unsetPublic()
   * @see #isPublic()
   * @generated
   */
  void setPublic(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic <em>Public</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPublic()
   * @see #isPublic()
   * @see #setPublic(boolean)
   * @generated
   */
  void unsetPublic();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic <em>Public</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Public</em>' attribute is set.
   * @see #unsetPublic()
   * @see #isPublic()
   * @see #setPublic(boolean)
   * @generated
   */
  boolean isSetPublic();
} // BaseToolkitOperatorType
