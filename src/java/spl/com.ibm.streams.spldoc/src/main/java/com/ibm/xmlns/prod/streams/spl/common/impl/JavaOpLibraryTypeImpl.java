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
import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Java Op Library Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl#getManagedLibrary
 *       <em>Managed Library</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpLibraryTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpLibraryType {
  /**
   * The cached value of the '{@link #getDescription() <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDescription()
   * @generated
   * @ordered
   */
  protected DescriptionType description;

  /**
   * The cached value of the '{@link #getManagedLibrary() <em>Managed Library</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getManagedLibrary()
   * @generated
   * @ordered
   */
  protected JavaOpManagedLibraryType managedLibrary;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpLibraryTypeImpl() {
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
    return CommonPackage.Literals.JAVA_OP_LIBRARY_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DescriptionType getDescription() {
    return description;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetDescription(
      DescriptionType newDescription, NotificationChain msgs) {
    DescriptionType oldDescription = description;
    description = newDescription;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION,
              oldDescription,
              newDescription);
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
  public void setDescription(DescriptionType newDescription) {
    if (newDescription != description) {
      NotificationChain msgs = null;
      if (description != null)
        msgs =
            ((InternalEObject) description)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION,
              newDescription,
              newDescription));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpManagedLibraryType getManagedLibrary() {
    return managedLibrary;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetManagedLibrary(
      JavaOpManagedLibraryType newManagedLibrary, NotificationChain msgs) {
    JavaOpManagedLibraryType oldManagedLibrary = managedLibrary;
    managedLibrary = newManagedLibrary;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY,
              oldManagedLibrary,
              newManagedLibrary);
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
  public void setManagedLibrary(JavaOpManagedLibraryType newManagedLibrary) {
    if (newManagedLibrary != managedLibrary) {
      NotificationChain msgs = null;
      if (managedLibrary != null)
        msgs =
            ((InternalEObject) managedLibrary)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY,
                    null,
                    msgs);
      if (newManagedLibrary != null)
        msgs =
            ((InternalEObject) newManagedLibrary)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY,
                    null,
                    msgs);
      msgs = basicSetManagedLibrary(newManagedLibrary, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY,
              newManagedLibrary,
              newManagedLibrary));
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
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY:
        return basicSetManagedLibrary(null, msgs);
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
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION:
        return getDescription();
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY:
        return getManagedLibrary();
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
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY:
        setManagedLibrary((JavaOpManagedLibraryType) newValue);
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
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY:
        setManagedLibrary((JavaOpManagedLibraryType) null);
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
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__DESCRIPTION:
        return description != null;
      case CommonPackage.JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY:
        return managedLibrary != null;
    }
    return super.eIsSet(featureID);
  }
} // JavaOpLibraryTypeImpl
