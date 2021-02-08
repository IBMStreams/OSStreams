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

import com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType;
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
 * An implementation of the model object '<em><b>Java Op Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl#getContext
 *       <em>Context</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl#getInputPorts
 *       <em>Input Ports</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl#getOutputPorts
 *       <em>Output Ports</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpModelTypeImpl extends MinimalEObjectImpl.Container implements JavaOpModelType {
  /**
   * The cached value of the '{@link #getContext() <em>Context</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getContext()
   * @generated
   * @ordered
   */
  protected JavaOpContextType context;

  /**
   * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameters()
   * @generated
   * @ordered
   */
  protected JavaOpParametersType parameters;

  /**
   * The cached value of the '{@link #getInputPorts() <em>Input Ports</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInputPorts()
   * @generated
   * @ordered
   */
  protected JavaOpInputPortsType inputPorts;

  /**
   * The cached value of the '{@link #getOutputPorts() <em>Output Ports</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputPorts()
   * @generated
   * @ordered
   */
  protected JavaOpOutputPortsType outputPorts;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpModelTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpContextType getContext() {
    return context;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetContext(JavaOpContextType newContext, NotificationChain msgs) {
    JavaOpContextType oldContext = context;
    context = newContext;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT,
              oldContext,
              newContext);
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
  public void setContext(JavaOpContextType newContext) {
    if (newContext != context) {
      NotificationChain msgs = null;
      if (context != null)
        msgs =
            ((InternalEObject) context)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT,
                    null,
                    msgs);
      if (newContext != null)
        msgs =
            ((InternalEObject) newContext)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT,
                    null,
                    msgs);
      msgs = basicSetContext(newContext, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT,
              newContext,
              newContext));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpParametersType getParameters() {
    return parameters;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetParameters(
      JavaOpParametersType newParameters, NotificationChain msgs) {
    JavaOpParametersType oldParameters = parameters;
    parameters = newParameters;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS,
              oldParameters,
              newParameters);
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
  public void setParameters(JavaOpParametersType newParameters) {
    if (newParameters != parameters) {
      NotificationChain msgs = null;
      if (parameters != null)
        msgs =
            ((InternalEObject) parameters)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS,
                    null,
                    msgs);
      if (newParameters != null)
        msgs =
            ((InternalEObject) newParameters)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS,
                    null,
                    msgs);
      msgs = basicSetParameters(newParameters, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS,
              newParameters,
              newParameters));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpInputPortsType getInputPorts() {
    return inputPorts;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetInputPorts(
      JavaOpInputPortsType newInputPorts, NotificationChain msgs) {
    JavaOpInputPortsType oldInputPorts = inputPorts;
    inputPorts = newInputPorts;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS,
              oldInputPorts,
              newInputPorts);
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
  public void setInputPorts(JavaOpInputPortsType newInputPorts) {
    if (newInputPorts != inputPorts) {
      NotificationChain msgs = null;
      if (inputPorts != null)
        msgs =
            ((InternalEObject) inputPorts)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS,
                    null,
                    msgs);
      if (newInputPorts != null)
        msgs =
            ((InternalEObject) newInputPorts)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS,
                    null,
                    msgs);
      msgs = basicSetInputPorts(newInputPorts, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS,
              newInputPorts,
              newInputPorts));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpOutputPortsType getOutputPorts() {
    return outputPorts;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputPorts(
      JavaOpOutputPortsType newOutputPorts, NotificationChain msgs) {
    JavaOpOutputPortsType oldOutputPorts = outputPorts;
    outputPorts = newOutputPorts;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS,
              oldOutputPorts,
              newOutputPorts);
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
  public void setOutputPorts(JavaOpOutputPortsType newOutputPorts) {
    if (newOutputPorts != outputPorts) {
      NotificationChain msgs = null;
      if (outputPorts != null)
        msgs =
            ((InternalEObject) outputPorts)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS,
                    null,
                    msgs);
      if (newOutputPorts != null)
        msgs =
            ((InternalEObject) newOutputPorts)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS,
                    null,
                    msgs);
      msgs = basicSetOutputPorts(newOutputPorts, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS,
              newOutputPorts,
              newOutputPorts));
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
      case OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT:
        return basicSetContext(null, msgs);
      case OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS:
        return basicSetParameters(null, msgs);
      case OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS:
        return basicSetInputPorts(null, msgs);
      case OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS:
        return basicSetOutputPorts(null, msgs);
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
      case OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT:
        return getContext();
      case OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS:
        return getParameters();
      case OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS:
        return getInputPorts();
      case OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS:
        return getOutputPorts();
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
      case OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT:
        setContext((JavaOpContextType) newValue);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS:
        setParameters((JavaOpParametersType) newValue);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS:
        setInputPorts((JavaOpInputPortsType) newValue);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS:
        setOutputPorts((JavaOpOutputPortsType) newValue);
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
      case OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT:
        setContext((JavaOpContextType) null);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS:
        setParameters((JavaOpParametersType) null);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS:
        setInputPorts((JavaOpInputPortsType) null);
        return;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS:
        setOutputPorts((JavaOpOutputPortsType) null);
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
      case OperatorPackage.JAVA_OP_MODEL_TYPE__CONTEXT:
        return context != null;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__PARAMETERS:
        return parameters != null;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__INPUT_PORTS:
        return inputPorts != null;
      case OperatorPackage.JAVA_OP_MODEL_TYPE__OUTPUT_PORTS:
        return outputPorts != null;
    }
    return super.eIsSet(featureID);
  }
} // JavaOpModelTypeImpl
