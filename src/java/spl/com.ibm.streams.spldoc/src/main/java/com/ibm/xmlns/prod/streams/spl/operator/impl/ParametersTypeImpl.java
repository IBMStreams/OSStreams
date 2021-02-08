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
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.ParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.ParametersType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Parameters Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl#isAllowAny <em>Allow
 *       Any</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl#getParameter
 *       <em>Parameter</em>}
 * </ul>
 *
 * @generated
 */
public class ParametersTypeImpl extends MinimalEObjectImpl.Container implements ParametersType {
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
   * The default value of the '{@link #isAllowAny() <em>Allow Any</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowAny()
   * @generated
   * @ordered
   */
  protected static final boolean ALLOW_ANY_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isAllowAny() <em>Allow Any</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowAny()
   * @generated
   * @ordered
   */
  protected boolean allowAny = ALLOW_ANY_EDEFAULT;

  /**
   * This is true if the Allow Any attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean allowAnyESet;

  /**
   * The cached value of the '{@link #getParameter() <em>Parameter</em>}' containment reference
   * list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameter()
   * @generated
   * @ordered
   */
  protected EList<ParameterType> parameter;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ParametersTypeImpl() {
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
    return OperatorPackage.Literals.PARAMETERS_TYPE;
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
              OperatorPackage.PARAMETERS_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETERS_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.PARAMETERS_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETERS_TYPE__DESCRIPTION,
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
  public boolean isAllowAny() {
    return allowAny;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAllowAny(boolean newAllowAny) {
    boolean oldAllowAny = allowAny;
    allowAny = newAllowAny;
    boolean oldAllowAnyESet = allowAnyESet;
    allowAnyESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY,
              oldAllowAny,
              allowAny,
              !oldAllowAnyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetAllowAny() {
    boolean oldAllowAny = allowAny;
    boolean oldAllowAnyESet = allowAnyESet;
    allowAny = ALLOW_ANY_EDEFAULT;
    allowAnyESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY,
              oldAllowAny,
              ALLOW_ANY_EDEFAULT,
              oldAllowAnyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetAllowAny() {
    return allowAnyESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ParameterType> getParameter() {
    if (parameter == null) {
      parameter =
          new EObjectContainmentEList<ParameterType>(
              ParameterType.class, this, OperatorPackage.PARAMETERS_TYPE__PARAMETER);
    }
    return parameter;
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
      case OperatorPackage.PARAMETERS_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.PARAMETERS_TYPE__PARAMETER:
        return ((InternalEList<?>) getParameter()).basicRemove(otherEnd, msgs);
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
      case OperatorPackage.PARAMETERS_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY:
        return isAllowAny();
      case OperatorPackage.PARAMETERS_TYPE__PARAMETER:
        return getParameter();
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
      case OperatorPackage.PARAMETERS_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY:
        setAllowAny((Boolean) newValue);
        return;
      case OperatorPackage.PARAMETERS_TYPE__PARAMETER:
        getParameter().clear();
        getParameter().addAll((Collection<? extends ParameterType>) newValue);
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
      case OperatorPackage.PARAMETERS_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY:
        unsetAllowAny();
        return;
      case OperatorPackage.PARAMETERS_TYPE__PARAMETER:
        getParameter().clear();
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
      case OperatorPackage.PARAMETERS_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.PARAMETERS_TYPE__ALLOW_ANY:
        return isSetAllowAny();
      case OperatorPackage.PARAMETERS_TYPE__PARAMETER:
        return parameter != null && !parameter.isEmpty();
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
    result.append(" (allowAny: ");
    if (allowAnyESet) result.append(allowAny);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // ParametersTypeImpl
