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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Body Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl#getTypes
 *       <em>Types</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl#getGraph
 *       <em>Graph</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl#getConfigs
 *       <em>Configs</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeBodyTypeImpl extends EObjectImpl implements CompositeBodyType {
  /**
   * The cached value of the '{@link #getTypes() <em>Types</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getTypes()
   * @generated
   * @ordered
   */
  protected CompositeTypesType types;

  /**
   * The cached value of the '{@link #getParameters() <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getParameters()
   * @generated
   * @ordered
   */
  protected CompositeParametersType parameters;

  /**
   * The cached value of the '{@link #getGraph() <em>Graph</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getGraph()
   * @generated
   * @ordered
   */
  protected CompositeGraphType graph;

  /**
   * The cached value of the '{@link #getConfigs() <em>Configs</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getConfigs()
   * @generated
   * @ordered
   */
  protected ConfigsType configs;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeBodyTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_BODY_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeTypesType getTypes() {
    return types;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetTypes(CompositeTypesType newTypes, NotificationChain msgs) {
    CompositeTypesType oldTypes = types;
    types = newTypes;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_BODY_TYPE__TYPES, oldTypes, newTypes);
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
  public void setTypes(CompositeTypesType newTypes) {
    if (newTypes != types) {
      NotificationChain msgs = null;
      if (types != null)
        msgs =
            ((InternalEObject) types)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__TYPES,
                    null,
                    msgs);
      if (newTypes != null)
        msgs =
            ((InternalEObject) newTypes)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__TYPES,
                    null,
                    msgs);
      msgs = basicSetTypes(newTypes, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_BODY_TYPE__TYPES, newTypes, newTypes));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParametersType getParameters() {
    return parameters;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetParameters(
      CompositeParametersType newParameters, NotificationChain msgs) {
    CompositeParametersType oldParameters = parameters;
    parameters = newParameters;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS,
              oldParameters,
              newParameters);
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
  public void setParameters(CompositeParametersType newParameters) {
    if (newParameters != parameters) {
      NotificationChain msgs = null;
      if (parameters != null)
        msgs =
            ((InternalEObject) parameters)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS,
                    null,
                    msgs);
      if (newParameters != null)
        msgs =
            ((InternalEObject) newParameters)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS,
                    null,
                    msgs);
      msgs = basicSetParameters(newParameters, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS,
              newParameters,
              newParameters));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeGraphType getGraph() {
    return graph;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetGraph(CompositeGraphType newGraph, NotificationChain msgs) {
    CompositeGraphType oldGraph = graph;
    graph = newGraph;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_BODY_TYPE__GRAPH, oldGraph, newGraph);
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
  public void setGraph(CompositeGraphType newGraph) {
    if (newGraph != graph) {
      NotificationChain msgs = null;
      if (graph != null)
        msgs =
            ((InternalEObject) graph)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__GRAPH,
                    null,
                    msgs);
      if (newGraph != null)
        msgs =
            ((InternalEObject) newGraph)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__GRAPH,
                    null,
                    msgs);
      msgs = basicSetGraph(newGraph, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this, Notification.SET, CodePackage.COMPOSITE_BODY_TYPE__GRAPH, newGraph, newGraph));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigsType getConfigs() {
    return configs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetConfigs(ConfigsType newConfigs, NotificationChain msgs) {
    ConfigsType oldConfigs = configs;
    configs = newConfigs;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_BODY_TYPE__CONFIGS,
              oldConfigs,
              newConfigs);
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
  public void setConfigs(ConfigsType newConfigs) {
    if (newConfigs != configs) {
      NotificationChain msgs = null;
      if (configs != null)
        msgs =
            ((InternalEObject) configs)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__CONFIGS,
                    null,
                    msgs);
      if (newConfigs != null)
        msgs =
            ((InternalEObject) newConfigs)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_BODY_TYPE__CONFIGS,
                    null,
                    msgs);
      msgs = basicSetConfigs(newConfigs, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_BODY_TYPE__CONFIGS,
              newConfigs,
              newConfigs));
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
      case CodePackage.COMPOSITE_BODY_TYPE__TYPES:
        return basicSetTypes(null, msgs);
      case CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS:
        return basicSetParameters(null, msgs);
      case CodePackage.COMPOSITE_BODY_TYPE__GRAPH:
        return basicSetGraph(null, msgs);
      case CodePackage.COMPOSITE_BODY_TYPE__CONFIGS:
        return basicSetConfigs(null, msgs);
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
      case CodePackage.COMPOSITE_BODY_TYPE__TYPES:
        return getTypes();
      case CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS:
        return getParameters();
      case CodePackage.COMPOSITE_BODY_TYPE__GRAPH:
        return getGraph();
      case CodePackage.COMPOSITE_BODY_TYPE__CONFIGS:
        return getConfigs();
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
      case CodePackage.COMPOSITE_BODY_TYPE__TYPES:
        setTypes((CompositeTypesType) newValue);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS:
        setParameters((CompositeParametersType) newValue);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__GRAPH:
        setGraph((CompositeGraphType) newValue);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__CONFIGS:
        setConfigs((ConfigsType) newValue);
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
      case CodePackage.COMPOSITE_BODY_TYPE__TYPES:
        setTypes((CompositeTypesType) null);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS:
        setParameters((CompositeParametersType) null);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__GRAPH:
        setGraph((CompositeGraphType) null);
        return;
      case CodePackage.COMPOSITE_BODY_TYPE__CONFIGS:
        setConfigs((ConfigsType) null);
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
      case CodePackage.COMPOSITE_BODY_TYPE__TYPES:
        return types != null;
      case CodePackage.COMPOSITE_BODY_TYPE__PARAMETERS:
        return parameters != null;
      case CodePackage.COMPOSITE_BODY_TYPE__GRAPH:
        return graph != null;
      case CodePackage.COMPOSITE_BODY_TYPE__CONFIGS:
        return configs != null;
    }
    return super.eIsSet(featureID);
  }
} // CompositeBodyTypeImpl
