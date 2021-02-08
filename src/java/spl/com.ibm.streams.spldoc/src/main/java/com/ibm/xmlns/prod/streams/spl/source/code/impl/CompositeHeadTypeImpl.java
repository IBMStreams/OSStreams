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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Head Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl#getSplDoc
 *       <em>Spl Doc</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl#getModifiers
 *       <em>Modifiers</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl#getInputs
 *       <em>Inputs</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl#getOutputs
 *       <em>Outputs</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl#getName
 *       <em>Name</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeHeadTypeImpl extends SourceLocationTypeImpl implements CompositeHeadType {
  /**
   * The cached value of the '{@link #getSplDoc() <em>Spl Doc</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSplDoc()
   * @generated
   * @ordered
   */
  protected SplDocType splDoc;

  /**
   * The cached value of the '{@link #getModifiers() <em>Modifiers</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getModifiers()
   * @generated
   * @ordered
   */
  protected CompositeModifiersType modifiers;

  /**
   * The cached value of the '{@link #getInputs() <em>Inputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getInputs()
   * @generated
   * @ordered
   */
  protected CompositeInputsType inputs;

  /**
   * The cached value of the '{@link #getOutputs() <em>Outputs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getOutputs()
   * @generated
   * @ordered
   */
  protected CompositeOutputsType outputs;

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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeHeadTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_HEAD_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocType getSplDoc() {
    return splDoc;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSplDoc(SplDocType newSplDoc, NotificationChain msgs) {
    SplDocType oldSplDoc = splDoc;
    splDoc = newSplDoc;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC,
              oldSplDoc,
              newSplDoc);
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
  public void setSplDoc(SplDocType newSplDoc) {
    if (newSplDoc != splDoc) {
      NotificationChain msgs = null;
      if (splDoc != null)
        msgs =
            ((InternalEObject) splDoc)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC,
                    null,
                    msgs);
      if (newSplDoc != null)
        msgs =
            ((InternalEObject) newSplDoc)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC,
                    null,
                    msgs);
      msgs = basicSetSplDoc(newSplDoc, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC,
              newSplDoc,
              newSplDoc));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeModifiersType getModifiers() {
    return modifiers;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetModifiers(
      CompositeModifiersType newModifiers, NotificationChain msgs) {
    CompositeModifiersType oldModifiers = modifiers;
    modifiers = newModifiers;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS,
              oldModifiers,
              newModifiers);
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
  public void setModifiers(CompositeModifiersType newModifiers) {
    if (newModifiers != modifiers) {
      NotificationChain msgs = null;
      if (modifiers != null)
        msgs =
            ((InternalEObject) modifiers)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS,
                    null,
                    msgs);
      if (newModifiers != null)
        msgs =
            ((InternalEObject) newModifiers)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS,
                    null,
                    msgs);
      msgs = basicSetModifiers(newModifiers, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS,
              newModifiers,
              newModifiers));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeInputsType getInputs() {
    return inputs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetInputs(CompositeInputsType newInputs, NotificationChain msgs) {
    CompositeInputsType oldInputs = inputs;
    inputs = newInputs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__INPUTS,
              oldInputs,
              newInputs);
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
  public void setInputs(CompositeInputsType newInputs) {
    if (newInputs != inputs) {
      NotificationChain msgs = null;
      if (inputs != null)
        msgs =
            ((InternalEObject) inputs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__INPUTS,
                    null,
                    msgs);
      if (newInputs != null)
        msgs =
            ((InternalEObject) newInputs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__INPUTS,
                    null,
                    msgs);
      msgs = basicSetInputs(newInputs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__INPUTS,
              newInputs,
              newInputs));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeOutputsType getOutputs() {
    return outputs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetOutputs(
      CompositeOutputsType newOutputs, NotificationChain msgs) {
    CompositeOutputsType oldOutputs = outputs;
    outputs = newOutputs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS,
              oldOutputs,
              newOutputs);
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
  public void setOutputs(CompositeOutputsType newOutputs) {
    if (newOutputs != outputs) {
      NotificationChain msgs = null;
      if (outputs != null)
        msgs =
            ((InternalEObject) outputs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS,
                    null,
                    msgs);
      if (newOutputs != null)
        msgs =
            ((InternalEObject) newOutputs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS,
                    null,
                    msgs);
      msgs = basicSetOutputs(newOutputs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS,
              newOutputs,
              newOutputs));
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
              this, Notification.SET, CodePackage.COMPOSITE_HEAD_TYPE__NAME, oldName, name));
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
      case CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC:
        return basicSetSplDoc(null, msgs);
      case CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS:
        return basicSetModifiers(null, msgs);
      case CodePackage.COMPOSITE_HEAD_TYPE__INPUTS:
        return basicSetInputs(null, msgs);
      case CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS:
        return basicSetOutputs(null, msgs);
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
      case CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC:
        return getSplDoc();
      case CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS:
        return getModifiers();
      case CodePackage.COMPOSITE_HEAD_TYPE__INPUTS:
        return getInputs();
      case CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS:
        return getOutputs();
      case CodePackage.COMPOSITE_HEAD_TYPE__NAME:
        return getName();
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
      case CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC:
        setSplDoc((SplDocType) newValue);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS:
        setModifiers((CompositeModifiersType) newValue);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__INPUTS:
        setInputs((CompositeInputsType) newValue);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS:
        setOutputs((CompositeOutputsType) newValue);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__NAME:
        setName((String) newValue);
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
      case CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC:
        setSplDoc((SplDocType) null);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS:
        setModifiers((CompositeModifiersType) null);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__INPUTS:
        setInputs((CompositeInputsType) null);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS:
        setOutputs((CompositeOutputsType) null);
        return;
      case CodePackage.COMPOSITE_HEAD_TYPE__NAME:
        setName(NAME_EDEFAULT);
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
      case CodePackage.COMPOSITE_HEAD_TYPE__SPL_DOC:
        return splDoc != null;
      case CodePackage.COMPOSITE_HEAD_TYPE__MODIFIERS:
        return modifiers != null;
      case CodePackage.COMPOSITE_HEAD_TYPE__INPUTS:
        return inputs != null;
      case CodePackage.COMPOSITE_HEAD_TYPE__OUTPUTS:
        return outputs != null;
      case CodePackage.COMPOSITE_HEAD_TYPE__NAME:
        return NAME_EDEFAULT == null ? name != null : !NAME_EDEFAULT.equals(name);
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
    result.append(')');
    return result.toString();
  }
} // CompositeHeadTypeImpl
