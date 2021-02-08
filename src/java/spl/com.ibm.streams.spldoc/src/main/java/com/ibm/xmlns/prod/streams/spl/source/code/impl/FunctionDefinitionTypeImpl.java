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
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Function Definition Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl#getFunctionHead
 *       <em>Function Head</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl#getFunctionBody
 *       <em>Function Body</em>}
 * </ul>
 *
 * @generated
 */
public class FunctionDefinitionTypeImpl extends ExtendedSourceLocationTypeImpl
    implements FunctionDefinitionType {
  /**
   * The cached value of the '{@link #getFunctionHead() <em>Function Head</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctionHead()
   * @generated
   * @ordered
   */
  protected FunctionHeadType functionHead;

  /**
   * The default value of the '{@link #getFunctionBody() <em>Function Body</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctionBody()
   * @generated
   * @ordered
   */
  protected static final String FUNCTION_BODY_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getFunctionBody() <em>Function Body</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctionBody()
   * @generated
   * @ordered
   */
  protected String functionBody = FUNCTION_BODY_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected FunctionDefinitionTypeImpl() {
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
    return CodePackage.Literals.FUNCTION_DEFINITION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionHeadType getFunctionHead() {
    return functionHead;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetFunctionHead(
      FunctionHeadType newFunctionHead, NotificationChain msgs) {
    FunctionHeadType oldFunctionHead = functionHead;
    functionHead = newFunctionHead;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD,
              oldFunctionHead,
              newFunctionHead);
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
  public void setFunctionHead(FunctionHeadType newFunctionHead) {
    if (newFunctionHead != functionHead) {
      NotificationChain msgs = null;
      if (functionHead != null)
        msgs =
            ((InternalEObject) functionHead)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD,
                    null,
                    msgs);
      if (newFunctionHead != null)
        msgs =
            ((InternalEObject) newFunctionHead)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD,
                    null,
                    msgs);
      msgs = basicSetFunctionHead(newFunctionHead, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD,
              newFunctionHead,
              newFunctionHead));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getFunctionBody() {
    return functionBody;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setFunctionBody(String newFunctionBody) {
    String oldFunctionBody = functionBody;
    functionBody = newFunctionBody;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_BODY,
              oldFunctionBody,
              functionBody));
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
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD:
        return basicSetFunctionHead(null, msgs);
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
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD:
        return getFunctionHead();
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_BODY:
        return getFunctionBody();
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
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD:
        setFunctionHead((FunctionHeadType) newValue);
        return;
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_BODY:
        setFunctionBody((String) newValue);
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
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD:
        setFunctionHead((FunctionHeadType) null);
        return;
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_BODY:
        setFunctionBody(FUNCTION_BODY_EDEFAULT);
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
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD:
        return functionHead != null;
      case CodePackage.FUNCTION_DEFINITION_TYPE__FUNCTION_BODY:
        return FUNCTION_BODY_EDEFAULT == null
            ? functionBody != null
            : !FUNCTION_BODY_EDEFAULT.equals(functionBody);
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
    result.append(" (functionBody: ");
    result.append(functionBody);
    result.append(')');
    return result.toString();
  }
} // FunctionDefinitionTypeImpl
