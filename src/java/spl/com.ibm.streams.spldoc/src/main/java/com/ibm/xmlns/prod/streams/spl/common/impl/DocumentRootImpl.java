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
package com.ibm.xmlns.prod.streams.spl.common.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.LibraryType;
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
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl#getMixed <em>Mixed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl#getXMLNSPrefixMap
 *       <em>XMLNS Prefix Map</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl#getXSISchemaLocation
 *       <em>XSI Schema Location</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl#getJavaOpLibrary
 *       <em>Java Op Library</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl#getLibrary
 *       <em>Library</em>}
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
    return CommonPackage.Literals.DOCUMENT_ROOT;
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
      mixed = new BasicFeatureMap(this, CommonPackage.DOCUMENT_ROOT__MIXED);
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
              CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
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
              CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
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
  public JavaOpLibraryType getJavaOpLibrary() {
    return (JavaOpLibraryType)
        getMixed().get(CommonPackage.Literals.DOCUMENT_ROOT__JAVA_OP_LIBRARY, true);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetJavaOpLibrary(
      JavaOpLibraryType newJavaOpLibrary, NotificationChain msgs) {
    return ((FeatureMap.Internal) getMixed())
        .basicAdd(CommonPackage.Literals.DOCUMENT_ROOT__JAVA_OP_LIBRARY, newJavaOpLibrary, msgs);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setJavaOpLibrary(JavaOpLibraryType newJavaOpLibrary) {
    ((FeatureMap.Internal) getMixed())
        .set(CommonPackage.Literals.DOCUMENT_ROOT__JAVA_OP_LIBRARY, newJavaOpLibrary);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LibraryType getLibrary() {
    return (LibraryType) getMixed().get(CommonPackage.Literals.DOCUMENT_ROOT__LIBRARY, true);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetLibrary(LibraryType newLibrary, NotificationChain msgs) {
    return ((FeatureMap.Internal) getMixed())
        .basicAdd(CommonPackage.Literals.DOCUMENT_ROOT__LIBRARY, newLibrary, msgs);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setLibrary(LibraryType newLibrary) {
    ((FeatureMap.Internal) getMixed())
        .set(CommonPackage.Literals.DOCUMENT_ROOT__LIBRARY, newLibrary);
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
      case CommonPackage.DOCUMENT_ROOT__MIXED:
        return ((InternalEList<?>) getMixed()).basicRemove(otherEnd, msgs);
      case CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        return ((InternalEList<?>) getXMLNSPrefixMap()).basicRemove(otherEnd, msgs);
      case CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        return ((InternalEList<?>) getXSISchemaLocation()).basicRemove(otherEnd, msgs);
      case CommonPackage.DOCUMENT_ROOT__JAVA_OP_LIBRARY:
        return basicSetJavaOpLibrary(null, msgs);
      case CommonPackage.DOCUMENT_ROOT__LIBRARY:
        return basicSetLibrary(null, msgs);
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
      case CommonPackage.DOCUMENT_ROOT__MIXED:
        if (coreType) return getMixed();
        return ((FeatureMap.Internal) getMixed()).getWrapper();
      case CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        if (coreType) return getXMLNSPrefixMap();
        else return getXMLNSPrefixMap().map();
      case CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        if (coreType) return getXSISchemaLocation();
        else return getXSISchemaLocation().map();
      case CommonPackage.DOCUMENT_ROOT__JAVA_OP_LIBRARY:
        return getJavaOpLibrary();
      case CommonPackage.DOCUMENT_ROOT__LIBRARY:
        return getLibrary();
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
      case CommonPackage.DOCUMENT_ROOT__MIXED:
        ((FeatureMap.Internal) getMixed()).set(newValue);
        return;
      case CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        ((EStructuralFeature.Setting) getXMLNSPrefixMap()).set(newValue);
        return;
      case CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        ((EStructuralFeature.Setting) getXSISchemaLocation()).set(newValue);
        return;
      case CommonPackage.DOCUMENT_ROOT__JAVA_OP_LIBRARY:
        setJavaOpLibrary((JavaOpLibraryType) newValue);
        return;
      case CommonPackage.DOCUMENT_ROOT__LIBRARY:
        setLibrary((LibraryType) newValue);
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
      case CommonPackage.DOCUMENT_ROOT__MIXED:
        getMixed().clear();
        return;
      case CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        getXMLNSPrefixMap().clear();
        return;
      case CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        getXSISchemaLocation().clear();
        return;
      case CommonPackage.DOCUMENT_ROOT__JAVA_OP_LIBRARY:
        setJavaOpLibrary((JavaOpLibraryType) null);
        return;
      case CommonPackage.DOCUMENT_ROOT__LIBRARY:
        setLibrary((LibraryType) null);
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
      case CommonPackage.DOCUMENT_ROOT__MIXED:
        return mixed != null && !mixed.isEmpty();
      case CommonPackage.DOCUMENT_ROOT__XMLNS_PREFIX_MAP:
        return xMLNSPrefixMap != null && !xMLNSPrefixMap.isEmpty();
      case CommonPackage.DOCUMENT_ROOT__XSI_SCHEMA_LOCATION:
        return xSISchemaLocation != null && !xSISchemaLocation.isEmpty();
      case CommonPackage.DOCUMENT_ROOT__JAVA_OP_LIBRARY:
        return getJavaOpLibrary() != null;
      case CommonPackage.DOCUMENT_ROOT__LIBRARY:
        return getLibrary() != null;
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
