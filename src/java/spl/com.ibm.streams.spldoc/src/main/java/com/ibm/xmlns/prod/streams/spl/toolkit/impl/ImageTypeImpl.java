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

import com.ibm.xmlns.prod.streams.spl.toolkit.ImageType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Image Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl#getSize <em>Size</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl#getUriIndex <em>Uri
 *       Index</em>}
 * </ul>
 *
 * @generated
 */
public class ImageTypeImpl extends EObjectImpl implements ImageType {
  /**
   * The default value of the '{@link #getSize() <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSize()
   * @generated
   * @ordered
   */
  protected static final int SIZE_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getSize() <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSize()
   * @generated
   * @ordered
   */
  protected int size = SIZE_EDEFAULT;

  /**
   * This is true if the Size attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean sizeESet;

  /**
   * The default value of the '{@link #getUriIndex() <em>Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUriIndex()
   * @generated
   * @ordered
   */
  protected static final int URI_INDEX_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getUriIndex() <em>Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUriIndex()
   * @generated
   * @ordered
   */
  protected int uriIndex = URI_INDEX_EDEFAULT;

  /**
   * This is true if the Uri Index attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean uriIndexESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ImageTypeImpl() {
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
    return ToolkitPackage.Literals.IMAGE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getSize() {
    return size;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSize(int newSize) {
    int oldSize = size;
    size = newSize;
    boolean oldSizeESet = sizeESet;
    sizeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.IMAGE_TYPE__SIZE,
              oldSize,
              size,
              !oldSizeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetSize() {
    int oldSize = size;
    boolean oldSizeESet = sizeESet;
    size = SIZE_EDEFAULT;
    sizeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.IMAGE_TYPE__SIZE,
              oldSize,
              SIZE_EDEFAULT,
              oldSizeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetSize() {
    return sizeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getUriIndex() {
    return uriIndex;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setUriIndex(int newUriIndex) {
    int oldUriIndex = uriIndex;
    uriIndex = newUriIndex;
    boolean oldUriIndexESet = uriIndexESet;
    uriIndexESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.IMAGE_TYPE__URI_INDEX,
              oldUriIndex,
              uriIndex,
              !oldUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetUriIndex() {
    int oldUriIndex = uriIndex;
    boolean oldUriIndexESet = uriIndexESet;
    uriIndex = URI_INDEX_EDEFAULT;
    uriIndexESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.IMAGE_TYPE__URI_INDEX,
              oldUriIndex,
              URI_INDEX_EDEFAULT,
              oldUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetUriIndex() {
    return uriIndexESet;
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
      case ToolkitPackage.IMAGE_TYPE__SIZE:
        return getSize();
      case ToolkitPackage.IMAGE_TYPE__URI_INDEX:
        return getUriIndex();
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
      case ToolkitPackage.IMAGE_TYPE__SIZE:
        setSize((Integer) newValue);
        return;
      case ToolkitPackage.IMAGE_TYPE__URI_INDEX:
        setUriIndex((Integer) newValue);
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
      case ToolkitPackage.IMAGE_TYPE__SIZE:
        unsetSize();
        return;
      case ToolkitPackage.IMAGE_TYPE__URI_INDEX:
        unsetUriIndex();
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
      case ToolkitPackage.IMAGE_TYPE__SIZE:
        return isSetSize();
      case ToolkitPackage.IMAGE_TYPE__URI_INDEX:
        return isSetUriIndex();
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
    result.append(" (size: ");
    if (sizeESet) result.append(size);
    else result.append("<unset>");
    result.append(", uriIndex: ");
    if (uriIndexESet) result.append(uriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // ImageTypeImpl
