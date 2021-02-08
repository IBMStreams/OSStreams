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

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getCppOperatorModel
 *       <em>Cpp Operator Model</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getJavaOperatorModel
 *       <em>Java Operator Model</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOperatorModelType()
 * @model extendedMetaData="name='operatorModelType' kind='elementOnly'"
 * @generated
 */
public interface OperatorModelType extends EObject {
  /**
   * Returns the value of the '<em><b>Cpp Operator Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cpp Operator Model</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cpp Operator Model</em>' containment reference.
   * @see #setCppOperatorModel(OpModelType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOperatorModelType_CppOperatorModel()
   * @model containment="true" extendedMetaData="kind='element' name='cppOperatorModel'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OpModelType getCppOperatorModel();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getCppOperatorModel <em>Cpp Operator
   * Model</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cpp Operator Model</em>' containment reference.
   * @see #getCppOperatorModel()
   * @generated
   */
  void setCppOperatorModel(OpModelType value);

  /**
   * Returns the value of the '<em><b>Java Operator Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Java Operator Model</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Java Operator Model</em>' containment reference.
   * @see #setJavaOperatorModel(JavaOpModelType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOperatorModelType_JavaOperatorModel()
   * @model containment="true" extendedMetaData="kind='element' name='javaOperatorModel'
   *     namespace='##targetNamespace'"
   * @generated
   */
  JavaOpModelType getJavaOperatorModel();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getJavaOperatorModel <em>Java
   * Operator Model</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Java Operator Model</em>' containment reference.
   * @see #getJavaOperatorModel()
   * @generated
   */
  void setJavaOperatorModel(JavaOpModelType value);
} // OperatorModelType
