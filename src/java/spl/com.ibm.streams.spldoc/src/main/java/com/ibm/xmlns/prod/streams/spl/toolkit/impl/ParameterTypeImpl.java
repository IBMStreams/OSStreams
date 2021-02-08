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

import com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl#getDefaultValue
 *       <em>Default Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl#isMutable
 *       <em>Mutable</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl#getType <em>Type</em>}
 * </ul>
 *
 * @generated
 */
public class ParameterTypeImpl extends ToolkitElementTypeImpl implements ParameterType {
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
   * The default value of the '{@link #isMutable() <em>Mutable</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isMutable()
   * @generated
   * @ordered
   */
  protected static final boolean MUTABLE_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isMutable() <em>Mutable</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isMutable()
   * @generated
   * @ordered
   */
  protected boolean mutable = MUTABLE_EDEFAULT;

  /**
   * This is true if the Mutable attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean mutableESet;

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
  protected ParameterTypeImpl() {
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
    return ToolkitPackage.Literals.PARAMETER_TYPE;
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
              ToolkitPackage.PARAMETER_TYPE__DEFAULT_VALUE,
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
  public boolean isMutable() {
    return mutable;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setMutable(boolean newMutable) {
    boolean oldMutable = mutable;
    mutable = newMutable;
    boolean oldMutableESet = mutableESet;
    mutableESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PARAMETER_TYPE__MUTABLE,
              oldMutable,
              mutable,
              !oldMutableESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetMutable() {
    boolean oldMutable = mutable;
    boolean oldMutableESet = mutableESet;
    mutable = MUTABLE_EDEFAULT;
    mutableESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PARAMETER_TYPE__MUTABLE,
              oldMutable,
              MUTABLE_EDEFAULT,
              oldMutableESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetMutable() {
    return mutableESet;
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
              this, Notification.SET, ToolkitPackage.PARAMETER_TYPE__TYPE, oldType, type));
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
      case ToolkitPackage.PARAMETER_TYPE__DEFAULT_VALUE:
        return getDefaultValue();
      case ToolkitPackage.PARAMETER_TYPE__MUTABLE:
        return isMutable();
      case ToolkitPackage.PARAMETER_TYPE__TYPE:
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
      case ToolkitPackage.PARAMETER_TYPE__DEFAULT_VALUE:
        setDefaultValue((String) newValue);
        return;
      case ToolkitPackage.PARAMETER_TYPE__MUTABLE:
        setMutable((Boolean) newValue);
        return;
      case ToolkitPackage.PARAMETER_TYPE__TYPE:
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
      case ToolkitPackage.PARAMETER_TYPE__DEFAULT_VALUE:
        setDefaultValue(DEFAULT_VALUE_EDEFAULT);
        return;
      case ToolkitPackage.PARAMETER_TYPE__MUTABLE:
        unsetMutable();
        return;
      case ToolkitPackage.PARAMETER_TYPE__TYPE:
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
      case ToolkitPackage.PARAMETER_TYPE__DEFAULT_VALUE:
        return DEFAULT_VALUE_EDEFAULT == null
            ? defaultValue != null
            : !DEFAULT_VALUE_EDEFAULT.equals(defaultValue);
      case ToolkitPackage.PARAMETER_TYPE__MUTABLE:
        return isSetMutable();
      case ToolkitPackage.PARAMETER_TYPE__TYPE:
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
    result.append(" (defaultValue: ");
    result.append(defaultValue);
    result.append(", mutable: ");
    if (mutableESet) result.append(mutable);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(')');
    return result.toString();
  }
} // ParameterTypeImpl
