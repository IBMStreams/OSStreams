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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import java.math.BigInteger;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Java Op Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#isOptional
 *       <em>Optional</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#getType
 *       <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl#getCardinality
 *       <em>Cardinality</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpParameterTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpParameterType {
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
   * The default value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected static final JavaOpExpressionModeType EXPRESSION_MODE_EDEFAULT =
      JavaOpExpressionModeType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected JavaOpExpressionModeType expressionMode = EXPRESSION_MODE_EDEFAULT;

  /**
   * This is true if the Expression Mode attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean expressionModeESet;

  /**
   * The default value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected static final String TYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected String type = TYPE_EDEFAULT;

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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpParameterTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_PARAMETER_TYPE;
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
              this, Notification.SET, OperatorPackage.JAVA_OP_PARAMETER_TYPE__NAME, oldName, name));
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
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION,
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
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL,
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
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL,
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
  public JavaOpExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setExpressionMode(JavaOpExpressionModeType newExpressionMode) {
    JavaOpExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode == null ? EXPRESSION_MODE_EDEFAULT : newExpressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE,
              oldExpressionMode,
              expressionMode,
              !oldExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetExpressionMode() {
    JavaOpExpressionModeType oldExpressionMode = expressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionMode = EXPRESSION_MODE_EDEFAULT;
    expressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE,
              oldExpressionMode,
              EXPRESSION_MODE_EDEFAULT,
              oldExpressionModeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetExpressionMode() {
    return expressionModeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getType() {
    return type;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setType(String newType) {
    String oldType = type;
    type = newType;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, OperatorPackage.JAVA_OP_PARAMETER_TYPE__TYPE, oldType, type));
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
              OperatorPackage.JAVA_OP_PARAMETER_TYPE__CARDINALITY,
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
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION:
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
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__NAME:
        return getName();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL:
        return isOptional();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__TYPE:
        return getType();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__CARDINALITY:
        return getCardinality();
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
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__NAME:
        setName((String) newValue);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL:
        setOptional((Boolean) newValue);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        setExpressionMode((JavaOpExpressionModeType) newValue);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__TYPE:
        setType((String) newValue);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__CARDINALITY:
        setCardinality((BigInteger) newValue);
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
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL:
        unsetOptional();
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        unsetExpressionMode();
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__TYPE:
        setType(TYPE_EDEFAULT);
        return;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__CARDINALITY:
        setCardinality(CARDINALITY_EDEFAULT);
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
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__OPTIONAL:
        return isSetOptional();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        return isSetExpressionMode();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__TYPE:
        return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT.equals(type);
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE__CARDINALITY:
        return CARDINALITY_EDEFAULT == null
            ? cardinality != null
            : !CARDINALITY_EDEFAULT.equals(cardinality);
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
    result.append(", optional: ");
    if (optionalESet) result.append(optional);
    else result.append("<unset>");
    result.append(", expressionMode: ");
    if (expressionModeESet) result.append(expressionMode);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(", cardinality: ");
    result.append(cardinality);
    result.append(')');
    return result.toString();
  }
} // JavaOpParameterTypeImpl
