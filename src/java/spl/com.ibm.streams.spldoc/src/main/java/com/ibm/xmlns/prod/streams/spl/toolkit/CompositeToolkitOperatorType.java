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
package com.ibm.xmlns.prod.streams.spl.toolkit;

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.util.FeatureMap;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Composite Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getGroup
 *       <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getInputPort
 *       <em>Input Port</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getOutputPort
 *       <em>Output Port</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getType
 *       <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getEnumLiterals
 *       <em>Enum Literals</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine
 *       <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain
 *       <em>Potential Main</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex
 *       <em>Uri Index</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType()
 * @model extendedMetaData="name='compositeToolkitOperatorType' kind='elementOnly'"
 * @generated
 */
public interface CompositeToolkitOperatorType extends BaseToolkitOperatorType {
  /**
   * Returns the value of the '<em><b>Group</b></em>' attribute list. The list contents are of type
   * {@link org.eclipse.emf.ecore.util.FeatureMap.Entry}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Group</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Group</em>' attribute list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_Group()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='group' name='group:5'"
   * @generated
   */
  FeatureMap getGroup();

  /**
   * Returns the value of the '<em><b>Parameter</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameter</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameter</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_Parameter()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='parameter' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<CompositeOpParameterType> getParameter();

  /**
   * Returns the value of the '<em><b>Input Port</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Input Port</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Input Port</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_InputPort()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='inputPort' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<CompositeOpPortType> getInputPort();

  /**
   * Returns the value of the '<em><b>Output Port</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Port</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Port</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_OutputPort()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='outputPort' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<CompositeOpPortType> getOutputPort();

  /**
   * Returns the value of the '<em><b>Type</b></em>' containment reference list. The list contents
   * are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type</em>' containment reference list isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_Type()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='type' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<ToolkitTypeType> getType();

  /**
   * Returns the value of the '<em><b>Enum Literals</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Enum Literals</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Enum Literals</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_EnumLiterals()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='enumLiterals' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<EnumLiteralsType> getEnumLiterals();

  /**
   * Returns the value of the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Column</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Column</em>' attribute.
   * @see #isSetColumn()
   * @see #unsetColumn()
   * @see #setColumn(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_Column()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='column'"
   * @generated
   */
  int getColumn();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn <em>Column</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Column</em>' attribute.
   * @see #isSetColumn()
   * @see #unsetColumn()
   * @see #getColumn()
   * @generated
   */
  void setColumn(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn <em>Column</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetColumn()
   * @see #getColumn()
   * @see #setColumn(int)
   * @generated
   */
  void unsetColumn();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn <em>Column</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Column</em>' attribute is set.
   * @see #unsetColumn()
   * @see #getColumn()
   * @see #setColumn(int)
   * @generated
   */
  boolean isSetColumn();

  /**
   * Returns the value of the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Line</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Line</em>' attribute.
   * @see #isSetLine()
   * @see #unsetLine()
   * @see #setLine(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_Line()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='line'"
   * @generated
   */
  int getLine();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine <em>Line</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Line</em>' attribute.
   * @see #isSetLine()
   * @see #unsetLine()
   * @see #getLine()
   * @generated
   */
  void setLine(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine <em>Line</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetLine()
   * @see #getLine()
   * @see #setLine(int)
   * @generated
   */
  void unsetLine();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine <em>Line</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Line</em>' attribute is set.
   * @see #unsetLine()
   * @see #getLine()
   * @see #setLine(int)
   * @generated
   */
  boolean isSetLine();

  /**
   * Returns the value of the '<em><b>Potential Main</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Potential Main</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Potential Main</em>' attribute.
   * @see #isSetPotentialMain()
   * @see #unsetPotentialMain()
   * @see #setPotentialMain(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_PotentialMain()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='potentialMain'"
   * @generated
   */
  boolean isPotentialMain();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain
   * <em>Potential Main</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Potential Main</em>' attribute.
   * @see #isSetPotentialMain()
   * @see #unsetPotentialMain()
   * @see #isPotentialMain()
   * @generated
   */
  void setPotentialMain(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain
   * <em>Potential Main</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPotentialMain()
   * @see #isPotentialMain()
   * @see #setPotentialMain(boolean)
   * @generated
   */
  void unsetPotentialMain();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain
   * <em>Potential Main</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Potential Main</em>' attribute is set.
   * @see #unsetPotentialMain()
   * @see #isPotentialMain()
   * @see #setPotentialMain(boolean)
   * @generated
   */
  boolean isSetPotentialMain();

  /**
   * Returns the value of the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Uri Index</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Uri Index</em>' attribute.
   * @see #isSetUriIndex()
   * @see #unsetUriIndex()
   * @see #setUriIndex(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeToolkitOperatorType_UriIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='uriIndex'"
   * @generated
   */
  int getUriIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex <em>Uri
   * Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Uri Index</em>' attribute.
   * @see #isSetUriIndex()
   * @see #unsetUriIndex()
   * @see #getUriIndex()
   * @generated
   */
  void setUriIndex(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex <em>Uri
   * Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetUriIndex()
   * @see #getUriIndex()
   * @see #setUriIndex(int)
   * @generated
   */
  void unsetUriIndex();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex <em>Uri
   * Index</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Uri Index</em>' attribute is set.
   * @see #unsetUriIndex()
   * @see #getUriIndex()
   * @see #setUriIndex(int)
   * @generated
   */
  boolean isSetUriIndex();
} // CompositeToolkitOperatorType
