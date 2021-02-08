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
import com.ibm.xmlns.prod.streams.spl.operator.ContextType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType;
import com.ibm.xmlns.prod.streams.spl.operator.IconUriType;
import com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType;
import com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricsType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType;
import com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EDataTypeEList;
import org.eclipse.emf.ecore.util.EObjectContainmentEList;
import org.eclipse.emf.ecore.util.InternalEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Context Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getIconUri <em>Icon
 *       Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getMetrics
 *       <em>Metrics</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getCustomLiterals
 *       <em>Custom Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getCustomOutputFunctions
 *       <em>Custom Output Functions</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getLibraryDependencies
 *       <em>Library Dependencies</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getProvidesSingleThreadedContext
 *       <em>Provides Single Threaded Context</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getIncrementalCompilationStrategy
 *       <em>Incremental Compilation Strategy</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#isAllowCustomLogic
 *       <em>Allow Custom Logic</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getCodeTemplates
 *       <em>Code Templates</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getSplExpressionTree
 *       <em>Spl Expression Tree</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getCapability
 *       <em>Capability</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl#getVerificationModule
 *       <em>Verification Module</em>}
 * </ul>
 *
 * @generated
 */
public class ContextTypeImpl extends MinimalEObjectImpl.Container implements ContextType {
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
   * The cached value of the '{@link #getCustomOutputFunctions() <em>Custom Output Functions</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCustomOutputFunctions()
   * @generated
   * @ordered
   */
  protected CustomOutputFunctionsType customOutputFunctions;

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
  protected LibraryDependenciesType libraryDependencies;

  /**
   * The default value of the '{@link #getProvidesSingleThreadedContext() <em>Provides Single
   * Threaded Context</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getProvidesSingleThreadedContext()
   * @generated
   * @ordered
   */
  protected static final SingleThreadedContextType PROVIDES_SINGLE_THREADED_CONTEXT_EDEFAULT =
      SingleThreadedContextType.NEVER;

  /**
   * The cached value of the '{@link #getProvidesSingleThreadedContext() <em>Provides Single
   * Threaded Context</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getProvidesSingleThreadedContext()
   * @generated
   * @ordered
   */
  protected SingleThreadedContextType providesSingleThreadedContext =
      PROVIDES_SINGLE_THREADED_CONTEXT_EDEFAULT;

  /**
   * This is true if the Provides Single Threaded Context attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean providesSingleThreadedContextESet;

  /**
   * The default value of the '{@link #getIncrementalCompilationStrategy() <em>Incremental
   * Compilation Strategy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIncrementalCompilationStrategy()
   * @generated
   * @ordered
   */
  protected static final IncrementalCompilationStrategyType
      INCREMENTAL_COMPILATION_STRATEGY_EDEFAULT =
          IncrementalCompilationStrategyType.SOURCE_DEPENDENT;

  /**
   * The cached value of the '{@link #getIncrementalCompilationStrategy() <em>Incremental
   * Compilation Strategy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getIncrementalCompilationStrategy()
   * @generated
   * @ordered
   */
  protected IncrementalCompilationStrategyType incrementalCompilationStrategy =
      INCREMENTAL_COMPILATION_STRATEGY_EDEFAULT;

  /**
   * This is true if the Incremental Compilation Strategy attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean incrementalCompilationStrategyESet;

  /**
   * The default value of the '{@link #isAllowCustomLogic() <em>Allow Custom Logic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowCustomLogic()
   * @generated
   * @ordered
   */
  protected static final boolean ALLOW_CUSTOM_LOGIC_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isAllowCustomLogic() <em>Allow Custom Logic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isAllowCustomLogic()
   * @generated
   * @ordered
   */
  protected boolean allowCustomLogic = ALLOW_CUSTOM_LOGIC_EDEFAULT;

  /**
   * This is true if the Allow Custom Logic attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean allowCustomLogicESet;

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
   * The cached value of the '{@link #getSplExpressionTree() <em>Spl Expression Tree</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSplExpressionTree()
   * @generated
   * @ordered
   */
  protected SplExpressionTreeType splExpressionTree;

  /**
   * The cached value of the '{@link #getCapability() <em>Capability</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCapability()
   * @generated
   * @ordered
   */
  protected EList<String> capability;

  /**
   * The default value of the '{@link #getVerificationModule() <em>Verification Module</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getVerificationModule()
   * @generated
   * @ordered
   */
  protected static final String VERIFICATION_MODULE_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getVerificationModule() <em>Verification Module</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getVerificationModule()
   * @generated
   * @ordered
   */
  protected String verificationModule = VERIFICATION_MODULE_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ContextTypeImpl() {
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
    return OperatorPackage.Literals.CONTEXT_TYPE;
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
              OperatorPackage.CONTEXT_TYPE__DESCRIPTION,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      if (newDescription != null)
        msgs =
            ((InternalEObject) newDescription)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__DESCRIPTION,
                    null,
                    msgs);
      msgs = basicSetDescription(newDescription, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__DESCRIPTION,
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
              IconUriType.class, this, OperatorPackage.CONTEXT_TYPE__ICON_URI);
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
              OperatorPackage.CONTEXT_TYPE__METRICS,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__METRICS,
                    null,
                    msgs);
      if (newMetrics != null)
        msgs =
            ((InternalEObject) newMetrics)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__METRICS,
                    null,
                    msgs);
      msgs = basicSetMetrics(newMetrics, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__METRICS,
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
              OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS,
                    null,
                    msgs);
      if (newCustomLiterals != null)
        msgs =
            ((InternalEObject) newCustomLiterals)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS,
                    null,
                    msgs);
      msgs = basicSetCustomLiterals(newCustomLiterals, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS,
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
  public CustomOutputFunctionsType getCustomOutputFunctions() {
    return customOutputFunctions;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCustomOutputFunctions(
      CustomOutputFunctionsType newCustomOutputFunctions, NotificationChain msgs) {
    CustomOutputFunctionsType oldCustomOutputFunctions = customOutputFunctions;
    customOutputFunctions = newCustomOutputFunctions;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS,
              oldCustomOutputFunctions,
              newCustomOutputFunctions);
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
  public void setCustomOutputFunctions(CustomOutputFunctionsType newCustomOutputFunctions) {
    if (newCustomOutputFunctions != customOutputFunctions) {
      NotificationChain msgs = null;
      if (customOutputFunctions != null)
        msgs =
            ((InternalEObject) customOutputFunctions)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS,
                    null,
                    msgs);
      if (newCustomOutputFunctions != null)
        msgs =
            ((InternalEObject) newCustomOutputFunctions)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS,
                    null,
                    msgs);
      msgs = basicSetCustomOutputFunctions(newCustomOutputFunctions, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS,
              newCustomOutputFunctions,
              newCustomOutputFunctions));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LibraryDependenciesType getLibraryDependencies() {
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
      LibraryDependenciesType newLibraryDependencies, NotificationChain msgs) {
    LibraryDependenciesType oldLibraryDependencies = libraryDependencies;
    libraryDependencies = newLibraryDependencies;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
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
  public void setLibraryDependencies(LibraryDependenciesType newLibraryDependencies) {
    if (newLibraryDependencies != libraryDependencies) {
      NotificationChain msgs = null;
      if (libraryDependencies != null)
        msgs =
            ((InternalEObject) libraryDependencies)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
                    null,
                    msgs);
      if (newLibraryDependencies != null)
        msgs =
            ((InternalEObject) newLibraryDependencies)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
                    null,
                    msgs);
      msgs = basicSetLibraryDependencies(newLibraryDependencies, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES,
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
  public SingleThreadedContextType getProvidesSingleThreadedContext() {
    return providesSingleThreadedContext;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setProvidesSingleThreadedContext(
      SingleThreadedContextType newProvidesSingleThreadedContext) {
    SingleThreadedContextType oldProvidesSingleThreadedContext = providesSingleThreadedContext;
    providesSingleThreadedContext =
        newProvidesSingleThreadedContext == null
            ? PROVIDES_SINGLE_THREADED_CONTEXT_EDEFAULT
            : newProvidesSingleThreadedContext;
    boolean oldProvidesSingleThreadedContextESet = providesSingleThreadedContextESet;
    providesSingleThreadedContextESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT,
              oldProvidesSingleThreadedContext,
              providesSingleThreadedContext,
              !oldProvidesSingleThreadedContextESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetProvidesSingleThreadedContext() {
    SingleThreadedContextType oldProvidesSingleThreadedContext = providesSingleThreadedContext;
    boolean oldProvidesSingleThreadedContextESet = providesSingleThreadedContextESet;
    providesSingleThreadedContext = PROVIDES_SINGLE_THREADED_CONTEXT_EDEFAULT;
    providesSingleThreadedContextESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT,
              oldProvidesSingleThreadedContext,
              PROVIDES_SINGLE_THREADED_CONTEXT_EDEFAULT,
              oldProvidesSingleThreadedContextESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetProvidesSingleThreadedContext() {
    return providesSingleThreadedContextESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IncrementalCompilationStrategyType getIncrementalCompilationStrategy() {
    return incrementalCompilationStrategy;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setIncrementalCompilationStrategy(
      IncrementalCompilationStrategyType newIncrementalCompilationStrategy) {
    IncrementalCompilationStrategyType oldIncrementalCompilationStrategy =
        incrementalCompilationStrategy;
    incrementalCompilationStrategy =
        newIncrementalCompilationStrategy == null
            ? INCREMENTAL_COMPILATION_STRATEGY_EDEFAULT
            : newIncrementalCompilationStrategy;
    boolean oldIncrementalCompilationStrategyESet = incrementalCompilationStrategyESet;
    incrementalCompilationStrategyESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY,
              oldIncrementalCompilationStrategy,
              incrementalCompilationStrategy,
              !oldIncrementalCompilationStrategyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetIncrementalCompilationStrategy() {
    IncrementalCompilationStrategyType oldIncrementalCompilationStrategy =
        incrementalCompilationStrategy;
    boolean oldIncrementalCompilationStrategyESet = incrementalCompilationStrategyESet;
    incrementalCompilationStrategy = INCREMENTAL_COMPILATION_STRATEGY_EDEFAULT;
    incrementalCompilationStrategyESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY,
              oldIncrementalCompilationStrategy,
              INCREMENTAL_COMPILATION_STRATEGY_EDEFAULT,
              oldIncrementalCompilationStrategyESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetIncrementalCompilationStrategy() {
    return incrementalCompilationStrategyESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isAllowCustomLogic() {
    return allowCustomLogic;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setAllowCustomLogic(boolean newAllowCustomLogic) {
    boolean oldAllowCustomLogic = allowCustomLogic;
    allowCustomLogic = newAllowCustomLogic;
    boolean oldAllowCustomLogicESet = allowCustomLogicESet;
    allowCustomLogicESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC,
              oldAllowCustomLogic,
              allowCustomLogic,
              !oldAllowCustomLogicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetAllowCustomLogic() {
    boolean oldAllowCustomLogic = allowCustomLogic;
    boolean oldAllowCustomLogicESet = allowCustomLogicESet;
    allowCustomLogic = ALLOW_CUSTOM_LOGIC_EDEFAULT;
    allowCustomLogicESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC,
              oldAllowCustomLogic,
              ALLOW_CUSTOM_LOGIC_EDEFAULT,
              oldAllowCustomLogicESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetAllowCustomLogic() {
    return allowCustomLogicESet;
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
              OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES,
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
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES,
                    null,
                    msgs);
      if (newCodeTemplates != null)
        msgs =
            ((InternalEObject) newCodeTemplates)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES,
                    null,
                    msgs);
      msgs = basicSetCodeTemplates(newCodeTemplates, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES,
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
  public SplExpressionTreeType getSplExpressionTree() {
    return splExpressionTree;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSplExpressionTree(
      SplExpressionTreeType newSplExpressionTree, NotificationChain msgs) {
    SplExpressionTreeType oldSplExpressionTree = splExpressionTree;
    splExpressionTree = newSplExpressionTree;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE,
              oldSplExpressionTree,
              newSplExpressionTree);
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
  public void setSplExpressionTree(SplExpressionTreeType newSplExpressionTree) {
    if (newSplExpressionTree != splExpressionTree) {
      NotificationChain msgs = null;
      if (splExpressionTree != null)
        msgs =
            ((InternalEObject) splExpressionTree)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE,
                    null,
                    msgs);
      if (newSplExpressionTree != null)
        msgs =
            ((InternalEObject) newSplExpressionTree)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE,
                    null,
                    msgs);
      msgs = basicSetSplExpressionTree(newSplExpressionTree, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE,
              newSplExpressionTree,
              newSplExpressionTree));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<String> getCapability() {
    if (capability == null) {
      capability =
          new EDataTypeEList<String>(String.class, this, OperatorPackage.CONTEXT_TYPE__CAPABILITY);
    }
    return capability;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getVerificationModule() {
    return verificationModule;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setVerificationModule(String newVerificationModule) {
    String oldVerificationModule = verificationModule;
    verificationModule = newVerificationModule;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.CONTEXT_TYPE__VERIFICATION_MODULE,
              oldVerificationModule,
              verificationModule));
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
      case OperatorPackage.CONTEXT_TYPE__DESCRIPTION:
        return basicSetDescription(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__ICON_URI:
        return ((InternalEList<?>) getIconUri()).basicRemove(otherEnd, msgs);
      case OperatorPackage.CONTEXT_TYPE__METRICS:
        return basicSetMetrics(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS:
        return basicSetCustomLiterals(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS:
        return basicSetCustomOutputFunctions(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return basicSetLibraryDependencies(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES:
        return basicSetCodeTemplates(null, msgs);
      case OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE:
        return basicSetSplExpressionTree(null, msgs);
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
      case OperatorPackage.CONTEXT_TYPE__DESCRIPTION:
        return getDescription();
      case OperatorPackage.CONTEXT_TYPE__ICON_URI:
        return getIconUri();
      case OperatorPackage.CONTEXT_TYPE__METRICS:
        return getMetrics();
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS:
        return getCustomLiterals();
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS:
        return getCustomOutputFunctions();
      case OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return getLibraryDependencies();
      case OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT:
        return getProvidesSingleThreadedContext();
      case OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY:
        return getIncrementalCompilationStrategy();
      case OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC:
        return isAllowCustomLogic();
      case OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES:
        return getCodeTemplates();
      case OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE:
        return getSplExpressionTree();
      case OperatorPackage.CONTEXT_TYPE__CAPABILITY:
        return getCapability();
      case OperatorPackage.CONTEXT_TYPE__VERIFICATION_MODULE:
        return getVerificationModule();
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
      case OperatorPackage.CONTEXT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__ICON_URI:
        getIconUri().clear();
        getIconUri().addAll((Collection<? extends IconUriType>) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__METRICS:
        setMetrics((MetricsType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS:
        setCustomLiterals((EnumerationsType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS:
        setCustomOutputFunctions((CustomOutputFunctionsType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        setLibraryDependencies((LibraryDependenciesType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT:
        setProvidesSingleThreadedContext((SingleThreadedContextType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY:
        setIncrementalCompilationStrategy((IncrementalCompilationStrategyType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC:
        setAllowCustomLogic((Boolean) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES:
        setCodeTemplates((CodeTemplatesType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE:
        setSplExpressionTree((SplExpressionTreeType) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__CAPABILITY:
        getCapability().clear();
        getCapability().addAll((Collection<? extends String>) newValue);
        return;
      case OperatorPackage.CONTEXT_TYPE__VERIFICATION_MODULE:
        setVerificationModule((String) newValue);
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
      case OperatorPackage.CONTEXT_TYPE__DESCRIPTION:
        setDescription((DescriptionType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__ICON_URI:
        getIconUri().clear();
        return;
      case OperatorPackage.CONTEXT_TYPE__METRICS:
        setMetrics((MetricsType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS:
        setCustomLiterals((EnumerationsType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS:
        setCustomOutputFunctions((CustomOutputFunctionsType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        setLibraryDependencies((LibraryDependenciesType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT:
        unsetProvidesSingleThreadedContext();
        return;
      case OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY:
        unsetIncrementalCompilationStrategy();
        return;
      case OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC:
        unsetAllowCustomLogic();
        return;
      case OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES:
        setCodeTemplates((CodeTemplatesType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE:
        setSplExpressionTree((SplExpressionTreeType) null);
        return;
      case OperatorPackage.CONTEXT_TYPE__CAPABILITY:
        getCapability().clear();
        return;
      case OperatorPackage.CONTEXT_TYPE__VERIFICATION_MODULE:
        setVerificationModule(VERIFICATION_MODULE_EDEFAULT);
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
      case OperatorPackage.CONTEXT_TYPE__DESCRIPTION:
        return description != null;
      case OperatorPackage.CONTEXT_TYPE__ICON_URI:
        return iconUri != null && !iconUri.isEmpty();
      case OperatorPackage.CONTEXT_TYPE__METRICS:
        return metrics != null;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_LITERALS:
        return customLiterals != null;
      case OperatorPackage.CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS:
        return customOutputFunctions != null;
      case OperatorPackage.CONTEXT_TYPE__LIBRARY_DEPENDENCIES:
        return libraryDependencies != null;
      case OperatorPackage.CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT:
        return isSetProvidesSingleThreadedContext();
      case OperatorPackage.CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY:
        return isSetIncrementalCompilationStrategy();
      case OperatorPackage.CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC:
        return isSetAllowCustomLogic();
      case OperatorPackage.CONTEXT_TYPE__CODE_TEMPLATES:
        return codeTemplates != null;
      case OperatorPackage.CONTEXT_TYPE__SPL_EXPRESSION_TREE:
        return splExpressionTree != null;
      case OperatorPackage.CONTEXT_TYPE__CAPABILITY:
        return capability != null && !capability.isEmpty();
      case OperatorPackage.CONTEXT_TYPE__VERIFICATION_MODULE:
        return VERIFICATION_MODULE_EDEFAULT == null
            ? verificationModule != null
            : !VERIFICATION_MODULE_EDEFAULT.equals(verificationModule);
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
    result.append(" (providesSingleThreadedContext: ");
    if (providesSingleThreadedContextESet) result.append(providesSingleThreadedContext);
    else result.append("<unset>");
    result.append(", incrementalCompilationStrategy: ");
    if (incrementalCompilationStrategyESet) result.append(incrementalCompilationStrategy);
    else result.append("<unset>");
    result.append(", allowCustomLogic: ");
    if (allowCustomLogicESet) result.append(allowCustomLogic);
    else result.append("<unset>");
    result.append(", capability: ");
    result.append(capability);
    result.append(", verificationModule: ");
    result.append(verificationModule);
    result.append(')');
    return result.toString();
  }
} // ContextTypeImpl
