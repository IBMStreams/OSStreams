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
 * A representation of the model object '<em><b>Function Head Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getSplDoc <em>Spl
 *       Doc</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getModifiers
 *       <em>Modifiers</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getReturnType <em>Return
 *       Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType()
 * @model extendedMetaData="name='functionHeadType' kind='elementOnly'"
 * @generated
 */
public interface FunctionHeadType extends SourceLocationType {
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
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType_SplDoc()
   * @model containment="true" extendedMetaData="kind='element' name='splDoc'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplDocType getSplDoc();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getSplDoc <em>Spl Doc</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Doc</em>' containment reference.
   * @see #getSplDoc()
   * @generated
   */
  void setSplDoc(SplDocType value);

  /**
   * Returns the value of the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Modifiers</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Modifiers</em>' containment reference.
   * @see #setModifiers(FunctionModifiersType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType_Modifiers()
   * @model containment="true" extendedMetaData="kind='element' name='modifiers'
   *     namespace='##targetNamespace'"
   * @generated
   */
  FunctionModifiersType getModifiers();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getModifiers <em>Modifiers</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Modifiers</em>' containment reference.
   * @see #getModifiers()
   * @generated
   */
  void setModifiers(FunctionModifiersType value);

  /**
   * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameters</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameters</em>' containment reference.
   * @see #setParameters(FunctionParametersType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType_Parameters()
   * @model containment="true" extendedMetaData="kind='element' name='parameters'
   *     namespace='##targetNamespace'"
   * @generated
   */
  FunctionParametersType getParameters();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getParameters <em>Parameters</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Parameters</em>' containment reference.
   * @see #getParameters()
   * @generated
   */
  void setParameters(FunctionParametersType value);

  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Name</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='name'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getName <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Return Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Return Type</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Return Type</em>' attribute.
   * @see #setReturnType(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionHeadType_ReturnType()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='returnType'"
   * @generated
   */
  String getReturnType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getReturnType <em>Return
   * Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Return Type</em>' attribute.
   * @see #getReturnType()
   * @generated
   */
  void setReturnType(String value);
} // FunctionHeadType
