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
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
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
 * An implementation of the model object '<em><b>Java Op Input Port Open Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl#getWindowingDescription
 *       <em>Windowing Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl#getWindowingMode
 *       <em>Windowing Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl#getWindowPunctuationInputMode
 *       <em>Window Punctuation Input Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl#isControlPort
 *       <em>Control Port</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpInputPortOpenSetTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpInputPortOpenSetType {
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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpInputPortOpenSetTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE;
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
                        - OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
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
                        - OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
                    null,
                    msgs);
      if (newWindowingDescription != null)
        msgs =
            ((InternalEObject) newWindowingDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetWindowingDescription(newWindowingDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT,
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
              OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT,
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
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
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
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        return getWindowingDescription();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        return getWindowingMode();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        return getWindowPunctuationInputMode();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        return isControlPort();
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
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        setWindowingDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        setWindowingMode((WindowingModeType) newValue);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        setWindowPunctuationInputMode((WindowPunctuationInputModeType) newValue);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        setControlPort((Boolean) newValue);
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
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        setWindowingDescription((DescriptionType) null);
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        unsetWindowingMode();
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        unsetWindowPunctuationInputMode();
        return;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        unsetControlPort();
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
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION:
        return windowingDescription != null;
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE:
        return isSetWindowingMode();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE:
        return isSetWindowPunctuationInputMode();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT:
        return isSetControlPort();
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
    result.append(" (windowingMode: ");
    if (windowingModeESet) result.append(windowingMode);
    else result.append("<unset>");
    result.append(", windowPunctuationInputMode: ");
    if (windowPunctuationInputModeESet) result.append(windowPunctuationInputMode);
    else result.append("<unset>");
    result.append(", controlPort: ");
    if (controlPortESet) result.append(controlPort);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // JavaOpInputPortOpenSetTypeImpl
