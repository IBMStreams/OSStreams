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

import com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType;
import java.util.Collection;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Sab Files Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl#getGroup
 *       <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl#getInclude
 *       <em>Include</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl#getExclude
 *       <em>Exclude</em>}
 * </ul>
 *
 * @generated
 */
public class SabFilesTypeImpl extends EObjectImpl implements SabFilesType {
  /**
   * The cached value of the '{@link #getGroup() <em>Group</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getGroup()
   * @generated
   * @ordered
   */
  protected FeatureMap group;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected SabFilesTypeImpl() {
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
    return InfoPackage.Literals.SAB_FILES_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FeatureMap getGroup() {
    if (group == null) {
      group = new BasicFeatureMap(this, InfoPackage.SAB_FILES_TYPE__GROUP);
    }
    return group;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<IncludeType> getInclude() {
    return getGroup().list(InfoPackage.Literals.SAB_FILES_TYPE__INCLUDE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ExcludeType> getExclude() {
    return getGroup().list(InfoPackage.Literals.SAB_FILES_TYPE__EXCLUDE);
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
      case InfoPackage.SAB_FILES_TYPE__GROUP:
        return ((InternalEList<?>) getGroup()).basicRemove(otherEnd, msgs);
      case InfoPackage.SAB_FILES_TYPE__INCLUDE:
        return ((InternalEList<?>) getInclude()).basicRemove(otherEnd, msgs);
      case InfoPackage.SAB_FILES_TYPE__EXCLUDE:
        return ((InternalEList<?>) getExclude()).basicRemove(otherEnd, msgs);
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
      case InfoPackage.SAB_FILES_TYPE__GROUP:
        if (coreType) return getGroup();
        return ((FeatureMap.Internal) getGroup()).getWrapper();
      case InfoPackage.SAB_FILES_TYPE__INCLUDE:
        return getInclude();
      case InfoPackage.SAB_FILES_TYPE__EXCLUDE:
        return getExclude();
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
      case InfoPackage.SAB_FILES_TYPE__GROUP:
        ((FeatureMap.Internal) getGroup()).set(newValue);
        return;
      case InfoPackage.SAB_FILES_TYPE__INCLUDE:
        getInclude().clear();
        getInclude().addAll((Collection<? extends IncludeType>) newValue);
        return;
      case InfoPackage.SAB_FILES_TYPE__EXCLUDE:
        getExclude().clear();
        getExclude().addAll((Collection<? extends ExcludeType>) newValue);
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
      case InfoPackage.SAB_FILES_TYPE__GROUP:
        getGroup().clear();
        return;
      case InfoPackage.SAB_FILES_TYPE__INCLUDE:
        getInclude().clear();
        return;
      case InfoPackage.SAB_FILES_TYPE__EXCLUDE:
        getExclude().clear();
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
      case InfoPackage.SAB_FILES_TYPE__GROUP:
        return group != null && !group.isEmpty();
      case InfoPackage.SAB_FILES_TYPE__INCLUDE:
        return !getInclude().isEmpty();
      case InfoPackage.SAB_FILES_TYPE__EXCLUDE:
        return !getExclude().isEmpty();
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
    result.append(" (group: ");
    result.append(group);
    result.append(')');
    return result.toString();
  }
} // SabFilesTypeImpl
