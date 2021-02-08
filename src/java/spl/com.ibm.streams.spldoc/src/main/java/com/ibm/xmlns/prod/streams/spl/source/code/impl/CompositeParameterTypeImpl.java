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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl#getDefaultValue
 *       <em>Default Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl#getName
 *       <em>Name</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeParameterTypeImpl extends SourceLocationTypeImpl
    implements CompositeParameterType {
  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected CompositeParameterExpressionModeType expressionMode;

  /**
   * The default value of the '{@link #getDefaultValue() <em>Default Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDefaultValue()
   * @generated
   * @ordered
   */
  protected static final String DEFAULT_VALUE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getDefaultValue() <em>Default Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDefaultValue()
   * @generated
   * @ordered
   */
  protected String defaultValue = DEFAULT_VALUE_EDEFAULT;

  /**
   * The default value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected static final String NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected String name = NAME_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeParameterTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_PARAMETER_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetExpressionMode(
      CompositeParameterExpressionModeType newExpressionMode, NotificationChain msgs) {
    CompositeParameterExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE,
              oldExpressionMode,
              newExpressionMode);
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
  public void setExpressionMode(CompositeParameterExpressionModeType newExpressionMode) {
    if (newExpressionMode != expressionMode) {
      NotificationChain msgs = null;
      if (expressionMode != null)
        msgs =
            ((InternalEObject) expressionMode)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE,
                    null,
                    msgs);
      if (newExpressionMode != null)
        msgs =
            ((InternalEObject) newExpressionMode)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE,
                    null,
                    msgs);
      msgs = basicSetExpressionMode(newExpressionMode, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE,
              newExpressionMode,
              newExpressionMode));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getDefaultValue() {
    return defaultValue;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setDefaultValue(String newDefaultValue) {
    String oldDefaultValue = defaultValue;
    defaultValue = newDefaultValue;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE,
              oldDefaultValue,
              defaultValue));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getName() {
    return name;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setName(String newName) {
    String oldName = name;
    name = newName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_PARAMETER_TYPE__NAME, oldName, name));
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
      case CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE:
        return basicSetExpressionMode(null, msgs);
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
      case CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case CodePackage.COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE:
        return getDefaultValue();
      case CodePackage.COMPOSITE_PARAMETER_TYPE__NAME:
        return getName();
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
      case CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE:
        setExpressionMode((CompositeParameterExpressionModeType) newValue);
        return;
      case CodePackage.COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE:
        setDefaultValue((String) newValue);
        return;
      case CodePackage.COMPOSITE_PARAMETER_TYPE__NAME:
        setName((String) newValue);
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
      case CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE:
        setExpressionMode((CompositeParameterExpressionModeType) null);
        return;
      case CodePackage.COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE:
        setDefaultValue(DEFAULT_VALUE_EDEFAULT);
        return;
      case CodePackage.COMPOSITE_PARAMETER_TYPE__NAME:
        setName(NAME_EDEFAULT);
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
      case CodePackage.COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE:
        return expressionMode != null;
      case CodePackage.COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE:
        return DEFAULT_VALUE_EDEFAULT == null
            ? defaultValue != null
            : !DEFAULT_VALUE_EDEFAULT.equals(defaultValue);
      case CodePackage.COMPOSITE_PARAMETER_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
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
    result.append(" (defaultValue: ");
    result.append(defaultValue);
    result.append(", name: ");
    result.append(name);
    result.append(')');
    return result.toString();
  }
} // CompositeParameterTypeImpl
