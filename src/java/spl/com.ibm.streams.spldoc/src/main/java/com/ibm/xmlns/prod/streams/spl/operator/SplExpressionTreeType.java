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
 * A representation of the model object '<em><b>Spl Expression Tree Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode <em>Cpp
 *       Code</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput
 *       <em>Output</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam
 *       <em>Param</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getSplExpressionTreeType()
 * @model extendedMetaData="name='splExpressionTreeType' kind='empty'"
 * @generated
 */
public interface SplExpressionTreeType extends EObject {
  /**
   * Returns the value of the '<em><b>Cpp Code</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cpp Code</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cpp Code</em>' attribute.
   * @see #isSetCppCode()
   * @see #unsetCppCode()
   * @see #setCppCode(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getSplExpressionTreeType_CppCode()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='cppCode'"
   * @generated
   */
  boolean isCppCode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode <em>Cpp Code</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cpp Code</em>' attribute.
   * @see #isSetCppCode()
   * @see #unsetCppCode()
   * @see #isCppCode()
   * @generated
   */
  void setCppCode(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode <em>Cpp Code</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetCppCode()
   * @see #isCppCode()
   * @see #setCppCode(boolean)
   * @generated
   */
  void unsetCppCode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode <em>Cpp Code</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Cpp Code</em>' attribute is set.
   * @see #unsetCppCode()
   * @see #isCppCode()
   * @see #setCppCode(boolean)
   * @generated
   */
  boolean isSetCppCode();

  /**
   * Returns the value of the '<em><b>Output</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output</em>' attribute.
   * @see #isSetOutput()
   * @see #unsetOutput()
   * @see #setOutput(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getSplExpressionTreeType_Output()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='output'"
   * @generated
   */
  boolean isOutput();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput <em>Output</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output</em>' attribute.
   * @see #isSetOutput()
   * @see #unsetOutput()
   * @see #isOutput()
   * @generated
   */
  void setOutput(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput <em>Output</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetOutput()
   * @see #isOutput()
   * @see #setOutput(boolean)
   * @generated
   */
  void unsetOutput();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput <em>Output</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Output</em>' attribute is set.
   * @see #unsetOutput()
   * @see #isOutput()
   * @see #setOutput(boolean)
   * @generated
   */
  boolean isSetOutput();

  /**
   * Returns the value of the '<em><b>Param</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Param</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Param</em>' attribute.
   * @see #isSetParam()
   * @see #unsetParam()
   * @see #setParam(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getSplExpressionTreeType_Param()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='param'"
   * @generated
   */
  boolean isParam();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam <em>Param</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Param</em>' attribute.
   * @see #isSetParam()
   * @see #unsetParam()
   * @see #isParam()
   * @generated
   */
  void setParam(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam <em>Param</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetParam()
   * @see #isParam()
   * @see #setParam(boolean)
   * @generated
   */
  void unsetParam();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam <em>Param</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Param</em>' attribute is set.
   * @see #unsetParam()
   * @see #isParam()
   * @see #setParam(boolean)
   * @generated
   */
  boolean isSetParam();
} // SplExpressionTreeType
