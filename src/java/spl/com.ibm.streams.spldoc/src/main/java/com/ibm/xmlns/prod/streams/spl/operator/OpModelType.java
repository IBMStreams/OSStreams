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
 * A representation of the model object '<em><b>Op Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getContext <em>Context</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getInputPorts <em>Input
 *       Ports</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getOutputPorts <em>Output
 *       Ports</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOpModelType()
 * @model extendedMetaData="name='opModelType' kind='elementOnly'"
 * @generated
 */
public interface OpModelType extends EObject {
  /**
   * Returns the value of the '<em><b>Context</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Context</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Context</em>' containment reference.
   * @see #setContext(ContextType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOpModelType_Context()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='context'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ContextType getContext();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getContext
   * <em>Context</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Context</em>' containment reference.
   * @see #getContext()
   * @generated
   */
  void setContext(ContextType value);

  /**
   * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameters</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameters</em>' containment reference.
   * @see #setParameters(ParametersType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOpModelType_Parameters()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='parameters'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ParametersType getParameters();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getParameters
   * <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Parameters</em>' containment reference.
   * @see #getParameters()
   * @generated
   */
  void setParameters(ParametersType value);

  /**
   * Returns the value of the '<em><b>Input Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Input Ports</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Input Ports</em>' containment reference.
   * @see #setInputPorts(InputPortsType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOpModelType_InputPorts()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='inputPorts'
   *     namespace='##targetNamespace'"
   * @generated
   */
  InputPortsType getInputPorts();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getInputPorts
   * <em>Input Ports</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Input Ports</em>' containment reference.
   * @see #getInputPorts()
   * @generated
   */
  void setInputPorts(InputPortsType value);

  /**
   * Returns the value of the '<em><b>Output Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Ports</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Ports</em>' containment reference.
   * @see #setOutputPorts(OutputPortsType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOpModelType_OutputPorts()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='outputPorts'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OutputPortsType getOutputPorts();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getOutputPorts <em>Output Ports</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Ports</em>' containment reference.
   * @see #getOutputPorts()
   * @generated
   */
  void setOutputPorts(OutputPortsType value);
} // OpModelType
