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
import com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Use Directive Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl#getNamespaceName
 *       <em>Namespace Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl#getTail
 *       <em>Tail</em>}
 * </ul>
 *
 * @generated
 */
public class UseDirectiveTypeImpl extends SourceLocationTypeImpl implements UseDirectiveType {
  /**
   * The default value of the '{@link #getNamespaceName() <em>Namespace Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getNamespaceName()
   * @generated
   * @ordered
   */
  protected static final String NAMESPACE_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getNamespaceName() <em>Namespace Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getNamespaceName()
   * @generated
   * @ordered
   */
  protected String namespaceName = NAMESPACE_NAME_EDEFAULT;

  /**
   * The default value of the '{@link #getTail() <em>Tail</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTail()
   * @generated
   * @ordered
   */
  protected static final String TAIL_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getTail() <em>Tail</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTail()
   * @generated
   * @ordered
   */
  protected String tail = TAIL_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected UseDirectiveTypeImpl() {
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
    return CodePackage.Literals.USE_DIRECTIVE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getNamespaceName() {
    return namespaceName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setNamespaceName(String newNamespaceName) {
    String oldNamespaceName = namespaceName;
    namespaceName = newNamespaceName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.USE_DIRECTIVE_TYPE__NAMESPACE_NAME,
              oldNamespaceName,
              namespaceName));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getTail() {
    return tail;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setTail(String newTail) {
    String oldTail = tail;
    tail = newTail;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.USE_DIRECTIVE_TYPE__TAIL, oldTail, tail));
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
      case CodePackage.USE_DIRECTIVE_TYPE__NAMESPACE_NAME:
        return getNamespaceName();
      case CodePackage.USE_DIRECTIVE_TYPE__TAIL:
        return getTail();
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
      case CodePackage.USE_DIRECTIVE_TYPE__NAMESPACE_NAME:
        setNamespaceName((String) newValue);
        return;
      case CodePackage.USE_DIRECTIVE_TYPE__TAIL:
        setTail((String) newValue);
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
      case CodePackage.USE_DIRECTIVE_TYPE__NAMESPACE_NAME:
        setNamespaceName(NAMESPACE_NAME_EDEFAULT);
        return;
      case CodePackage.USE_DIRECTIVE_TYPE__TAIL:
        setTail(TAIL_EDEFAULT);
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
      case CodePackage.USE_DIRECTIVE_TYPE__NAMESPACE_NAME:
        return NAMESPACE_NAME_EDEFAULT == null
            ? namespaceName != null
            : !NAMESPACE_NAME_EDEFAULT.equals(namespaceName);
      case CodePackage.USE_DIRECTIVE_TYPE__TAIL:
        return TAIL_EDEFAULT == null ? tail != null : !TAIL_EDEFAULT.equals(tail);
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
    result.append(" (namespaceName: ");
    result.append(namespaceName);
    result.append(", tail: ");
    result.append(tail);
    result.append(')');
    return result.toString();
  }
} // UseDirectiveTypeImpl
