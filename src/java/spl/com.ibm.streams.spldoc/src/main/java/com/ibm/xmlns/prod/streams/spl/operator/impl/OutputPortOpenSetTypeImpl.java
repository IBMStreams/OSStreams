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
package com.ibm.xmlns.prod.streams.spl.operator.impl;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.PortScopeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType;
import java.math.BigInteger;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Output Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#isAutoAssignment
 *       <em>Auto Assignment</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#isCompleteAssignment
 *       <em>Complete Assignment</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#isRewriteAllowed
 *       <em>Rewrite Allowed</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getOutputFunctions
 *       <em>Output Functions</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getWindowPunctuationOutputMode
 *       <em>Window Punctuation Output Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getWindowPunctuationInputPort
 *       <em>Window Punctuation Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getFinalPunctuationPortScope
 *       <em>Final Punctuation Port Scope</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#isTupleMutationAllowed
 *       <em>Tuple Mutation Allowed</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#getOutputAssignmentPortScope
 *       <em>Output Assignment Port Scope</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl#isAllowNestedCustomOutputFunctions
 *       <em>Allow Nested Custom Output Functions</em>}
 * </ul>
 *
 * @generated
 */
public class OutputPortOpenSetTypeImpl extends MinimalEObjectImpl.Container
    implements OutputPortOpenSetType {
  /**
   * The cached value of the '{@link #getDescription() <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDescription()
   * @generated
   * @ordered
   */
  protected DescriptionType description;

  /**
   * The default value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected static final ExpressionModeType EXPRESSION_MODE_EDEFAULT = ExpressionModeType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected ExpressionModeType expressionMode = EXPRESSION_MODE_EDEFAULT;

  /**
   * This is true if the Expression Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean expressionModeESet;

  /**
   * The default value of the '{@link #isAutoAssignment() <em>Auto Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAutoAssignment()
   * @generated
   * @ordered
   */
  protected static final boolean AUTO_ASSIGNMENT_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isAutoAssignment() <em>Auto Assignment</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAutoAssignment()
   * @generated
   * @ordered
   */
  protected boolean autoAssignment = AUTO_ASSIGNMENT_EDEFAULT;

  /**
   * This is true if the Auto Assignment attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean autoAssignmentESet;

  /**
   * The default value of the '{@link #isCompleteAssignment() <em>Complete Assignment</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isCompleteAssignment()
   * @generated
   * @ordered
   */
  protected static final boolean COMPLETE_ASSIGNMENT_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isCompleteAssignment() <em>Complete Assignment</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isCompleteAssignment()
   * @generated
   * @ordered
   */
  protected boolean completeAssignment = COMPLETE_ASSIGNMENT_EDEFAULT;

  /**
   * This is true if the Complete Assignment attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean completeAssignmentESet;

  /**
   * The default value of the '{@link #isRewriteAllowed() <em>Rewrite Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowed()
   * @generated
   * @ordered
   */
  protected static final boolean REWRITE_ALLOWED_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isRewriteAllowed() <em>Rewrite Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowed()
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowed = REWRITE_ALLOWED_EDEFAULT;

  /**
   * This is true if the Rewrite Allowed attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowedESet;

  /**
   * The cached value of the '{@link #getOutputFunctions() <em>Output Functions</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputFunctions()
   * @generated
   * @ordered
   */
  protected OutputFunctionsType outputFunctions;

  /**
   * The default value of the '{@link #getWindowPunctuationOutputMode() <em>Window Punctuation
   * Output Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationOutputMode()
   * @generated
   * @ordered
   */
  protected static final WindowPunctuationOutputModeType WINDOW_PUNCTUATION_OUTPUT_MODE_EDEFAULT =
      WindowPunctuationOutputModeType.GENERATING;

  /**
   * The cached value of the '{@link #getWindowPunctuationOutputMode() <em>Window Punctuation Output
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationOutputMode()
   * @generated
   * @ordered
   */
  protected WindowPunctuationOutputModeType windowPunctuationOutputMode =
      WINDOW_PUNCTUATION_OUTPUT_MODE_EDEFAULT;

  /**
   * This is true if the Window Punctuation Output Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowPunctuationOutputModeESet;

  /**
   * The default value of the '{@link #getWindowPunctuationInputPort() <em>Window Punctuation Input
   * Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationInputPort()
   * @generated
   * @ordered
   */
  protected static final BigInteger WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getWindowPunctuationInputPort() <em>Window Punctuation Input
   * Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationInputPort()
   * @generated
   * @ordered
   */
  protected BigInteger windowPunctuationInputPort = WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT;

  /**
   * The cached value of the '{@link #getFinalPunctuationPortScope() <em>Final Punctuation Port
   * Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFinalPunctuationPortScope()
   * @generated
   * @ordered
   */
  protected OptionalPortScopeType finalPunctuationPortScope;

  /**
   * The default value of the '{@link #isTupleMutationAllowed() <em>Tuple Mutation Allowed</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isTupleMutationAllowed()
   * @generated
   * @ordered
   */
  protected static final boolean TUPLE_MUTATION_ALLOWED_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isTupleMutationAllowed() <em>Tuple Mutation Allowed</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isTupleMutationAllowed()
   * @generated
   * @ordered
   */
  protected boolean tupleMutationAllowed = TUPLE_MUTATION_ALLOWED_EDEFAULT;

  /**
   * This is true if the Tuple Mutation Allowed attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean tupleMutationAllowedESet;

  /**
   * The cached value of the '{@link #getOutputAssignmentPortScope() <em>Output Assignment Port
   * Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputAssignmentPortScope()
   * @generated
   * @ordered
   */
  protected PortScopeType outputAssignmentPortScope;

  /**
   * The default value of the '{@link #isAllowNestedCustomOutputFunctions() <em>Allow Nested Custom
   * Output Functions</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowNestedCustomOutputFunctions()
   * @generated
   * @ordered
   */
  protected static final boolean ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isAllowNestedCustomOutputFunctions() <em>Allow Nested Custom
   * Output Functions</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowNestedCustomOutputFunctions()
   * @generated
   * @ordered
   */
  protected boolean allowNestedCustomOutputFunctions =
      ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS_EDEFAULT;

  /**
   * This is true if the Allow Nested Custom Output Functions attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean allowNestedCustomOutputFunctionsESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OutputPortOpenSetTypeImpl() {
    super();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  protected EClass eStaticClass() {
    return OperatorPackage.Literals.OUTPUT_PORT_OPEN_SET_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DescriptionType getDescription() {
    return description;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetDescription(
      DescriptionType newDescription, NotificationChain msgs) {
    DescriptionType oldDescription = description;
    description = newDescription;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
              oldDescription,
              newDescription);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setDescription(DescriptionType newDescription) {
    if (newDescription != description) {
      NotificationChain msgs = null;
      if (description != null)
        msgs =
            ((InternalEObject) description)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
              newDescription,
              newDescription));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setExpressionMode(ExpressionModeType newExpressionMode) {
    ExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode == null ? EXPRESSION_MODE_EDEFAULT : newExpressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE,
              oldExpressionMode,
              expressionMode,
              !oldExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetExpressionMode() {
    ExpressionModeType oldExpressionMode = expressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionMode = EXPRESSION_MODE_EDEFAULT;
    expressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE,
              oldExpressionMode,
              EXPRESSION_MODE_EDEFAULT,
              oldExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetExpressionMode() {
    return expressionModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isAutoAssignment() {
    return autoAssignment;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAutoAssignment(boolean newAutoAssignment) {
    boolean oldAutoAssignment = autoAssignment;
    autoAssignment = newAutoAssignment;
    boolean oldAutoAssignmentESet = autoAssignmentESet;
    autoAssignmentESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT,
              oldAutoAssignment,
              autoAssignment,
              !oldAutoAssignmentESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetAutoAssignment() {
    boolean oldAutoAssignment = autoAssignment;
    boolean oldAutoAssignmentESet = autoAssignmentESet;
    autoAssignment = AUTO_ASSIGNMENT_EDEFAULT;
    autoAssignmentESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT,
              oldAutoAssignment,
              AUTO_ASSIGNMENT_EDEFAULT,
              oldAutoAssignmentESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetAutoAssignment() {
    return autoAssignmentESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isCompleteAssignment() {
    return completeAssignment;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCompleteAssignment(boolean newCompleteAssignment) {
    boolean oldCompleteAssignment = completeAssignment;
    completeAssignment = newCompleteAssignment;
    boolean oldCompleteAssignmentESet = completeAssignmentESet;
    completeAssignmentESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT,
              oldCompleteAssignment,
              completeAssignment,
              !oldCompleteAssignmentESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetCompleteAssignment() {
    boolean oldCompleteAssignment = completeAssignment;
    boolean oldCompleteAssignmentESet = completeAssignmentESet;
    completeAssignment = COMPLETE_ASSIGNMENT_EDEFAULT;
    completeAssignmentESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT,
              oldCompleteAssignment,
              COMPLETE_ASSIGNMENT_EDEFAULT,
              oldCompleteAssignmentESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetCompleteAssignment() {
    return completeAssignmentESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isRewriteAllowed() {
    return rewriteAllowed;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRewriteAllowed(boolean newRewriteAllowed) {
    boolean oldRewriteAllowed = rewriteAllowed;
    rewriteAllowed = newRewriteAllowed;
    boolean oldRewriteAllowedESet = rewriteAllowedESet;
    rewriteAllowedESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED,
              oldRewriteAllowed,
              rewriteAllowed,
              !oldRewriteAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetRewriteAllowed() {
    boolean oldRewriteAllowed = rewriteAllowed;
    boolean oldRewriteAllowedESet = rewriteAllowedESet;
    rewriteAllowed = REWRITE_ALLOWED_EDEFAULT;
    rewriteAllowedESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED,
              oldRewriteAllowed,
              REWRITE_ALLOWED_EDEFAULT,
              oldRewriteAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetRewriteAllowed() {
    return rewriteAllowedESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputFunctionsType getOutputFunctions() {
    return outputFunctions;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputFunctions(
      OutputFunctionsType newOutputFunctions, NotificationChain msgs) {
    OutputFunctionsType oldOutputFunctions = outputFunctions;
    outputFunctions = newOutputFunctions;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS,
              oldOutputFunctions,
              newOutputFunctions);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOutputFunctions(OutputFunctionsType newOutputFunctions) {
    if (newOutputFunctions != outputFunctions) {
      NotificationChain msgs = null;
      if (outputFunctions != null)
        msgs =
            ((InternalEObject) outputFunctions)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS,
                    null,
                    msgs);
      if (newOutputFunctions != null)
        msgs =
            ((InternalEObject) newOutputFunctions)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS,
                    null,
                    msgs);
      msgs = basicSetOutputFunctions(newOutputFunctions, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS,
              newOutputFunctions,
              newOutputFunctions));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPunctuationOutputModeType getWindowPunctuationOutputMode() {
    return windowPunctuationOutputMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPunctuationOutputMode(
      WindowPunctuationOutputModeType newWindowPunctuationOutputMode) {
    WindowPunctuationOutputModeType oldWindowPunctuationOutputMode = windowPunctuationOutputMode;
    windowPunctuationOutputMode =
        newWindowPunctuationOutputMode == null
            ? WINDOW_PUNCTUATION_OUTPUT_MODE_EDEFAULT
            : newWindowPunctuationOutputMode;
    boolean oldWindowPunctuationOutputModeESet = windowPunctuationOutputModeESet;
    windowPunctuationOutputModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE,
              oldWindowPunctuationOutputMode,
              windowPunctuationOutputMode,
              !oldWindowPunctuationOutputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowPunctuationOutputMode() {
    WindowPunctuationOutputModeType oldWindowPunctuationOutputMode = windowPunctuationOutputMode;
    boolean oldWindowPunctuationOutputModeESet = windowPunctuationOutputModeESet;
    windowPunctuationOutputMode = WINDOW_PUNCTUATION_OUTPUT_MODE_EDEFAULT;
    windowPunctuationOutputModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE,
              oldWindowPunctuationOutputMode,
              WINDOW_PUNCTUATION_OUTPUT_MODE_EDEFAULT,
              oldWindowPunctuationOutputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowPunctuationOutputMode() {
    return windowPunctuationOutputModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getWindowPunctuationInputPort() {
    return windowPunctuationInputPort;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPunctuationInputPort(BigInteger newWindowPunctuationInputPort) {
    BigInteger oldWindowPunctuationInputPort = windowPunctuationInputPort;
    windowPunctuationInputPort = newWindowPunctuationInputPort;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT,
              oldWindowPunctuationInputPort,
              windowPunctuationInputPort));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OptionalPortScopeType getFinalPunctuationPortScope() {
    return finalPunctuationPortScope;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetFinalPunctuationPortScope(
      OptionalPortScopeType newFinalPunctuationPortScope, NotificationChain msgs) {
    OptionalPortScopeType oldFinalPunctuationPortScope = finalPunctuationPortScope;
    finalPunctuationPortScope = newFinalPunctuationPortScope;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
              oldFinalPunctuationPortScope,
              newFinalPunctuationPortScope);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setFinalPunctuationPortScope(OptionalPortScopeType newFinalPunctuationPortScope) {
    if (newFinalPunctuationPortScope != finalPunctuationPortScope) {
      NotificationChain msgs = null;
      if (finalPunctuationPortScope != null)
        msgs =
            ((InternalEObject) finalPunctuationPortScope)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
                    null,
                    msgs);
      if (newFinalPunctuationPortScope != null)
        msgs =
            ((InternalEObject) newFinalPunctuationPortScope)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
                    null,
                    msgs);
      msgs = basicSetFinalPunctuationPortScope(newFinalPunctuationPortScope, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
              newFinalPunctuationPortScope,
              newFinalPunctuationPortScope));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isTupleMutationAllowed() {
    return tupleMutationAllowed;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setTupleMutationAllowed(boolean newTupleMutationAllowed) {
    boolean oldTupleMutationAllowed = tupleMutationAllowed;
    tupleMutationAllowed = newTupleMutationAllowed;
    boolean oldTupleMutationAllowedESet = tupleMutationAllowedESet;
    tupleMutationAllowedESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED,
              oldTupleMutationAllowed,
              tupleMutationAllowed,
              !oldTupleMutationAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetTupleMutationAllowed() {
    boolean oldTupleMutationAllowed = tupleMutationAllowed;
    boolean oldTupleMutationAllowedESet = tupleMutationAllowedESet;
    tupleMutationAllowed = TUPLE_MUTATION_ALLOWED_EDEFAULT;
    tupleMutationAllowedESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED,
              oldTupleMutationAllowed,
              TUPLE_MUTATION_ALLOWED_EDEFAULT,
              oldTupleMutationAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetTupleMutationAllowed() {
    return tupleMutationAllowedESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PortScopeType getOutputAssignmentPortScope() {
    return outputAssignmentPortScope;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputAssignmentPortScope(
      PortScopeType newOutputAssignmentPortScope, NotificationChain msgs) {
    PortScopeType oldOutputAssignmentPortScope = outputAssignmentPortScope;
    outputAssignmentPortScope = newOutputAssignmentPortScope;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE,
              oldOutputAssignmentPortScope,
              newOutputAssignmentPortScope);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOutputAssignmentPortScope(PortScopeType newOutputAssignmentPortScope) {
    if (newOutputAssignmentPortScope != outputAssignmentPortScope) {
      NotificationChain msgs = null;
      if (outputAssignmentPortScope != null)
        msgs =
            ((InternalEObject) outputAssignmentPortScope)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE,
                    null,
                    msgs);
      if (newOutputAssignmentPortScope != null)
        msgs =
            ((InternalEObject) newOutputAssignmentPortScope)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE,
                    null,
                    msgs);
      msgs = basicSetOutputAssignmentPortScope(newOutputAssignmentPortScope, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE,
              newOutputAssignmentPortScope,
              newOutputAssignmentPortScope));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isAllowNestedCustomOutputFunctions() {
    return allowNestedCustomOutputFunctions;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAllowNestedCustomOutputFunctions(boolean newAllowNestedCustomOutputFunctions) {
    boolean oldAllowNestedCustomOutputFunctions = allowNestedCustomOutputFunctions;
    allowNestedCustomOutputFunctions = newAllowNestedCustomOutputFunctions;
    boolean oldAllowNestedCustomOutputFunctionsESet = allowNestedCustomOutputFunctionsESet;
    allowNestedCustomOutputFunctionsESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS,
              oldAllowNestedCustomOutputFunctions,
              allowNestedCustomOutputFunctions,
              !oldAllowNestedCustomOutputFunctionsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetAllowNestedCustomOutputFunctions() {
    boolean oldAllowNestedCustomOutputFunctions = allowNestedCustomOutputFunctions;
    boolean oldAllowNestedCustomOutputFunctionsESet = allowNestedCustomOutputFunctionsESet;
    allowNestedCustomOutputFunctions = ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS_EDEFAULT;
    allowNestedCustomOutputFunctionsESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS,
              oldAllowNestedCustomOutputFunctions,
              ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS_EDEFAULT,
              oldAllowNestedCustomOutputFunctionsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetAllowNestedCustomOutputFunctions() {
    return allowNestedCustomOutputFunctionsESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS:
        return basicSetOutputFunctions(null, msgs);
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return basicSetFinalPunctuationPortScope(null, msgs);
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE:
        return basicSetOutputAssignmentPortScope(null, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType) {
    switch (featureID) {
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT:
        return isAutoAssignment();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT:
        return isCompleteAssignment();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED:
        return isRewriteAllowed();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS:
        return getOutputFunctions();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        return getWindowPunctuationOutputMode();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        return getWindowPunctuationInputPort();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return getFinalPunctuationPortScope();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        return isTupleMutationAllowed();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE:
        return getOutputAssignmentPortScope();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS:
        return isAllowNestedCustomOutputFunctions();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE:
        setExpressionMode((ExpressionModeType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT:
        setAutoAssignment((Boolean) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT:
        setCompleteAssignment((Boolean) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED:
        setRewriteAllowed((Boolean) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS:
        setOutputFunctions((OutputFunctionsType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        setWindowPunctuationOutputMode((WindowPunctuationOutputModeType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        setWindowPunctuationInputPort((BigInteger) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        setFinalPunctuationPortScope((OptionalPortScopeType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        setTupleMutationAllowed((Boolean) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE:
        setOutputAssignmentPortScope((PortScopeType) newValue);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS:
        setAllowNestedCustomOutputFunctions((Boolean) newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public void eUnset(int featureID) {
    switch (featureID) {
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE:
        unsetExpressionMode();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT:
        unsetAutoAssignment();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT:
        unsetCompleteAssignment();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED:
        unsetRewriteAllowed();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS:
        setOutputFunctions((OutputFunctionsType) null);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        unsetWindowPunctuationOutputMode();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        setWindowPunctuationInputPort(WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        setFinalPunctuationPortScope((OptionalPortScopeType) null);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        unsetTupleMutationAllowed();
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE:
        setOutputAssignmentPortScope((PortScopeType) null);
        return;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS:
        unsetAllowNestedCustomOutputFunctions();
        return;
    }
    super.eUnset(featureID);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID) {
    switch (featureID) {
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE:
        return isSetExpressionMode();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT:
        return isSetAutoAssignment();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT:
        return isSetCompleteAssignment();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED:
        return isSetRewriteAllowed();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS:
        return outputFunctions != null;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        return isSetWindowPunctuationOutputMode();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        return WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT == null
            ? windowPunctuationInputPort != null
            : !WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT.equals(windowPunctuationInputPort);
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return finalPunctuationPortScope != null;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        return isSetTupleMutationAllowed();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE:
        return outputAssignmentPortScope != null;
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS:
        return isSetAllowNestedCustomOutputFunctions();
    }
    return super.eIsSet(featureID);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public String toString() {
    if (eIsProxy()) return super.toString();

    StringBuffer result = new StringBuffer(super.toString());
    result.append(" (expressionMode: ");
    if (expressionModeESet) result.append(expressionMode);
    else result.append("<unset>");
    result.append(", autoAssignment: ");
    if (autoAssignmentESet) result.append(autoAssignment);
    else result.append("<unset>");
    result.append(", completeAssignment: ");
    if (completeAssignmentESet) result.append(completeAssignment);
    else result.append("<unset>");
    result.append(", rewriteAllowed: ");
    if (rewriteAllowedESet) result.append(rewriteAllowed);
    else result.append("<unset>");
    result.append(", windowPunctuationOutputMode: ");
    if (windowPunctuationOutputModeESet) result.append(windowPunctuationOutputMode);
    else result.append("<unset>");
    result.append(", windowPunctuationInputPort: ");
    result.append(windowPunctuationInputPort);
    result.append(", tupleMutationAllowed: ");
    if (tupleMutationAllowedESet) result.append(tupleMutationAllowed);
    else result.append("<unset>");
    result.append(", allowNestedCustomOutputFunctions: ");
    if (allowNestedCustomOutputFunctionsESet) result.append(allowNestedCustomOutputFunctions);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // OutputPortOpenSetTypeImpl
