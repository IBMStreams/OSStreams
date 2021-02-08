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
package com.ibm.xmlns.prod.streams.spl.toolkit.impl;

import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Primitive Op Input Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl#getWindowPolicy
 *       <em>Window Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl#getWindowPunctInputMode
 *       <em>Window Punct Input Mode</em>}
 * </ul>
 *
 * @generated
 */
public class PrimitiveOpInputPortTypeImpl extends PrimitivePortDescriptionTypeImpl
    implements PrimitiveOpInputPortType {
  /**
   * The default value of the '{@link #getWindowPolicy() <em>Window Policy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPolicy()
   * @generated
   * @ordered
   */
  protected static final EnumWindowPolicyType WINDOW_POLICY_EDEFAULT =
      EnumWindowPolicyType.NON_WINDOWED;

  /**
   * The cached value of the '{@link #getWindowPolicy() <em>Window Policy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPolicy()
   * @generated
   * @ordered
   */
  protected EnumWindowPolicyType windowPolicy = WINDOW_POLICY_EDEFAULT;

  /**
   * This is true if the Window Policy attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowPolicyESet;

  /**
   * The default value of the '{@link #getWindowPunctInputMode() <em>Window Punct Input Mode</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctInputMode()
   * @generated
   * @ordered
   */
  protected static final EnumWindowPunctInputModeType WINDOW_PUNCT_INPUT_MODE_EDEFAULT =
      EnumWindowPunctInputModeType.EXPECTING;

  /**
   * The cached value of the '{@link #getWindowPunctInputMode() <em>Window Punct Input Mode</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctInputMode()
   * @generated
   * @ordered
   */
  protected EnumWindowPunctInputModeType windowPunctInputMode = WINDOW_PUNCT_INPUT_MODE_EDEFAULT;

  /**
   * This is true if the Window Punct Input Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowPunctInputModeESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected PrimitiveOpInputPortTypeImpl() {
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
    return ToolkitPackage.Literals.PRIMITIVE_OP_INPUT_PORT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPolicyType getWindowPolicy() {
    return windowPolicy;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPolicy(EnumWindowPolicyType newWindowPolicy) {
    EnumWindowPolicyType oldWindowPolicy = windowPolicy;
    windowPolicy = newWindowPolicy == null ? WINDOW_POLICY_EDEFAULT : newWindowPolicy;
    boolean oldWindowPolicyESet = windowPolicyESet;
    windowPolicyESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY,
              oldWindowPolicy,
              windowPolicy,
              !oldWindowPolicyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowPolicy() {
    EnumWindowPolicyType oldWindowPolicy = windowPolicy;
    boolean oldWindowPolicyESet = windowPolicyESet;
    windowPolicy = WINDOW_POLICY_EDEFAULT;
    windowPolicyESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY,
              oldWindowPolicy,
              WINDOW_POLICY_EDEFAULT,
              oldWindowPolicyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowPolicy() {
    return windowPolicyESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPunctInputModeType getWindowPunctInputMode() {
    return windowPunctInputMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPunctInputMode(EnumWindowPunctInputModeType newWindowPunctInputMode) {
    EnumWindowPunctInputModeType oldWindowPunctInputMode = windowPunctInputMode;
    windowPunctInputMode =
        newWindowPunctInputMode == null
            ? WINDOW_PUNCT_INPUT_MODE_EDEFAULT
            : newWindowPunctInputMode;
    boolean oldWindowPunctInputModeESet = windowPunctInputModeESet;
    windowPunctInputModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE,
              oldWindowPunctInputMode,
              windowPunctInputMode,
              !oldWindowPunctInputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowPunctInputMode() {
    EnumWindowPunctInputModeType oldWindowPunctInputMode = windowPunctInputMode;
    boolean oldWindowPunctInputModeESet = windowPunctInputModeESet;
    windowPunctInputMode = WINDOW_PUNCT_INPUT_MODE_EDEFAULT;
    windowPunctInputModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE,
              oldWindowPunctInputMode,
              WINDOW_PUNCT_INPUT_MODE_EDEFAULT,
              oldWindowPunctInputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowPunctInputMode() {
    return windowPunctInputModeESet;
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
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY:
        return getWindowPolicy();
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE:
        return getWindowPunctInputMode();
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
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY:
        setWindowPolicy((EnumWindowPolicyType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE:
        setWindowPunctInputMode((EnumWindowPunctInputModeType) newValue);
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
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY:
        unsetWindowPolicy();
        return;
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE:
        unsetWindowPunctInputMode();
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
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY:
        return isSetWindowPolicy();
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE:
        return isSetWindowPunctInputMode();
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
    result.append(" (windowPolicy: ");
    if (windowPolicyESet) result.append(windowPolicy);
    else result.append("<unset>");
    result.append(", windowPunctInputMode: ");
    if (windowPunctInputModeESet) result.append(windowPunctInputMode);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // PrimitiveOpInputPortTypeImpl
