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
package com.ibm.xmlns.prod.streams.spl.operator;

import java.math.BigInteger;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Optional Port Scope Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType#getPort <em>Port</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOptionalPortScopeType()
 * @model extendedMetaData="name='optionalPortScopeType' kind='elementOnly'"
 * @generated
 */
public interface OptionalPortScopeType extends EObject {
  /**
   * Returns the value of the '<em><b>Port</b></em>' attribute list. The list contents are of type
   * {@link java.math.BigInteger}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Port</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Port</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOptionalPortScopeType_Port()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.NonNegativeInteger"
   *     extendedMetaData="kind='element' name='port' namespace='##targetNamespace'"
   * @generated
   */
  EList<BigInteger> getPort();
} // OptionalPortScopeType
