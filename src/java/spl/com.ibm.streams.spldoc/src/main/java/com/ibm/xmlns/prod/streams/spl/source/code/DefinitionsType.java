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
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Definitions Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getTypeDefinition
 *       <em>Type Definition</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getFunctionDefinition
 *       <em>Function Definition</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getCompositeDefinition
 *       <em>Composite Definition</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getDefinitionsType()
 * @model extendedMetaData="name='definitionsType' kind='elementOnly'"
 * @generated
 */
public interface DefinitionsType extends EObject {
  /**
   * Returns the value of the '<em><b>Type Definition</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type Definition</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type Definition</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getDefinitionsType_TypeDefinition()
   * @model containment="true" extendedMetaData="kind='element' name='typeDefinition'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<TypeDefinitionType> getTypeDefinition();

  /**
   * Returns the value of the '<em><b>Function Definition</b></em>' containment reference list. The
   * list contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Function Definition</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Function Definition</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getDefinitionsType_FunctionDefinition()
   * @model containment="true" extendedMetaData="kind='element' name='functionDefinition'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<FunctionDefinitionType> getFunctionDefinition();

  /**
   * Returns the value of the '<em><b>Composite Definition</b></em>' containment reference list. The
   * list contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Composite Definition</em>' containment reference list isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Composite Definition</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getDefinitionsType_CompositeDefinition()
   * @model containment="true" extendedMetaData="kind='element' name='compositeDefinition'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<CompositeDefinitionType> getCompositeDefinition();
} // DefinitionsType
