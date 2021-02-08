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
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType;
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
 * An implementation of the model object '<em><b>Java Op Output Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl#getWindowPunctuationOutputMode
 *       <em>Window Punctuation Output Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl#getWindowPunctuationInputPort
 *       <em>Window Punctuation Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl#getFinalPunctuationPortScope
 *       <em>Final Punctuation Port Scope</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpOutputPortOpenSetTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpOutputPortOpenSetType {
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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpOutputPortOpenSetTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE;
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
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE,
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
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE,
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
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT,
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
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
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
                        - OperatorPackage
                            .JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
                    null,
                    msgs);
      if (newFinalPunctuationPortScope != null)
        msgs =
            ((InternalEObject) newFinalPunctuationPortScope)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage
                            .JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
                    null,
                    msgs);
      msgs = basicSetFinalPunctuationPortScope(newFinalPunctuationPortScope, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE,
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
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return basicSetFinalPunctuationPortScope(null, msgs);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        return getWindowPunctuationOutputMode();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        return getWindowPunctuationInputPort();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return getFinalPunctuationPortScope();
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        setWindowPunctuationOutputMode((WindowPunctuationOutputModeType) newValue);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        setWindowPunctuationInputPort((BigInteger) newValue);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        setFinalPunctuationPortScope((OptionalPortScopeType) newValue);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        unsetWindowPunctuationOutputMode();
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        setWindowPunctuationInputPort(WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        setFinalPunctuationPortScope((OptionalPortScopeType) null);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE:
        return isSetWindowPunctuationOutputMode();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT:
        return WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT == null
            ? windowPunctuationInputPort != null
            : !WINDOW_PUNCTUATION_INPUT_PORT_EDEFAULT.equals(windowPunctuationInputPort);
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE:
        return finalPunctuationPortScope != null;
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
    result.append(" (windowPunctuationOutputMode: ");
    if (windowPunctuationOutputModeESet) result.append(windowPunctuationOutputMode);
    else result.append("<unset>");
    result.append(", windowPunctuationInputPort: ");
    result.append(windowPunctuationInputPort);
    result.append(')');
    return result.toString();
  }
} // JavaOpOutputPortOpenSetTypeImpl
