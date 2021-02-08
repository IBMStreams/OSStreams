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

import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType;
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
 * An implementation of the model object '<em><b>Namespace Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getGroup
 *       <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getPrimitiveOp
 *       <em>Primitive Op</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getCompositeOp
 *       <em>Composite Op</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getFunction
 *       <em>Function</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getType
 *       <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getEnumLiterals
 *       <em>Enum Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl#getRelPathToNamespace
 *       <em>Rel Path To Namespace</em>}
 * </ul>
 *
 * @generated
 */
public class ToolkitNamespaceTypeImpl extends ToolkitElementTypeImpl
    implements ToolkitNamespaceType {
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
   * The default value of the '{@link #getRelPathToNamespace() <em>Rel Path To Namespace</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRelPathToNamespace()
   * @generated
   * @ordered
   */
  protected static final String REL_PATH_TO_NAMESPACE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getRelPathToNamespace() <em>Rel Path To Namespace</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getRelPathToNamespace()
   * @generated
   * @ordered
   */
  protected String relPathToNamespace = REL_PATH_TO_NAMESPACE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitNamespaceTypeImpl() {
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
    return ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE;
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
      group = new BasicFeatureMap(this, ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP);
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
  public EList<PrimitiveToolkitOperatorType> getPrimitiveOp() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CompositeToolkitOperatorType> getCompositeOp() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitFunctionType> getFunction() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE__FUNCTION);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<ToolkitTypeType> getType() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE__TYPE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<EnumLiteralsType> getEnumLiterals() {
    return getGroup().list(ToolkitPackage.Literals.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getRelPathToNamespace() {
    return relPathToNamespace;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setRelPathToNamespace(String newRelPathToNamespace) {
    String oldRelPathToNamespace = relPathToNamespace;
    relPathToNamespace = newRelPathToNamespace;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE,
              oldRelPathToNamespace,
              relPathToNamespace));
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
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP:
        return ((InternalEList<?>) getGroup()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP:
        return ((InternalEList<?>) getPrimitiveOp()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP:
        return ((InternalEList<?>) getCompositeOp()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__FUNCTION:
        return ((InternalEList<?>) getFunction()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__TYPE:
        return ((InternalEList<?>) getType()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS:
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
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP:
        if (coreType) return getGroup();
        return ((FeatureMap.Internal) getGroup()).getWrapper();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP:
        return getPrimitiveOp();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP:
        return getCompositeOp();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__FUNCTION:
        return getFunction();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__TYPE:
        return getType();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS:
        return getEnumLiterals();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE:
        return getRelPathToNamespace();
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
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP:
        ((FeatureMap.Internal) getGroup()).set(newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP:
        getPrimitiveOp().clear();
        getPrimitiveOp().addAll((Collection<? extends PrimitiveToolkitOperatorType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP:
        getCompositeOp().clear();
        getCompositeOp().addAll((Collection<? extends CompositeToolkitOperatorType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__FUNCTION:
        getFunction().clear();
        getFunction().addAll((Collection<? extends ToolkitFunctionType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__TYPE:
        getType().clear();
        getType().addAll((Collection<? extends ToolkitTypeType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS:
        getEnumLiterals().clear();
        getEnumLiterals().addAll((Collection<? extends EnumLiteralsType>) newValue);
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE:
        setRelPathToNamespace((String) newValue);
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
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP:
        getGroup().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP:
        getPrimitiveOp().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP:
        getCompositeOp().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__FUNCTION:
        getFunction().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__TYPE:
        getType().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS:
        getEnumLiterals().clear();
        return;
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE:
        setRelPathToNamespace(REL_PATH_TO_NAMESPACE_EDEFAULT);
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
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__GROUP:
        return group != null && !group.isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP:
        return !getPrimitiveOp().isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP:
        return !getCompositeOp().isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__FUNCTION:
        return !getFunction().isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__TYPE:
        return !getType().isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS:
        return !getEnumLiterals().isEmpty();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE:
        return REL_PATH_TO_NAMESPACE_EDEFAULT == null
            ? relPathToNamespace != null
            : !REL_PATH_TO_NAMESPACE_EDEFAULT.equals(relPathToNamespace);
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
    result.append(", relPathToNamespace: ");
    result.append(relPathToNamespace);
    result.append(')');
    return result.toString();
  }
} // ToolkitNamespaceTypeImpl
