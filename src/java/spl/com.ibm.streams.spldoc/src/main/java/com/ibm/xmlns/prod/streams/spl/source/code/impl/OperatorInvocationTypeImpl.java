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
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl#getOperatorInvocationHead
 *       <em>Operator Invocation Head</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl#getOperatorInvocationBody
 *       <em>Operator Invocation Body</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationTypeImpl extends EObjectImpl implements OperatorInvocationType {
  /**
   * The cached value of the '{@link #getOperatorInvocationHead() <em>Operator Invocation
   * Head</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOperatorInvocationHead()
   * @generated
   * @ordered
   */
  protected OperatorInvocationHeadType operatorInvocationHead;

  /**
   * The cached value of the '{@link #getOperatorInvocationBody() <em>Operator Invocation
   * Body</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOperatorInvocationBody()
   * @generated
   * @ordered
   */
  protected OperatorInvocationBodyType operatorInvocationBody;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationHeadType getOperatorInvocationHead() {
    return operatorInvocationHead;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOperatorInvocationHead(
      OperatorInvocationHeadType newOperatorInvocationHead, NotificationChain msgs) {
    OperatorInvocationHeadType oldOperatorInvocationHead = operatorInvocationHead;
    operatorInvocationHead = newOperatorInvocationHead;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD,
              oldOperatorInvocationHead,
              newOperatorInvocationHead);
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
  public void setOperatorInvocationHead(OperatorInvocationHeadType newOperatorInvocationHead) {
    if (newOperatorInvocationHead != operatorInvocationHead) {
      NotificationChain msgs = null;
      if (operatorInvocationHead != null)
        msgs =
            ((InternalEObject) operatorInvocationHead)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD,
                    null,
                    msgs);
      if (newOperatorInvocationHead != null)
        msgs =
            ((InternalEObject) newOperatorInvocationHead)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD,
                    null,
                    msgs);
      msgs = basicSetOperatorInvocationHead(newOperatorInvocationHead, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD,
              newOperatorInvocationHead,
              newOperatorInvocationHead));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationBodyType getOperatorInvocationBody() {
    return operatorInvocationBody;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOperatorInvocationBody(
      OperatorInvocationBodyType newOperatorInvocationBody, NotificationChain msgs) {
    OperatorInvocationBodyType oldOperatorInvocationBody = operatorInvocationBody;
    operatorInvocationBody = newOperatorInvocationBody;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY,
              oldOperatorInvocationBody,
              newOperatorInvocationBody);
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
  public void setOperatorInvocationBody(OperatorInvocationBodyType newOperatorInvocationBody) {
    if (newOperatorInvocationBody != operatorInvocationBody) {
      NotificationChain msgs = null;
      if (operatorInvocationBody != null)
        msgs =
            ((InternalEObject) operatorInvocationBody)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY,
                    null,
                    msgs);
      if (newOperatorInvocationBody != null)
        msgs =
            ((InternalEObject) newOperatorInvocationBody)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY,
                    null,
                    msgs);
      msgs = basicSetOperatorInvocationBody(newOperatorInvocationBody, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY,
              newOperatorInvocationBody,
              newOperatorInvocationBody));
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
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD:
        return basicSetOperatorInvocationHead(null, msgs);
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY:
        return basicSetOperatorInvocationBody(null, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD:
        return getOperatorInvocationHead();
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY:
        return getOperatorInvocationBody();
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
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD:
        setOperatorInvocationHead((OperatorInvocationHeadType) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY:
        setOperatorInvocationBody((OperatorInvocationBodyType) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD:
        setOperatorInvocationHead((OperatorInvocationHeadType) null);
        return;
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY:
        setOperatorInvocationBody((OperatorInvocationBodyType) null);
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
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD:
        return operatorInvocationHead != null;
      case CodePackage.OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY:
        return operatorInvocationBody != null;
    }
    return super.eIsSet(featureID);
  }
} // OperatorInvocationTypeImpl
