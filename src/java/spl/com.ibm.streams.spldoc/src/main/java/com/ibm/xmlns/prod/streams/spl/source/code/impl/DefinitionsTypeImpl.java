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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType;
import java.util.Collection;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.EObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Definitions Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl#getTypeDefinition
 *       <em>Type Definition</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl#getFunctionDefinition
 *       <em>Function Definition</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl#getCompositeDefinition
 *       <em>Composite Definition</em>}
 * </ul>
 *
 * @generated
 */
public class DefinitionsTypeImpl extends EObjectImpl implements DefinitionsType {
  /**
   * The cached value of the '{@link #getTypeDefinition() <em>Type Definition</em>}' containment
   * reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTypeDefinition()
   * @generated
   * @ordered
   */
  protected EList<TypeDefinitionType> typeDefinition;

  /**
   * The cached value of the '{@link #getFunctionDefinition() <em>Function Definition</em>}'
   * containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getFunctionDefinition()
   * @generated
   * @ordered
   */
  protected EList<FunctionDefinitionType> functionDefinition;

  /**
   * The cached value of the '{@link #getCompositeDefinition() <em>Composite Definition</em>}'
   * containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCompositeDefinition()
   * @generated
   * @ordered
   */
  protected EList<CompositeDefinitionType> compositeDefinition;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected DefinitionsTypeImpl() {
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
    return CodePackage.Literals.DEFINITIONS_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<TypeDefinitionType> getTypeDefinition() {
    if (typeDefinition == null) {
      typeDefinition =
          new EObjectContainmentEList<TypeDefinitionType>(
              TypeDefinitionType.class, this, CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION);
    }
    return typeDefinition;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<FunctionDefinitionType> getFunctionDefinition() {
    if (functionDefinition == null) {
      functionDefinition =
          new EObjectContainmentEList<FunctionDefinitionType>(
              FunctionDefinitionType.class,
              this,
              CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION);
    }
    return functionDefinition;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CompositeDefinitionType> getCompositeDefinition() {
    if (compositeDefinition == null) {
      compositeDefinition =
          new EObjectContainmentEList<CompositeDefinitionType>(
              CompositeDefinitionType.class,
              this,
              CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION);
    }
    return compositeDefinition;
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
      case CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION:
        return ((InternalEList<?>) getTypeDefinition()).basicRemove(otherEnd, msgs);
      case CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION:
        return ((InternalEList<?>) getFunctionDefinition()).basicRemove(otherEnd, msgs);
      case CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION:
        return ((InternalEList<?>) getCompositeDefinition()).basicRemove(otherEnd, msgs);
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
      case CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION:
        return getTypeDefinition();
      case CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION:
        return getFunctionDefinition();
      case CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION:
        return getCompositeDefinition();
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
      case CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION:
        getTypeDefinition().clear();
        getTypeDefinition().addAll((Collection<? extends TypeDefinitionType>) newValue);
        return;
      case CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION:
        getFunctionDefinition().clear();
        getFunctionDefinition().addAll((Collection<? extends FunctionDefinitionType>) newValue);
        return;
      case CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION:
        getCompositeDefinition().clear();
        getCompositeDefinition().addAll((Collection<? extends CompositeDefinitionType>) newValue);
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
      case CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION:
        getTypeDefinition().clear();
        return;
      case CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION:
        getFunctionDefinition().clear();
        return;
      case CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION:
        getCompositeDefinition().clear();
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
      case CodePackage.DEFINITIONS_TYPE__TYPE_DEFINITION:
        return typeDefinition != null && !typeDefinition.isEmpty();
      case CodePackage.DEFINITIONS_TYPE__FUNCTION_DEFINITION:
        return functionDefinition != null && !functionDefinition.isEmpty();
      case CodePackage.DEFINITIONS_TYPE__COMPOSITE_DEFINITION:
        return compositeDefinition != null && !compositeDefinition.isEmpty();
    }
    return super.eIsSet(featureID);
  }
} // DefinitionsTypeImpl
