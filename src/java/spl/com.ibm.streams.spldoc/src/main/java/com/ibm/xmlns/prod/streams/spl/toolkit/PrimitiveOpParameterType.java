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
package com.ibm.xmlns.prod.streams.spl.toolkit;

import org.eclipse.emf.common.util.EList;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Primitive Op Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getPortScope
 *       <em>Port Scope</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpParameterType()
 * @model extendedMetaData="name='primitiveOpParameterType' kind='elementOnly'"
 * @generated
 */
public interface PrimitiveOpParameterType extends ParameterType {
  /**
   * Returns the value of the '<em><b>Port Scope</b></em>' attribute list. The list contents are of
   * type {@link java.lang.Integer}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Port Scope</em>' attribute list isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Port Scope</em>' attribute list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpParameterType_PortScope()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='element' name='portScope' namespace='##targetNamespace'"
   * @generated
   */
  EList<Integer> getPortScope();

  /**
   * Returns the value of the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cardinality</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cardinality</em>' attribute.
   * @see #isSetCardinality()
   * @see #unsetCardinality()
   * @see #setCardinality(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpParameterType_Cardinality()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='cardinality'"
   * @generated
   */
  int getCardinality();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality
   * <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cardinality</em>' attribute.
   * @see #isSetCardinality()
   * @see #unsetCardinality()
   * @see #getCardinality()
   * @generated
   */
  void setCardinality(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality
   * <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetCardinality()
   * @see #getCardinality()
   * @see #setCardinality(int)
   * @generated
   */
  void unsetCardinality();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality
   * <em>Cardinality</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Cardinality</em>' attribute is set.
   * @see #unsetCardinality()
   * @see #getCardinality()
   * @see #setCardinality(int)
   * @generated
   */
  boolean isSetCardinality();

  /**
   * Returns the value of the '<em><b>Expression Mode</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Expression Mode</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpParameterType_ExpressionMode()
   * @model unsettable="true" extendedMetaData="kind='attribute' name='expressionMode'"
   * @generated
   */
  EnumExpressionModeType getExpressionMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode
   * <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @generated
   */
  void setExpressionMode(EnumExpressionModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode
   * <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @generated
   */
  void unsetExpressionMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode
   * <em>Expression Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Expression Mode</em>' attribute is set.
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(EnumExpressionModeType)
   * @generated
   */
  boolean isSetExpressionMode();

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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveOpParameterType_Optional()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='attribute' name='optional'"
   * @generated
   */
  boolean isOptional();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional <em>Optional</em>}'
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional <em>Optional</em>}'
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional <em>Optional</em>}'
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
} // PrimitiveOpParameterType
