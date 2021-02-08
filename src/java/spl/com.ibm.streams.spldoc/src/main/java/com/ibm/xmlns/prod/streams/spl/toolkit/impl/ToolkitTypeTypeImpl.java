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

import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Type Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl#getLine
 *       <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl#isStatic
 *       <em>Static</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl#getType
 *       <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl#getUriIndex <em>Uri
 *       Index</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitTypeTypeImpl extends ToolkitElementTypeImpl implements ToolkitTypeType {
  /**
   * The default value of the '{@link #getColumn() <em>Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getColumn()
   * @generated
   * @ordered
   */
  protected static final int COLUMN_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getColumn() <em>Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getColumn()
   * @generated
   * @ordered
   */
  protected int column = COLUMN_EDEFAULT;

  /**
   * This is true if the Column attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean columnESet;

  /**
   * The default value of the '{@link #getLine() <em>Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLine()
   * @generated
   * @ordered
   */
  protected static final int LINE_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getLine() <em>Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLine()
   * @generated
   * @ordered
   */
  protected int line = LINE_EDEFAULT;

  /**
   * This is true if the Line attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean lineESet;

  /**
   * The default value of the '{@link #isStatic() <em>Static</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isStatic()
   * @generated
   * @ordered
   */
  protected static final boolean STATIC_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isStatic() <em>Static</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isStatic()
   * @generated
   * @ordered
   */
  protected boolean static_ = STATIC_EDEFAULT;

  /**
   * This is true if the Static attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean staticESet;

  /**
   * The default value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected static final String TYPE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getType() <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getType()
   * @generated
   * @ordered
   */
  protected String type = TYPE_EDEFAULT;

  /**
   * The default value of the '{@link #getUriIndex() <em>Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUriIndex()
   * @generated
   * @ordered
   */
  protected static final int URI_INDEX_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getUriIndex() <em>Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUriIndex()
   * @generated
   * @ordered
   */
  protected int uriIndex = URI_INDEX_EDEFAULT;

  /**
   * This is true if the Uri Index attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean uriIndexESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitTypeTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_TYPE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getColumn() {
    return column;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setColumn(int newColumn) {
    int oldColumn = column;
    column = newColumn;
    boolean oldColumnESet = columnESet;
    columnESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN,
              oldColumn,
              column,
              !oldColumnESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetColumn() {
    int oldColumn = column;
    boolean oldColumnESet = columnESet;
    column = COLUMN_EDEFAULT;
    columnESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN,
              oldColumn,
              COLUMN_EDEFAULT,
              oldColumnESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetColumn() {
    return columnESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getLine() {
    return line;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setLine(int newLine) {
    int oldLine = line;
    line = newLine;
    boolean oldLineESet = lineESet;
    lineESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE,
              oldLine,
              line,
              !oldLineESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetLine() {
    int oldLine = line;
    boolean oldLineESet = lineESet;
    line = LINE_EDEFAULT;
    lineESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE,
              oldLine,
              LINE_EDEFAULT,
              oldLineESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetLine() {
    return lineESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isStatic() {
    return static_;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setStatic(boolean newStatic) {
    boolean oldStatic = static_;
    static_ = newStatic;
    boolean oldStaticESet = staticESet;
    staticESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC,
              oldStatic,
              static_,
              !oldStaticESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetStatic() {
    boolean oldStatic = static_;
    boolean oldStaticESet = staticESet;
    static_ = STATIC_EDEFAULT;
    staticESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC,
              oldStatic,
              STATIC_EDEFAULT,
              oldStaticESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetStatic() {
    return staticESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getType() {
    return type;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setType(String newType) {
    String oldType = type;
    type = newType;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, ToolkitPackage.TOOLKIT_TYPE_TYPE__TYPE, oldType, type));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public int getUriIndex() {
    return uriIndex;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setUriIndex(int newUriIndex) {
    int oldUriIndex = uriIndex;
    uriIndex = newUriIndex;
    boolean oldUriIndexESet = uriIndexESet;
    uriIndexESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX,
              oldUriIndex,
              uriIndex,
              !oldUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetUriIndex() {
    int oldUriIndex = uriIndex;
    boolean oldUriIndexESet = uriIndexESet;
    uriIndex = URI_INDEX_EDEFAULT;
    uriIndexESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX,
              oldUriIndex,
              URI_INDEX_EDEFAULT,
              oldUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetUriIndex() {
    return uriIndexESet;
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
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN:
        return getColumn();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE:
        return getLine();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC:
        return isStatic();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__TYPE:
        return getType();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX:
        return getUriIndex();
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
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN:
        setColumn((Integer) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE:
        setLine((Integer) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC:
        setStatic((Boolean) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__TYPE:
        setType((String) newValue);
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX:
        setUriIndex((Integer) newValue);
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
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN:
        unsetColumn();
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE:
        unsetLine();
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC:
        unsetStatic();
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__TYPE:
        setType(TYPE_EDEFAULT);
        return;
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX:
        unsetUriIndex();
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
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__COLUMN:
        return isSetColumn();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__LINE:
        return isSetLine();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__STATIC:
        return isSetStatic();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__TYPE:
        return TYPE_EDEFAULT == null ? type != null : !TYPE_EDEFAULT.equals(type);
      case ToolkitPackage.TOOLKIT_TYPE_TYPE__URI_INDEX:
        return isSetUriIndex();
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
    result.append(" (column: ");
    if (columnESet) result.append(column);
    else result.append("<unset>");
    result.append(", line: ");
    if (lineESet) result.append(line);
    else result.append("<unset>");
    result.append(", static: ");
    if (staticESet) result.append(static_);
    else result.append("<unset>");
    result.append(", type: ");
    result.append(type);
    result.append(", uriIndex: ");
    if (uriIndexESet) result.append(uriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // ToolkitTypeTypeImpl
