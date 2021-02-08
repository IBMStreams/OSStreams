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
package com.ibm.xmlns.prod.streams.spl.function.impl;

import com.ibm.xmlns.prod.streams.spl.function.FunctionModelType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionPackage;
import com.ibm.xmlns.prod.streams.spl.function.FunctionSetType;
import java.util.Collection;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionModelTypeImpl#getFunctionSet
 *       <em>Function Set</em>}
 * </ul>
 *
 * @generated
 */
public class FunctionModelTypeImpl extends EObjectImpl implements FunctionModelType {
  /**
   * The cached value of the '{@link #getFunctionSet() <em>Function Set</em>}' containment reference
   * list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctionSet()
   * @generated
   * @ordered
   */
  protected EList<FunctionSetType> functionSet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected FunctionModelTypeImpl() {
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
    return FunctionPackage.Literals.FUNCTION_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<FunctionSetType> getFunctionSet() {
    if (functionSet == null) {
      functionSet =
          new EObjectContainmentEList<FunctionSetType>(
              FunctionSetType.class, this, FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET);
    }
    return functionSet;
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
      case FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET:
        return ((InternalEList<?>) getFunctionSet()).basicRemove(otherEnd, msgs);
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
      case FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET:
        return getFunctionSet();
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
      case FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET:
        getFunctionSet().clear();
        getFunctionSet().addAll((Collection<? extends FunctionSetType>) newValue);
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
      case FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET:
        getFunctionSet().clear();
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
      case FunctionPackage.FUNCTION_MODEL_TYPE__FUNCTION_SET:
        return functionSet != null && !functionSet.isEmpty();
    }
    return super.eIsSet(featureID);
  }
} // FunctionModelTypeImpl
