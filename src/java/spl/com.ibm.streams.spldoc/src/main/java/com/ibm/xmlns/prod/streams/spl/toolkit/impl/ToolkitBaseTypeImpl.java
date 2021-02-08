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

import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Base Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitBaseTypeImpl#getUriTable <em>Uri
 *       Table</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitBaseTypeImpl extends ToolkitElementTypeImpl implements ToolkitBaseType {
  /**
   * The cached value of the '{@link #getUriTable() <em>Uri Table</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUriTable()
   * @generated
   * @ordered
   */
  protected UriTableType uriTable;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitBaseTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_BASE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UriTableType getUriTable() {
    return uriTable;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetUriTable(UriTableType newUriTable, NotificationChain msgs) {
    UriTableType oldUriTable = uriTable;
    uriTable = newUriTable;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE,
              oldUriTable,
              newUriTable);
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
  public void setUriTable(UriTableType newUriTable) {
    if (newUriTable != uriTable) {
      NotificationChain msgs = null;
      if (uriTable != null)
        msgs =
            ((InternalEObject) uriTable)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE,
                    null,
                    msgs);
      if (newUriTable != null)
        msgs =
            ((InternalEObject) newUriTable)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE,
                    null,
                    msgs);
      msgs = basicSetUriTable(newUriTable, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE,
              newUriTable,
              newUriTable));
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
      case ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE:
        return basicSetUriTable(null, msgs);
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
      case ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE:
        return getUriTable();
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
      case ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE:
        setUriTable((UriTableType) newValue);
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
      case ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE:
        setUriTable((UriTableType) null);
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
      case ToolkitPackage.TOOLKIT_BASE_TYPE__URI_TABLE:
        return uriTable != null;
    }
    return super.eIsSet(featureID);
  }
} // ToolkitBaseTypeImpl
