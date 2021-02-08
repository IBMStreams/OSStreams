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

import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Op Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl#getMetaType
 *       <em>Meta Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeOpParameterTypeImpl extends ParameterTypeImpl
    implements CompositeOpParameterType {
  /**
   * The default value of the '{@link #getMetaType() <em>Meta Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMetaType()
   * @generated
   * @ordered
   */
  protected static final CompositeParamEnumType META_TYPE_EDEFAULT =
      CompositeParamEnumType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getMetaType() <em>Meta Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMetaType()
   * @generated
   * @ordered
   */
  protected CompositeParamEnumType metaType = META_TYPE_EDEFAULT;

  /**
   * This is true if the Meta Type attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean metaTypeESet;

  /**
   * The default value of the '{@link #isOptional() <em>Optional</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isOptional()
   * @generated
   * @ordered
   */
  protected static final boolean OPTIONAL_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isOptional() <em>Optional</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isOptional()
   * @generated
   * @ordered
   */
  protected boolean optional = OPTIONAL_EDEFAULT;

  /**
   * This is true if the Optional attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean optionalESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeOpParameterTypeImpl() {
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
    return ToolkitPackage.Literals.COMPOSITE_OP_PARAMETER_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParamEnumType getMetaType() {
    return metaType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setMetaType(CompositeParamEnumType newMetaType) {
    CompositeParamEnumType oldMetaType = metaType;
    metaType = newMetaType == null ? META_TYPE_EDEFAULT : newMetaType;
    boolean oldMetaTypeESet = metaTypeESet;
    metaTypeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE,
              oldMetaType,
              metaType,
              !oldMetaTypeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetMetaType() {
    CompositeParamEnumType oldMetaType = metaType;
    boolean oldMetaTypeESet = metaTypeESet;
    metaType = META_TYPE_EDEFAULT;
    metaTypeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE,
              oldMetaType,
              META_TYPE_EDEFAULT,
              oldMetaTypeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetMetaType() {
    return metaTypeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isOptional() {
    return optional;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOptional(boolean newOptional) {
    boolean oldOptional = optional;
    optional = newOptional;
    boolean oldOptionalESet = optionalESet;
    optionalESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL,
              oldOptional,
              optional,
              !oldOptionalESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetOptional() {
    boolean oldOptional = optional;
    boolean oldOptionalESet = optionalESet;
    optional = OPTIONAL_EDEFAULT;
    optionalESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL,
              oldOptional,
              OPTIONAL_EDEFAULT,
              oldOptionalESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetOptional() {
    return optionalESet;
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
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE:
        return getMetaType();
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL:
        return isOptional();
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
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE:
        setMetaType((CompositeParamEnumType) newValue);
        return;
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL:
        setOptional((Boolean) newValue);
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
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE:
        unsetMetaType();
        return;
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL:
        unsetOptional();
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
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__META_TYPE:
        return isSetMetaType();
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL:
        return isSetOptional();
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
    result.append(" (metaType: ");
    if (metaTypeESet) result.append(metaType);
    else result.append("<unset>");
    result.append(", optional: ");
    if (optionalESet) result.append(optional);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // CompositeOpParameterTypeImpl
