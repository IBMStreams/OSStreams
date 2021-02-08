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
package com.ibm.xmlns.prod.streams.spl.function.impl;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionPackage;
import com.ibm.xmlns.prod.streams.spl.function.FunctionType;
import com.ibm.xmlns.prod.streams.spl.function.PrototypeType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl#getJavaFunctionName
 *       <em>Java Function Name</em>}
 * </ul>
 *
 * @generated
 */
public class FunctionTypeImpl extends EObjectImpl implements FunctionType {
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
   * The cached value of the '{@link #getPrototype() <em>Prototype</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getPrototype()
   * @generated
   * @ordered
   */
  protected PrototypeType prototype;

  /**
   * The default value of the '{@link #getJavaFunctionName() <em>Java Function Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getJavaFunctionName()
   * @generated
   * @ordered
   */
  protected static final String JAVA_FUNCTION_NAME_EDEFAULT = null;
  /**
   * The cached value of the '{@link #getJavaFunctionName() <em>Java Function Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getJavaFunctionName()
   * @generated
   * @ordered
   */
  protected String javaFunctionName = JAVA_FUNCTION_NAME_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected FunctionTypeImpl() {
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
    return FunctionPackage.Literals.FUNCTION_TYPE;
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
              FunctionPackage.FUNCTION_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_TYPE__DESCRIPTION,
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
  public PrototypeType getPrototype() {
    return prototype;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetPrototype(PrototypeType newPrototype, NotificationChain msgs) {
    PrototypeType oldPrototype = prototype;
    prototype = newPrototype;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_TYPE__PROTOTYPE,
              oldPrototype,
              newPrototype);
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
  public void setPrototype(PrototypeType newPrototype) {
    if (newPrototype != prototype) {
      NotificationChain msgs = null;
      if (prototype != null)
        msgs =
            ((InternalEObject) prototype)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_TYPE__PROTOTYPE,
                    null,
                    msgs);
      if (newPrototype != null)
        msgs =
            ((InternalEObject) newPrototype)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_TYPE__PROTOTYPE,
                    null,
                    msgs);
      msgs = basicSetPrototype(newPrototype, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_TYPE__PROTOTYPE,
              newPrototype,
              newPrototype));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getJavaFunctionName() {
    return javaFunctionName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setJavaFunctionName(String newJavaFunctionName) {
    String oldJavaFunctionName = javaFunctionName;
    javaFunctionName = newJavaFunctionName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_TYPE__JAVA_FUNCTION_NAME,
              oldJavaFunctionName,
              javaFunctionName));
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
      case FunctionPackage.FUNCTION_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case FunctionPackage.FUNCTION_TYPE__PROTOTYPE:
        return basicSetPrototype(null, msgs);
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
      case FunctionPackage.FUNCTION_TYPE__DESCRIPTION:
        return getDescription();
      case FunctionPackage.FUNCTION_TYPE__PROTOTYPE:
        return getPrototype();
      case FunctionPackage.FUNCTION_TYPE__JAVA_FUNCTION_NAME:
        return getJavaFunctionName();
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
      case FunctionPackage.FUNCTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case FunctionPackage.FUNCTION_TYPE__PROTOTYPE:
        setPrototype((PrototypeType) newValue);
        return;
      case FunctionPackage.FUNCTION_TYPE__JAVA_FUNCTION_NAME:
        setJavaFunctionName((String) newValue);
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
      case FunctionPackage.FUNCTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case FunctionPackage.FUNCTION_TYPE__PROTOTYPE:
        setPrototype((PrototypeType) null);
        return;
      case FunctionPackage.FUNCTION_TYPE__JAVA_FUNCTION_NAME:
        setJavaFunctionName(JAVA_FUNCTION_NAME_EDEFAULT);
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
      case FunctionPackage.FUNCTION_TYPE__DESCRIPTION:
        return description != null;
      case FunctionPackage.FUNCTION_TYPE__PROTOTYPE:
        return prototype != null;
      case FunctionPackage.FUNCTION_TYPE__JAVA_FUNCTION_NAME:
        return JAVA_FUNCTION_NAME_EDEFAULT == null
            ? javaFunctionName != null
            : !JAVA_FUNCTION_NAME_EDEFAULT.equals(javaFunctionName);
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
    result.append(" (javaFunctionName: ");
    result.append(javaFunctionName);
    result.append(')');
    return result.toString();
  }
} // FunctionTypeImpl
