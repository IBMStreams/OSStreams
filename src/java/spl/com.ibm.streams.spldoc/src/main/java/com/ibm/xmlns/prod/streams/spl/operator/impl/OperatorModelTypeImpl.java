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

import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
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
 * An implementation of the model object '<em><b>Model Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl#getCppOperatorModel
 *       <em>Cpp Operator Model</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl#getJavaOperatorModel
 *       <em>Java Operator Model</em>}
 * </ul>
 *
 * @generated
 */
public class OperatorModelTypeImpl extends MinimalEObjectImpl.Container
    implements OperatorModelType {
  /**
   * The cached value of the '{@link #getCppOperatorModel() <em>Cpp Operator Model</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCppOperatorModel()
   * @generated
   * @ordered
   */
  protected OpModelType cppOperatorModel;

  /**
   * The cached value of the '{@link #getJavaOperatorModel() <em>Java Operator Model</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getJavaOperatorModel()
   * @generated
   * @ordered
   */
  protected JavaOpModelType javaOperatorModel;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected OperatorModelTypeImpl() {
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
    return OperatorPackage.Literals.OPERATOR_MODEL_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OpModelType getCppOperatorModel() {
    return cppOperatorModel;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCppOperatorModel(
      OpModelType newCppOperatorModel, NotificationChain msgs) {
    OpModelType oldCppOperatorModel = cppOperatorModel;
    cppOperatorModel = newCppOperatorModel;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL,
              oldCppOperatorModel,
              newCppOperatorModel);
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
  public void setCppOperatorModel(OpModelType newCppOperatorModel) {
    if (newCppOperatorModel != cppOperatorModel) {
      NotificationChain msgs = null;
      if (cppOperatorModel != null)
        msgs =
            ((InternalEObject) cppOperatorModel)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL,
                    null,
                    msgs);
      if (newCppOperatorModel != null)
        msgs =
            ((InternalEObject) newCppOperatorModel)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL,
                    null,
                    msgs);
      msgs = basicSetCppOperatorModel(newCppOperatorModel, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL,
              newCppOperatorModel,
              newCppOperatorModel));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpModelType getJavaOperatorModel() {
    return javaOperatorModel;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetJavaOperatorModel(
      JavaOpModelType newJavaOperatorModel, NotificationChain msgs) {
    JavaOpModelType oldJavaOperatorModel = javaOperatorModel;
    javaOperatorModel = newJavaOperatorModel;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL,
              oldJavaOperatorModel,
              newJavaOperatorModel);
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
  public void setJavaOperatorModel(JavaOpModelType newJavaOperatorModel) {
    if (newJavaOperatorModel != javaOperatorModel) {
      NotificationChain msgs = null;
      if (javaOperatorModel != null)
        msgs =
            ((InternalEObject) javaOperatorModel)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL,
                    null,
                    msgs);
      if (newJavaOperatorModel != null)
        msgs =
            ((InternalEObject) newJavaOperatorModel)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL,
                    null,
                    msgs);
      msgs = basicSetJavaOperatorModel(newJavaOperatorModel, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL,
              newJavaOperatorModel,
              newJavaOperatorModel));
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
      case OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL:
        return basicSetCppOperatorModel(null, msgs);
      case OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL:
        return basicSetJavaOperatorModel(null, msgs);
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
      case OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL:
        return getCppOperatorModel();
      case OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL:
        return getJavaOperatorModel();
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
      case OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL:
        setCppOperatorModel((OpModelType) newValue);
        return;
      case OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL:
        setJavaOperatorModel((JavaOpModelType) newValue);
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
      case OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL:
        setCppOperatorModel((OpModelType) null);
        return;
      case OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL:
        setJavaOperatorModel((JavaOpModelType) null);
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
      case OperatorPackage.OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL:
        return cppOperatorModel != null;
      case OperatorPackage.OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL:
        return javaOperatorModel != null;
    }
    return super.eIsSet(featureID);
  }
} // OperatorModelTypeImpl
