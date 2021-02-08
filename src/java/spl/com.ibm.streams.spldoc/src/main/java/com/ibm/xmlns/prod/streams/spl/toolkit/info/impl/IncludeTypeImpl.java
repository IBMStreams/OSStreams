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

import com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Include Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl#getPath
 *       <em>Path</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl#getRoot
 *       <em>Root</em>}
 * </ul>
 *
 * @generated
 */
public class IncludeTypeImpl extends EObjectImpl implements IncludeType {
  /**
   * The default value of the '{@link #getPath() <em>Path</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPath()
   * @generated
   * @ordered
   */
  protected static final String PATH_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPath() <em>Path</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPath()
   * @generated
   * @ordered
   */
  protected String path = PATH_EDEFAULT;

  /**
   * The default value of the '{@link #getRoot() <em>Root</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRoot()
   * @generated
   * @ordered
   */
  protected static final RootType ROOT_EDEFAULT = RootType.TOOLKIT_DIR;

  /**
   * The cached value of the '{@link #getRoot() <em>Root</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRoot()
   * @generated
   * @ordered
   */
  protected RootType root = ROOT_EDEFAULT;

  /**
   * This is true if the Root attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean rootESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected IncludeTypeImpl() {
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
    return InfoPackage.Literals.INCLUDE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getPath() {
    return path;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPath(String newPath) {
    String oldPath = path;
    path = newPath;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, InfoPackage.INCLUDE_TYPE__PATH, oldPath, path));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public RootType getRoot() {
    return root;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRoot(RootType newRoot) {
    RootType oldRoot = root;
    root = newRoot == null ? ROOT_EDEFAULT : newRoot;
    boolean oldRootESet = rootESet;
    rootESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, InfoPackage.INCLUDE_TYPE__ROOT, oldRoot, root, !oldRootESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetRoot() {
    RootType oldRoot = root;
    boolean oldRootESet = rootESet;
    root = ROOT_EDEFAULT;
    rootESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              InfoPackage.INCLUDE_TYPE__ROOT,
              oldRoot,
              ROOT_EDEFAULT,
              oldRootESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetRoot() {
    return rootESet;
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
      case InfoPackage.INCLUDE_TYPE__PATH:
        return getPath();
      case InfoPackage.INCLUDE_TYPE__ROOT:
        return getRoot();
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
      case InfoPackage.INCLUDE_TYPE__PATH:
        setPath((String) newValue);
        return;
      case InfoPackage.INCLUDE_TYPE__ROOT:
        setRoot((RootType) newValue);
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
      case InfoPackage.INCLUDE_TYPE__PATH:
        setPath(PATH_EDEFAULT);
        return;
      case InfoPackage.INCLUDE_TYPE__ROOT:
        unsetRoot();
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
      case InfoPackage.INCLUDE_TYPE__PATH:
        return PATH_EDEFAULT == null ? path != null : !PATH_EDEFAULT.equals(path);
      case InfoPackage.INCLUDE_TYPE__ROOT:
        return isSetRoot();
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
    result.append(" (path: ");
    result.append(path);
    result.append(", root: ");
    if (rootESet) result.append(root);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // IncludeTypeImpl
