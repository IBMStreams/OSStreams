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
package com.ibm.xmlns.prod.streams.spl.source.code.impl;

import com.ibm.xmlns.prod.streams.spl.source.code.CodePackage;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Parameter Expression Mode Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl#getMode
 *       <em>Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl#getType
 *       <em>Type</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeParameterExpressionModeTypeImpl extends EObjectImpl
    implements CompositeParameterExpressionModeType {
  /**
   * The default value of the '{@link #getMode() <em>Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMode()
   * @generated
   * @ordered
   */
  protected static final CompositeParameterExpressionModeEnumType MODE_EDEFAULT =
      CompositeParameterExpressionModeEnumType.OPERATOR;

  /**
   * The cached value of the '{@link #getMode() <em>Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMode()
   * @generated
   * @ordered
   */
  protected CompositeParameterExpressionModeEnumType mode = MODE_EDEFAULT;

  /**
   * This is true if the Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean modeESet;

  /**
   * The default value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected static final String TYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected String type = TYPE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeParameterExpressionModeTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterExpressionModeEnumType getMode() {
    return mode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setMode(CompositeParameterExpressionModeEnumType newMode) {
    CompositeParameterExpressionModeEnumType oldMode = mode;
    mode = newMode == null ? MODE_EDEFAULT : newMode;
    boolean oldModeESet = modeESet;
    modeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE,
              oldMode,
              mode,
              !oldModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetMode() {
    CompositeParameterExpressionModeEnumType oldMode = mode;
    boolean oldModeESet = modeESet;
    mode = MODE_EDEFAULT;
    modeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE,
              oldMode,
              MODE_EDEFAULT,
              oldModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetMode() {
    return modeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getType() {
    return type;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setType(String newType) {
    String oldType = type;
    type = newType;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE,
              oldType,
              type));
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
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE:
        return getMode();
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE:
        return getType();
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
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE:
        setMode((CompositeParameterExpressionModeEnumType) newValue);
        return;
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE:
        setType((String) newValue);
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
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE:
        unsetMode();
        return;
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE:
        setType(TYPE_EDEFAULT);
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
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE:
        return isSetMode();
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE:
        return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT.equals(type);
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
    result.append(" (mode: ");
    if (modeESet) result.append(mode);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(')');
    return result.toString();
  }
} // CompositeParameterExpressionModeTypeImpl
