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

import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.util.BasicFeatureMap;
import org.eclipse.emf.ecore.util.FeatureMap;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getGroup
 *       <em>Group</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getInputPort
 *       <em>Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getOutputPort
 *       <em>Output Port</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getType
 *       <em>Type</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getEnumLiterals
 *       <em>Enum Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getLine
 *       <em>Line</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#isPotentialMain
 *       <em>Potential Main</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl#getUriIndex
 *       <em>Uri Index</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeToolkitOperatorTypeImpl extends BaseToolkitOperatorTypeImpl
    implements CompositeToolkitOperatorType {
  /**
   * The cached value of the '{@link #getGroup() <em>Group</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getGroup()
   * @generated
   * @ordered
   */
  protected FeatureMap group;

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
   * The default value of the '{@link #isPotentialMain() <em>Potential Main</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPotentialMain()
   * @generated
   * @ordered
   */
  protected static final boolean POTENTIAL_MAIN_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isPotentialMain() <em>Potential Main</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isPotentialMain()
   * @generated
   * @ordered
   */
  protected boolean potentialMain = POTENTIAL_MAIN_EDEFAULT;

  /**
   * This is true if the Potential Main attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean potentialMainESet;

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
  protected CompositeToolkitOperatorTypeImpl() {
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
    return ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FeatureMap getGroup() {
    if (group == null) {
      group = new BasicFeatureMap(this, ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP);
    }
    return group;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CompositeOpParameterType> getParameter() {
    return getGroup().list(ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CompositeOpPortType> getInputPort() {
    return getGroup().list(ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CompositeOpPortType> getOutputPort() {
    return getGroup().list(ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitTypeType> getType() {
    return getGroup().list(ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<EnumLiteralsType> getEnumLiterals() {
    return getGroup().list(ToolkitPackage.Literals.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS);
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN,
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN,
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE,
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE,
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
  public boolean isPotentialMain() {
    return potentialMain;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setPotentialMain(boolean newPotentialMain) {
    boolean oldPotentialMain = potentialMain;
    potentialMain = newPotentialMain;
    boolean oldPotentialMainESet = potentialMainESet;
    potentialMainESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN,
              oldPotentialMain,
              potentialMain,
              !oldPotentialMainESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetPotentialMain() {
    boolean oldPotentialMain = potentialMain;
    boolean oldPotentialMainESet = potentialMainESet;
    potentialMain = POTENTIAL_MAIN_EDEFAULT;
    potentialMainESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN,
              oldPotentialMain,
              POTENTIAL_MAIN_EDEFAULT,
              oldPotentialMainESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetPotentialMain() {
    return potentialMainESet;
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX,
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
              ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX,
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
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP:
        return ((InternalEList<?>) getGroup()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return ((InternalEList<?>) getParameter()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return ((InternalEList<?>) getInputPort()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return ((InternalEList<?>) getOutputPort()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE:
        return ((InternalEList<?>) getType()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS:
        return ((InternalEList<?>) getEnumLiterals()).basicRemove(otherEnd, msgs);
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
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP:
        if (coreType) return getGroup();
        return ((FeatureMap.Internal) getGroup()).getWrapper();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return getParameter();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return getInputPort();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return getOutputPort();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE:
        return getType();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS:
        return getEnumLiterals();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN:
        return getColumn();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE:
        return getLine();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN:
        return isPotentialMain();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX:
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
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP:
        ((FeatureMap.Internal) getGroup()).set(newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        getParameter().clear();
        getParameter().addAll((Collection<? extends CompositeOpParameterType>) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        getInputPort().clear();
        getInputPort().addAll((Collection<? extends CompositeOpPortType>) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        getOutputPort().clear();
        getOutputPort().addAll((Collection<? extends CompositeOpPortType>) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE:
        getType().clear();
        getType().addAll((Collection<? extends ToolkitTypeType>) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS:
        getEnumLiterals().clear();
        getEnumLiterals().addAll((Collection<? extends EnumLiteralsType>) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN:
        setColumn((Integer) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE:
        setLine((Integer) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN:
        setPotentialMain((Boolean) newValue);
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX:
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
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP:
        getGroup().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        getParameter().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        getInputPort().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        getOutputPort().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE:
        getType().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS:
        getEnumLiterals().clear();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN:
        unsetColumn();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE:
        unsetLine();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN:
        unsetPotentialMain();
        return;
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX:
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
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP:
        return group != null && !group.isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return !getParameter().isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return !getInputPort().isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return !getOutputPort().isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE:
        return !getType().isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS:
        return !getEnumLiterals().isEmpty();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN:
        return isSetColumn();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE:
        return isSetLine();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN:
        return isSetPotentialMain();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX:
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
    result.append(" (group: ");
    result.append(group);
    result.append(", column: ");
    if (columnESet) result.append(column);
    else result.append("<unset>");
    result.append(", line: ");
    if (lineESet) result.append(line);
    else result.append("<unset>");
    result.append(", potentialMain: ");
    if (potentialMainESet) result.append(potentialMain);
    else result.append("<unset>");
    result.append(", uriIndex: ");
    if (uriIndexESet) result.append(uriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // CompositeToolkitOperatorTypeImpl
