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
import com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType;
import com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
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
 * An implementation of the model object '<em><b>Primitive Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getGroup
 *       <em>Group</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getCustomLiterals
 *       <em>Custom Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getOutputFunctions
 *       <em>Output Functions</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getParametersDescription
 *       <em>Parameters Description</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getInputPort
 *       <em>Input Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getOutputPort
 *       <em>Output Port</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getCodeTemplate
 *       <em>Code Template</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#isInvalid
 *       <em>Invalid</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getLanguage
 *       <em>Language</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl#getModelUriIndex
 *       <em>Model Uri Index</em>}
 * </ul>
 *
 * @generated
 */
public class PrimitiveToolkitOperatorTypeImpl extends BaseToolkitOperatorTypeImpl
    implements PrimitiveToolkitOperatorType {
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
   * The default value of the '{@link #isInvalid() <em>Invalid</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isInvalid()
   * @generated
   * @ordered
   */
  protected static final boolean INVALID_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isInvalid() <em>Invalid</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isInvalid()
   * @generated
   * @ordered
   */
  protected boolean invalid = INVALID_EDEFAULT;

  /**
   * This is true if the Invalid attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean invalidESet;

  /**
   * The default value of the '{@link #getLanguage() <em>Language</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLanguage()
   * @generated
   * @ordered
   */
  protected static final LanguageType LANGUAGE_EDEFAULT = LanguageType.CPP;

  /**
   * The cached value of the '{@link #getLanguage() <em>Language</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLanguage()
   * @generated
   * @ordered
   */
  protected LanguageType language = LANGUAGE_EDEFAULT;

  /**
   * This is true if the Language attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean languageESet;

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
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected PrimitiveToolkitOperatorTypeImpl() {
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
    return ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE;
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
      group = new BasicFeatureMap(this, ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP);
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
  public EList<CustomLiteralsType> getCustomLiterals() {
    return getGroup()
        .list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<OutputFunctionsType> getOutputFunctions() {
    return getGroup()
        .list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<DescriptionType> getParametersDescription() {
    return getGroup()
        .list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<PrimitiveOpParameterType> getParameter() {
    return getGroup().list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<PrimitiveOpInputPortType> getInputPort() {
    return getGroup().list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<PrimitiveOpOutputPortType> getOutputPort() {
    return getGroup().list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<CodeTemplateType> getCodeTemplate() {
    return getGroup().list(ToolkitPackage.Literals.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isInvalid() {
    return invalid;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setInvalid(boolean newInvalid) {
    boolean oldInvalid = invalid;
    invalid = newInvalid;
    boolean oldInvalidESet = invalidESet;
    invalidESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID,
              oldInvalid,
              invalid,
              !oldInvalidESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetInvalid() {
    boolean oldInvalid = invalid;
    boolean oldInvalidESet = invalidESet;
    invalid = INVALID_EDEFAULT;
    invalidESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID,
              oldInvalid,
              INVALID_EDEFAULT,
              oldInvalidESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetInvalid() {
    return invalidESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LanguageType getLanguage() {
    return language;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setLanguage(LanguageType newLanguage) {
    LanguageType oldLanguage = language;
    language = newLanguage == null ? LANGUAGE_EDEFAULT : newLanguage;
    boolean oldLanguageESet = languageESet;
    languageESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE,
              oldLanguage,
              language,
              !oldLanguageESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetLanguage() {
    LanguageType oldLanguage = language;
    boolean oldLanguageESet = languageESet;
    language = LANGUAGE_EDEFAULT;
    languageESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE,
              oldLanguage,
              LANGUAGE_EDEFAULT,
              oldLanguageESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetLanguage() {
    return languageESet;
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
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX,
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
              ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX,
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
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP:
        return ((InternalEList<?>) getGroup()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS:
        return ((InternalEList<?>) getCustomLiterals()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS:
        return ((InternalEList<?>) getOutputFunctions()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION:
        return ((InternalEList<?>) getParametersDescription()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return ((InternalEList<?>) getParameter()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return ((InternalEList<?>) getInputPort()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return ((InternalEList<?>) getOutputPort()).basicRemove(otherEnd, msgs);
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE:
        return ((InternalEList<?>) getCodeTemplate()).basicRemove(otherEnd, msgs);
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
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP:
        if (coreType) return getGroup();
        return ((FeatureMap.Internal) getGroup()).getWrapper();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS:
        return getCustomLiterals();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS:
        return getOutputFunctions();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION:
        return getParametersDescription();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return getParameter();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return getInputPort();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return getOutputPort();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE:
        return getCodeTemplate();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID:
        return isInvalid();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE:
        return getLanguage();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX:
        return getModelUriIndex();
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
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP:
        ((FeatureMap.Internal) getGroup()).set(newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS:
        getCustomLiterals().clear();
        getCustomLiterals().addAll((Collection<? extends CustomLiteralsType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS:
        getOutputFunctions().clear();
        getOutputFunctions().addAll((Collection<? extends OutputFunctionsType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION:
        getParametersDescription().clear();
        getParametersDescription().addAll((Collection<? extends DescriptionType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        getParameter().clear();
        getParameter().addAll((Collection<? extends PrimitiveOpParameterType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        getInputPort().clear();
        getInputPort().addAll((Collection<? extends PrimitiveOpInputPortType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        getOutputPort().clear();
        getOutputPort().addAll((Collection<? extends PrimitiveOpOutputPortType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE:
        getCodeTemplate().clear();
        getCodeTemplate().addAll((Collection<? extends CodeTemplateType>) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID:
        setInvalid((Boolean) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE:
        setLanguage((LanguageType) newValue);
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX:
        setModelUriIndex((Integer) newValue);
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
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP:
        getGroup().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS:
        getCustomLiterals().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS:
        getOutputFunctions().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION:
        getParametersDescription().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        getParameter().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        getInputPort().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        getOutputPort().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE:
        getCodeTemplate().clear();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID:
        unsetInvalid();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE:
        unsetLanguage();
        return;
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX:
        unsetModelUriIndex();
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
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP:
        return group != null && !group.isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS:
        return !getCustomLiterals().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS:
        return !getOutputFunctions().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION:
        return !getParametersDescription().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER:
        return !getParameter().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT:
        return !getInputPort().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT:
        return !getOutputPort().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE:
        return !getCodeTemplate().isEmpty();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID:
        return isSetInvalid();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE:
        return isSetLanguage();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX:
        return isSetModelUriIndex();
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
    result.append(", invalid: ");
    if (invalidESet) result.append(invalid);
    else result.append("<unset>");
    result.append(", language: ");
    if (languageESet) result.append(language);
    else result.append("<unset>");
    result.append(", modelUriIndex: ");
    if (modelUriIndexESet) result.append(modelUriIndex);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // PrimitiveToolkitOperatorTypeImpl
