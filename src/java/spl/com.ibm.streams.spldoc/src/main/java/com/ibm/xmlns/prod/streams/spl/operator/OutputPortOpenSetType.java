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
 * A representation of the model object '<em><b>Output Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment
 *       <em>Auto Assignment</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment
 *       <em>Complete Assignment</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed
 *       <em>Rewrite Allowed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputFunctions
 *       <em>Output Functions</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode
 *       <em>Window Punctuation Output Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationInputPort
 *       <em>Window Punctuation Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getFinalPunctuationPortScope
 *       <em>Final Punctuation Port Scope</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed
 *       <em>Tuple Mutation Allowed</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputAssignmentPortScope
 *       <em>Output Assignment Port Scope</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions
 *       <em>Allow Nested Custom Output Functions</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType()
 * @model extendedMetaData="name='outputPortOpenSetType' kind='elementOnly'"
 * @generated
 */
public interface OutputPortOpenSetType extends EObject {
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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getDescription
   * <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_ExpressionMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element' name='expressionMode'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ExpressionModeType getExpressionMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode <em>Expression
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
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode <em>Expression
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
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode <em>Expression
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
   * Returns the value of the '<em><b>Auto Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Auto Assignment</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Auto Assignment</em>' attribute.
   * @see #isSetAutoAssignment()
   * @see #unsetAutoAssignment()
   * @see #setAutoAssignment(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_AutoAssignment()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='autoAssignment' namespace='##targetNamespace'"
   * @generated
   */
  boolean isAutoAssignment();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment <em>Auto
   * Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Auto Assignment</em>' attribute.
   * @see #isSetAutoAssignment()
   * @see #unsetAutoAssignment()
   * @see #isAutoAssignment()
   * @generated
   */
  void setAutoAssignment(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment <em>Auto
   * Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetAutoAssignment()
   * @see #isAutoAssignment()
   * @see #setAutoAssignment(boolean)
   * @generated
   */
  void unsetAutoAssignment();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment <em>Auto
   * Assignment</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Auto Assignment</em>' attribute is set.
   * @see #unsetAutoAssignment()
   * @see #isAutoAssignment()
   * @see #setAutoAssignment(boolean)
   * @generated
   */
  boolean isSetAutoAssignment();

  /**
   * Returns the value of the '<em><b>Complete Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Complete Assignment</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Complete Assignment</em>' attribute.
   * @see #isSetCompleteAssignment()
   * @see #unsetCompleteAssignment()
   * @see #setCompleteAssignment(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_CompleteAssignment()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='completeAssignment' namespace='##targetNamespace'"
   * @generated
   */
  boolean isCompleteAssignment();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment <em>Complete
   * Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Complete Assignment</em>' attribute.
   * @see #isSetCompleteAssignment()
   * @see #unsetCompleteAssignment()
   * @see #isCompleteAssignment()
   * @generated
   */
  void setCompleteAssignment(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment <em>Complete
   * Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetCompleteAssignment()
   * @see #isCompleteAssignment()
   * @see #setCompleteAssignment(boolean)
   * @generated
   */
  void unsetCompleteAssignment();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment <em>Complete
   * Assignment</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Complete Assignment</em>' attribute is set.
   * @see #unsetCompleteAssignment()
   * @see #isCompleteAssignment()
   * @see #setCompleteAssignment(boolean)
   * @generated
   */
  boolean isSetCompleteAssignment();

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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_RewriteAllowed()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='rewriteAllowed' namespace='##targetNamespace'"
   * @generated
   */
  boolean isRewriteAllowed();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed <em>Rewrite
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
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed <em>Rewrite
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
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed <em>Rewrite
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
   * Returns the value of the '<em><b>Output Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Functions</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Functions</em>' containment reference.
   * @see #setOutputFunctions(OutputFunctionsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_OutputFunctions()
   * @model containment="true" extendedMetaData="kind='element' name='outputFunctions'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OutputFunctionsType getOutputFunctions();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputFunctions <em>Output
   * Functions</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Functions</em>' containment reference.
   * @see #getOutputFunctions()
   * @generated
   */
  void setOutputFunctions(OutputFunctionsType value);

  /**
   * Returns the value of the '<em><b>Window Punctuation Output Mode</b></em>' attribute. The
   * literals are from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punctuation Output Mode</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punctuation Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see #isSetWindowPunctuationOutputMode()
   * @see #unsetWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_WindowPunctuationOutputMode()
   * @model unsettable="true" required="true" extendedMetaData="kind='element'
   *     name='windowPunctuationOutputMode' namespace='##targetNamespace'"
   * @generated
   */
  WindowPunctuationOutputModeType getWindowPunctuationOutputMode();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punctuation Output Mode</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see #isSetWindowPunctuationOutputMode()
   * @see #unsetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @generated
   */
  void setWindowPunctuationOutputMode(WindowPunctuationOutputModeType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @generated
   */
  void unsetWindowPunctuationOutputMode();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Window Punctuation Output Mode</em>' attribute is set.
   * @see #unsetWindowPunctuationOutputMode()
   * @see #getWindowPunctuationOutputMode()
   * @see #setWindowPunctuationOutputMode(WindowPunctuationOutputModeType)
   * @generated
   */
  boolean isSetWindowPunctuationOutputMode();

  /**
   * Returns the value of the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Window Punctuation Input Port</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Window Punctuation Input Port</em>' attribute.
   * @see #setWindowPunctuationInputPort(BigInteger)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_WindowPunctuationInputPort()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Integer" extendedMetaData="kind='element'
   *     name='windowPunctuationInputPort' namespace='##targetNamespace'"
   * @generated
   */
  BigInteger getWindowPunctuationInputPort();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationInputPort
   * <em>Window Punctuation Input Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Window Punctuation Input Port</em>' attribute.
   * @see #getWindowPunctuationInputPort()
   * @generated
   */
  void setWindowPunctuationInputPort(BigInteger value);

  /**
   * Returns the value of the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Final Punctuation Port Scope</em>' containment reference isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Final Punctuation Port Scope</em>' containment reference.
   * @see #setFinalPunctuationPortScope(OptionalPortScopeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_FinalPunctuationPortScope()
   * @model containment="true" extendedMetaData="kind='element' name='finalPunctuationPortScope'
   *     namespace='##targetNamespace'"
   * @generated
   */
  OptionalPortScopeType getFinalPunctuationPortScope();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getFinalPunctuationPortScope
   * <em>Final Punctuation Port Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Final Punctuation Port Scope</em>' containment
   *     reference.
   * @see #getFinalPunctuationPortScope()
   * @generated
   */
  void setFinalPunctuationPortScope(OptionalPortScopeType value);

  /**
   * Returns the value of the '<em><b>Tuple Mutation Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Tuple Mutation Allowed</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Tuple Mutation Allowed</em>' attribute.
   * @see #isSetTupleMutationAllowed()
   * @see #unsetTupleMutationAllowed()
   * @see #setTupleMutationAllowed(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_TupleMutationAllowed()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='element' name='tupleMutationAllowed' namespace='##targetNamespace'"
   * @generated
   */
  boolean isTupleMutationAllowed();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed <em>Tuple
   * Mutation Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Tuple Mutation Allowed</em>' attribute.
   * @see #isSetTupleMutationAllowed()
   * @see #unsetTupleMutationAllowed()
   * @see #isTupleMutationAllowed()
   * @generated
   */
  void setTupleMutationAllowed(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed <em>Tuple
   * Mutation Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetTupleMutationAllowed()
   * @see #isTupleMutationAllowed()
   * @see #setTupleMutationAllowed(boolean)
   * @generated
   */
  void unsetTupleMutationAllowed();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed <em>Tuple
   * Mutation Allowed</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Tuple Mutation Allowed</em>' attribute is set.
   * @see #unsetTupleMutationAllowed()
   * @see #isTupleMutationAllowed()
   * @see #setTupleMutationAllowed(boolean)
   * @generated
   */
  boolean isSetTupleMutationAllowed();

  /**
   * Returns the value of the '<em><b>Output Assignment Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Assignment Port Scope</em>' containment reference isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Assignment Port Scope</em>' containment reference.
   * @see #setOutputAssignmentPortScope(PortScopeType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_OutputAssignmentPortScope()
   * @model containment="true" extendedMetaData="kind='element' name='outputAssignmentPortScope'
   *     namespace='##targetNamespace'"
   * @generated
   */
  PortScopeType getOutputAssignmentPortScope();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputAssignmentPortScope
   * <em>Output Assignment Port Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Output Assignment Port Scope</em>' containment
   *     reference.
   * @see #getOutputAssignmentPortScope()
   * @generated
   */
  void setOutputAssignmentPortScope(PortScopeType value);

  /**
   * Returns the value of the '<em><b>Allow Nested Custom Output Functions</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Allow Nested Custom Output Functions</em>' attribute isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Allow Nested Custom Output Functions</em>' attribute.
   * @see #isSetAllowNestedCustomOutputFunctions()
   * @see #unsetAllowNestedCustomOutputFunctions()
   * @see #setAllowNestedCustomOutputFunctions(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getOutputPortOpenSetType_AllowNestedCustomOutputFunctions()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='element' name='allowNestedCustomOutputFunctions'
   *     namespace='##targetNamespace'"
   * @generated
   */
  boolean isAllowNestedCustomOutputFunctions();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions
   * <em>Allow Nested Custom Output Functions</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Allow Nested Custom Output Functions</em>' attribute.
   * @see #isSetAllowNestedCustomOutputFunctions()
   * @see #unsetAllowNestedCustomOutputFunctions()
   * @see #isAllowNestedCustomOutputFunctions()
   * @generated
   */
  void setAllowNestedCustomOutputFunctions(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions
   * <em>Allow Nested Custom Output Functions</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetAllowNestedCustomOutputFunctions()
   * @see #isAllowNestedCustomOutputFunctions()
   * @see #setAllowNestedCustomOutputFunctions(boolean)
   * @generated
   */
  void unsetAllowNestedCustomOutputFunctions();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions
   * <em>Allow Nested Custom Output Functions</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Allow Nested Custom Output Functions</em>' attribute is
   *     set.
   * @see #unsetAllowNestedCustomOutputFunctions()
   * @see #isAllowNestedCustomOutputFunctions()
   * @see #setAllowNestedCustomOutputFunctions(boolean)
   * @generated
   */
  boolean isSetAllowNestedCustomOutputFunctions();
} // OutputPortOpenSetType
