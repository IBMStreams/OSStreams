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
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Description Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl#getValue
 *       <em>Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl#getDocHref <em>Doc
 *       Href</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl#getSampleUri
 *       <em>Sample Uri</em>}
 * </ul>
 *
 * @generated
 */
public class DescriptionTypeImpl extends MinimalEObjectImpl.Container implements DescriptionType {
  /**
   * The default value of the '{@link #getValue() <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getValue()
   * @generated
   * @ordered
   */
  protected static final String VALUE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getValue() <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getValue()
   * @generated
   * @ordered
   */
  protected String value = VALUE_EDEFAULT;

  /**
   * The default value of the '{@link #getDocHref() <em>Doc Href</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDocHref()
   * @generated
   * @ordered
   */
  protected static final String DOC_HREF_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getDocHref() <em>Doc Href</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDocHref()
   * @generated
   * @ordered
   */
  protected String docHref = DOC_HREF_EDEFAULT;

  /**
   * The default value of the '{@link #getSampleUri() <em>Sample Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSampleUri()
   * @generated
   * @ordered
   */
  protected static final String SAMPLE_URI_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getSampleUri() <em>Sample Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSampleUri()
   * @generated
   * @ordered
   */
  protected String sampleUri = SAMPLE_URI_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected DescriptionTypeImpl() {
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
    return CommonPackage.Literals.DESCRIPTION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getValue() {
    return value;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setValue(String newValue) {
    String oldValue = value;
    value = newValue;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CommonPackage.DESCRIPTION_TYPE__VALUE, oldValue, value));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getDocHref() {
    return docHref;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setDocHref(String newDocHref) {
    String oldDocHref = docHref;
    docHref = newDocHref;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.DESCRIPTION_TYPE__DOC_HREF,
              oldDocHref,
              docHref));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getSampleUri() {
    return sampleUri;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSampleUri(String newSampleUri) {
    String oldSampleUri = sampleUri;
    sampleUri = newSampleUri;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.DESCRIPTION_TYPE__SAMPLE_URI,
              oldSampleUri,
              sampleUri));
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
      case CommonPackage.DESCRIPTION_TYPE__VALUE:
        return getValue();
      case CommonPackage.DESCRIPTION_TYPE__DOC_HREF:
        return getDocHref();
      case CommonPackage.DESCRIPTION_TYPE__SAMPLE_URI:
        return getSampleUri();
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
      case CommonPackage.DESCRIPTION_TYPE__VALUE:
        setValue((String) newValue);
        return;
      case CommonPackage.DESCRIPTION_TYPE__DOC_HREF:
        setDocHref((String) newValue);
        return;
      case CommonPackage.DESCRIPTION_TYPE__SAMPLE_URI:
        setSampleUri((String) newValue);
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
      case CommonPackage.DESCRIPTION_TYPE__VALUE:
        setValue(VALUE_EDEFAULT);
        return;
      case CommonPackage.DESCRIPTION_TYPE__DOC_HREF:
        setDocHref(DOC_HREF_EDEFAULT);
        return;
      case CommonPackage.DESCRIPTION_TYPE__SAMPLE_URI:
        setSampleUri(SAMPLE_URI_EDEFAULT);
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
      case CommonPackage.DESCRIPTION_TYPE__VALUE:
        return VALUE_EDEFAULT == null ? value != null : !VALUE_EDEFAULT.equals(value);
      case CommonPackage.DESCRIPTION_TYPE__DOC_HREF:
        return DOC_HREF_EDEFAULT == null ? docHref != null : !DOC_HREF_EDEFAULT.equals(docHref);
      case CommonPackage.DESCRIPTION_TYPE__SAMPLE_URI:
        return SAMPLE_URI_EDEFAULT == null
            ? sampleUri != null
            : !SAMPLE_URI_EDEFAULT.equals(sampleUri);
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
    result.append(" (value: ");
    result.append(value);
    result.append(", docHref: ");
    result.append(docHref);
    result.append(", sampleUri: ");
    result.append(sampleUri);
    result.append(')');
    return result.toString();
  }
} // DescriptionTypeImpl
