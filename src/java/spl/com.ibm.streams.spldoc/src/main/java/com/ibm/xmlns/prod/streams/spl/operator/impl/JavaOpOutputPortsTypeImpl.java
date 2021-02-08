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

import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
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
 * An implementation of the model object '<em><b>Java Op Output Ports Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl#getOutputPortSet
 *       <em>Output Port Set</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl#getOutputPortOpenSet
 *       <em>Output Port Open Set</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpOutputPortsTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpOutputPortsType {
  /**
   * The cached value of the '{@link #getOutputPortSet() <em>Output Port Set</em>}' containment
   * reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputPortSet()
   * @generated
   * @ordered
   */
  protected EList<JavaOpOutputPortSetType> outputPortSet;

  /**
   * The cached value of the '{@link #getOutputPortOpenSet() <em>Output Port Open Set</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputPortOpenSet()
   * @generated
   * @ordered
   */
  protected JavaOpOutputPortOpenSetType outputPortOpenSet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpOutputPortsTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_OUTPUT_PORTS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<JavaOpOutputPortSetType> getOutputPortSet() {
    if (outputPortSet == null) {
      outputPortSet =
          new EObjectContainmentEList<JavaOpOutputPortSetType>(
              JavaOpOutputPortSetType.class,
              this,
              OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET);
    }
    return outputPortSet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpOutputPortOpenSetType getOutputPortOpenSet() {
    return outputPortOpenSet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputPortOpenSet(
      JavaOpOutputPortOpenSetType newOutputPortOpenSet, NotificationChain msgs) {
    JavaOpOutputPortOpenSetType oldOutputPortOpenSet = outputPortOpenSet;
    outputPortOpenSet = newOutputPortOpenSet;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET,
              oldOutputPortOpenSet,
              newOutputPortOpenSet);
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
  public void setOutputPortOpenSet(JavaOpOutputPortOpenSetType newOutputPortOpenSet) {
    if (newOutputPortOpenSet != outputPortOpenSet) {
      NotificationChain msgs = null;
      if (outputPortOpenSet != null)
        msgs =
            ((InternalEObject) outputPortOpenSet)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET,
                    null,
                    msgs);
      if (newOutputPortOpenSet != null)
        msgs =
            ((InternalEObject) newOutputPortOpenSet)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET,
                    null,
                    msgs);
      msgs = basicSetOutputPortOpenSet(newOutputPortOpenSet, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET,
              newOutputPortOpenSet,
              newOutputPortOpenSet));
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET:
        return ((InternalEList<?>) getOutputPortSet()).basicRemove(otherEnd, msgs);
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET:
        return basicSetOutputPortOpenSet(null, msgs);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET:
        return getOutputPortSet();
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET:
        return getOutputPortOpenSet();
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET:
        getOutputPortSet().clear();
        getOutputPortSet().addAll((Collection<? extends JavaOpOutputPortSetType>) newValue);
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET:
        setOutputPortOpenSet((JavaOpOutputPortOpenSetType) newValue);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET:
        getOutputPortSet().clear();
        return;
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET:
        setOutputPortOpenSet((JavaOpOutputPortOpenSetType) null);
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
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET:
        return outputPortSet != null && !outputPortSet.isEmpty();
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET:
        return outputPortOpenSet != null;
    }
    return super.eIsSet(featureID);
  }
} // JavaOpOutputPortsTypeImpl
