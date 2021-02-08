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

import com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Java Op Execution Settings Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl#getClassName
 *       <em>Class Name</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl#getVmArgs
 *       <em>Vm Args</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpExecutionSettingsTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpExecutionSettingsType {
  /**
   * The default value of the '{@link #getClassName() <em>Class Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getClassName()
   * @generated
   * @ordered
   */
  protected static final String CLASS_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getClassName() <em>Class Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getClassName()
   * @generated
   * @ordered
   */
  protected String className = CLASS_NAME_EDEFAULT;

  /**
   * The cached value of the '{@link #getVmArgs() <em>Vm Args</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getVmArgs()
   * @generated
   * @ordered
   */
  protected JavaOpVMArgsType vmArgs;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpExecutionSettingsTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_EXECUTION_SETTINGS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getClassName() {
    return className;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setClassName(String newClassName) {
    String oldClassName = className;
    className = newClassName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME,
              oldClassName,
              className));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpVMArgsType getVmArgs() {
    return vmArgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetVmArgs(JavaOpVMArgsType newVmArgs, NotificationChain msgs) {
    JavaOpVMArgsType oldVmArgs = vmArgs;
    vmArgs = newVmArgs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS,
              oldVmArgs,
              newVmArgs);
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
  public void setVmArgs(JavaOpVMArgsType newVmArgs) {
    if (newVmArgs != vmArgs) {
      NotificationChain msgs = null;
      if (vmArgs != null)
        msgs =
            ((InternalEObject) vmArgs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS,
                    null,
                    msgs);
      if (newVmArgs != null)
        msgs =
            ((InternalEObject) newVmArgs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS,
                    null,
                    msgs);
      msgs = basicSetVmArgs(newVmArgs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS,
              newVmArgs,
              newVmArgs));
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
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS:
        return basicSetVmArgs(null, msgs);
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
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME:
        return getClassName();
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS:
        return getVmArgs();
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
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME:
        setClassName((String) newValue);
        return;
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS:
        setVmArgs((JavaOpVMArgsType) newValue);
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
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME:
        setClassName(CLASS_NAME_EDEFAULT);
        return;
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS:
        setVmArgs((JavaOpVMArgsType) null);
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
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME:
        return CLASS_NAME_EDEFAULT == null
            ? className != null
            : !CLASS_NAME_EDEFAULT.equals(className);
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS:
        return vmArgs != null;
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
    result.append(" (className: ");
    result.append(className);
    result.append(')');
    return result.toString();
  }
} // JavaOpExecutionSettingsTypeImpl
