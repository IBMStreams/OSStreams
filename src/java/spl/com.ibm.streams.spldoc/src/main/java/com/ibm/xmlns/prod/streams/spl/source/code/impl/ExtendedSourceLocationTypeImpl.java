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
import com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType;
import java.math.BigInteger;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Extended Source Location Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl#getEndColumn
 *       <em>End Column</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl#getEndLine
 *       <em>End Line</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl#getStartColumn
 *       <em>Start Column</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl#getStartLine
 *       <em>Start Line</em>}
 * </ul>
 *
 * @generated
 */
public class ExtendedSourceLocationTypeImpl extends EObjectImpl
    implements ExtendedSourceLocationType {
  /**
   * The default value of the '{@link #getEndColumn() <em>End Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEndColumn()
   * @generated
   * @ordered
   */
  protected static final BigInteger END_COLUMN_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getEndColumn() <em>End Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEndColumn()
   * @generated
   * @ordered
   */
  protected BigInteger endColumn = END_COLUMN_EDEFAULT;

  /**
   * The default value of the '{@link #getEndLine() <em>End Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEndLine()
   * @generated
   * @ordered
   */
  protected static final BigInteger END_LINE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getEndLine() <em>End Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEndLine()
   * @generated
   * @ordered
   */
  protected BigInteger endLine = END_LINE_EDEFAULT;

  /**
   * The default value of the '{@link #getStartColumn() <em>Start Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getStartColumn()
   * @generated
   * @ordered
   */
  protected static final BigInteger START_COLUMN_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getStartColumn() <em>Start Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getStartColumn()
   * @generated
   * @ordered
   */
  protected BigInteger startColumn = START_COLUMN_EDEFAULT;

  /**
   * The default value of the '{@link #getStartLine() <em>Start Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getStartLine()
   * @generated
   * @ordered
   */
  protected static final BigInteger START_LINE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getStartLine() <em>Start Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getStartLine()
   * @generated
   * @ordered
   */
  protected BigInteger startLine = START_LINE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ExtendedSourceLocationTypeImpl() {
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
    return CodePackage.Literals.EXTENDED_SOURCE_LOCATION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getEndColumn() {
    return endColumn;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setEndColumn(BigInteger newEndColumn) {
    BigInteger oldEndColumn = endColumn;
    endColumn = newEndColumn;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN,
              oldEndColumn,
              endColumn));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getEndLine() {
    return endLine;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setEndLine(BigInteger newEndLine) {
    BigInteger oldEndLine = endLine;
    endLine = newEndLine;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_LINE,
              oldEndLine,
              endLine));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getStartColumn() {
    return startColumn;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setStartColumn(BigInteger newStartColumn) {
    BigInteger oldStartColumn = startColumn;
    startColumn = newStartColumn;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN,
              oldStartColumn,
              startColumn));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BigInteger getStartLine() {
    return startLine;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setStartLine(BigInteger newStartLine) {
    BigInteger oldStartLine = startLine;
    startLine = newStartLine;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_LINE,
              oldStartLine,
              startLine));
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
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN:
        return getEndColumn();
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_LINE:
        return getEndLine();
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN:
        return getStartColumn();
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_LINE:
        return getStartLine();
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
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN:
        setEndColumn((BigInteger) newValue);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_LINE:
        setEndLine((BigInteger) newValue);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN:
        setStartColumn((BigInteger) newValue);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_LINE:
        setStartLine((BigInteger) newValue);
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
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN:
        setEndColumn(END_COLUMN_EDEFAULT);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_LINE:
        setEndLine(END_LINE_EDEFAULT);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN:
        setStartColumn(START_COLUMN_EDEFAULT);
        return;
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_LINE:
        setStartLine(START_LINE_EDEFAULT);
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
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN:
        return END_COLUMN_EDEFAULT == null
            ? endColumn != null
            : !END_COLUMN_EDEFAULT.equals(endColumn);
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__END_LINE:
        return END_LINE_EDEFAULT == null ? endLine != null : !END_LINE_EDEFAULT.equals(endLine);
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN:
        return START_COLUMN_EDEFAULT == null
            ? startColumn != null
            : !START_COLUMN_EDEFAULT.equals(startColumn);
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE__START_LINE:
        return START_LINE_EDEFAULT == null
            ? startLine != null
            : !START_LINE_EDEFAULT.equals(startLine);
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
    result.append(" (endColumn: ");
    result.append(endColumn);
    result.append(", endLine: ");
    result.append(endLine);
    result.append(", startColumn: ");
    result.append(startColumn);
    result.append(", startLine: ");
    result.append(startLine);
    result.append(')');
    return result.toString();
  }
} // ExtendedSourceLocationTypeImpl
