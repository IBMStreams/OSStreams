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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType;
import java.math.BigInteger;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Port Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl#getIndex
 *       <em>Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl#getType
 *       <em>Type</em>}
 * </ul>
 *
 * @generated
 */
public class CompositePortTypeImpl extends SourceLocationTypeImpl implements CompositePortType {
  /**
   * The default value of the '{@link #getIndex() <em>Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIndex()
   * @generated
   * @ordered
   */
  protected static final BigInteger INDEX_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getIndex() <em>Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIndex()
   * @generated
   * @ordered
   */
  protected BigInteger index = INDEX_EDEFAULT;

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
  protected CompositePortTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_PORT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getIndex() {
    return index;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setIndex(BigInteger newIndex) {
    BigInteger oldIndex = index;
    index = newIndex;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_PORT_TYPE__INDEX, oldIndex, index));
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
              this, Notification.SET, CodePackage.COMPOSITE_PORT_TYPE__NAME, oldName, name));
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
              this, Notification.SET, CodePackage.COMPOSITE_PORT_TYPE__TYPE, oldType, type));
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
      case CodePackage.COMPOSITE_PORT_TYPE__INDEX:
        return getIndex();
      case CodePackage.COMPOSITE_PORT_TYPE__NAME:
        return getName();
      case CodePackage.COMPOSITE_PORT_TYPE__TYPE:
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
      case CodePackage.COMPOSITE_PORT_TYPE__INDEX:
        setIndex((BigInteger) newValue);
        return;
      case CodePackage.COMPOSITE_PORT_TYPE__NAME:
        setName((String) newValue);
        return;
      case CodePackage.COMPOSITE_PORT_TYPE__TYPE:
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
      case CodePackage.COMPOSITE_PORT_TYPE__INDEX:
        setIndex(INDEX_EDEFAULT);
        return;
      case CodePackage.COMPOSITE_PORT_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case CodePackage.COMPOSITE_PORT_TYPE__TYPE:
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
      case CodePackage.COMPOSITE_PORT_TYPE__INDEX:
        return INDEX_EDEFAULT == null ? index != null : !INDEX_EDEFAULT.equals(index);
      case CodePackage.COMPOSITE_PORT_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case CodePackage.COMPOSITE_PORT_TYPE__TYPE:
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
    result.append(" (index: ");
    result.append(index);
    result.append(", name: ");
    result.append(name);
    result.append(", type: ");
    result.append(type);
    result.append(')');
    return result.toString();
  }
} // CompositePortTypeImpl
