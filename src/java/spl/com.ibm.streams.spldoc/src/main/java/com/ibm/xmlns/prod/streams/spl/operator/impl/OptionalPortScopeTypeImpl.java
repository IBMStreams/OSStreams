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

import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType;
import java.math.BigInteger;
import java.util.Collection;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EDataTypeEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Optional Port Scope Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OptionalPortScopeTypeImpl#getPort
 *       <em>Port</em>}
 * </ul>
 *
 * @generated
 */
public class OptionalPortScopeTypeImpl extends MinimalEObjectImpl.Container
    implements OptionalPortScopeType {
  /**
   * The cached value of the '{@link #getPort() <em>Port</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPort()
   * @generated
   * @ordered
   */
  protected EList<BigInteger> port;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OptionalPortScopeTypeImpl() {
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
    return OperatorPackage.Literals.OPTIONAL_PORT_SCOPE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<BigInteger> getPort() {
    if (port == null) {
      port =
          new EDataTypeEList<BigInteger>(
              BigInteger.class, this, OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE__PORT);
    }
    return port;
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
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE__PORT:
        return getPort();
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
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE__PORT:
        getPort().clear();
        getPort().addAll((Collection<? extends BigInteger>) newValue);
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
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE__PORT:
        getPort().clear();
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
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE__PORT:
        return port != null && !port.isEmpty();
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
    result.append(" (port: ");
    result.append(port);
    result.append(')');
    return result.toString();
  }
} // OptionalPortScopeTypeImpl
