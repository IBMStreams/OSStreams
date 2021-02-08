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
import com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Window Policy Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl#getAttribute
 *       <em>Attribute</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl#getKind
 *       <em>Kind</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl#getSize
 *       <em>Size</em>}
 * </ul>
 *
 * @generated
 */
public class WindowPolicyTypeImpl extends SourceLocationTypeImpl implements WindowPolicyType {
  /**
   * The default value of the '{@link #getAttribute() <em>Attribute</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getAttribute()
   * @generated
   * @ordered
   */
  protected static final String ATTRIBUTE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getAttribute() <em>Attribute</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getAttribute()
   * @generated
   * @ordered
   */
  protected String attribute = ATTRIBUTE_EDEFAULT;

  /**
   * The default value of the '{@link #getKind() <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getKind()
   * @generated
   * @ordered
   */
  protected static final WindowPolicyKindEnumType KIND_EDEFAULT = WindowPolicyKindEnumType.COUNT;

  /**
   * The cached value of the '{@link #getKind() <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getKind()
   * @generated
   * @ordered
   */
  protected WindowPolicyKindEnumType kind = KIND_EDEFAULT;

  /**
   * This is true if the Kind attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean kindESet;

  /**
   * The default value of the '{@link #getSize() <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSize()
   * @generated
   * @ordered
   */
  protected static final String SIZE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getSize() <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSize()
   * @generated
   * @ordered
   */
  protected String size = SIZE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected WindowPolicyTypeImpl() {
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
    return CodePackage.Literals.WINDOW_POLICY_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getAttribute() {
    return attribute;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAttribute(String newAttribute) {
    String oldAttribute = attribute;
    attribute = newAttribute;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.WINDOW_POLICY_TYPE__ATTRIBUTE,
              oldAttribute,
              attribute));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyKindEnumType getKind() {
    return kind;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setKind(WindowPolicyKindEnumType newKind) {
    WindowPolicyKindEnumType oldKind = kind;
    kind = newKind == null ? KIND_EDEFAULT : newKind;
    boolean oldKindESet = kindESet;
    kindESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.WINDOW_POLICY_TYPE__KIND,
              oldKind,
              kind,
              !oldKindESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetKind() {
    WindowPolicyKindEnumType oldKind = kind;
    boolean oldKindESet = kindESet;
    kind = KIND_EDEFAULT;
    kindESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              CodePackage.WINDOW_POLICY_TYPE__KIND,
              oldKind,
              KIND_EDEFAULT,
              oldKindESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetKind() {
    return kindESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getSize() {
    return size;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSize(String newSize) {
    String oldSize = size;
    size = newSize;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.WINDOW_POLICY_TYPE__SIZE, oldSize, size));
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
      case CodePackage.WINDOW_POLICY_TYPE__ATTRIBUTE:
        return getAttribute();
      case CodePackage.WINDOW_POLICY_TYPE__KIND:
        return getKind();
      case CodePackage.WINDOW_POLICY_TYPE__SIZE:
        return getSize();
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
      case CodePackage.WINDOW_POLICY_TYPE__ATTRIBUTE:
        setAttribute((String) newValue);
        return;
      case CodePackage.WINDOW_POLICY_TYPE__KIND:
        setKind((WindowPolicyKindEnumType) newValue);
        return;
      case CodePackage.WINDOW_POLICY_TYPE__SIZE:
        setSize((String) newValue);
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
      case CodePackage.WINDOW_POLICY_TYPE__ATTRIBUTE:
        setAttribute(ATTRIBUTE_EDEFAULT);
        return;
      case CodePackage.WINDOW_POLICY_TYPE__KIND:
        unsetKind();
        return;
      case CodePackage.WINDOW_POLICY_TYPE__SIZE:
        setSize(SIZE_EDEFAULT);
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
      case CodePackage.WINDOW_POLICY_TYPE__ATTRIBUTE:
        return ATTRIBUTE_EDEFAULT == null
            ? attribute != null
            : !ATTRIBUTE_EDEFAULT.equals(attribute);
      case CodePackage.WINDOW_POLICY_TYPE__KIND:
        return isSetKind();
      case CodePackage.WINDOW_POLICY_TYPE__SIZE:
        return SIZE_EDEFAULT == null ? size != null : !SIZE_EDEFAULT.equals(size);
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
    result.append(" (attribute: ");
    result.append(attribute);
    result.append(", kind: ");
    if (kindESet) result.append(kind);
    else result.append("<unset>");
    result.append(", size: ");
    result.append(size);
    result.append(')');
    return result.toString();
  }
} // WindowPolicyTypeImpl
