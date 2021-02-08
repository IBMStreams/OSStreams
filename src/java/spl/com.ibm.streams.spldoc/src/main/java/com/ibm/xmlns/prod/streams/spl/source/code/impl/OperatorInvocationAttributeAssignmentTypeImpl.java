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
package com.ibm.xmlns.prod.streams.spl.source.code.impl;

import com.ibm.xmlns.prod.streams.spl.source.code.CodePackage;
import com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Operator Invocation Attribute Assignment
 * Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl#getValue
 *       <em>Value</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl#getName
 *       <em>Name</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl#getOutputFunction
 *       <em>Output Function</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationAttributeAssignmentTypeImpl extends SourceLocationTypeImpl
    implements OperatorInvocationAttributeAssignmentType {
  /**
   * The cached value of the '{@link #getValue() <em>Value</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getValue()
   * @generated
   * @ordered
   */
  protected EList<ExpressionType> value;

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
   * The default value of the '{@link #getOutputFunction() <em>Output Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputFunction()
   * @generated
   * @ordered
   */
  protected static final String OUTPUT_FUNCTION_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getOutputFunction() <em>Output Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputFunction()
   * @generated
   * @ordered
   */
  protected String outputFunction = OUTPUT_FUNCTION_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationAttributeAssignmentTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ExpressionType> getValue() {
    if (value == null) {
      value =
          new EObjectContainmentEList<ExpressionType>(
              ExpressionType.class,
              this,
              CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE);
    }
    return value;
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
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME,
              oldName,
              name));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getOutputFunction() {
    return outputFunction;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOutputFunction(String newOutputFunction) {
    String oldOutputFunction = outputFunction;
    outputFunction = newOutputFunction;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION,
              oldOutputFunction,
              outputFunction));
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
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE:
        return ((InternalEList<?>) getValue()).basicRemove(otherEnd, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE:
        return getValue();
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME:
        return getName();
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION:
        return getOutputFunction();
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
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE:
        getValue().clear();
        getValue().addAll((Collection<? extends ExpressionType>) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME:
        setName((String) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION:
        setOutputFunction((String) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE:
        getValue().clear();
        return;
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME:
        setName(NAME_EDEFAULT);
        return;
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION:
        setOutputFunction(OUTPUT_FUNCTION_EDEFAULT);
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
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE:
        return value != null && !value.isEmpty();
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION:
        return OUTPUT_FUNCTION_EDEFAULT == null
            ? outputFunction != null
            : !OUTPUT_FUNCTION_EDEFAULT.equals(outputFunction);
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
    result.append(", outputFunction: ");
    result.append(outputFunction);
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationAttributeAssignmentTypeImpl
