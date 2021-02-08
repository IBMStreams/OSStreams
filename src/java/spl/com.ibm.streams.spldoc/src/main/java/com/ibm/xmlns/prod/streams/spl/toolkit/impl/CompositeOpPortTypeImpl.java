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

import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Op Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl#getPortIndex
 *       <em>Port Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl#getType
 *       <em>Type</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeOpPortTypeImpl extends ToolkitElementTypeImpl implements CompositeOpPortType {
  /**
   * The default value of the '{@link #getPortIndex() <em>Port Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortIndex()
   * @generated
   * @ordered
   */
  protected static final int PORT_INDEX_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getPortIndex() <em>Port Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortIndex()
   * @generated
   * @ordered
   */
  protected int portIndex = PORT_INDEX_EDEFAULT;

  /**
   * This is true if the Port Index attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean portIndexESet;

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
  protected CompositeOpPortTypeImpl() {
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
    return ToolkitPackage.Literals.COMPOSITE_OP_PORT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getPortIndex() {
    return portIndex;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPortIndex(int newPortIndex) {
    int oldPortIndex = portIndex;
    portIndex = newPortIndex;
    boolean oldPortIndexESet = portIndexESet;
    portIndexESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX,
              oldPortIndex,
              portIndex,
              !oldPortIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPortIndex() {
    int oldPortIndex = portIndex;
    boolean oldPortIndexESet = portIndexESet;
    portIndex = PORT_INDEX_EDEFAULT;
    portIndexESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX,
              oldPortIndex,
              PORT_INDEX_EDEFAULT,
              oldPortIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPortIndex() {
    return portIndexESet;
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
              this, Notification.SET, ToolkitPackage.COMPOSITE_OP_PORT_TYPE__TYPE, oldType, type));
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
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX:
        return getPortIndex();
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__TYPE:
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
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX:
        setPortIndex((Integer) newValue);
        return;
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__TYPE:
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
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX:
        unsetPortIndex();
        return;
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__TYPE:
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
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__PORT_INDEX:
        return isSetPortIndex();
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE__TYPE:
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
    result.append(" (portIndex: ");
    if (portIndexESet) result.append(portIndex);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(')');
    return result.toString();
  }
} // CompositeOpPortTypeImpl
