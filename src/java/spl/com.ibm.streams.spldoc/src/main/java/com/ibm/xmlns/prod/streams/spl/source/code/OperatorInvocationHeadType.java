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
 * A representation of the model object '<em><b>Operator Invocation Head Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getSplDoc
 *       <em>Spl Doc</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOutputs
 *       <em>Outputs</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInputs
 *       <em>Inputs</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInvocationAlias
 *       <em>Invocation Alias</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOperatorName
 *       <em>Operator Name</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType()
 * @model extendedMetaData="name='operatorInvocationHeadType' kind='elementOnly'"
 * @generated
 */
public interface OperatorInvocationHeadType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Spl Doc</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Spl Doc</em>' containment reference.
   * @see #setSplDoc(SplDocType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType_SplDoc()
   * @model containment="true" extendedMetaData="kind='element' name='splDoc'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplDocType getSplDoc();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getSplDoc <em>Spl
   * Doc</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Doc</em>' containment reference.
   * @see #getSplDoc()
   * @generated
   */
  void setSplDoc(SplDocType value);

  /**
   * Returns the value of the '<em><b>Outputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Outputs</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Outputs</em>' containment reference.
   * @see #setOutputs(OperatorInvocationOutputsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType_Outputs()
   * @model containment="true" extendedMetaData="kind='element' name='outputs'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationOutputsType getOutputs();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOutputs
   * <em>Outputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Outputs</em>' containment reference.
   * @see #getOutputs()
   * @generated
   */
  void setOutputs(OperatorInvocationOutputsType value);

  /**
   * Returns the value of the '<em><b>Inputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Inputs</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Inputs</em>' containment reference.
   * @see #setInputs(OperatorInvocationInputsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType_Inputs()
   * @model containment="true" extendedMetaData="kind='element' name='inputs'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OperatorInvocationInputsType getInputs();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInputs
   * <em>Inputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Inputs</em>' containment reference.
   * @see #getInputs()
   * @generated
   */
  void setInputs(OperatorInvocationInputsType value);

  /**
   * Returns the value of the '<em><b>Invocation Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Invocation Alias</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Invocation Alias</em>' attribute.
   * @see #setInvocationAlias(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType_InvocationAlias()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='invocationAlias'"
   * @generated
   */
  String getInvocationAlias();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInvocationAlias
   * <em>Invocation Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Invocation Alias</em>' attribute.
   * @see #getInvocationAlias()
   * @generated
   */
  void setInvocationAlias(String value);

  /**
   * Returns the value of the '<em><b>Operator Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Operator Name</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Operator Name</em>' attribute.
   * @see #setOperatorName(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getOperatorInvocationHeadType_OperatorName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='operatorName'"
   * @generated
   */
  String getOperatorName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOperatorName
   * <em>Operator Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Operator Name</em>' attribute.
   * @see #getOperatorName()
   * @generated
   */
  void setOperatorName(String value);
} // OperatorInvocationHeadType
