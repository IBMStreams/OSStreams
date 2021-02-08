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
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Body Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl#getLogic
 *       <em>Logic</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl#getWindows
 *       <em>Windows</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl#getOutputAssignments
 *       <em>Output Assignments</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl#getConfigs
 *       <em>Configs</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationBodyTypeImpl extends EObjectImpl
    implements OperatorInvocationBodyType {
  /**
   * The cached value of the '{@link #getLogic() <em>Logic</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLogic()
   * @generated
   * @ordered
   */
  protected OperatorInvocationLogicType logic;

  /**
   * The cached value of the '{@link #getWindows() <em>Windows</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getWindows()
   * @generated
   * @ordered
   */
  protected OperatorInvocationWindowsType windows;

  /**
   * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameters()
   * @generated
   * @ordered
   */
  protected OperatorInvocationParametersType parameters;

  /**
   * The cached value of the '{@link #getOutputAssignments() <em>Output Assignments</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputAssignments()
   * @generated
   * @ordered
   */
  protected OperatorInvocationOutputAssignmentsType outputAssignments;

  /**
   * The cached value of the '{@link #getConfigs() <em>Configs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getConfigs()
   * @generated
   * @ordered
   */
  protected ConfigsType configs;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationBodyTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_BODY_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationLogicType getLogic() {
    return logic;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetLogic(
      OperatorInvocationLogicType newLogic, NotificationChain msgs) {
    OperatorInvocationLogicType oldLogic = logic;
    logic = newLogic;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC,
              oldLogic,
              newLogic);
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
  public void setLogic(OperatorInvocationLogicType newLogic) {
    if (newLogic != logic) {
      NotificationChain msgs = null;
      if (logic != null)
        msgs =
            ((InternalEObject) logic)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC,
                    null,
                    msgs);
      if (newLogic != null)
        msgs =
            ((InternalEObject) newLogic)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC,
                    null,
                    msgs);
      msgs = basicSetLogic(newLogic, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC,
              newLogic,
              newLogic));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationWindowsType getWindows() {
    return windows;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetWindows(
      OperatorInvocationWindowsType newWindows, NotificationChain msgs) {
    OperatorInvocationWindowsType oldWindows = windows;
    windows = newWindows;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS,
              oldWindows,
              newWindows);
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
  public void setWindows(OperatorInvocationWindowsType newWindows) {
    if (newWindows != windows) {
      NotificationChain msgs = null;
      if (windows != null)
        msgs =
            ((InternalEObject) windows)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS,
                    null,
                    msgs);
      if (newWindows != null)
        msgs =
            ((InternalEObject) newWindows)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS,
                    null,
                    msgs);
      msgs = basicSetWindows(newWindows, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS,
              newWindows,
              newWindows));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationParametersType getParameters() {
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
      OperatorInvocationParametersType newParameters, NotificationChain msgs) {
    OperatorInvocationParametersType oldParameters = parameters;
    parameters = newParameters;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS,
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
  public void setParameters(OperatorInvocationParametersType newParameters) {
    if (newParameters != parameters) {
      NotificationChain msgs = null;
      if (parameters != null)
        msgs =
            ((InternalEObject) parameters)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS,
                    null,
                    msgs);
      if (newParameters != null)
        msgs =
            ((InternalEObject) newParameters)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS,
                    null,
                    msgs);
      msgs = basicSetParameters(newParameters, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS,
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
  public OperatorInvocationOutputAssignmentsType getOutputAssignments() {
    return outputAssignments;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputAssignments(
      OperatorInvocationOutputAssignmentsType newOutputAssignments, NotificationChain msgs) {
    OperatorInvocationOutputAssignmentsType oldOutputAssignments = outputAssignments;
    outputAssignments = newOutputAssignments;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS,
              oldOutputAssignments,
              newOutputAssignments);
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
  public void setOutputAssignments(OperatorInvocationOutputAssignmentsType newOutputAssignments) {
    if (newOutputAssignments != outputAssignments) {
      NotificationChain msgs = null;
      if (outputAssignments != null)
        msgs =
            ((InternalEObject) outputAssignments)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS,
                    null,
                    msgs);
      if (newOutputAssignments != null)
        msgs =
            ((InternalEObject) newOutputAssignments)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS,
                    null,
                    msgs);
      msgs = basicSetOutputAssignments(newOutputAssignments, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS,
              newOutputAssignments,
              newOutputAssignments));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigsType getConfigs() {
    return configs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetConfigs(ConfigsType newConfigs, NotificationChain msgs) {
    ConfigsType oldConfigs = configs;
    configs = newConfigs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS,
              oldConfigs,
              newConfigs);
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
  public void setConfigs(ConfigsType newConfigs) {
    if (newConfigs != configs) {
      NotificationChain msgs = null;
      if (configs != null)
        msgs =
            ((InternalEObject) configs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS,
                    null,
                    msgs);
      if (newConfigs != null)
        msgs =
            ((InternalEObject) newConfigs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS,
                    null,
                    msgs);
      msgs = basicSetConfigs(newConfigs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS,
              newConfigs,
              newConfigs));
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
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC:
        return basicSetLogic(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS:
        return basicSetWindows(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS:
        return basicSetParameters(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS:
        return basicSetOutputAssignments(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS:
        return basicSetConfigs(null, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC:
        return getLogic();
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS:
        return getWindows();
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS:
        return getParameters();
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS:
        return getOutputAssignments();
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS:
        return getConfigs();
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
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC:
        setLogic((OperatorInvocationLogicType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS:
        setWindows((OperatorInvocationWindowsType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS:
        setParameters((OperatorInvocationParametersType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS:
        setOutputAssignments((OperatorInvocationOutputAssignmentsType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS:
        setConfigs((ConfigsType) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC:
        setLogic((OperatorInvocationLogicType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS:
        setWindows((OperatorInvocationWindowsType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS:
        setParameters((OperatorInvocationParametersType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS:
        setOutputAssignments((OperatorInvocationOutputAssignmentsType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS:
        setConfigs((ConfigsType) null);
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
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__LOGIC:
        return logic != null;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__WINDOWS:
        return windows != null;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS:
        return parameters != null;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS:
        return outputAssignments != null;
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE__CONFIGS:
        return configs != null;
    }
    return super.eIsSet(featureID);
  }
} // OperatorInvocationBodyTypeImpl
