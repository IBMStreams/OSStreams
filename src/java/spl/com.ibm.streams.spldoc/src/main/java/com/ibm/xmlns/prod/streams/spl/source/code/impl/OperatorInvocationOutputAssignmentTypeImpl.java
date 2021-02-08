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
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType;
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
 * An implementation of the model object '<em><b>Operator Invocation Output Assignment
 * Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl#getAttributeAssignment
 *       <em>Attribute Assignment</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl#getPortName
 *       <em>Port Name</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorInvocationOutputAssignmentTypeImpl extends SourceLocationTypeImpl
    implements OperatorInvocationOutputAssignmentType {
  /**
   * The cached value of the '{@link #getAttributeAssignment() <em>Attribute Assignment</em>}'
   * containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getAttributeAssignment()
   * @generated
   * @ordered
   */
  protected EList<OperatorInvocationAttributeAssignmentType> attributeAssignment;

  /**
   * The default value of the '{@link #getPortName() <em>Port Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortName()
   * @generated
   * @ordered
   */
  protected static final String PORT_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPortName() <em>Port Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPortName()
   * @generated
   * @ordered
   */
  protected String portName = PORT_NAME_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorInvocationOutputAssignmentTypeImpl() {
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
    return CodePackage.Literals.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<OperatorInvocationAttributeAssignmentType> getAttributeAssignment() {
    if (attributeAssignment == null) {
      attributeAssignment =
          new EObjectContainmentEList<OperatorInvocationAttributeAssignmentType>(
              OperatorInvocationAttributeAssignmentType.class,
              this,
              CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT);
    }
    return attributeAssignment;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getPortName() {
    return portName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPortName(String newPortName) {
    String oldPortName = portName;
    portName = newPortName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME,
              oldPortName,
              portName));
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
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT:
        return ((InternalEList<?>) getAttributeAssignment()).basicRemove(otherEnd, msgs);
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
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT:
        return getAttributeAssignment();
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME:
        return getPortName();
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
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT:
        getAttributeAssignment().clear();
        getAttributeAssignment()
            .addAll((Collection<? extends OperatorInvocationAttributeAssignmentType>) newValue);
        return;
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME:
        setPortName((String) newValue);
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
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT:
        getAttributeAssignment().clear();
        return;
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME:
        setPortName(PORT_NAME_EDEFAULT);
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
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT:
        return attributeAssignment != null && !attributeAssignment.isEmpty();
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME:
        return PORT_NAME_EDEFAULT == null ? portName != null : !PORT_NAME_EDEFAULT.equals(portName);
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
    result.append(" (portName: ");
    result.append(portName);
    result.append(')');
    return result.toString();
  }
} // OperatorInvocationOutputAssignmentTypeImpl
