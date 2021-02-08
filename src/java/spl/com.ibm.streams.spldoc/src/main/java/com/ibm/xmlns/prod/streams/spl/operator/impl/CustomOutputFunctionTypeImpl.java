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
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Custom Output Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl#isPseudoFunction
 *       <em>Pseudo Function</em>}
 * </ul>
 *
 * @generated
 */
public class CustomOutputFunctionTypeImpl extends MinimalEObjectImpl.Container
    implements CustomOutputFunctionType {
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
   * The default value of the '{@link #getPrototype() <em>Prototype</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPrototype()
   * @generated
   * @ordered
   */
  protected static final String PROTOTYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getPrototype() <em>Prototype</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPrototype()
   * @generated
   * @ordered
   */
  protected String prototype = PROTOTYPE_EDEFAULT;

  /**
   * The default value of the '{@link #isPseudoFunction() <em>Pseudo Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPseudoFunction()
   * @generated
   * @ordered
   */
  protected static final boolean PSEUDO_FUNCTION_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPseudoFunction() <em>Pseudo Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPseudoFunction()
   * @generated
   * @ordered
   */
  protected boolean pseudoFunction = PSEUDO_FUNCTION_EDEFAULT;

  /**
   * This is true if the Pseudo Function attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean pseudoFunctionESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CustomOutputFunctionTypeImpl() {
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
    return OperatorPackage.Literals.CUSTOM_OUTPUT_FUNCTION_TYPE;
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
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION,
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
  public String getPrototype() {
    return prototype;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPrototype(String newPrototype) {
    String oldPrototype = prototype;
    prototype = newPrototype;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE,
              oldPrototype,
              prototype));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isPseudoFunction() {
    return pseudoFunction;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPseudoFunction(boolean newPseudoFunction) {
    boolean oldPseudoFunction = pseudoFunction;
    pseudoFunction = newPseudoFunction;
    boolean oldPseudoFunctionESet = pseudoFunctionESet;
    pseudoFunctionESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION,
              oldPseudoFunction,
              pseudoFunction,
              !oldPseudoFunctionESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPseudoFunction() {
    boolean oldPseudoFunction = pseudoFunction;
    boolean oldPseudoFunctionESet = pseudoFunctionESet;
    pseudoFunction = PSEUDO_FUNCTION_EDEFAULT;
    pseudoFunctionESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION,
              oldPseudoFunction,
              PSEUDO_FUNCTION_EDEFAULT,
              oldPseudoFunctionESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPseudoFunction() {
    return pseudoFunctionESet;
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION:
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE:
        return getPrototype();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION:
        return isPseudoFunction();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE:
        setPrototype((String) newValue);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION:
        setPseudoFunction((Boolean) newValue);
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE:
        setPrototype(PROTOTYPE_EDEFAULT);
        return;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION:
        unsetPseudoFunction();
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
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE:
        return PROTOTYPE_EDEFAULT == null
            ? prototype != null
            : !PROTOTYPE_EDEFAULT.equals(prototype);
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION:
        return isSetPseudoFunction();
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
    result.append(" (prototype: ");
    result.append(prototype);
    result.append(", pseudoFunction: ");
    if (pseudoFunctionESet) result.append(pseudoFunction);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // CustomOutputFunctionTypeImpl
