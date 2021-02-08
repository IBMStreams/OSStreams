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
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import java.util.Collection;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Custom Output Functions Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionsTypeImpl#getCustomOutputFunction
 *       <em>Custom Output Function</em>}
 * </ul>
 *
 * @generated
 */
public class CustomOutputFunctionsTypeImpl extends MinimalEObjectImpl.Container
    implements CustomOutputFunctionsType {
  /**
   * The cached value of the '{@link #getCustomOutputFunction() <em>Custom Output Function</em>}'
   * containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCustomOutputFunction()
   * @generated
   * @ordered
   */
  protected EList<CustomOutputFunctionSetType> customOutputFunction;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CustomOutputFunctionsTypeImpl() {
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
    return OperatorPackage.Literals.CUSTOM_OUTPUT_FUNCTIONS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CustomOutputFunctionSetType> getCustomOutputFunction() {
    if (customOutputFunction == null) {
      customOutputFunction =
          new EObjectContainmentEList<CustomOutputFunctionSetType>(
              CustomOutputFunctionSetType.class,
              this,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION);
    }
    return customOutputFunction;
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION:
        return ((InternalEList<?>) getCustomOutputFunction()).basicRemove(otherEnd, msgs);
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION:
        return getCustomOutputFunction();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION:
        getCustomOutputFunction().clear();
        getCustomOutputFunction()
            .addAll((Collection<? extends CustomOutputFunctionSetType>) newValue);
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION:
        getCustomOutputFunction().clear();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION:
        return customOutputFunction != null && !customOutputFunction.isEmpty();
    }
    return super.eIsSet(featureID);
  }
} // CustomOutputFunctionsTypeImpl
