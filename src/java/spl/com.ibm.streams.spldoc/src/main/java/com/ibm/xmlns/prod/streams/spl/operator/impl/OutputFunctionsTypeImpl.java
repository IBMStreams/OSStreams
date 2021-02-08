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

import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Output Functions Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl#getDefault
 *       <em>Default</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl#getType
 *       <em>Type</em>}
 * </ul>
 *
 * @generated
 */
public class OutputFunctionsTypeImpl extends MinimalEObjectImpl.Container
    implements OutputFunctionsType {
  /**
   * The default value of the '{@link #getDefault() <em>Default</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDefault()
   * @generated
   * @ordered
   */
  protected static final String DEFAULT_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getDefault() <em>Default</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDefault()
   * @generated
   * @ordered
   */
  protected String default_ = DEFAULT_EDEFAULT;

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
  protected OutputFunctionsTypeImpl() {
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
    return OperatorPackage.Literals.OUTPUT_FUNCTIONS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getDefault() {
    return default_;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setDefault(String newDefault) {
    String oldDefault = default_;
    default_ = newDefault;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OUTPUT_FUNCTIONS_TYPE__DEFAULT,
              oldDefault,
              default_));
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
              this, Notification.SET, OperatorPackage.OUTPUT_FUNCTIONS_TYPE__TYPE, oldType, type));
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
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__DEFAULT:
        return getDefault();
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__TYPE:
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
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__DEFAULT:
        setDefault((String) newValue);
        return;
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__TYPE:
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
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__DEFAULT:
        setDefault(DEFAULT_EDEFAULT);
        return;
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__TYPE:
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
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__DEFAULT:
        return DEFAULT_EDEFAULT == null ? default_ != null : !DEFAULT_EDEFAULT.equals(default_);
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE__TYPE:
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
    result.append(" (default: ");
    result.append(default_);
    result.append(", type: ");
    result.append(type);
    result.append(')');
    return result.toString();
  }
} // OutputFunctionsTypeImpl
