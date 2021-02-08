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
package com.ibm.xmlns.prod.streams.spl.toolkit.impl;

import com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Base Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl#getReturnType
 *       <em>Return Type</em>}
 * </ul>
 *
 * @generated
 */
public class BaseFunctionTypeImpl extends ToolkitElementTypeImpl implements BaseFunctionType {
  /**
   * The default value of the '{@link #getPrototype() <em>Prototype</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPrototype()
   * @generated
   * @ordered
   */
  protected static final String PROTOTYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPrototype() <em>Prototype</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPrototype()
   * @generated
   * @ordered
   */
  protected String prototype = PROTOTYPE_EDEFAULT;

  /**
   * The cached value of the '{@link #getParameter() <em>Parameter</em>}' containment reference
   * list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameter()
   * @generated
   * @ordered
   */
  protected EList<ParameterType> parameter;

  /**
   * The default value of the '{@link #getReturnType() <em>Return Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getReturnType()
   * @generated
   * @ordered
   */
  protected static final String RETURN_TYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getReturnType() <em>Return Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getReturnType()
   * @generated
   * @ordered
   */
  protected String returnType = RETURN_TYPE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected BaseFunctionTypeImpl() {
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
    return ToolkitPackage.Literals.BASE_FUNCTION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getPrototype() {
    return prototype;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPrototype(String newPrototype) {
    String oldPrototype = prototype;
    prototype = newPrototype;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.BASE_FUNCTION_TYPE__PROTOTYPE,
              oldPrototype,
              prototype));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ParameterType> getParameter() {
    if (parameter == null) {
      parameter =
          new EObjectContainmentEList<ParameterType>(
              ParameterType.class, this, ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER);
    }
    return parameter;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getReturnType() {
    return returnType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setReturnType(String newReturnType) {
    String oldReturnType = returnType;
    returnType = newReturnType;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.BASE_FUNCTION_TYPE__RETURN_TYPE,
              oldReturnType,
              returnType));
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
      case ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER:
        return ((InternalEList<?>) getParameter()).basicRemove(otherEnd, msgs);
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
      case ToolkitPackage.BASE_FUNCTION_TYPE__PROTOTYPE:
        return getPrototype();
      case ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER:
        return getParameter();
      case ToolkitPackage.BASE_FUNCTION_TYPE__RETURN_TYPE:
        return getReturnType();
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
      case ToolkitPackage.BASE_FUNCTION_TYPE__PROTOTYPE:
        setPrototype((String) newValue);
        return;
      case ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER:
        getParameter().clear();
        getParameter().addAll((Collection<? extends ParameterType>) newValue);
        return;
      case ToolkitPackage.BASE_FUNCTION_TYPE__RETURN_TYPE:
        setReturnType((String) newValue);
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
      case ToolkitPackage.BASE_FUNCTION_TYPE__PROTOTYPE:
        setPrototype(PROTOTYPE_EDEFAULT);
        return;
      case ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER:
        getParameter().clear();
        return;
      case ToolkitPackage.BASE_FUNCTION_TYPE__RETURN_TYPE:
        setReturnType(RETURN_TYPE_EDEFAULT);
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
      case ToolkitPackage.BASE_FUNCTION_TYPE__PROTOTYPE:
        return PROTOTYPE_EDEFAULT == null
            ? prototype != null
            : !PROTOTYPE_EDEFAULT.equals(prototype);
      case ToolkitPackage.BASE_FUNCTION_TYPE__PARAMETER:
        return parameter != null && !parameter.isEmpty();
      case ToolkitPackage.BASE_FUNCTION_TYPE__RETURN_TYPE:
        return RETURN_TYPE_EDEFAULT == null
            ? returnType != null
            : !RETURN_TYPE_EDEFAULT.equals(returnType);
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
    result.append(" (prototype: ");
    result.append(prototype);
    result.append(", returnType: ");
    result.append(returnType);
    result.append(')');
    return result.toString();
  }
} // BaseFunctionTypeImpl
