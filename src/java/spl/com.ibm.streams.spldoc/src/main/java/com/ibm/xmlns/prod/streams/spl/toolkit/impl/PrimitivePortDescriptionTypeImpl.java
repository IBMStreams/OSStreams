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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Primitive Port Description Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl#getMaxNumPorts
 *       <em>Max Num Ports</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl#getMinNumPorts
 *       <em>Min Num Ports</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @generated
 */
public class PrimitivePortDescriptionTypeImpl extends EObjectImpl
    implements PrimitivePortDescriptionType {
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
   * The default value of the '{@link #getMaxNumPorts() <em>Max Num Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMaxNumPorts()
   * @generated
   * @ordered
   */
  protected static final int MAX_NUM_PORTS_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getMaxNumPorts() <em>Max Num Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMaxNumPorts()
   * @generated
   * @ordered
   */
  protected int maxNumPorts = MAX_NUM_PORTS_EDEFAULT;

  /**
   * This is true if the Max Num Ports attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean maxNumPortsESet;

  /**
   * The default value of the '{@link #getMinNumPorts() <em>Min Num Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMinNumPorts()
   * @generated
   * @ordered
   */
  protected static final int MIN_NUM_PORTS_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getMinNumPorts() <em>Min Num Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMinNumPorts()
   * @generated
   * @ordered
   */
  protected int minNumPorts = MIN_NUM_PORTS_EDEFAULT;

  /**
   * This is true if the Min Num Ports attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean minNumPortsESet;

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
  protected PrimitivePortDescriptionTypeImpl() {
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
    return ToolkitPackage.Literals.PRIMITIVE_PORT_DESCRIPTION_TYPE;
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
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION,
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
                        - ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION,
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
  public int getMaxNumPorts() {
    return maxNumPorts;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setMaxNumPorts(int newMaxNumPorts) {
    int oldMaxNumPorts = maxNumPorts;
    maxNumPorts = newMaxNumPorts;
    boolean oldMaxNumPortsESet = maxNumPortsESet;
    maxNumPortsESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS,
              oldMaxNumPorts,
              maxNumPorts,
              !oldMaxNumPortsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetMaxNumPorts() {
    int oldMaxNumPorts = maxNumPorts;
    boolean oldMaxNumPortsESet = maxNumPortsESet;
    maxNumPorts = MAX_NUM_PORTS_EDEFAULT;
    maxNumPortsESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS,
              oldMaxNumPorts,
              MAX_NUM_PORTS_EDEFAULT,
              oldMaxNumPortsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetMaxNumPorts() {
    return maxNumPortsESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getMinNumPorts() {
    return minNumPorts;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setMinNumPorts(int newMinNumPorts) {
    int oldMinNumPorts = minNumPorts;
    minNumPorts = newMinNumPorts;
    boolean oldMinNumPortsESet = minNumPortsESet;
    minNumPortsESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS,
              oldMinNumPorts,
              minNumPorts,
              !oldMinNumPortsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetMinNumPorts() {
    int oldMinNumPorts = minNumPorts;
    boolean oldMinNumPortsESet = minNumPortsESet;
    minNumPorts = MIN_NUM_PORTS_EDEFAULT;
    minNumPortsESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS,
              oldMinNumPorts,
              MIN_NUM_PORTS_EDEFAULT,
              oldMinNumPortsESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetMinNumPorts() {
    return minNumPortsESet;
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
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL,
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
              ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL,
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
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION:
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
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION:
        return getDescription();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS:
        return getMaxNumPorts();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS:
        return getMinNumPorts();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL:
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
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS:
        setMaxNumPorts((Integer) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS:
        setMinNumPorts((Integer) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL:
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
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS:
        unsetMaxNumPorts();
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS:
        unsetMinNumPorts();
        return;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL:
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
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION:
        return description != null;
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS:
        return isSetMaxNumPorts();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS:
        return isSetMinNumPorts();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL:
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
    result.append(" (maxNumPorts: ");
    if (maxNumPortsESet) result.append(maxNumPorts);
    else result.append("<unset>");
    result.append(", minNumPorts: ");
    if (minNumPortsESet) result.append(minNumPorts);
    else result.append("<unset>");
    result.append(", optional: ");
    if (optionalESet) result.append(optional);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // PrimitivePortDescriptionTypeImpl
