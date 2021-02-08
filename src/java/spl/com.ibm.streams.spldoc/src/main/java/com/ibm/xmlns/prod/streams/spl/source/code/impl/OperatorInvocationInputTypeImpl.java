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
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType;
import java.math.BigInteger;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Input Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl#getIstream
 *       <em>Istream</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl#getAlias
 *       <em>Alias</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl#getIndex
 *       <em>Index</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationInputTypeImpl extends SourceLocationTypeImpl
    implements OperatorInvocationInputType {
  /**
   * The cached value of the '{@link #getIstream() <em>Istream</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIstream()
   * @generated
   * @ordered
   */
  protected EList<OperatorInvocationInputStreamType> istream;

  /**
   * The default value of the '{@link #getAlias() <em>Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getAlias()
   * @generated
   * @ordered
   */
  protected static final String ALIAS_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getAlias() <em>Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getAlias()
   * @generated
   * @ordered
   */
  protected String alias = ALIAS_EDEFAULT;

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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationInputTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_INPUT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<OperatorInvocationInputStreamType> getIstream() {
    if (istream == null) {
      istream =
          new EObjectContainmentEList<OperatorInvocationInputStreamType>(
              OperatorInvocationInputStreamType.class,
              this,
              CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM);
    }
    return istream;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getAlias() {
    return alias;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAlias(String newAlias) {
    String oldAlias = alias;
    alias = newAlias;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ALIAS,
              oldAlias,
              alias));
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
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__INDEX,
              oldIndex,
              index));
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
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM:
        return ((InternalEList<?>) getIstream()).basicRemove(otherEnd, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM:
        return getIstream();
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ALIAS:
        return getAlias();
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__INDEX:
        return getIndex();
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
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM:
        getIstream().clear();
        getIstream().addAll((Collection<? extends OperatorInvocationInputStreamType>) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ALIAS:
        setAlias((String) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__INDEX:
        setIndex((BigInteger) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM:
        getIstream().clear();
        return;
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ALIAS:
        setAlias(ALIAS_EDEFAULT);
        return;
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__INDEX:
        setIndex(INDEX_EDEFAULT);
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
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM:
        return istream != null && !istream.isEmpty();
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__ALIAS:
        return ALIAS_EDEFAULT == null ? alias != null : !ALIAS_EDEFAULT.equals(alias);
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE__INDEX:
        return INDEX_EDEFAULT == null ? index != null : !INDEX_EDEFAULT.equals(index);
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
    result.append(" (alias: ");
    result.append(alias);
    result.append(", index: ");
    result.append(index);
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationInputTypeImpl
