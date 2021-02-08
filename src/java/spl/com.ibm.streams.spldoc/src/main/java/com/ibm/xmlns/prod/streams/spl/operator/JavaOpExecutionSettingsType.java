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
 * A representation of the model object '<em><b>Java Op Execution Settings Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getClassName
 *       <em>Class Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getVmArgs <em>Vm
 *       Args</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpExecutionSettingsType()
 * @model extendedMetaData="name='javaOpExecutionSettingsType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpExecutionSettingsType extends EObject {
  /**
   * Returns the value of the '<em><b>Class Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Class Name</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Class Name</em>' attribute.
   * @see #setClassName(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpExecutionSettingsType_ClassName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" required="true"
   *     extendedMetaData="kind='element' name='className' namespace='##targetNamespace'"
   * @generated
   */
  String getClassName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getClassName <em>Class
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Class Name</em>' attribute.
   * @see #getClassName()
   * @generated
   */
  void setClassName(String value);

  /**
   * Returns the value of the '<em><b>Vm Args</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Vm Args</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Vm Args</em>' containment reference.
   * @see #setVmArgs(JavaOpVMArgsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpExecutionSettingsType_VmArgs()
   * @model containment="true" extendedMetaData="kind='element' name='vmArgs'
   *     namespace='##targetNamespace'"
   * @generated
   */
  JavaOpVMArgsType getVmArgs();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getVmArgs <em>Vm
   * Args</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Vm Args</em>' containment reference.
   * @see #getVmArgs()
   * @generated
   */
  void setVmArgs(JavaOpVMArgsType value);
} // JavaOpExecutionSettingsType
