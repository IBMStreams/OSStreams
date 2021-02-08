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
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Window Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl#getEvictionPolicy
 *       <em>Eviction Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl#getTriggerPolicy
 *       <em>Trigger Policy</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl#isPartitioned
 *       <em>Partitioned</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl#getPortName
 *       <em>Port Name</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl#getWindowType
 *       <em>Window Type</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationWindowTypeImpl extends SourceLocationTypeImpl
    implements OperatorInvocationWindowType {
  /**
   * The cached value of the '{@link #getEvictionPolicy() <em>Eviction Policy</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEvictionPolicy()
   * @generated
   * @ordered
   */
  protected WindowPolicyType evictionPolicy;

  /**
   * The cached value of the '{@link #getTriggerPolicy() <em>Trigger Policy</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTriggerPolicy()
   * @generated
   * @ordered
   */
  protected WindowPolicyType triggerPolicy;

  /**
   * The default value of the '{@link #isPartitioned() <em>Partitioned</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPartitioned()
   * @generated
   * @ordered
   */
  protected static final boolean PARTITIONED_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPartitioned() <em>Partitioned</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPartitioned()
   * @generated
   * @ordered
   */
  protected boolean partitioned = PARTITIONED_EDEFAULT;

  /**
   * This is true if the Partitioned attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean partitionedESet;

  /**
   * The default value of the '{@link #getPortName() <em>Port Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortName()
   * @generated
   * @ordered
   */
  protected static final String PORT_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPortName() <em>Port Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortName()
   * @generated
   * @ordered
   */
  protected String portName = PORT_NAME_EDEFAULT;

  /**
   * The default value of the '{@link #getWindowType() <em>Window Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowType()
   * @generated
   * @ordered
   */
  protected static final WindowTypeEnumType WINDOW_TYPE_EDEFAULT = WindowTypeEnumType.TUMBLING;

  /**
   * The cached value of the '{@link #getWindowType() <em>Window Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindowType()
   * @generated
   * @ordered
   */
  protected WindowTypeEnumType windowType = WINDOW_TYPE_EDEFAULT;

  /**
   * This is true if the Window Type attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean windowTypeESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationWindowTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_WINDOW_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyType getEvictionPolicy() {
    return evictionPolicy;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetEvictionPolicy(
      WindowPolicyType newEvictionPolicy, NotificationChain msgs) {
    WindowPolicyType oldEvictionPolicy = evictionPolicy;
    evictionPolicy = newEvictionPolicy;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY,
              oldEvictionPolicy,
              newEvictionPolicy);
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
  public void setEvictionPolicy(WindowPolicyType newEvictionPolicy) {
    if (newEvictionPolicy != evictionPolicy) {
      NotificationChain msgs = null;
      if (evictionPolicy != null)
        msgs =
            ((InternalEObject) evictionPolicy)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY,
                    null,
                    msgs);
      if (newEvictionPolicy != null)
        msgs =
            ((InternalEObject) newEvictionPolicy)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY,
                    null,
                    msgs);
      msgs = basicSetEvictionPolicy(newEvictionPolicy, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY,
              newEvictionPolicy,
              newEvictionPolicy));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyType getTriggerPolicy() {
    return triggerPolicy;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetTriggerPolicy(
      WindowPolicyType newTriggerPolicy, NotificationChain msgs) {
    WindowPolicyType oldTriggerPolicy = triggerPolicy;
    triggerPolicy = newTriggerPolicy;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY,
              oldTriggerPolicy,
              newTriggerPolicy);
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
  public void setTriggerPolicy(WindowPolicyType newTriggerPolicy) {
    if (newTriggerPolicy != triggerPolicy) {
      NotificationChain msgs = null;
      if (triggerPolicy != null)
        msgs =
            ((InternalEObject) triggerPolicy)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY,
                    null,
                    msgs);
      if (newTriggerPolicy != null)
        msgs =
            ((InternalEObject) newTriggerPolicy)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY,
                    null,
                    msgs);
      msgs = basicSetTriggerPolicy(newTriggerPolicy, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY,
              newTriggerPolicy,
              newTriggerPolicy));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isPartitioned() {
    return partitioned;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPartitioned(boolean newPartitioned) {
    boolean oldPartitioned = partitioned;
    partitioned = newPartitioned;
    boolean oldPartitionedESet = partitionedESet;
    partitionedESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED,
              oldPartitioned,
              partitioned,
              !oldPartitionedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPartitioned() {
    boolean oldPartitioned = partitioned;
    boolean oldPartitionedESet = partitionedESet;
    partitioned = PARTITIONED_EDEFAULT;
    partitionedESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED,
              oldPartitioned,
              PARTITIONED_EDEFAULT,
              oldPartitionedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPartitioned() {
    return partitionedESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getPortName() {
    return portName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPortName(String newPortName) {
    String oldPortName = portName;
    portName = newPortName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME,
              oldPortName,
              portName));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowTypeEnumType getWindowType() {
    return windowType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setWindowType(WindowTypeEnumType newWindowType) {
    WindowTypeEnumType oldWindowType = windowType;
    windowType = newWindowType == null ? WINDOW_TYPE_EDEFAULT : newWindowType;
    boolean oldWindowTypeESet = windowTypeESet;
    windowTypeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE,
              oldWindowType,
              windowType,
              !oldWindowTypeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetWindowType() {
    WindowTypeEnumType oldWindowType = windowType;
    boolean oldWindowTypeESet = windowTypeESet;
    windowType = WINDOW_TYPE_EDEFAULT;
    windowTypeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE,
              oldWindowType,
              WINDOW_TYPE_EDEFAULT,
              oldWindowTypeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetWindowType() {
    return windowTypeESet;
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
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY:
        return basicSetEvictionPolicy(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY:
        return basicSetTriggerPolicy(null, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY:
        return getEvictionPolicy();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY:
        return getTriggerPolicy();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED:
        return isPartitioned();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME:
        return getPortName();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE:
        return getWindowType();
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
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY:
        setEvictionPolicy((WindowPolicyType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY:
        setTriggerPolicy((WindowPolicyType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED:
        setPartitioned((Boolean) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME:
        setPortName((String) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE:
        setWindowType((WindowTypeEnumType) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY:
        setEvictionPolicy((WindowPolicyType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY:
        setTriggerPolicy((WindowPolicyType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED:
        unsetPartitioned();
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME:
        setPortName(PORT_NAME_EDEFAULT);
        return;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE:
        unsetWindowType();
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
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY:
        return evictionPolicy != null;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY:
        return triggerPolicy != null;
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED:
        return isSetPartitioned();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME:
        return PORT_NAME_EDEFAULT == null ? portName != null : !PORT_NAME_EDEFAULT.equals(portName);
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE:
        return isSetWindowType();
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
    result.append(" (partitioned: ");
    if (partitionedESet) result.append(partitioned);
    else result.append("<unset>");
    result.append(", portName: ");
    result.append(portName);
    result.append(", windowType: ");
    if (windowTypeESet) result.append(windowType);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationWindowTypeImpl
