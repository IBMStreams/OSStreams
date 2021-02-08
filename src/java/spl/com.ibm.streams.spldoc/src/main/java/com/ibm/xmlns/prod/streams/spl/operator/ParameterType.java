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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import java.math.BigInteger;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional <em>Optional</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed <em>Rewrite
 *       Allowed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getType <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getPortScope <em>Port
 *       Scope</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCustomOutputFunction
 *       <em>Custom Output Function</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType()
 * @model extendedMetaData="name='parameterType' kind='elementOnly'"
 * @generated
 */
public interface ParameterType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='name' namespace='##targetNamespace'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getName
   * <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getDescription <em>Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_Optional()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='optional' namespace='##targetNamespace'"
   * @generated
   */
  boolean isOptional();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional
   * <em>Optional</em>}' attribute.
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
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional <em>Optional</em>}' attribute.
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
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional <em>Optional</em>}' attribute
   * is set.
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

  /**
   * Returns the value of the '<em><b>Rewrite Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Rewrite Allowed</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Rewrite Allowed</em>' attribute.
   * @see #isSetRewriteAllowed()
   * @see #unsetRewriteAllowed()
   * @see #setRewriteAllowed(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_RewriteAllowed()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='rewriteAllowed' namespace='##targetNamespace'"
   * @generated
   */
  boolean isRewriteAllowed();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed <em>Rewrite
   * Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Rewrite Allowed</em>' attribute.
   * @see #isSetRewriteAllowed()
   * @see #unsetRewriteAllowed()
   * @see #isRewriteAllowed()
   * @generated
   */
  void setRewriteAllowed(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed <em>Rewrite
   * Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetRewriteAllowed()
   * @see #isRewriteAllowed()
   * @see #setRewriteAllowed(boolean)
   * @generated
   */
  void unsetRewriteAllowed();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed <em>Rewrite
   * Allowed</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Rewrite Allowed</em>' attribute is set.
   * @see #unsetRewriteAllowed()
   * @see #isRewriteAllowed()
   * @see #setRewriteAllowed(boolean)
   * @generated
   */
  boolean isSetRewriteAllowed();

  /**
   * Returns the value of the '<em><b>Expression Mode</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Expression Mode</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #setExpressionMode(ExpressionModeType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_ExpressionMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element' name='expressionMode'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ExpressionModeType getExpressionMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode <em>Expression
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Expression Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @see #isSetExpressionMode()
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @generated
   */
  void setExpressionMode(ExpressionModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode <em>Expression
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(ExpressionModeType)
   * @generated
   */
  void unsetExpressionMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode <em>Expression
   * Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Expression Mode</em>' attribute is set.
   * @see #unsetExpressionMode()
   * @see #getExpressionMode()
   * @see #setExpressionMode(ExpressionModeType)
   * @generated
   */
  boolean isSetExpressionMode();

  /**
   * Returns the value of the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type</em>' attribute.
   * @see #setType(String)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_Type()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='type' namespace='##targetNamespace'"
   * @generated
   */
  String getType();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getType
   * <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Type</em>' attribute.
   * @see #getType()
   * @generated
   */
  void setType(String value);

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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_Cardinality()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" extendedMetaData="kind='element'
   *     name='cardinality' namespace='##targetNamespace'"
   * @generated
   */
  BigInteger getCardinality();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCardinality <em>Cardinality</em>}'
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
   * Returns the value of the '<em><b>Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Port Scope</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Port Scope</em>' containment reference.
   * @see #setPortScope(PortScopeType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_PortScope()
   * @model containment="true" extendedMetaData="kind='element' name='portScope'
   *     namespace='##targetNamespace'"
   * @generated
   */
  PortScopeType getPortScope();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getPortScope <em>Port Scope</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Port Scope</em>' containment reference.
   * @see #getPortScope()
   * @generated
   */
  void setPortScope(PortScopeType value);

  /**
   * Returns the value of the '<em><b>Custom Output Function</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Custom Output Function</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Custom Output Function</em>' attribute.
   * @see #setCustomOutputFunction(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getParameterType_CustomOutputFunction()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='customOutputFunction' namespace='##targetNamespace'"
   * @generated
   */
  String getCustomOutputFunction();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCustomOutputFunction <em>Custom Output
   * Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Custom Output Function</em>' attribute.
   * @see #getCustomOutputFunction()
   * @generated
   */
  void setCustomOutputFunction(String value);
} // ParameterType
