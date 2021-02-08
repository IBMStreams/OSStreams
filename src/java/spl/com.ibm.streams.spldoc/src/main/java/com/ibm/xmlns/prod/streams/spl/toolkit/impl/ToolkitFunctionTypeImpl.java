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

import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#getLine
 *       <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#getModelUriIndex
 *       <em>Model Uri Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#isNative
 *       <em>Native</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#isPublic
 *       <em>Public</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#isStateful
 *       <em>Stateful</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl#getUriIndex
 *       <em>Uri Index</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitFunctionTypeImpl extends BaseFunctionTypeImpl implements ToolkitFunctionType {
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
   * The default value of the '{@link #getModelUriIndex() <em>Model Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getModelUriIndex()
   * @generated
   * @ordered
   */
  protected static final int MODEL_URI_INDEX_EDEFAULT = 0;

  /**
   * The cached value of the '{@link #getModelUriIndex() <em>Model Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getModelUriIndex()
   * @generated
   * @ordered
   */
  protected int modelUriIndex = MODEL_URI_INDEX_EDEFAULT;

  /**
   * This is true if the Model Uri Index attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean modelUriIndexESet;

  /**
   * The default value of the '{@link #isNative() <em>Native</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isNative()
   * @generated
   * @ordered
   */
  protected static final boolean NATIVE_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isNative() <em>Native</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isNative()
   * @generated
   * @ordered
   */
  protected boolean native_ = NATIVE_EDEFAULT;

  /**
   * This is true if the Native attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean nativeESet;

  /**
   * The default value of the '{@link #isPublic() <em>Public</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPublic()
   * @generated
   * @ordered
   */
  protected static final boolean PUBLIC_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPublic() <em>Public</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPublic()
   * @generated
   * @ordered
   */
  protected boolean public_ = PUBLIC_EDEFAULT;

  /**
   * This is true if the Public attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean publicESet;

  /**
   * The default value of the '{@link #isStateful() <em>Stateful</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isStateful()
   * @generated
   * @ordered
   */
  protected static final boolean STATEFUL_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isStateful() <em>Stateful</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isStateful()
   * @generated
   * @ordered
   */
  protected boolean stateful = STATEFUL_EDEFAULT;

  /**
   * This is true if the Stateful attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean statefulESet;

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
  protected ToolkitFunctionTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_FUNCTION_TYPE;
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN,
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN,
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE,
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE,
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
  public int getModelUriIndex() {
    return modelUriIndex;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setModelUriIndex(int newModelUriIndex) {
    int oldModelUriIndex = modelUriIndex;
    modelUriIndex = newModelUriIndex;
    boolean oldModelUriIndexESet = modelUriIndexESet;
    modelUriIndexESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX,
              oldModelUriIndex,
              modelUriIndex,
              !oldModelUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetModelUriIndex() {
    int oldModelUriIndex = modelUriIndex;
    boolean oldModelUriIndexESet = modelUriIndexESet;
    modelUriIndex = MODEL_URI_INDEX_EDEFAULT;
    modelUriIndexESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX,
              oldModelUriIndex,
              MODEL_URI_INDEX_EDEFAULT,
              oldModelUriIndexESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetModelUriIndex() {
    return modelUriIndexESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isNative() {
    return native_;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setNative(boolean newNative) {
    boolean oldNative = native_;
    native_ = newNative;
    boolean oldNativeESet = nativeESet;
    nativeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE,
              oldNative,
              native_,
              !oldNativeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetNative() {
    boolean oldNative = native_;
    boolean oldNativeESet = nativeESet;
    native_ = NATIVE_EDEFAULT;
    nativeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE,
              oldNative,
              NATIVE_EDEFAULT,
              oldNativeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetNative() {
    return nativeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isPublic() {
    return public_;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPublic(boolean newPublic) {
    boolean oldPublic = public_;
    public_ = newPublic;
    boolean oldPublicESet = publicESet;
    publicESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC,
              oldPublic,
              public_,
              !oldPublicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPublic() {
    boolean oldPublic = public_;
    boolean oldPublicESet = publicESet;
    public_ = PUBLIC_EDEFAULT;
    publicESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC,
              oldPublic,
              PUBLIC_EDEFAULT,
              oldPublicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPublic() {
    return publicESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isStateful() {
    return stateful;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setStateful(boolean newStateful) {
    boolean oldStateful = stateful;
    stateful = newStateful;
    boolean oldStatefulESet = statefulESet;
    statefulESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL,
              oldStateful,
              stateful,
              !oldStatefulESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetStateful() {
    boolean oldStateful = stateful;
    boolean oldStatefulESet = statefulESet;
    stateful = STATEFUL_EDEFAULT;
    statefulESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL,
              oldStateful,
              STATEFUL_EDEFAULT,
              oldStatefulESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetStateful() {
    return statefulESet;
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX,
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
              ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX,
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
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN:
        return getColumn();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE:
        return getLine();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX:
        return getModelUriIndex();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE:
        return isNative();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC:
        return isPublic();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL:
        return isStateful();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX:
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
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN:
        setColumn((Integer) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE:
        setLine((Integer) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX:
        setModelUriIndex((Integer) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE:
        setNative((Boolean) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC:
        setPublic((Boolean) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL:
        setStateful((Boolean) newValue);
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX:
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
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN:
        unsetColumn();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE:
        unsetLine();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX:
        unsetModelUriIndex();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE:
        unsetNative();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC:
        unsetPublic();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL:
        unsetStateful();
        return;
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX:
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
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__COLUMN:
        return isSetColumn();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__LINE:
        return isSetLine();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX:
        return isSetModelUriIndex();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__NATIVE:
        return isSetNative();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__PUBLIC:
        return isSetPublic();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__STATEFUL:
        return isSetStateful();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE__URI_INDEX:
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
    result.append(", modelUriIndex: ");
    if (modelUriIndexESet) result.append(modelUriIndex);
    else result.append("<unset>");
    result.append(", native: ");
    if (nativeESet) result.append(native_);
    else result.append("<unset>");
    result.append(", public: ");
    if (publicESet) result.append(public_);
    else result.append("<unset>");
    result.append(", stateful: ");
    if (statefulESet) result.append(stateful);
    else result.append("<unset>");
    result.append(", uriIndex: ");
    if (uriIndexESet) result.append(uriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // ToolkitFunctionTypeImpl
