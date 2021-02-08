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
 * A representation of the model object '<em><b>Function Definition Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionHead
 *       <em>Function Head</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionBody
 *       <em>Function Body</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionDefinitionType()
 * @model extendedMetaData="name='functionDefinitionType' kind='elementOnly'"
 * @generated
 */
public interface FunctionDefinitionType extends ExtendedSourceLocationType {
  /**
   * Returns the value of the '<em><b>Function Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Function Head</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Function Head</em>' containment reference.
   * @see #setFunctionHead(FunctionHeadType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionDefinitionType_FunctionHead()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='functionHead'
   *     namespace='##targetNamespace'"
   * @generated
   */
  FunctionHeadType getFunctionHead();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionHead <em>Function
   * Head</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Function Head</em>' containment reference.
   * @see #getFunctionHead()
   * @generated
   */
  void setFunctionHead(FunctionHeadType value);

  /**
   * Returns the value of the '<em><b>Function Body</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Function Body</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Function Body</em>' attribute.
   * @see #setFunctionBody(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getFunctionDefinitionType_FunctionBody()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='functionBody' namespace='##targetNamespace'"
   * @generated
   */
  String getFunctionBody();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionBody <em>Function
   * Body</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Function Body</em>' attribute.
   * @see #getFunctionBody()
   * @generated
   */
  void setFunctionBody(String value);
} // FunctionDefinitionType
