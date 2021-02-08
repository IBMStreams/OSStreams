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
package com.ibm.xmlns.prod.streams.spl.toolkit.info.impl;

import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
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
 * An implementation of the model object '<em><b>Dependencies Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DependenciesTypeImpl#getToolkit
 *       <em>Toolkit</em>}
 * </ul>
 *
 * @generated
 */
public class DependenciesTypeImpl extends EObjectImpl implements DependenciesType {
  /**
   * The cached value of the '{@link #getToolkit() <em>Toolkit</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getToolkit()
   * @generated
   * @ordered
   */
  protected EList<ToolkitDependencyType> toolkit;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected DependenciesTypeImpl() {
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
    return InfoPackage.Literals.DEPENDENCIES_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitDependencyType> getToolkit() {
    if (toolkit == null) {
      toolkit =
          new EObjectContainmentEList<ToolkitDependencyType>(
              ToolkitDependencyType.class, this, InfoPackage.DEPENDENCIES_TYPE__TOOLKIT);
    }
    return toolkit;
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
      case InfoPackage.DEPENDENCIES_TYPE__TOOLKIT:
        return ((InternalEList<?>) getToolkit()).basicRemove(otherEnd, msgs);
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
      case InfoPackage.DEPENDENCIES_TYPE__TOOLKIT:
        return getToolkit();
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
      case InfoPackage.DEPENDENCIES_TYPE__TOOLKIT:
        getToolkit().clear();
        getToolkit().addAll((Collection<? extends ToolkitDependencyType>) newValue);
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
      case InfoPackage.DEPENDENCIES_TYPE__TOOLKIT:
        getToolkit().clear();
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
      case InfoPackage.DEPENDENCIES_TYPE__TOOLKIT:
        return toolkit != null && !toolkit.isEmpty();
    }
    return super.eIsSet(featureID);
  }
} // DependenciesTypeImpl
