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
package com.ibm.xmlns.prod.streams.spl.operator.impl;

import com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import java.math.BigInteger;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Input Port Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @generated
 */
public class InputPortSetTypeImpl extends InputPortOpenSetTypeImpl implements InputPortSetType {
  /**
   * The default value of the '{@link #getCardinality() <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCardinality()
   * @generated
   * @ordered
   */
  protected static final BigInteger CARDINALITY_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getCardinality() <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCardinality()
   * @generated
   * @ordered
   */
  protected BigInteger cardinality = CARDINALITY_EDEFAULT;

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
  protected InputPortSetTypeImpl() {
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
    return OperatorPackage.Literals.INPUT_PORT_SET_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getCardinality() {
    return cardinality;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCardinality(BigInteger newCardinality) {
    BigInteger oldCardinality = cardinality;
    cardinality = newCardinality;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.INPUT_PORT_SET_TYPE__CARDINALITY,
              oldCardinality,
              cardinality));
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
              OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL,
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
              OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL,
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
      case OperatorPackage.INPUT_PORT_SET_TYPE__CARDINALITY:
        return getCardinality();
      case OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL:
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
      case OperatorPackage.INPUT_PORT_SET_TYPE__CARDINALITY:
        setCardinality((BigInteger) newValue);
        return;
      case OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL:
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
      case OperatorPackage.INPUT_PORT_SET_TYPE__CARDINALITY:
        setCardinality(CARDINALITY_EDEFAULT);
        return;
      case OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL:
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
      case OperatorPackage.INPUT_PORT_SET_TYPE__CARDINALITY:
        return CARDINALITY_EDEFAULT == null
            ? cardinality != null
            : !CARDINALITY_EDEFAULT.equals(cardinality);
      case OperatorPackage.INPUT_PORT_SET_TYPE__OPTIONAL:
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
    result.append(" (cardinality: ");
    result.append(cardinality);
    result.append(", optional: ");
    if (optionalESet) result.append(optional);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // InputPortSetTypeImpl
