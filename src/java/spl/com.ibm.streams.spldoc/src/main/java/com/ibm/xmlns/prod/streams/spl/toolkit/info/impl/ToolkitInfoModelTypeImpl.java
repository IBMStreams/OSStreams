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

import com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Toolkit Info Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl#getIdentity
 *       <em>Identity</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl#getDependencies
 *       <em>Dependencies</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl#getResources
 *       <em>Resources</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl#getSabFiles
 *       <em>Sab Files</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitInfoModelTypeImpl extends EObjectImpl implements ToolkitInfoModelType {
  /**
   * The cached value of the '{@link #getIdentity() <em>Identity</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIdentity()
   * @generated
   * @ordered
   */
  protected IdentityType identity;

  /**
   * The cached value of the '{@link #getDependencies() <em>Dependencies</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDependencies()
   * @generated
   * @ordered
   */
  protected DependenciesType dependencies;

  /**
   * The cached value of the '{@link #getResources() <em>Resources</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getResources()
   * @generated
   * @ordered
   */
  protected ResourcesType resources;

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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitInfoModelTypeImpl() {
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
    return InfoPackage.Literals.TOOLKIT_INFO_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IdentityType getIdentity() {
    return identity;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetIdentity(IdentityType newIdentity, NotificationChain msgs) {
    IdentityType oldIdentity = identity;
    identity = newIdentity;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY,
              oldIdentity,
              newIdentity);
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
  public void setIdentity(IdentityType newIdentity) {
    if (newIdentity != identity) {
      NotificationChain msgs = null;
      if (identity != null)
        msgs =
            ((InternalEObject) identity)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY,
                    null,
                    msgs);
      if (newIdentity != null)
        msgs =
            ((InternalEObject) newIdentity)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY,
                    null,
                    msgs);
      msgs = basicSetIdentity(newIdentity, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY,
              newIdentity,
              newIdentity));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DependenciesType getDependencies() {
    return dependencies;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetDependencies(
      DependenciesType newDependencies, NotificationChain msgs) {
    DependenciesType oldDependencies = dependencies;
    dependencies = newDependencies;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES,
              oldDependencies,
              newDependencies);
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
  public void setDependencies(DependenciesType newDependencies) {
    if (newDependencies != dependencies) {
      NotificationChain msgs = null;
      if (dependencies != null)
        msgs =
            ((InternalEObject) dependencies)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES,
                    null,
                    msgs);
      if (newDependencies != null)
        msgs =
            ((InternalEObject) newDependencies)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES,
                    null,
                    msgs);
      msgs = basicSetDependencies(newDependencies, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES,
              newDependencies,
              newDependencies));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ResourcesType getResources() {
    return resources;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetResources(ResourcesType newResources, NotificationChain msgs) {
    ResourcesType oldResources = resources;
    resources = newResources;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES,
              oldResources,
              newResources);
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
  public void setResources(ResourcesType newResources) {
    if (newResources != resources) {
      NotificationChain msgs = null;
      if (resources != null)
        msgs =
            ((InternalEObject) resources)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES,
                    null,
                    msgs);
      if (newResources != null)
        msgs =
            ((InternalEObject) newResources)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES,
                    null,
                    msgs);
      msgs = basicSetResources(newResources, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES,
              newResources,
              newResources));
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
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES,
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
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES,
                    null,
                    msgs);
      if (newSabFiles != null)
        msgs =
            ((InternalEObject) newSabFiles)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES,
                    null,
                    msgs);
      msgs = basicSetSabFiles(newSabFiles, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES,
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
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY:
        return basicSetIdentity(null, msgs);
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES:
        return basicSetDependencies(null, msgs);
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES:
        return basicSetResources(null, msgs);
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES:
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
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY:
        return getIdentity();
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES:
        return getDependencies();
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES:
        return getResources();
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES:
        return getSabFiles();
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
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY:
        setIdentity((IdentityType) newValue);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES:
        setDependencies((DependenciesType) newValue);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES:
        setResources((ResourcesType) newValue);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES:
        setSabFiles((SabFilesType) newValue);
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
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY:
        setIdentity((IdentityType) null);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES:
        setDependencies((DependenciesType) null);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES:
        setResources((ResourcesType) null);
        return;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES:
        setSabFiles((SabFilesType) null);
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
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__IDENTITY:
        return identity != null;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES:
        return dependencies != null;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__RESOURCES:
        return resources != null;
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE__SAB_FILES:
        return sabFiles != null;
    }
    return super.eIsSet(featureID);
  }
} // ToolkitInfoModelTypeImpl
