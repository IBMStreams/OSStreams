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

import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Types Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType#getType <em>Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeTypesType()
 * @model extendedMetaData="name='compositeTypesType' kind='elementOnly'"
 * @generated
 */
public interface CompositeTypesType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Type</b></em>' containment reference list. The list contents
   * are of type {@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type</em>' containment reference list isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeTypesType_Type()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='type'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<TypeDefinitionType> getType();
} // CompositeTypesType
