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
package com.ibm.xmlns.prod.streams.spl.source.code;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Definition Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeHead
 *       <em>Composite Head</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeBody
 *       <em>Composite Body</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeDefinitionType()
 * @model extendedMetaData="name='compositeDefinitionType' kind='elementOnly'"
 * @generated
 */
public interface CompositeDefinitionType extends ExtendedSourceLocationType {
  /**
   * Returns the value of the '<em><b>Composite Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Composite Head</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Composite Head</em>' containment reference.
   * @see #setCompositeHead(CompositeHeadType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeDefinitionType_CompositeHead()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='compositeHead'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CompositeHeadType getCompositeHead();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeHead
   * <em>Composite Head</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Composite Head</em>' containment reference.
   * @see #getCompositeHead()
   * @generated
   */
  void setCompositeHead(CompositeHeadType value);

  /**
   * Returns the value of the '<em><b>Composite Body</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Composite Body</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Composite Body</em>' containment reference.
   * @see #setCompositeBody(CompositeBodyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeDefinitionType_CompositeBody()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='compositeBody'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CompositeBodyType getCompositeBody();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeBody
   * <em>Composite Body</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Composite Body</em>' containment reference.
   * @see #getCompositeBody()
   * @generated
   */
  void setCompositeBody(CompositeBodyType value);
} // CompositeDefinitionType
