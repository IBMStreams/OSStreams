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
package com.ibm.xmlns.prod.streams.spl.source.code;

import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Body Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getTypes
 *       <em>Types</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getParameters
 *       <em>Parameters</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getGraph
 *       <em>Graph</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getConfigs
 *       <em>Configs</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeBodyType()
 * @model extendedMetaData="name='compositeBodyType' kind='elementOnly'"
 * @generated
 */
public interface CompositeBodyType extends EObject {
  /**
   * Returns the value of the '<em><b>Types</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Types</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Types</em>' containment reference.
   * @see #setTypes(CompositeTypesType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeBodyType_Types()
   * @model containment="true" extendedMetaData="kind='element' name='types'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CompositeTypesType getTypes();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getTypes <em>Types</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Types</em>' containment reference.
   * @see #getTypes()
   * @generated
   */
  void setTypes(CompositeTypesType value);

  /**
   * Returns the value of the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameters</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameters</em>' containment reference.
   * @see #setParameters(CompositeParametersType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeBodyType_Parameters()
   * @model containment="true" extendedMetaData="kind='element' name='parameters'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CompositeParametersType getParameters();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getParameters
   * <em>Parameters</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Parameters</em>' containment reference.
   * @see #getParameters()
   * @generated
   */
  void setParameters(CompositeParametersType value);

  /**
   * Returns the value of the '<em><b>Graph</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Graph</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Graph</em>' containment reference.
   * @see #setGraph(CompositeGraphType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeBodyType_Graph()
   * @model containment="true" extendedMetaData="kind='element' name='graph'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CompositeGraphType getGraph();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getGraph <em>Graph</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Graph</em>' containment reference.
   * @see #getGraph()
   * @generated
   */
  void setGraph(CompositeGraphType value);

  /**
   * Returns the value of the '<em><b>Configs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Configs</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Configs</em>' containment reference.
   * @see #setConfigs(ConfigsType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getCompositeBodyType_Configs()
   * @model containment="true" extendedMetaData="kind='element' name='configs'
   *     namespace='##targetNamespace'"
   * @generated
   */
  ConfigsType getConfigs();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getConfigs <em>Configs</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Configs</em>' containment reference.
   * @see #getConfigs()
   * @generated
   */
  void setConfigs(ConfigsType value);
} // CompositeBodyType
