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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Element Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl#getSampleUriIndex
 *       <em>Sample Uri Index</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitElementTypeImpl extends EObjectImpl implements ToolkitElementType {
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
   * The default value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected static final String NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getName() <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getName()
   * @generated
   * @ordered
   */
  protected String name = NAME_EDEFAULT;

  /**
   * The default value of the '{@link #getSampleUriIndex() <em>Sample Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSampleUriIndex()
   * @generated
   * @ordered
   */
  protected static final int SAMPLE_URI_INDEX_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getSampleUriIndex() <em>Sample Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSampleUriIndex()
   * @generated
   * @ordered
   */
  protected int sampleUriIndex = SAMPLE_URI_INDEX_EDEFAULT;

  /**
   * This is true if the Sample Uri Index attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean sampleUriIndexESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitElementTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_ELEMENT_TYPE;
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
              ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION,
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
  public String getName() {
    return name;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setName(String newName) {
    String oldName = name;
    name = newName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, ToolkitPackage.TOOLKIT_ELEMENT_TYPE__NAME, oldName, name));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getSampleUriIndex() {
    return sampleUriIndex;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSampleUriIndex(int newSampleUriIndex) {
    int oldSampleUriIndex = sampleUriIndex;
    sampleUriIndex = newSampleUriIndex;
    boolean oldSampleUriIndexESet = sampleUriIndexESet;
    sampleUriIndexESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX,
              oldSampleUriIndex,
              sampleUriIndex,
              !oldSampleUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetSampleUriIndex() {
    int oldSampleUriIndex = sampleUriIndex;
    boolean oldSampleUriIndexESet = sampleUriIndexESet;
    sampleUriIndex = SAMPLE_URI_INDEX_EDEFAULT;
    sampleUriIndexESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX,
              oldSampleUriIndex,
              SAMPLE_URI_INDEX_EDEFAULT,
              oldSampleUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetSampleUriIndex() {
    return sampleUriIndexESet;
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
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
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
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION:
        return getDescription();
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__NAME:
        return getName();
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX:
        return getSampleUriIndex();
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
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__NAME:
        setName((String) newValue);
        return;
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX:
        setSampleUriIndex((Integer) newValue);
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
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX:
        unsetSampleUriIndex();
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
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__DESCRIPTION:
        return description != null;
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX:
        return isSetSampleUriIndex();
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
    result.append(" (name: ");
    result.append(name);
    result.append(", sampleUriIndex: ");
    if (sampleUriIndexESet) result.append(sampleUriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // ToolkitElementTypeImpl
