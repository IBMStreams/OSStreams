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
import com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Input Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#getWindowingDescription
 *       <em>Windowing Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#isTupleMutationAllowed
 *       <em>Tuple Mutation Allowed</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#getWindowingMode
 *       <em>Windowing Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#getWindowPunctuationInputMode
 *       <em>Window Punctuation Input Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#isControlPort
 *       <em>Control Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#getWindowExpressionMode
 *       <em>Window Expression Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl#isRewriteAllowedForWindowExpression
 *       <em>Rewrite Allowed For Window Expression</em>}
 * </ul>
 *
 * @generated
 */
public class InputPortOpenSetTypeImpl extends MinimalEObjectImpl.Container
    implements InputPortOpenSetType {
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
   * The cached value of the '{@link #getWindowingDescription() <em>Windowing Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowingDescription()
   * @generated
   * @ordered
   */
  protected DescriptionType windowingDescription;

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
   * The default value of the '{@link #getWindowingMode() <em>Windowing Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowingMode()
   * @generated
   * @ordered
   */
  protected static final WindowingModeType WINDOWING_MODE_EDEFAULT = WindowingModeType.NON_WINDOWED;

  /**
   * The cached value of the '{@link #getWindowingMode() <em>Windowing Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowingMode()
   * @generated
   * @ordered
   */
  protected WindowingModeType windowingMode = WINDOWING_MODE_EDEFAULT;

  /**
   * This is true if the Windowing Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowingModeESet;

  /**
   * The default value of the '{@link #getWindowPunctuationInputMode() <em>Window Punctuation Input
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationInputMode()
   * @generated
   * @ordered
   */
  protected static final WindowPunctuationInputModeType WINDOW_PUNCTUATION_INPUT_MODE_EDEFAULT =
      WindowPunctuationInputModeType.EXPECTING;

  /**
   * The cached value of the '{@link #getWindowPunctuationInputMode() <em>Window Punctuation Input
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctuationInputMode()
   * @generated
   * @ordered
   */
  protected WindowPunctuationInputModeType windowPunctuationInputMode =
      WINDOW_PUNCTUATION_INPUT_MODE_EDEFAULT;

  /**
   * This is true if the Window Punctuation Input Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowPunctuationInputModeESet;

  /**
   * The default value of the '{@link #isControlPort() <em>Control Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isControlPort()
   * @generated
   * @ordered
   */
  protected static final boolean CONTROL_PORT_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isControlPort() <em>Control Port</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isControlPort()
   * @generated
   * @ordered
   */
  protected boolean controlPort = CONTROL_PORT_EDEFAULT;

  /**
   * This is true if the Control Port attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean controlPortESet;

  /**
   * The default value of the '{@link #getWindowExpressionMode() <em>Window Expression Mode</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowExpressionMode()
   * @generated
   * @ordered
   */
  protected static final WindowExpressionModeType WINDOW_EXPRESSION_MODE_EDEFAULT =
      WindowExpressionModeType.CONSTANT;

  /**
   * The cached value of the '{@link #getWindowExpressionMode() <em>Window Expression Mode</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowExpressionMode()
   * @generated
   * @ordered
   */
  protected WindowExpressionModeType windowExpressionMode = WINDOW_EXPRESSION_MODE_EDEFAULT;

  /**
   * This is true if the Window Expression Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowExpressionModeESet;

  /**
   * The default value of the '{@link #isRewriteAllowedForWindowExpression() <em>Rewrite Allowed For
   * Window Expression</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowedForWindowExpression()
   * @generated
   * @ordered
   */
  protected static final boolean REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isRewriteAllowedForWindowExpression() <em>Rewrite Allowed For
   * Window Expression</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowedForWindowExpression()
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowedForWindowExpression =
      REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION_EDEFAULT;

  /**
   * This is true if the Rewrite Allowed For Window Expression attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowedForWindowExpressionESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected InputPortOpenSetTypeImpl() {
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
    return OperatorPackage.Literals.INPUT_PORT_OPEN_SET_TYPE;
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
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
  public DescriptionType getWindowingDescription() {
    return windowingDescription;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetWindowingDescription(
      DescriptionType newWindowingDescription, NotificationChain msgs) {
    DescriptionType oldWindowingDescription = windowingDescription;
    windowingDescription = newWindowingDescription;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
              oldWindowingDescription,
              newWindowingDescription);
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
  public void setWindowingDescription(DescriptionType newWindowingDescription) {
    if (newWindowingDescription != windowingDescription) {
      NotificationChain msgs = null;
      if (windowingDescription != null)
        msgs =
            ((InternalEObject) windowingDescription)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
                    null,
                    msgs);
      if (newWindowingDescription != null)
        msgs =
            ((InternalEObject) newWindowingDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetWindowingDescription(newWindowingDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
              newWindowingDescription,
              newWindowingDescription));
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
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED,
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
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED,
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
  public WindowingModeType getWindowingMode() {
    return windowingMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowingMode(WindowingModeType newWindowingMode) {
    WindowingModeType oldWindowingMode = windowingMode;
    windowingMode = newWindowingMode == null ? WINDOWING_MODE_EDEFAULT : newWindowingMode;
    boolean oldWindowingModeESet = windowingModeESet;
    windowingModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE,
              oldWindowingMode,
              windowingMode,
              !oldWindowingModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowingMode() {
    WindowingModeType oldWindowingMode = windowingMode;
    boolean oldWindowingModeESet = windowingModeESet;
    windowingMode = WINDOWING_MODE_EDEFAULT;
    windowingModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE,
              oldWindowingMode,
              WINDOWING_MODE_EDEFAULT,
              oldWindowingModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowingMode() {
    return windowingModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPunctuationInputModeType getWindowPunctuationInputMode() {
    return windowPunctuationInputMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPunctuationInputMode(
      WindowPunctuationInputModeType newWindowPunctuationInputMode) {
    WindowPunctuationInputModeType oldWindowPunctuationInputMode = windowPunctuationInputMode;
    windowPunctuationInputMode =
        newWindowPunctuationInputMode == null
            ? WINDOW_PUNCTUATION_INPUT_MODE_EDEFAULT
            : newWindowPunctuationInputMode;
    boolean oldWindowPunctuationInputModeESet = windowPunctuationInputModeESet;
    windowPunctuationInputModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE,
              oldWindowPunctuationInputMode,
              windowPunctuationInputMode,
              !oldWindowPunctuationInputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowPunctuationInputMode() {
    WindowPunctuationInputModeType oldWindowPunctuationInputMode = windowPunctuationInputMode;
    boolean oldWindowPunctuationInputModeESet = windowPunctuationInputModeESet;
    windowPunctuationInputMode = WINDOW_PUNCTUATION_INPUT_MODE_EDEFAULT;
    windowPunctuationInputModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE,
              oldWindowPunctuationInputMode,
              WINDOW_PUNCTUATION_INPUT_MODE_EDEFAULT,
              oldWindowPunctuationInputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowPunctuationInputMode() {
    return windowPunctuationInputModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isControlPort() {
    return controlPort;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setControlPort(boolean newControlPort) {
    boolean oldControlPort = controlPort;
    controlPort = newControlPort;
    boolean oldControlPortESet = controlPortESet;
    controlPortESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT,
              oldControlPort,
              controlPort,
              !oldControlPortESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetControlPort() {
    boolean oldControlPort = controlPort;
    boolean oldControlPortESet = controlPortESet;
    controlPort = CONTROL_PORT_EDEFAULT;
    controlPortESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT,
              oldControlPort,
              CONTROL_PORT_EDEFAULT,
              oldControlPortESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetControlPort() {
    return controlPortESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowExpressionModeType getWindowExpressionMode() {
    return windowExpressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowExpressionMode(WindowExpressionModeType newWindowExpressionMode) {
    WindowExpressionModeType oldWindowExpressionMode = windowExpressionMode;
    windowExpressionMode =
        newWindowExpressionMode == null ? WINDOW_EXPRESSION_MODE_EDEFAULT : newWindowExpressionMode;
    boolean oldWindowExpressionModeESet = windowExpressionModeESet;
    windowExpressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE,
              oldWindowExpressionMode,
              windowExpressionMode,
              !oldWindowExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowExpressionMode() {
    WindowExpressionModeType oldWindowExpressionMode = windowExpressionMode;
    boolean oldWindowExpressionModeESet = windowExpressionModeESet;
    windowExpressionMode = WINDOW_EXPRESSION_MODE_EDEFAULT;
    windowExpressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE,
              oldWindowExpressionMode,
              WINDOW_EXPRESSION_MODE_EDEFAULT,
              oldWindowExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowExpressionMode() {
    return windowExpressionModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isRewriteAllowedForWindowExpression() {
    return rewriteAllowedForWindowExpression;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRewriteAllowedForWindowExpression(boolean newRewriteAllowedForWindowExpression) {
    boolean oldRewriteAllowedForWindowExpression = rewriteAllowedForWindowExpression;
    rewriteAllowedForWindowExpression = newRewriteAllowedForWindowExpression;
    boolean oldRewriteAllowedForWindowExpressionESet = rewriteAllowedForWindowExpressionESet;
    rewriteAllowedForWindowExpressionESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION,
              oldRewriteAllowedForWindowExpression,
              rewriteAllowedForWindowExpression,
              !oldRewriteAllowedForWindowExpressionESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetRewriteAllowedForWindowExpression() {
    boolean oldRewriteAllowedForWindowExpression = rewriteAllowedForWindowExpression;
    boolean oldRewriteAllowedForWindowExpressionESet = rewriteAllowedForWindowExpressionESet;
    rewriteAllowedForWindowExpression = REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION_EDEFAULT;
    rewriteAllowedForWindowExpressionESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION,
              oldRewriteAllowedForWindowExpression,
              REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION_EDEFAULT,
              oldRewriteAllowedForWindowExpressionESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetRewriteAllowedForWindowExpression() {
    return rewriteAllowedForWindowExpressionESet;
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
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        return basicSetWindowingDescription(null, msgs);
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
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        return getWindowingDescription();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        return isTupleMutationAllowed();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        return getWindowingMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        return getWindowPunctuationInputMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        return isControlPort();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE:
        return getWindowExpressionMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION:
        return isRewriteAllowedForWindowExpression();
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
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        setWindowingDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        setTupleMutationAllowed((Boolean) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        setWindowingMode((WindowingModeType) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        setWindowPunctuationInputMode((WindowPunctuationInputModeType) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        setControlPort((Boolean) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE:
        setWindowExpressionMode((WindowExpressionModeType) newValue);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION:
        setRewriteAllowedForWindowExpression((Boolean) newValue);
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
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        setWindowingDescription((DescriptionType) null);
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        unsetTupleMutationAllowed();
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        unsetWindowingMode();
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        unsetWindowPunctuationInputMode();
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        unsetControlPort();
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE:
        unsetWindowExpressionMode();
        return;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION:
        unsetRewriteAllowedForWindowExpression();
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
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        return windowingDescription != null;
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED:
        return isSetTupleMutationAllowed();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        return isSetWindowingMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        return isSetWindowPunctuationInputMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        return isSetControlPort();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE:
        return isSetWindowExpressionMode();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION:
        return isSetRewriteAllowedForWindowExpression();
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
    result.append(" (tupleMutationAllowed: ");
    if (tupleMutationAllowedESet) result.append(tupleMutationAllowed);
    else result.append("<unset>");
    result.append(", windowingMode: ");
    if (windowingModeESet) result.append(windowingMode);
    else result.append("<unset>");
    result.append(", windowPunctuationInputMode: ");
    if (windowPunctuationInputModeESet) result.append(windowPunctuationInputMode);
    else result.append("<unset>");
    result.append(", controlPort: ");
    if (controlPortESet) result.append(controlPort);
    else result.append("<unset>");
    result.append(", windowExpressionMode: ");
    if (windowExpressionModeESet) result.append(windowExpressionMode);
    else result.append("<unset>");
    result.append(", rewriteAllowedForWindowExpression: ");
    if (rewriteAllowedForWindowExpressionESet) result.append(rewriteAllowedForWindowExpression);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // InputPortOpenSetTypeImpl
