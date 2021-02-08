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
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Head Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl#getSplDoc
 *       <em>Spl Doc</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl#getOutputs
 *       <em>Outputs</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl#getInputs
 *       <em>Inputs</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl#getInvocationAlias
 *       <em>Invocation Alias</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl#getOperatorName
 *       <em>Operator Name</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationHeadTypeImpl extends SourceLocationTypeImpl
    implements OperatorInvocationHeadType {
  /**
   * The cached value of the '{@link #getSplDoc() <em>Spl Doc</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSplDoc()
   * @generated
   * @ordered
   */
  protected SplDocType splDoc;

  /**
   * The cached value of the '{@link #getOutputs() <em>Outputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputs()
   * @generated
   * @ordered
   */
  protected OperatorInvocationOutputsType outputs;

  /**
   * The cached value of the '{@link #getInputs() <em>Inputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInputs()
   * @generated
   * @ordered
   */
  protected OperatorInvocationInputsType inputs;

  /**
   * The default value of the '{@link #getInvocationAlias() <em>Invocation Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInvocationAlias()
   * @generated
   * @ordered
   */
  protected static final String INVOCATION_ALIAS_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getInvocationAlias() <em>Invocation Alias</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInvocationAlias()
   * @generated
   * @ordered
   */
  protected String invocationAlias = INVOCATION_ALIAS_EDEFAULT;

  /**
   * The default value of the '{@link #getOperatorName() <em>Operator Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOperatorName()
   * @generated
   * @ordered
   */
  protected static final String OPERATOR_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getOperatorName() <em>Operator Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOperatorName()
   * @generated
   * @ordered
   */
  protected String operatorName = OPERATOR_NAME_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationHeadTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_HEAD_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocType getSplDoc() {
    return splDoc;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSplDoc(SplDocType newSplDoc, NotificationChain msgs) {
    SplDocType oldSplDoc = splDoc;
    splDoc = newSplDoc;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC,
              oldSplDoc,
              newSplDoc);
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
  public void setSplDoc(SplDocType newSplDoc) {
    if (newSplDoc != splDoc) {
      NotificationChain msgs = null;
      if (splDoc != null)
        msgs =
            ((InternalEObject) splDoc)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC,
                    null,
                    msgs);
      if (newSplDoc != null)
        msgs =
            ((InternalEObject) newSplDoc)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC,
                    null,
                    msgs);
      msgs = basicSetSplDoc(newSplDoc, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC,
              newSplDoc,
              newSplDoc));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationOutputsType getOutputs() {
    return outputs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputs(
      OperatorInvocationOutputsType newOutputs, NotificationChain msgs) {
    OperatorInvocationOutputsType oldOutputs = outputs;
    outputs = newOutputs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS,
              oldOutputs,
              newOutputs);
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
  public void setOutputs(OperatorInvocationOutputsType newOutputs) {
    if (newOutputs != outputs) {
      NotificationChain msgs = null;
      if (outputs != null)
        msgs =
            ((InternalEObject) outputs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS,
                    null,
                    msgs);
      if (newOutputs != null)
        msgs =
            ((InternalEObject) newOutputs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS,
                    null,
                    msgs);
      msgs = basicSetOutputs(newOutputs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS,
              newOutputs,
              newOutputs));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationInputsType getInputs() {
    return inputs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetInputs(
      OperatorInvocationInputsType newInputs, NotificationChain msgs) {
    OperatorInvocationInputsType oldInputs = inputs;
    inputs = newInputs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS,
              oldInputs,
              newInputs);
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
  public void setInputs(OperatorInvocationInputsType newInputs) {
    if (newInputs != inputs) {
      NotificationChain msgs = null;
      if (inputs != null)
        msgs =
            ((InternalEObject) inputs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS,
                    null,
                    msgs);
      if (newInputs != null)
        msgs =
            ((InternalEObject) newInputs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS,
                    null,
                    msgs);
      msgs = basicSetInputs(newInputs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS,
              newInputs,
              newInputs));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getInvocationAlias() {
    return invocationAlias;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setInvocationAlias(String newInvocationAlias) {
    String oldInvocationAlias = invocationAlias;
    invocationAlias = newInvocationAlias;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS,
              oldInvocationAlias,
              invocationAlias));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getOperatorName() {
    return operatorName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOperatorName(String newOperatorName) {
    String oldOperatorName = operatorName;
    operatorName = newOperatorName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME,
              oldOperatorName,
              operatorName));
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
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC:
        return basicSetSplDoc(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS:
        return basicSetOutputs(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS:
        return basicSetInputs(null, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC:
        return getSplDoc();
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS:
        return getOutputs();
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS:
        return getInputs();
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS:
        return getInvocationAlias();
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME:
        return getOperatorName();
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
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC:
        setSplDoc((SplDocType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS:
        setOutputs((OperatorInvocationOutputsType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS:
        setInputs((OperatorInvocationInputsType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS:
        setInvocationAlias((String) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME:
        setOperatorName((String) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC:
        setSplDoc((SplDocType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS:
        setOutputs((OperatorInvocationOutputsType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS:
        setInputs((OperatorInvocationInputsType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS:
        setInvocationAlias(INVOCATION_ALIAS_EDEFAULT);
        return;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME:
        setOperatorName(OPERATOR_NAME_EDEFAULT);
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
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC:
        return splDoc != null;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS:
        return outputs != null;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INPUTS:
        return inputs != null;
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS:
        return INVOCATION_ALIAS_EDEFAULT == null
            ? invocationAlias != null
            : !INVOCATION_ALIAS_EDEFAULT.equals(invocationAlias);
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME:
        return OPERATOR_NAME_EDEFAULT == null
            ? operatorName != null
            : !OPERATOR_NAME_EDEFAULT.equals(operatorName);
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
    result.append(" (invocationAlias: ");
    result.append(invocationAlias);
    result.append(", operatorName: ");
    result.append(operatorName);
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationHeadTypeImpl
