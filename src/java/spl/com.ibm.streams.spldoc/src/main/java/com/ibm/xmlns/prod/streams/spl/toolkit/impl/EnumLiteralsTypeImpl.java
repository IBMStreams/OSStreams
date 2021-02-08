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
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.eclipse.emf.ecore.util.EDataTypeEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Enum Literals Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getEnum
 *       <em>Enum</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getLine
 *       <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#isStatic
 *       <em>Static</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getTypeName
 *       <em>Type Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl#getUriIndex <em>Uri
 *       Index</em>}
 * </ul>
 *
 * @generated
 */
public class EnumLiteralsTypeImpl extends EObjectImpl implements EnumLiteralsType {
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
   * The cached value of the '{@link #getEnum() <em>Enum</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getEnum()
   * @generated
   * @ordered
   */
  protected EList<String> enum_;

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
   * The default value of the '{@link #getTypeName() <em>Type Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTypeName()
   * @generated
   * @ordered
   */
  protected static final String TYPE_NAME_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getTypeName() <em>Type Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTypeName()
   * @generated
   * @ordered
   */
  protected String typeName = TYPE_NAME_EDEFAULT;

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
  protected EnumLiteralsTypeImpl() {
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
    return ToolkitPackage.Literals.ENUM_LITERALS_TYPE;
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
              ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION,
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
  public EList<String> getEnum() {
    if (enum_ == null) {
      enum_ =
          new EDataTypeEList<String>(String.class, this, ToolkitPackage.ENUM_LITERALS_TYPE__ENUM);
    }
    return enum_;
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
              ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__LINE,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__LINE,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__STATIC,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__STATIC,
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
  public String getTypeName() {
    return typeName;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setTypeName(String newTypeName) {
    String oldTypeName = typeName;
    typeName = newTypeName;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.ENUM_LITERALS_TYPE__TYPE_NAME,
              oldTypeName,
              typeName));
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
              ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX,
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
              ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX,
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
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION:
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
      case ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION:
        return getDescription();
      case ToolkitPackage.ENUM_LITERALS_TYPE__ENUM:
        return getEnum();
      case ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN:
        return getColumn();
      case ToolkitPackage.ENUM_LITERALS_TYPE__LINE:
        return getLine();
      case ToolkitPackage.ENUM_LITERALS_TYPE__STATIC:
        return isStatic();
      case ToolkitPackage.ENUM_LITERALS_TYPE__TYPE_NAME:
        return getTypeName();
      case ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX:
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
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__ENUM:
        getEnum().clear();
        getEnum().addAll((Collection<? extends String>) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN:
        setColumn((Integer) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__LINE:
        setLine((Integer) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__STATIC:
        setStatic((Boolean) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__TYPE_NAME:
        setTypeName((String) newValue);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX:
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
      case ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__ENUM:
        getEnum().clear();
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN:
        unsetColumn();
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__LINE:
        unsetLine();
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__STATIC:
        unsetStatic();
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__TYPE_NAME:
        setTypeName(TYPE_NAME_EDEFAULT);
        return;
      case ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX:
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
      case ToolkitPackage.ENUM_LITERALS_TYPE__DESCRIPTION:
        return description != null;
      case ToolkitPackage.ENUM_LITERALS_TYPE__ENUM:
        return enum_ != null && !enum_.isEmpty();
      case ToolkitPackage.ENUM_LITERALS_TYPE__COLUMN:
        return isSetColumn();
      case ToolkitPackage.ENUM_LITERALS_TYPE__LINE:
        return isSetLine();
      case ToolkitPackage.ENUM_LITERALS_TYPE__STATIC:
        return isSetStatic();
      case ToolkitPackage.ENUM_LITERALS_TYPE__TYPE_NAME:
        return TYPE_NAME_EDEFAULT == null ? typeName != null : !TYPE_NAME_EDEFAULT.equals(typeName);
      case ToolkitPackage.ENUM_LITERALS_TYPE__URI_INDEX:
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
    result.append(" (enum: ");
    result.append(enum_);
    result.append(", column: ");
    if (columnESet) result.append(column);
    else result.append("<unset>");
    result.append(", line: ");
    if (lineESet) result.append(line);
    else result.append("<unset>");
    result.append(", static: ");
    if (staticESet) result.append(static_);
    else result.append("<unset>");
    result.append(", typeName: ");
    result.append(typeName);
    result.append(", uriIndex: ");
    if (uriIndexESet) result.append(uriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // EnumLiteralsTypeImpl
