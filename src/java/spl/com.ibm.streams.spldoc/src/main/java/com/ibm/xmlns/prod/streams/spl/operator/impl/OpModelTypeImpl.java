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

import com.ibm.xmlns.prod.streams.spl.operator.ContextType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.OpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.ParametersType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Op Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl#getContext
 *       <em>Context</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl#getInputPorts <em>Input
 *       Ports</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl#getOutputPorts
 *       <em>Output Ports</em>}
 * </ul>
 *
 * @generated
 */
public class OpModelTypeImpl extends MinimalEObjectImpl.Container implements OpModelType {
  /**
   * The cached value of the '{@link #getContext() <em>Context</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getContext()
   * @generated
   * @ordered
   */
  protected ContextType context;

  /**
   * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameters()
   * @generated
   * @ordered
   */
  protected ParametersType parameters;

  /**
   * The cached value of the '{@link #getInputPorts() <em>Input Ports</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInputPorts()
   * @generated
   * @ordered
   */
  protected InputPortsType inputPorts;

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
  protected OutputPortsType outputPorts;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OpModelTypeImpl() {
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
    return OperatorPackage.Literals.OP_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ContextType getContext() {
    return context;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetContext(ContextType newContext, NotificationChain msgs) {
    ContextType oldContext = context;
    context = newContext;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__CONTEXT,
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
  public void setContext(ContextType newContext) {
    if (newContext != context) {
      NotificationChain msgs = null;
      if (context != null)
        msgs =
            ((InternalEObject) context)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__CONTEXT,
                    null,
                    msgs);
      if (newContext != null)
        msgs =
            ((InternalEObject) newContext)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__CONTEXT,
                    null,
                    msgs);
      msgs = basicSetContext(newContext, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__CONTEXT,
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
  public ParametersType getParameters() {
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
      ParametersType newParameters, NotificationChain msgs) {
    ParametersType oldParameters = parameters;
    parameters = newParameters;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__PARAMETERS,
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
  public void setParameters(ParametersType newParameters) {
    if (newParameters != parameters) {
      NotificationChain msgs = null;
      if (parameters != null)
        msgs =
            ((InternalEObject) parameters)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__PARAMETERS,
                    null,
                    msgs);
      if (newParameters != null)
        msgs =
            ((InternalEObject) newParameters)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__PARAMETERS,
                    null,
                    msgs);
      msgs = basicSetParameters(newParameters, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__PARAMETERS,
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
  public InputPortsType getInputPorts() {
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
      InputPortsType newInputPorts, NotificationChain msgs) {
    InputPortsType oldInputPorts = inputPorts;
    inputPorts = newInputPorts;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS,
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
  public void setInputPorts(InputPortsType newInputPorts) {
    if (newInputPorts != inputPorts) {
      NotificationChain msgs = null;
      if (inputPorts != null)
        msgs =
            ((InternalEObject) inputPorts)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS,
                    null,
                    msgs);
      if (newInputPorts != null)
        msgs =
            ((InternalEObject) newInputPorts)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS,
                    null,
                    msgs);
      msgs = basicSetInputPorts(newInputPorts, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS,
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
  public OutputPortsType getOutputPorts() {
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
      OutputPortsType newOutputPorts, NotificationChain msgs) {
    OutputPortsType oldOutputPorts = outputPorts;
    outputPorts = newOutputPorts;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS,
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
  public void setOutputPorts(OutputPortsType newOutputPorts) {
    if (newOutputPorts != outputPorts) {
      NotificationChain msgs = null;
      if (outputPorts != null)
        msgs =
            ((InternalEObject) outputPorts)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS,
                    null,
                    msgs);
      if (newOutputPorts != null)
        msgs =
            ((InternalEObject) newOutputPorts)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS,
                    null,
                    msgs);
      msgs = basicSetOutputPorts(newOutputPorts, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS,
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
      case OperatorPackage.OP_MODEL_TYPE__CONTEXT:
        return basicSetContext(null, msgs);
      case OperatorPackage.OP_MODEL_TYPE__PARAMETERS:
        return basicSetParameters(null, msgs);
      case OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS:
        return basicSetInputPorts(null, msgs);
      case OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS:
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
      case OperatorPackage.OP_MODEL_TYPE__CONTEXT:
        return getContext();
      case OperatorPackage.OP_MODEL_TYPE__PARAMETERS:
        return getParameters();
      case OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS:
        return getInputPorts();
      case OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS:
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
      case OperatorPackage.OP_MODEL_TYPE__CONTEXT:
        setContext((ContextType) newValue);
        return;
      case OperatorPackage.OP_MODEL_TYPE__PARAMETERS:
        setParameters((ParametersType) newValue);
        return;
      case OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS:
        setInputPorts((InputPortsType) newValue);
        return;
      case OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS:
        setOutputPorts((OutputPortsType) newValue);
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
      case OperatorPackage.OP_MODEL_TYPE__CONTEXT:
        setContext((ContextType) null);
        return;
      case OperatorPackage.OP_MODEL_TYPE__PARAMETERS:
        setParameters((ParametersType) null);
        return;
      case OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS:
        setInputPorts((InputPortsType) null);
        return;
      case OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS:
        setOutputPorts((OutputPortsType) null);
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
      case OperatorPackage.OP_MODEL_TYPE__CONTEXT:
        return context != null;
      case OperatorPackage.OP_MODEL_TYPE__PARAMETERS:
        return parameters != null;
      case OperatorPackage.OP_MODEL_TYPE__INPUT_PORTS:
        return inputPorts != null;
      case OperatorPackage.OP_MODEL_TYPE__OUTPUT_PORTS:
        return outputPorts != null;
    }
    return super.eIsSet(featureID);
  }
} // OpModelTypeImpl
