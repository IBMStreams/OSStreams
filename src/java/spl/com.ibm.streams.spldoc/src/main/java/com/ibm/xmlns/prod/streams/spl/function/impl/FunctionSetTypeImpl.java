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

import com.ibm.xmlns.prod.streams.spl.function.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionPackage;
import com.ibm.xmlns.prod.streams.spl.function.FunctionSetType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionsType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Set Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl#getHeaderFileName
 *       <em>Header File Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl#getCppNamespaceName
 *       <em>Cpp Namespace Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl#getFunctions
 *       <em>Functions</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl#getDependencies
 *       <em>Dependencies</em>}
 * </ul>
 *
 * @generated
 */
public class FunctionSetTypeImpl extends EObjectImpl implements FunctionSetType {
  /**
   * The default value of the '{@link #getHeaderFileName() <em>Header File Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getHeaderFileName()
   * @generated
   * @ordered
   */
  protected static final String HEADER_FILE_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getHeaderFileName() <em>Header File Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getHeaderFileName()
   * @generated
   * @ordered
   */
  protected String headerFileName = HEADER_FILE_NAME_EDEFAULT;

  /**
   * The default value of the '{@link #getCppNamespaceName() <em>Cpp Namespace Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCppNamespaceName()
   * @generated
   * @ordered
   */
  protected static final String CPP_NAMESPACE_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getCppNamespaceName() <em>Cpp Namespace Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCppNamespaceName()
   * @generated
   * @ordered
   */
  protected String cppNamespaceName = CPP_NAMESPACE_NAME_EDEFAULT;

  /**
   * The cached value of the '{@link #getFunctions() <em>Functions</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctions()
   * @generated
   * @ordered
   */
  protected FunctionsType functions;

  /**
   * The cached value of the '{@link #getDependencies() <em>Dependencies</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDependencies()
   * @generated
   * @ordered
   */
  protected DependenciesType dependencies;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected FunctionSetTypeImpl() {
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
    return FunctionPackage.Literals.FUNCTION_SET_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getHeaderFileName() {
    return headerFileName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setHeaderFileName(String newHeaderFileName) {
    String oldHeaderFileName = headerFileName;
    headerFileName = newHeaderFileName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__HEADER_FILE_NAME,
              oldHeaderFileName,
              headerFileName));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getCppNamespaceName() {
    return cppNamespaceName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCppNamespaceName(String newCppNamespaceName) {
    String oldCppNamespaceName = cppNamespaceName;
    cppNamespaceName = newCppNamespaceName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME,
              oldCppNamespaceName,
              cppNamespaceName));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionsType getFunctions() {
    return functions;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetFunctions(FunctionsType newFunctions, NotificationChain msgs) {
    FunctionsType oldFunctions = functions;
    functions = newFunctions;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS,
              oldFunctions,
              newFunctions);
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
  public void setFunctions(FunctionsType newFunctions) {
    if (newFunctions != functions) {
      NotificationChain msgs = null;
      if (functions != null)
        msgs =
            ((InternalEObject) functions)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS,
                    null,
                    msgs);
      if (newFunctions != null)
        msgs =
            ((InternalEObject) newFunctions)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS,
                    null,
                    msgs);
      msgs = basicSetFunctions(newFunctions, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS,
              newFunctions,
              newFunctions));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DependenciesType getDependencies() {
    return dependencies;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetDependencies(
      DependenciesType newDependencies, NotificationChain msgs) {
    DependenciesType oldDependencies = dependencies;
    dependencies = newDependencies;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES,
              oldDependencies,
              newDependencies);
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
  public void setDependencies(DependenciesType newDependencies) {
    if (newDependencies != dependencies) {
      NotificationChain msgs = null;
      if (dependencies != null)
        msgs =
            ((InternalEObject) dependencies)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES,
                    null,
                    msgs);
      if (newDependencies != null)
        msgs =
            ((InternalEObject) newDependencies)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES,
                    null,
                    msgs);
      msgs = basicSetDependencies(newDependencies, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES,
              newDependencies,
              newDependencies));
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
      case FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS:
        return basicSetFunctions(null, msgs);
      case FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES:
        return basicSetDependencies(null, msgs);
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
      case FunctionPackage.FUNCTION_SET_TYPE__HEADER_FILE_NAME:
        return getHeaderFileName();
      case FunctionPackage.FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME:
        return getCppNamespaceName();
      case FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS:
        return getFunctions();
      case FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES:
        return getDependencies();
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
      case FunctionPackage.FUNCTION_SET_TYPE__HEADER_FILE_NAME:
        setHeaderFileName((String) newValue);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME:
        setCppNamespaceName((String) newValue);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS:
        setFunctions((FunctionsType) newValue);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES:
        setDependencies((DependenciesType) newValue);
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
      case FunctionPackage.FUNCTION_SET_TYPE__HEADER_FILE_NAME:
        setHeaderFileName(HEADER_FILE_NAME_EDEFAULT);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME:
        setCppNamespaceName(CPP_NAMESPACE_NAME_EDEFAULT);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS:
        setFunctions((FunctionsType) null);
        return;
      case FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES:
        setDependencies((DependenciesType) null);
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
      case FunctionPackage.FUNCTION_SET_TYPE__HEADER_FILE_NAME:
        return HEADER_FILE_NAME_EDEFAULT == null
            ? headerFileName != null
            : !HEADER_FILE_NAME_EDEFAULT.equals(headerFileName);
      case FunctionPackage.FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME:
        return CPP_NAMESPACE_NAME_EDEFAULT == null
            ? cppNamespaceName != null
            : !CPP_NAMESPACE_NAME_EDEFAULT.equals(cppNamespaceName);
      case FunctionPackage.FUNCTION_SET_TYPE__FUNCTIONS:
        return functions != null;
      case FunctionPackage.FUNCTION_SET_TYPE__DEPENDENCIES:
        return dependencies != null;
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
    result.append(" (headerFileName: ");
    result.append(headerFileName);
    result.append(", cppNamespaceName: ");
    result.append(cppNamespaceName);
    result.append(')');
    return result.toString();
  }
} // FunctionSetTypeImpl
