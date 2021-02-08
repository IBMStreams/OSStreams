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

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Output Port Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortSetType()
 * @model extendedMetaData="name='outputPortSetType' kind='elementOnly'"
 * @generated
 */
public interface OutputPortSetType extends OutputPortOpenSetType {
  /**
   * Returns the value of the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cardinality</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cardinality</em>' attribute.
   * @see #setCardinality(BigInteger)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortSetType_Cardinality()
   * @model dataType="org.eclipse.emf.ecore.xml.type.NonNegativeInteger" required="true"
   *     extendedMetaData="kind='element' name='cardinality' namespace='##targetNamespace'"
   * @generated
   */
  BigInteger getCardinality();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#getCardinality <em>Cardinality</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cardinality</em>' attribute.
   * @see #getCardinality()
   * @generated
   */
  void setCardinality(BigInteger value);

  /**
   * Returns the value of the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Optional</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Optional</em>' attribute.
   * @see #isSetOptional()
   * @see #unsetOptional()
   * @see #setOptional(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortSetType_Optional()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='optional' namespace='##targetNamespace'"
   * @generated
   */
  boolean isOptional();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional <em>Optional</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Optional</em>' attribute.
   * @see #isSetOptional()
   * @see #unsetOptional()
   * @see #isOptional()
   * @generated
   */
  void setOptional(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional <em>Optional</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetOptional()
   * @see #isOptional()
   * @see #setOptional(boolean)
   * @generated
   */
  void unsetOptional();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional <em>Optional</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Optional</em>' attribute is set.
   * @see #unsetOptional()
   * @see #isOptional()
   * @see #setOptional(boolean)
   * @generated
   */
  boolean isSetOptional();
} // OutputPortSetType
