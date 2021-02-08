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

import com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EDataTypeEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Primitive Op Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl#getPortScope
 *       <em>Port Scope</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @generated
 */
public class PrimitiveOpParameterTypeImpl extends ParameterTypeImpl
    implements PrimitiveOpParameterType {
  /**
   * The cached value of the '{@link #getPortScope() <em>Port Scope</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortScope()
   * @generated
   * @ordered
   */
  protected EList<Integer> portScope;

  /**
   * The default value of the '{@link #getCardinality() <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCardinality()
   * @generated
   * @ordered
   */
  protected static final int CARDINALITY_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getCardinality() <em>Cardinality</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCardinality()
   * @generated
   * @ordered
   */
  protected int cardinality = CARDINALITY_EDEFAULT;

  /**
   * This is true if the Cardinality attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean cardinalityESet;

  /**
   * The default value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected static final EnumExpressionModeType EXPRESSION_MODE_EDEFAULT =
      EnumExpressionModeType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected EnumExpressionModeType expressionMode = EXPRESSION_MODE_EDEFAULT;

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
  protected PrimitiveOpParameterTypeImpl() {
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
    return ToolkitPackage.Literals.PRIMITIVE_OP_PARAMETER_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<Integer> getPortScope() {
    if (portScope == null) {
      portScope =
          new EDataTypeEList<Integer>(
              Integer.class, this, ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE);
    }
    return portScope;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getCardinality() {
    return cardinality;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCardinality(int newCardinality) {
    int oldCardinality = cardinality;
    cardinality = newCardinality;
    boolean oldCardinalityESet = cardinalityESet;
    cardinalityESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY,
              oldCardinality,
              cardinality,
              !oldCardinalityESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetCardinality() {
    int oldCardinality = cardinality;
    boolean oldCardinalityESet = cardinalityESet;
    cardinality = CARDINALITY_EDEFAULT;
    cardinalityESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY,
              oldCardinality,
              CARDINALITY_EDEFAULT,
              oldCardinalityESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetCardinality() {
    return cardinalityESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setExpressionMode(EnumExpressionModeType newExpressionMode) {
    EnumExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode == null ? EXPRESSION_MODE_EDEFAULT : newExpressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE,
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
    EnumExpressionModeType oldExpressionMode = expressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionMode = EXPRESSION_MODE_EDEFAULT;
    expressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE,
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
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL,
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
              ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL,
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
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE:
        return getPortScope();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY:
        return getCardinality();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL:
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
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE:
        getPortScope().clear();
        getPortScope().addAll((Collection<? extends Integer>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY:
        setCardinality((Integer) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        setExpressionMode((EnumExpressionModeType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL:
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
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE:
        getPortScope().clear();
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY:
        unsetCardinality();
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        unsetExpressionMode();
        return;
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL:
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
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE:
        return portScope != null && !portScope.isEmpty();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY:
        return isSetCardinality();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE:
        return isSetExpressionMode();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL:
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
    result.append(" (portScope: ");
    result.append(portScope);
    result.append(", cardinality: ");
    if (cardinalityESet) result.append(cardinality);
    else result.append("<unset>");
    result.append(", expressionMode: ");
    if (expressionModeESet) result.append(expressionMode);
    else result.append("<unset>");
    result.append(", optional: ");
    if (optionalESet) result.append(optional);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // PrimitiveOpParameterTypeImpl
