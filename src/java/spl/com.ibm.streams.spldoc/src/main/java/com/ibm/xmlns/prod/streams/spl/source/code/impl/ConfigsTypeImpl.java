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
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType;
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
 * An implementation of the model object '<em><b>Configs Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigsTypeImpl#getConfig
 *       <em>Config</em>}
 * </ul>
 *
 * @generated
 */
public class ConfigsTypeImpl extends EObjectImpl implements ConfigsType {
  /**
   * The cached value of the '{@link #getConfig() <em>Config</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getConfig()
   * @generated
   * @ordered
   */
  protected EList<ConfigType> config;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ConfigsTypeImpl() {
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
    return CodePackage.Literals.CONFIGS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ConfigType> getConfig() {
    if (config == null) {
      config =
          new EObjectContainmentEList<ConfigType>(
              ConfigType.class, this, CodePackage.CONFIGS_TYPE__CONFIG);
    }
    return config;
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
      case CodePackage.CONFIGS_TYPE__CONFIG:
        return ((InternalEList<?>) getConfig()).basicRemove(otherEnd, msgs);
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
      case CodePackage.CONFIGS_TYPE__CONFIG:
        return getConfig();
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
      case CodePackage.CONFIGS_TYPE__CONFIG:
        getConfig().clear();
        getConfig().addAll((Collection<? extends ConfigType>) newValue);
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
      case CodePackage.CONFIGS_TYPE__CONFIG:
        getConfig().clear();
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
      case CodePackage.CONFIGS_TYPE__CONFIG:
        return config != null && !config.isEmpty();
    }
    return super.eIsSet(featureID);
  }
} // ConfigsTypeImpl
