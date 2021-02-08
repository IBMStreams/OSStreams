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

import com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Base Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl#getImages
 *       <em>Images</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl#isPublic
 *       <em>Public</em>}
 * </ul>
 *
 * @generated
 */
public class BaseToolkitOperatorTypeImpl extends ToolkitElementTypeImpl
    implements BaseToolkitOperatorType {
  /**
   * The cached value of the '{@link #getImages() <em>Images</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getImages()
   * @generated
   * @ordered
   */
  protected ImagesType images;

  /**
   * The default value of the '{@link #isPublic() <em>Public</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPublic()
   * @generated
   * @ordered
   */
  protected static final boolean PUBLIC_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPublic() <em>Public</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPublic()
   * @generated
   * @ordered
   */
  protected boolean public_ = PUBLIC_EDEFAULT;

  /**
   * This is true if the Public attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean publicESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected BaseToolkitOperatorTypeImpl() {
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
    return ToolkitPackage.Literals.BASE_TOOLKIT_OPERATOR_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ImagesType getImages() {
    return images;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetImages(ImagesType newImages, NotificationChain msgs) {
    ImagesType oldImages = images;
    images = newImages;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES,
              oldImages,
              newImages);
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
  public void setImages(ImagesType newImages) {
    if (newImages != images) {
      NotificationChain msgs = null;
      if (images != null)
        msgs =
            ((InternalEObject) images)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES,
                    null,
                    msgs);
      if (newImages != null)
        msgs =
            ((InternalEObject) newImages)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES,
                    null,
                    msgs);
      msgs = basicSetImages(newImages, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES,
              newImages,
              newImages));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isPublic() {
    return public_;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPublic(boolean newPublic) {
    boolean oldPublic = public_;
    public_ = newPublic;
    boolean oldPublicESet = publicESet;
    publicESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC,
              oldPublic,
              public_,
              !oldPublicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPublic() {
    boolean oldPublic = public_;
    boolean oldPublicESet = publicESet;
    public_ = PUBLIC_EDEFAULT;
    publicESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC,
              oldPublic,
              PUBLIC_EDEFAULT,
              oldPublicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPublic() {
    return publicESet;
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
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES:
        return basicSetImages(null, msgs);
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
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES:
        return getImages();
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC:
        return isPublic();
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
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES:
        setImages((ImagesType) newValue);
        return;
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC:
        setPublic((Boolean) newValue);
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
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES:
        setImages((ImagesType) null);
        return;
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC:
        unsetPublic();
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
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__IMAGES:
        return images != null;
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC:
        return isSetPublic();
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
    result.append(" (public: ");
    if (publicESet) result.append(public_);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // BaseToolkitOperatorTypeImpl
