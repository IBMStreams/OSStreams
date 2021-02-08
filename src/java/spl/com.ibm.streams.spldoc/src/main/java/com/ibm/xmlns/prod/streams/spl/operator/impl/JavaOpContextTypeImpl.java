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

/** */
package com.ibm.xmlns.prod.streams.spl.operator.impl;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType;
import com.ibm.xmlns.prod.streams.spl.operator.IconUriType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricsType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Java Op Context Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getIconUri
 *       <em>Icon Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getMetrics
 *       <em>Metrics</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getCustomLiterals
 *       <em>Custom Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getExecutionSettings
 *       <em>Execution Settings</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getLibraryDependencies
 *       <em>Library Dependencies</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl#getCodeTemplates
 *       <em>Code Templates</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpContextTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpContextType {
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
   * The cached value of the '{@link #getIconUri() <em>Icon Uri</em>}' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIconUri()
   * @generated
   * @ordered
   */
  protected EList<IconUriType> iconUri;

  /**
   * The cached value of the '{@link #getMetrics() <em>Metrics</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getMetrics()
   * @generated
   * @ordered
   */
  protected MetricsType metrics;

  /**
   * The cached value of the '{@link #getCustomLiterals() <em>Custom Literals</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCustomLiterals()
   * @generated
   * @ordered
   */
  protected EnumerationsType customLiterals;

  /**
   * The cached value of the '{@link #getExecutionSettings() <em>Execution Settings</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getExecutionSettings()
   * @generated
   * @ordered
   */
  protected JavaOpExecutionSettingsType executionSettings;

  /**
   * The cached value of the '{@link #getLibraryDependencies() <em>Library Dependencies</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLibraryDependencies()
   * @generated
   * @ordered
   */
  protected JavaOpLibraryDependenciesType libraryDependencies;

  /**
   * The cached value of the '{@link #getCodeTemplates() <em>Code Templates</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCodeTemplates()
   * @generated
   * @ordered
   */
  protected CodeTemplatesType codeTemplates;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpContextTypeImpl() {
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
    return OperatorPackage.Literals.JAVA_OP_CONTEXT_TYPE;
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
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION,
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
  public EList<IconUriType> getIconUri() {
    if (iconUri == null) {
      iconUri =
          new EObjectContainmentEList<IconUriType>(
              IconUriType.class, this, OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI);
    }
    return iconUri;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MetricsType getMetrics() {
    return metrics;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetMetrics(MetricsType newMetrics, NotificationChain msgs) {
    MetricsType oldMetrics = metrics;
    metrics = newMetrics;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS,
              oldMetrics,
              newMetrics);
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
  public void setMetrics(MetricsType newMetrics) {
    if (newMetrics != metrics) {
      NotificationChain msgs = null;
      if (metrics != null)
        msgs =
            ((InternalEObject) metrics)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS,
                    null,
                    msgs);
      if (newMetrics != null)
        msgs =
            ((InternalEObject) newMetrics)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS,
                    null,
                    msgs);
      msgs = basicSetMetrics(newMetrics, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS,
              newMetrics,
              newMetrics));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumerationsType getCustomLiterals() {
    return customLiterals;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCustomLiterals(
      EnumerationsType newCustomLiterals, NotificationChain msgs) {
    EnumerationsType oldCustomLiterals = customLiterals;
    customLiterals = newCustomLiterals;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS,
              oldCustomLiterals,
              newCustomLiterals);
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
  public void setCustomLiterals(EnumerationsType newCustomLiterals) {
    if (newCustomLiterals != customLiterals) {
      NotificationChain msgs = null;
      if (customLiterals != null)
        msgs =
            ((InternalEObject) customLiterals)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS,
                    null,
                    msgs);
      if (newCustomLiterals != null)
        msgs =
            ((InternalEObject) newCustomLiterals)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS,
                    null,
                    msgs);
      msgs = basicSetCustomLiterals(newCustomLiterals, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS,
              newCustomLiterals,
              newCustomLiterals));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpExecutionSettingsType getExecutionSettings() {
    return executionSettings;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetExecutionSettings(
      JavaOpExecutionSettingsType newExecutionSettings, NotificationChain msgs) {
    JavaOpExecutionSettingsType oldExecutionSettings = executionSettings;
    executionSettings = newExecutionSettings;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS,
              oldExecutionSettings,
              newExecutionSettings);
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
  public void setExecutionSettings(JavaOpExecutionSettingsType newExecutionSettings) {
    if (newExecutionSettings != executionSettings) {
      NotificationChain msgs = null;
      if (executionSettings != null)
        msgs =
            ((InternalEObject) executionSettings)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS,
                    null,
                    msgs);
      if (newExecutionSettings != null)
        msgs =
            ((InternalEObject) newExecutionSettings)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS,
                    null,
                    msgs);
      msgs = basicSetExecutionSettings(newExecutionSettings, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS,
              newExecutionSettings,
              newExecutionSettings));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpLibraryDependenciesType getLibraryDependencies() {
    return libraryDependencies;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetLibraryDependencies(
      JavaOpLibraryDependenciesType newLibraryDependencies, NotificationChain msgs) {
    JavaOpLibraryDependenciesType oldLibraryDependencies = libraryDependencies;
    libraryDependencies = newLibraryDependencies;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
              oldLibraryDependencies,
              newLibraryDependencies);
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
  public void setLibraryDependencies(JavaOpLibraryDependenciesType newLibraryDependencies) {
    if (newLibraryDependencies != libraryDependencies) {
      NotificationChain msgs = null;
      if (libraryDependencies != null)
        msgs =
            ((InternalEObject) libraryDependencies)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
                    null,
                    msgs);
      if (newLibraryDependencies != null)
        msgs =
            ((InternalEObject) newLibraryDependencies)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE
                        - OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
                    null,
                    msgs);
      msgs = basicSetLibraryDependencies(newLibraryDependencies, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
              newLibraryDependencies,
              newLibraryDependencies));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeTemplatesType getCodeTemplates() {
    return codeTemplates;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCodeTemplates(
      CodeTemplatesType newCodeTemplates, NotificationChain msgs) {
    CodeTemplatesType oldCodeTemplates = codeTemplates;
    codeTemplates = newCodeTemplates;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES,
              oldCodeTemplates,
              newCodeTemplates);
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
  public void setCodeTemplates(CodeTemplatesType newCodeTemplates) {
    if (newCodeTemplates != codeTemplates) {
      NotificationChain msgs = null;
      if (codeTemplates != null)
        msgs =
            ((InternalEObject) codeTemplates)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES,
                    null,
                    msgs);
      if (newCodeTemplates != null)
        msgs =
            ((InternalEObject) newCodeTemplates)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES,
                    null,
                    msgs);
      msgs = basicSetCodeTemplates(newCodeTemplates, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES,
              newCodeTemplates,
              newCodeTemplates));
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
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI:
        return ((InternalEList<?>) getIconUri()).basicRemove(otherEnd, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS:
        return basicSetMetrics(null, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS:
        return basicSetCustomLiterals(null, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS:
        return basicSetExecutionSettings(null, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return basicSetLibraryDependencies(null, msgs);
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES:
        return basicSetCodeTemplates(null, msgs);
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
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI:
        return getIconUri();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS:
        return getMetrics();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS:
        return getCustomLiterals();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS:
        return getExecutionSettings();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return getLibraryDependencies();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES:
        return getCodeTemplates();
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
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI:
        getIconUri().clear();
        getIconUri().addAll((Collection<? extends IconUriType>) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS:
        setMetrics((MetricsType) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS:
        setCustomLiterals((EnumerationsType) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS:
        setExecutionSettings((JavaOpExecutionSettingsType) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        setLibraryDependencies((JavaOpLibraryDependenciesType) newValue);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES:
        setCodeTemplates((CodeTemplatesType) newValue);
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
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI:
        getIconUri().clear();
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS:
        setMetrics((MetricsType) null);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS:
        setCustomLiterals((EnumerationsType) null);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS:
        setExecutionSettings((JavaOpExecutionSettingsType) null);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        setLibraryDependencies((JavaOpLibraryDependenciesType) null);
        return;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES:
        setCodeTemplates((CodeTemplatesType) null);
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
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__ICON_URI:
        return iconUri != null && !iconUri.isEmpty();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__METRICS:
        return metrics != null;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS:
        return customLiterals != null;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS:
        return executionSettings != null;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return libraryDependencies != null;
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES:
        return codeTemplates != null;
    }
    return super.eIsSet(featureID);
  }
} // JavaOpContextTypeImpl
