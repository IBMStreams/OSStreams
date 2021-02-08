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
import com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.ParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.PortScopeType;
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
 * An implementation of the model object '<em><b>Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#isOptional
 *       <em>Optional</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#isRewriteAllowed
 *       <em>Rewrite Allowed</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getExpressionMode
 *       <em>Expression Mode</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getType
 *       <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getCardinality
 *       <em>Cardinality</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getPortScope <em>Port
 *       Scope</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl#getCustomOutputFunction
 *       <em>Custom Output Function</em>}
 * </ul>
 *
 * @generated
 */
public class ParameterTypeImpl extends MinimalEObjectImpl.Container implements ParameterType {
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
   * The default value of the '{@link #isRewriteAllowed() <em>Rewrite Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowed()
   * @generated
   * @ordered
   */
  protected static final boolean REWRITE_ALLOWED_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isRewriteAllowed() <em>Rewrite Allowed</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isRewriteAllowed()
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowed = REWRITE_ALLOWED_EDEFAULT;

  /**
   * This is true if the Rewrite Allowed attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean rewriteAllowedESet;

  /**
   * The default value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected static final ExpressionModeType EXPRESSION_MODE_EDEFAULT = ExpressionModeType.ATTRIBUTE;

  /**
   * The cached value of the '{@link #getExpressionMode() <em>Expression Mode</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExpressionMode()
   * @generated
   * @ordered
   */
  protected ExpressionModeType expressionMode = EXPRESSION_MODE_EDEFAULT;

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
   * The cached value of the '{@link #getPortScope() <em>Port Scope</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortScope()
   * @generated
   * @ordered
   */
  protected PortScopeType portScope;

  /**
   * The default value of the '{@link #getCustomOutputFunction() <em>Custom Output Function</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCustomOutputFunction()
   * @generated
   * @ordered
   */
  protected static final String CUSTOM_OUTPUT_FUNCTION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getCustomOutputFunction() <em>Custom Output Function</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCustomOutputFunction()
   * @generated
   * @ordered
   */
  protected String customOutputFunction = CUSTOM_OUTPUT_FUNCTION_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ParameterTypeImpl() {
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
    return OperatorPackage.Literals.PARAMETER_TYPE;
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
              this, Notification.SET, OperatorPackage.PARAMETER_TYPE__NAME, oldName, name));
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
              OperatorPackage.PARAMETER_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETER_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETER_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__DESCRIPTION,
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
              OperatorPackage.PARAMETER_TYPE__OPTIONAL,
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
              OperatorPackage.PARAMETER_TYPE__OPTIONAL,
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
  public boolean isRewriteAllowed() {
    return rewriteAllowed;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRewriteAllowed(boolean newRewriteAllowed) {
    boolean oldRewriteAllowed = rewriteAllowed;
    rewriteAllowed = newRewriteAllowed;
    boolean oldRewriteAllowedESet = rewriteAllowedESet;
    rewriteAllowedESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED,
              oldRewriteAllowed,
              rewriteAllowed,
              !oldRewriteAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetRewriteAllowed() {
    boolean oldRewriteAllowed = rewriteAllowed;
    boolean oldRewriteAllowedESet = rewriteAllowedESet;
    rewriteAllowed = REWRITE_ALLOWED_EDEFAULT;
    rewriteAllowedESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED,
              oldRewriteAllowed,
              REWRITE_ALLOWED_EDEFAULT,
              oldRewriteAllowedESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetRewriteAllowed() {
    return rewriteAllowedESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ExpressionModeType getExpressionMode() {
    return expressionMode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setExpressionMode(ExpressionModeType newExpressionMode) {
    ExpressionModeType oldExpressionMode = expressionMode;
    expressionMode = newExpressionMode == null ? EXPRESSION_MODE_EDEFAULT : newExpressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionModeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE,
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
    ExpressionModeType oldExpressionMode = expressionMode;
    boolean oldExpressionModeESet = expressionModeESet;
    expressionMode = EXPRESSION_MODE_EDEFAULT;
    expressionModeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE,
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
              this, Notification.SET, OperatorPackage.PARAMETER_TYPE__TYPE, oldType, type));
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
              OperatorPackage.PARAMETER_TYPE__CARDINALITY,
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
  public PortScopeType getPortScope() {
    return portScope;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetPortScope(PortScopeType newPortScope, NotificationChain msgs) {
    PortScopeType oldPortScope = portScope;
    portScope = newPortScope;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__PORT_SCOPE,
              oldPortScope,
              newPortScope);
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
  public void setPortScope(PortScopeType newPortScope) {
    if (newPortScope != portScope) {
      NotificationChain msgs = null;
      if (portScope != null)
        msgs =
            ((InternalEObject) portScope)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETER_TYPE__PORT_SCOPE,
                    null,
                    msgs);
      if (newPortScope != null)
        msgs =
            ((InternalEObject) newPortScope)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETER_TYPE__PORT_SCOPE,
                    null,
                    msgs);
      msgs = basicSetPortScope(newPortScope, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__PORT_SCOPE,
              newPortScope,
              newPortScope));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getCustomOutputFunction() {
    return customOutputFunction;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCustomOutputFunction(String newCustomOutputFunction) {
    String oldCustomOutputFunction = customOutputFunction;
    customOutputFunction = newCustomOutputFunction;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION,
              oldCustomOutputFunction,
              customOutputFunction));
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
      case OperatorPackage.PARAMETER_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.PARAMETER_TYPE__PORT_SCOPE:
        return basicSetPortScope(null, msgs);
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
      case OperatorPackage.PARAMETER_TYPE__NAME:
        return getName();
      case OperatorPackage.PARAMETER_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.PARAMETER_TYPE__OPTIONAL:
        return isOptional();
      case OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED:
        return isRewriteAllowed();
      case OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE:
        return getExpressionMode();
      case OperatorPackage.PARAMETER_TYPE__TYPE:
        return getType();
      case OperatorPackage.PARAMETER_TYPE__CARDINALITY:
        return getCardinality();
      case OperatorPackage.PARAMETER_TYPE__PORT_SCOPE:
        return getPortScope();
      case OperatorPackage.PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION:
        return getCustomOutputFunction();
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
      case OperatorPackage.PARAMETER_TYPE__NAME:
        setName((String) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__OPTIONAL:
        setOptional((Boolean) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED:
        setRewriteAllowed((Boolean) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE:
        setExpressionMode((ExpressionModeType) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__TYPE:
        setType((String) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__CARDINALITY:
        setCardinality((BigInteger) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__PORT_SCOPE:
        setPortScope((PortScopeType) newValue);
        return;
      case OperatorPackage.PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION:
        setCustomOutputFunction((String) newValue);
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
      case OperatorPackage.PARAMETER_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case OperatorPackage.PARAMETER_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.PARAMETER_TYPE__OPTIONAL:
        unsetOptional();
        return;
      case OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED:
        unsetRewriteAllowed();
        return;
      case OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE:
        unsetExpressionMode();
        return;
      case OperatorPackage.PARAMETER_TYPE__TYPE:
        setType(TYPE_EDEFAULT);
        return;
      case OperatorPackage.PARAMETER_TYPE__CARDINALITY:
        setCardinality(CARDINALITY_EDEFAULT);
        return;
      case OperatorPackage.PARAMETER_TYPE__PORT_SCOPE:
        setPortScope((PortScopeType) null);
        return;
      case OperatorPackage.PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION:
        setCustomOutputFunction(CUSTOM_OUTPUT_FUNCTION_EDEFAULT);
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
      case OperatorPackage.PARAMETER_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case OperatorPackage.PARAMETER_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.PARAMETER_TYPE__OPTIONAL:
        return isSetOptional();
      case OperatorPackage.PARAMETER_TYPE__REWRITE_ALLOWED:
        return isSetRewriteAllowed();
      case OperatorPackage.PARAMETER_TYPE__EXPRESSION_MODE:
        return isSetExpressionMode();
      case OperatorPackage.PARAMETER_TYPE__TYPE:
        return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT.equals(type);
      case OperatorPackage.PARAMETER_TYPE__CARDINALITY:
        return CARDINALITY_EDEFAULT == null
            ? cardinality != null
            : !CARDINALITY_EDEFAULT.equals(cardinality);
      case OperatorPackage.PARAMETER_TYPE__PORT_SCOPE:
        return portScope != null;
      case OperatorPackage.PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION:
        return CUSTOM_OUTPUT_FUNCTION_EDEFAULT == null
            ? customOutputFunction != null
            : !CUSTOM_OUTPUT_FUNCTION_EDEFAULT.equals(customOutputFunction);
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
    result.append(", rewriteAllowed: ");
    if (rewriteAllowedESet) result.append(rewriteAllowed);
    else result.append("<unset>");
    result.append(", expressionMode: ");
    if (expressionModeESet) result.append(expressionMode);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(", cardinality: ");
    result.append(cardinality);
    result.append(", customOutputFunction: ");
    result.append(customOutputFunction);
    result.append(')');
    return result.toString();
  }
} // ParameterTypeImpl
