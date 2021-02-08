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

import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType;
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
 * An implementation of the model object '<em><b>Custom Output Function Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl#getFunction
 *       <em>Function</em>}
 * </ul>
 *
 * @generated
 */
public class CustomOutputFunctionSetTypeImpl extends MinimalEObjectImpl.Container
    implements CustomOutputFunctionSetType {
  /**
   * The default value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected static final String NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected String name = NAME_EDEFAULT;

  /**
   * The cached value of the '{@link #getFunction() <em>Function</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunction()
   * @generated
   * @ordered
   */
  protected EList<CustomOutputFunctionType> function;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CustomOutputFunctionSetTypeImpl() {
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
    return OperatorPackage.Literals.CUSTOM_OUTPUT_FUNCTION_SET_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getName() {
    return name;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setName(String newName) {
    String oldName = name;
    name = newName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME,
              oldName,
              name));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CustomOutputFunctionType> getFunction() {
    if (function == null) {
      function =
          new EObjectContainmentEList<CustomOutputFunctionType>(
              CustomOutputFunctionType.class,
              this,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION);
    }
    return function;
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION:
        return ((InternalEList<?>) getFunction()).basicRemove(otherEnd, msgs);
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME:
        return getName();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION:
        return getFunction();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME:
        setName((String) newValue);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION:
        getFunction().clear();
        getFunction().addAll((Collection<? extends CustomOutputFunctionType>) newValue);
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION:
        getFunction().clear();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION:
        return function != null && !function.isEmpty();
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
    result.append(" (name: ");
    result.append(name);
    result.append(')');
    return result.toString();
  }
} // CustomOutputFunctionSetTypeImpl
