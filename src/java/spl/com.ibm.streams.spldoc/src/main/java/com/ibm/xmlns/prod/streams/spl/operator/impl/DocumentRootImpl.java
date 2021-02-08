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

import com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EMap;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EStructuralFeature;
import org.eclipse.emf.ecore.EcorePackage;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.EStringToStringMapEntryImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.EcoreEMap;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Document Root</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl#getMixed
 *       <em>Mixed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl#getXMLNSPrefixMap
 *       <em>XMLNS Prefix Map</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl#getXSISchemaLocation
 *       <em>XSI Schema Location</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl#getOperatorModel
 *       <em>Operator Model</em>}
 * </ul>
 *
 * @generated
 */
public class DocumentRootImpl extends MinimalEObjectImpl.Container implements DocumentRoot {
  /**
   * The cached value of the '{@link #getMixed() <em>Mixed</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMixed()
   * @generated
   * @ordered
   */
  protected FeatureMap mixed;

  /**
   * The cached value of the '{@link #getXMLNSPrefixMap() <em>XMLNS Prefix Map</em>}' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getXMLNSPrefixMap()
   * @generated
   * @ordered
   */
  protected EMap<String, String> xMLNSPrefixMap;

  /**
   * The cached value of the '{@link #getXSISchemaLocation() <em>XSI Schema Location</em>}' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getXSISchemaLocation()
   * @generated
   * @ordered
   */
  protected EMap<String, String> xSISchemaLocation;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected DocumentRootImpl() {
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
    return OperatorPackage.Literals.DOCUMENT_ROOT;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FeatureMap getMixed() {
    if (mixed == null) {
      mixed = new BasicFeatureMap(this, OperatorPackage.DOCUMENT_ROOT__MIXED);
    }
    return mixed;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EMap<String, String> getXMLNSPrefixMap() {
    if (xMLNSPrefixMap == null) {
      xMLNSPrefixMap =
          new EcoreEMap<String, String>(
              EcorePackage.Literals.ESTRING_TO_STRING_MAP_ENTRY,
              EStringToStringMapEntryImpl.class,
              this,
              OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    }
    return xMLNSPrefixMap;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EMap<String, String> getXSISchemaLocation() {
    if (xSISchemaLocation == null) {
      xSISchemaLocation =
          new EcoreEMap<String, String>(
              EcorePackage.Literals.ESTRING_TO_STRING_MAP_ENTRY,
              EStringToStringMapEntryImpl.class,
              this,
              OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    }
    return xSISchemaLocation;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorModelType getOperatorModel() {
    return (OperatorModelType)
        getMixed().get(OperatorPackage.Literals.DOCUMENT_ROOT__OPERATOR_MODEL, true);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOperatorModel(
      OperatorModelType newOperatorModel, NotificationChain msgs) {
    return ((FeatureMap.Internal) getMixed())
        .basicAdd(OperatorPackage.Literals.DOCUMENT_ROOT__OPERATOR_MODEL, newOperatorModel, msgs);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOperatorModel(OperatorModelType newOperatorModel) {
    ((FeatureMap.Internal) getMixed())
        .set(OperatorPackage.Literals.DOCUMENT_ROOT__OPERATOR_MODEL, newOperatorModel);
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
      case OperatorPackage.DOCUMENT_ROOT__MIXED:
        return ((InternalEList<?>) getMixed()).basicRemove(otherEnd, msgs);
      case OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        return ((InternalEList<?>) getXMLNSPrefixMap()).basicRemove(otherEnd, msgs);
      case OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        return ((InternalEList<?>) getXSISchemaLocation()).basicRemove(otherEnd, msgs);
      case OperatorPackage.DOCUMENT_ROOT__OPERATOR_MODEL:
        return basicSetOperatorModel(null, msgs);
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
      case OperatorPackage.DOCUMENT_ROOT__MIXED:
        if (coreType) return getMixed();
        return ((FeatureMap.Internal) getMixed()).getWrapper();
      case OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        if (coreType) return getXMLNSPrefixMap();
        else return getXMLNSPrefixMap().map();
      case OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        if (coreType) return getXSISchemaLocation();
        else return getXSISchemaLocation().map();
      case OperatorPackage.DOCUMENT_ROOT__OPERATOR_MODEL:
        return getOperatorModel();
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
      case OperatorPackage.DOCUMENT_ROOT__MIXED:
        ((FeatureMap.Internal) getMixed()).set(newValue);
        return;
      case OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        ((EStructuralFeature.Setting) getXMLNSPrefixMap()).set(newValue);
        return;
      case OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        ((EStructuralFeature.Setting) getXSISchemaLocation()).set(newValue);
        return;
      case OperatorPackage.DOCUMENT_ROOT__OPERATOR_MODEL:
        setOperatorModel((OperatorModelType) newValue);
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
      case OperatorPackage.DOCUMENT_ROOT__MIXED:
        getMixed().clear();
        return;
      case OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        getXMLNSPrefixMap().clear();
        return;
      case OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        getXSISchemaLocation().clear();
        return;
      case OperatorPackage.DOCUMENT_ROOT__OPERATOR_MODEL:
        setOperatorModel((OperatorModelType) null);
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
      case OperatorPackage.DOCUMENT_ROOT__MIXED:
        return mixed != null && !mixed.isEmpty();
      case OperatorPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        return xMLNSPrefixMap != null && !xMLNSPrefixMap.isEmpty();
      case OperatorPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        return xSISchemaLocation != null && !xSISchemaLocation.isEmpty();
      case OperatorPackage.DOCUMENT_ROOT__OPERATOR_MODEL:
        return getOperatorModel() != null;
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
    result.append(" (mixed: ");
    result.append(mixed);
    result.append(')');
    return result.toString();
  }
} // DocumentRootImpl
