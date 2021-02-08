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

import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getGroup <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getNamespace
 *       <em>Namespace</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getDependency
 *       <em>Dependency</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getSabFiles <em>Sab
 *       Files</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#isHasNLResources <em>Has
 *       NL Resources</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getRequiredProductVersion
 *       <em>Required Product Version</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getUri <em>Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl#getVersion
 *       <em>Version</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitTypeImpl extends ToolkitBaseTypeImpl implements ToolkitType {
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
   * The cached value of the '{@link #getSabFiles() <em>Sab Files</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSabFiles()
   * @generated
   * @ordered
   */
  protected SabFilesType sabFiles;
  /**
   * The default value of the '{@link #isHasNLResources() <em>Has NL Resources</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isHasNLResources()
   * @generated
   * @ordered
   */
  protected static final boolean HAS_NL_RESOURCES_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isHasNLResources() <em>Has NL Resources</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isHasNLResources()
   * @generated
   * @ordered
   */
  protected boolean hasNLResources = HAS_NL_RESOURCES_EDEFAULT;

  /**
   * This is true if the Has NL Resources attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean hasNLResourcesESet;

  /**
   * The default value of the '{@link #getRequiredProductVersion() <em>Required Product
   * Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRequiredProductVersion()
   * @generated
   * @ordered
   */
  protected static final String REQUIRED_PRODUCT_VERSION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getRequiredProductVersion() <em>Required Product
   * Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRequiredProductVersion()
   * @generated
   * @ordered
   */
  protected String requiredProductVersion = REQUIRED_PRODUCT_VERSION_EDEFAULT;

  /**
   * The default value of the '{@link #getUri() <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUri()
   * @generated
   * @ordered
   */
  protected static final String URI_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getUri() <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUri()
   * @generated
   * @ordered
   */
  protected String uri = URI_EDEFAULT;

  /**
   * The default value of the '{@link #getVersion() <em>Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getVersion()
   * @generated
   * @ordered
   */
  protected static final String VERSION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getVersion() <em>Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getVersion()
   * @generated
   * @ordered
   */
  protected String version = VERSION_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_TYPE;
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
      group = new BasicFeatureMap(this, ToolkitPackage.TOOLKIT_TYPE__GROUP);
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
  public EList<ToolkitNamespaceType> getNamespace() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_TYPE__NAMESPACE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitDependencyType> getDependency() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_TYPE__DEPENDENCY);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SabFilesType getSabFiles() {
    return sabFiles;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSabFiles(SabFilesType newSabFiles, NotificationChain msgs) {
    SabFilesType oldSabFiles = sabFiles;
    sabFiles = newSabFiles;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE__SAB_FILES,
              oldSabFiles,
              newSabFiles);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSabFiles(SabFilesType newSabFiles) {
    if (newSabFiles != sabFiles) {
      NotificationChain msgs = null;
      if (sabFiles != null)
        msgs =
            ((InternalEObject) sabFiles)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_TYPE__SAB_FILES,
                    null,
                    msgs);
      if (newSabFiles != null)
        msgs =
            ((InternalEObject) newSabFiles)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_TYPE__SAB_FILES,
                    null,
                    msgs);
      msgs = basicSetSabFiles(newSabFiles, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE__SAB_FILES,
              newSabFiles,
              newSabFiles));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isHasNLResources() {
    return hasNLResources;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setHasNLResources(boolean newHasNLResources) {
    boolean oldHasNLResources = hasNLResources;
    hasNLResources = newHasNLResources;
    boolean oldHasNLResourcesESet = hasNLResourcesESet;
    hasNLResourcesESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES,
              oldHasNLResources,
              hasNLResources,
              !oldHasNLResourcesESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetHasNLResources() {
    boolean oldHasNLResources = hasNLResources;
    boolean oldHasNLResourcesESet = hasNLResourcesESet;
    hasNLResources = HAS_NL_RESOURCES_EDEFAULT;
    hasNLResourcesESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES,
              oldHasNLResources,
              HAS_NL_RESOURCES_EDEFAULT,
              oldHasNLResourcesESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetHasNLResources() {
    return hasNLResourcesESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getRequiredProductVersion() {
    return requiredProductVersion;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRequiredProductVersion(String newRequiredProductVersion) {
    String oldRequiredProductVersion = requiredProductVersion;
    requiredProductVersion = newRequiredProductVersion;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION,
              oldRequiredProductVersion,
              requiredProductVersion));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getUri() {
    return uri;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setUri(String newUri) {
    String oldUri = uri;
    uri = newUri;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, ToolkitPackage.TOOLKIT_TYPE__URI, oldUri, uri));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getVersion() {
    return version;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setVersion(String newVersion) {
    String oldVersion = version;
    version = newVersion;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, ToolkitPackage.TOOLKIT_TYPE__VERSION, oldVersion, version));
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
      case ToolkitPackage.TOOLKIT_TYPE__GROUP:
        return ((InternalEList<?>) getGroup()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_TYPE__NAMESPACE:
        return ((InternalEList<?>) getNamespace()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_TYPE__DEPENDENCY:
        return ((InternalEList<?>) getDependency()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_TYPE__SAB_FILES:
        return basicSetSabFiles(null, msgs);
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
      case ToolkitPackage.TOOLKIT_TYPE__GROUP:
        if (coreType) return getGroup();
        return ((FeatureMap.Internal) getGroup()).getWrapper();
      case ToolkitPackage.TOOLKIT_TYPE__NAMESPACE:
        return getNamespace();
      case ToolkitPackage.TOOLKIT_TYPE__DEPENDENCY:
        return getDependency();
      case ToolkitPackage.TOOLKIT_TYPE__SAB_FILES:
        return getSabFiles();
      case ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES:
        return isHasNLResources();
      case ToolkitPackage.TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION:
        return getRequiredProductVersion();
      case ToolkitPackage.TOOLKIT_TYPE__URI:
        return getUri();
      case ToolkitPackage.TOOLKIT_TYPE__VERSION:
        return getVersion();
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
      case ToolkitPackage.TOOLKIT_TYPE__GROUP:
        ((FeatureMap.Internal) getGroup()).set(newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__NAMESPACE:
        getNamespace().clear();
        getNamespace().addAll((Collection<? extends ToolkitNamespaceType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__DEPENDENCY:
        getDependency().clear();
        getDependency().addAll((Collection<? extends ToolkitDependencyType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__SAB_FILES:
        setSabFiles((SabFilesType) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES:
        setHasNLResources((Boolean) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION:
        setRequiredProductVersion((String) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__URI:
        setUri((String) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__VERSION:
        setVersion((String) newValue);
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
      case ToolkitPackage.TOOLKIT_TYPE__GROUP:
        getGroup().clear();
        return;
      case ToolkitPackage.TOOLKIT_TYPE__NAMESPACE:
        getNamespace().clear();
        return;
      case ToolkitPackage.TOOLKIT_TYPE__DEPENDENCY:
        getDependency().clear();
        return;
      case ToolkitPackage.TOOLKIT_TYPE__SAB_FILES:
        setSabFiles((SabFilesType) null);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES:
        unsetHasNLResources();
        return;
      case ToolkitPackage.TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION:
        setRequiredProductVersion(REQUIRED_PRODUCT_VERSION_EDEFAULT);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__URI:
        setUri(URI_EDEFAULT);
        return;
      case ToolkitPackage.TOOLKIT_TYPE__VERSION:
        setVersion(VERSION_EDEFAULT);
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
      case ToolkitPackage.TOOLKIT_TYPE__GROUP:
        return group != null && !group.isEmpty();
      case ToolkitPackage.TOOLKIT_TYPE__NAMESPACE:
        return !getNamespace().isEmpty();
      case ToolkitPackage.TOOLKIT_TYPE__DEPENDENCY:
        return !getDependency().isEmpty();
      case ToolkitPackage.TOOLKIT_TYPE__SAB_FILES:
        return sabFiles != null;
      case ToolkitPackage.TOOLKIT_TYPE__HAS_NL_RESOURCES:
        return isSetHasNLResources();
      case ToolkitPackage.TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION:
        return REQUIRED_PRODUCT_VERSION_EDEFAULT == null
            ? requiredProductVersion != null
            : !REQUIRED_PRODUCT_VERSION_EDEFAULT.equals(requiredProductVersion);
      case ToolkitPackage.TOOLKIT_TYPE__URI:
        return URI_EDEFAULT == null ? uri != null : !URI_EDEFAULT.equals(uri);
      case ToolkitPackage.TOOLKIT_TYPE__VERSION:
        return VERSION_EDEFAULT == null ? version != null : !VERSION_EDEFAULT.equals(version);
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
    result.append(", hasNLResources: ");
    if (hasNLResourcesESet) result.append(hasNLResources);
    else result.append("<unset>");
    result.append(", requiredProductVersion: ");
    result.append(requiredProductVersion);
    result.append(", uri: ");
    result.append(uri);
    result.append(", version: ");
    result.append(version);
    result.append(')');
    return result.toString();
  }
} // ToolkitTypeImpl
