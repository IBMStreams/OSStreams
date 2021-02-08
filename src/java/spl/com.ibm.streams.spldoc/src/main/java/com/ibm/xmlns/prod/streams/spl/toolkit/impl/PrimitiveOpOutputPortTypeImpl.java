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

import com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Primitive Op Output Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl#getOutputFunctionsType
 *       <em>Output Functions Type</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl#getWindowPunctOutputMode
 *       <em>Window Punct Output Mode</em>}
 * </ul>
 *
 * @generated
 */
public class PrimitiveOpOutputPortTypeImpl extends PrimitivePortDescriptionTypeImpl
    implements PrimitiveOpOutputPortType {
  /**
   * The cached value of the '{@link #getOutputFunctionsType() <em>Output Functions Type</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputFunctionsType()
   * @generated
   * @ordered
   */
  protected OutputFunctionsTypeType outputFunctionsType;

  /**
   * The default value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected static final EnumExpressionModeType EXPRESSION_MODE_EDEFAULT =
      EnumExpressionModeType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected EnumExpressionModeType expressionMode = EXPRESSION_MODE_EDEFAULT;

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
   * The default value of the '{@link #getWindowPunctOutputMode() <em>Window Punct Output
   * Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctOutputMode()
   * @generated
   * @ordered
   */
  protected static final EnumWindowPunctOutputModeType WINDOW_PUNCT_OUTPUT_MODE_EDEFAULT =
      EnumWindowPunctOutputModeType.GENERATING;

  /**
   * The cached value of the '{@link #getWindowPunctOutputMode() <em>Window Punct Output Mode</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowPunctOutputMode()
   * @generated
   * @ordered
   */
  protected EnumWindowPunctOutputModeType windowPunctOutputMode = WINDOW_PUNCT_OUTPUT_MODE_EDEFAULT;

  /**
   * This is true if the Window Punct Output Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowPunctOutputModeESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected PrimitiveOpOutputPortTypeImpl() {
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
    return ToolkitPackage.Literals.PRIMITIVE_OP_OUTPUT_PORT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputFunctionsTypeType getOutputFunctionsType() {
    return outputFunctionsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputFunctionsType(
      OutputFunctionsTypeType newOutputFunctionsType, NotificationChain msgs) {
    OutputFunctionsTypeType oldOutputFunctionsType = outputFunctionsType;
    outputFunctionsType = newOutputFunctionsType;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE,
              oldOutputFunctionsType,
              newOutputFunctionsType);
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
  public void setOutputFunctionsType(OutputFunctionsTypeType newOutputFunctionsType) {
    if (newOutputFunctionsType != outputFunctionsType) {
      NotificationChain msgs = null;
      if (outputFunctionsType != null)
        msgs =
            ((InternalEObject) outputFunctionsType)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE,
                    null,
                    msgs);
      if (newOutputFunctionsType != null)
        msgs =
            ((InternalEObject) newOutputFunctionsType)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE,
                    null,
                    msgs);
      msgs = basicSetOutputFunctionsType(newOutputFunctionsType, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE,
              newOutputFunctionsType,
              newOutputFunctionsType));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setExpressionMode(EnumExpressionModeType newExpressionMode) {
    EnumExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode == null ? EXPRESSION_MODE_EDEFAULT : newExpressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE,
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
    EnumExpressionModeType oldExpressionMode = expressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionMode = EXPRESSION_MODE_EDEFAULT;
    expressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE,
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
  public EnumWindowPunctOutputModeType getWindowPunctOutputMode() {
    return windowPunctOutputMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowPunctOutputMode(EnumWindowPunctOutputModeType newWindowPunctOutputMode) {
    EnumWindowPunctOutputModeType oldWindowPunctOutputMode = windowPunctOutputMode;
    windowPunctOutputMode =
        newWindowPunctOutputMode == null
            ? WINDOW_PUNCT_OUTPUT_MODE_EDEFAULT
            : newWindowPunctOutputMode;
    boolean oldWindowPunctOutputModeESet = windowPunctOutputModeESet;
    windowPunctOutputModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE,
              oldWindowPunctOutputMode,
              windowPunctOutputMode,
              !oldWindowPunctOutputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowPunctOutputMode() {
    EnumWindowPunctOutputModeType oldWindowPunctOutputMode = windowPunctOutputMode;
    boolean oldWindowPunctOutputModeESet = windowPunctOutputModeESet;
    windowPunctOutputMode = WINDOW_PUNCT_OUTPUT_MODE_EDEFAULT;
    windowPunctOutputModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE,
              oldWindowPunctOutputMode,
              WINDOW_PUNCT_OUTPUT_MODE_EDEFAULT,
              oldWindowPunctOutputModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowPunctOutputMode() {
    return windowPunctOutputModeESet;
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
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE:
        return basicSetOutputFunctionsType(null, msgs);
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
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE:
        return getOutputFunctionsType();
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE:
        return getWindowPunctOutputMode();
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
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE:
        setOutputFunctionsType((OutputFunctionsTypeType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE:
        setExpressionMode((EnumExpressionModeType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE:
        setWindowPunctOutputMode((EnumWindowPunctOutputModeType) newValue);
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
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE:
        setOutputFunctionsType((OutputFunctionsTypeType) null);
        return;
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE:
        unsetExpressionMode();
        return;
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE:
        unsetWindowPunctOutputMode();
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
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE:
        return outputFunctionsType != null;
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE:
        return isSetExpressionMode();
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE:
        return isSetWindowPunctOutputMode();
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
    result.append(", windowPunctOutputMode: ");
    if (windowPunctOutputModeESet) result.append(windowPunctOutputMode);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // PrimitiveOpOutputPortTypeImpl
