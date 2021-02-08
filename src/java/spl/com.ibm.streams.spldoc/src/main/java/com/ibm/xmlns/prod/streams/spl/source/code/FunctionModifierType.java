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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Function Modifier Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName
 *       <em>Name</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionModifierType()
 * @model extendedMetaData="name='functionModifierType' kind='empty'"
 * @generated
 */
public interface FunctionModifierType extends EObject {
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Name</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Name</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @see #isSetName()
   * @see #unsetName()
   * @see #setName(FunctionModifierEnumType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionModifierType_Name()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute' name='name'"
   * @generated
   */
  FunctionModifierEnumType getName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName <em>Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @see #isSetName()
   * @see #unsetName()
   * @see #getName()
   * @generated
   */
  void setName(FunctionModifierEnumType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName <em>Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetName()
   * @see #getName()
   * @see #setName(FunctionModifierEnumType)
   * @generated
   */
  void unsetName();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName <em>Name</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Name</em>' attribute is set.
   * @see #unsetName()
   * @see #getName()
   * @see #setName(FunctionModifierEnumType)
   * @generated
   */
  boolean isSetName();
} // FunctionModifierType
