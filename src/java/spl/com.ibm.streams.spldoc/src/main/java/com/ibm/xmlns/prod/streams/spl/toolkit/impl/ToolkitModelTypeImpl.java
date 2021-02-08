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

import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl#getToolkit
 *       <em>Toolkit</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl#getProductVersion
 *       <em>Product Version</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitModelTypeImpl extends EObjectImpl implements ToolkitModelType {
  /**
   * The cached value of the '{@link #getToolkit() <em>Toolkit</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getToolkit()
   * @generated
   * @ordered
   */
  protected EList<ToolkitType> toolkit;

  /**
   * The default value of the '{@link #getProductVersion() <em>Product Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getProductVersion()
   * @generated
   * @ordered
   */
  protected static final String PRODUCT_VERSION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getProductVersion() <em>Product Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getProductVersion()
   * @generated
   * @ordered
   */
  protected String productVersion = PRODUCT_VERSION_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitModelTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitType> getToolkit() {
    if (toolkit == null) {
      toolkit =
          new EObjectContainmentEList<ToolkitType>(
              ToolkitType.class, this, ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT);
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
  public String getProductVersion() {
    return productVersion;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setProductVersion(String newProductVersion) {
    String oldProductVersion = productVersion;
    productVersion = newProductVersion;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_MODEL_TYPE__PRODUCT_VERSION,
              oldProductVersion,
              productVersion));
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
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT:
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
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT:
        return getToolkit();
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__PRODUCT_VERSION:
        return getProductVersion();
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
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT:
        getToolkit().clear();
        getToolkit().addAll((Collection<? extends ToolkitType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__PRODUCT_VERSION:
        setProductVersion((String) newValue);
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
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT:
        getToolkit().clear();
        return;
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__PRODUCT_VERSION:
        setProductVersion(PRODUCT_VERSION_EDEFAULT);
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
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__TOOLKIT:
        return toolkit != null && !toolkit.isEmpty();
      case ToolkitPackage.TOOLKIT_MODEL_TYPE__PRODUCT_VERSION:
        return PRODUCT_VERSION_EDEFAULT == null
            ? productVersion != null
            : !PRODUCT_VERSION_EDEFAULT.equals(productVersion);
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
    result.append(" (productVersion: ");
    result.append(productVersion);
    result.append(')');
    return result.toString();
  }
} // ToolkitModelTypeImpl
