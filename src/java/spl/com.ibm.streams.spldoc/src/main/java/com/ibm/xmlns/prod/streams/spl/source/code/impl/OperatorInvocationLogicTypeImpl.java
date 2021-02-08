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
import com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType;
import com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType;
import com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Logic Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl#getOnProcess
 *       <em>On Process</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl#getOnTuple
 *       <em>On Tuple</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl#getOnPunct
 *       <em>On Punct</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl#isHasState
 *       <em>Has State</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationLogicTypeImpl extends MinimalEObjectImpl.Container
    implements OperatorInvocationLogicType {
  /**
   * The cached value of the '{@link #getOnProcess() <em>On Process</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOnProcess()
   * @generated
   * @ordered
   */
  protected OnProcessType onProcess;

  /**
   * The cached value of the '{@link #getOnTuple() <em>On Tuple</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOnTuple()
   * @generated
   * @ordered
   */
  protected EList<OnTupleType> onTuple;

  /**
   * The cached value of the '{@link #getOnPunct() <em>On Punct</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOnPunct()
   * @generated
   * @ordered
   */
  protected EList<OnPunctType> onPunct;

  /**
   * The default value of the '{@link #isHasState() <em>Has State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isHasState()
   * @generated
   * @ordered
   */
  protected static final boolean HAS_STATE_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isHasState() <em>Has State</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isHasState()
   * @generated
   * @ordered
   */
  protected boolean hasState = HAS_STATE_EDEFAULT;

  /**
   * This is true if the Has State attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean hasStateESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationLogicTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_LOGIC_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OnProcessType getOnProcess() {
    return onProcess;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOnProcess(OnProcessType newOnProcess, NotificationChain msgs) {
    OnProcessType oldOnProcess = onProcess;
    onProcess = newOnProcess;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS,
              oldOnProcess,
              newOnProcess);
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
  public void setOnProcess(OnProcessType newOnProcess) {
    if (newOnProcess != onProcess) {
      NotificationChain msgs = null;
      if (onProcess != null)
        msgs =
            ((InternalEObject) onProcess)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS,
                    null,
                    msgs);
      if (newOnProcess != null)
        msgs =
            ((InternalEObject) newOnProcess)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS,
                    null,
                    msgs);
      msgs = basicSetOnProcess(newOnProcess, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS,
              newOnProcess,
              newOnProcess));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<OnTupleType> getOnTuple() {
    if (onTuple == null) {
      onTuple =
          new EObjectContainmentEList<OnTupleType>(
              OnTupleType.class, this, CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE);
    }
    return onTuple;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<OnPunctType> getOnPunct() {
    if (onPunct == null) {
      onPunct =
          new EObjectContainmentEList<OnPunctType>(
              OnPunctType.class, this, CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT);
    }
    return onPunct;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isHasState() {
    return hasState;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setHasState(boolean newHasState) {
    boolean oldHasState = hasState;
    hasState = newHasState;
    boolean oldHasStateESet = hasStateESet;
    hasStateESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE,
              oldHasState,
              hasState,
              !oldHasStateESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetHasState() {
    boolean oldHasState = hasState;
    boolean oldHasStateESet = hasStateESet;
    hasState = HAS_STATE_EDEFAULT;
    hasStateESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE,
              oldHasState,
              HAS_STATE_EDEFAULT,
              oldHasStateESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetHasState() {
    return hasStateESet;
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
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS:
        return basicSetOnProcess(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE:
        return ((InternalEList<?>) getOnTuple()).basicRemove(otherEnd, msgs);
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT:
        return ((InternalEList<?>) getOnPunct()).basicRemove(otherEnd, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS:
        return getOnProcess();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE:
        return getOnTuple();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT:
        return getOnPunct();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE:
        return isHasState();
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
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS:
        setOnProcess((OnProcessType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE:
        getOnTuple().clear();
        getOnTuple().addAll((Collection<? extends OnTupleType>) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT:
        getOnPunct().clear();
        getOnPunct().addAll((Collection<? extends OnPunctType>) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE:
        setHasState((Boolean) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS:
        setOnProcess((OnProcessType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE:
        getOnTuple().clear();
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT:
        getOnPunct().clear();
        return;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE:
        unsetHasState();
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
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS:
        return onProcess != null;
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE:
        return onTuple != null && !onTuple.isEmpty();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT:
        return onPunct != null && !onPunct.isEmpty();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE:
        return isSetHasState();
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
    result.append(" (hasState: ");
    if (hasStateESet) result.append(hasState);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationLogicTypeImpl
