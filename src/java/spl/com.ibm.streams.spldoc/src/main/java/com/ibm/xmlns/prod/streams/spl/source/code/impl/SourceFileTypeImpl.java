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
import com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Source File Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl#getCompilationUnit
 *       <em>Compilation Unit</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl#getUri
 *       <em>Uri</em>}
 * </ul>
 *
 * @generated
 */
public class SourceFileTypeImpl extends EObjectImpl implements SourceFileType {
  /**
   * The cached value of the '{@link #getCompilationUnit() <em>Compilation Unit</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCompilationUnit()
   * @generated
   * @ordered
   */
  protected CompilationUnitType compilationUnit;

  /**
   * The default value of the '{@link #getUri() <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUri()
   * @generated
   * @ordered
   */
  protected static final String URI_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getUri() <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUri()
   * @generated
   * @ordered
   */
  protected String uri = URI_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected SourceFileTypeImpl() {
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
    return CodePackage.Literals.SOURCE_FILE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompilationUnitType getCompilationUnit() {
    return compilationUnit;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCompilationUnit(
      CompilationUnitType newCompilationUnit, NotificationChain msgs) {
    CompilationUnitType oldCompilationUnit = compilationUnit;
    compilationUnit = newCompilationUnit;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT,
              oldCompilationUnit,
              newCompilationUnit);
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
  public void setCompilationUnit(CompilationUnitType newCompilationUnit) {
    if (newCompilationUnit != compilationUnit) {
      NotificationChain msgs = null;
      if (compilationUnit != null)
        msgs =
            ((InternalEObject) compilationUnit)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT,
                    null,
                    msgs);
      if (newCompilationUnit != null)
        msgs =
            ((InternalEObject) newCompilationUnit)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT,
                    null,
                    msgs);
      msgs = basicSetCompilationUnit(newCompilationUnit, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT,
              newCompilationUnit,
              newCompilationUnit));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getUri() {
    return uri;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setUri(String newUri) {
    String oldUri = uri;
    uri = newUri;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.SOURCE_FILE_TYPE__URI, oldUri, uri));
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
      case CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT:
        return basicSetCompilationUnit(null, msgs);
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
      case CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT:
        return getCompilationUnit();
      case CodePackage.SOURCE_FILE_TYPE__URI:
        return getUri();
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
      case CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT:
        setCompilationUnit((CompilationUnitType) newValue);
        return;
      case CodePackage.SOURCE_FILE_TYPE__URI:
        setUri((String) newValue);
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
      case CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT:
        setCompilationUnit((CompilationUnitType) null);
        return;
      case CodePackage.SOURCE_FILE_TYPE__URI:
        setUri(URI_EDEFAULT);
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
      case CodePackage.SOURCE_FILE_TYPE__COMPILATION_UNIT:
        return compilationUnit != null;
      case CodePackage.SOURCE_FILE_TYPE__URI:
        return URI_EDEFAULT == null ? uri != null : !URI_EDEFAULT.equals(uri);
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
    result.append(" (uri: ");
    result.append(uri);
    result.append(')');
    return result.toString();
  }
} // SourceFileTypeImpl
