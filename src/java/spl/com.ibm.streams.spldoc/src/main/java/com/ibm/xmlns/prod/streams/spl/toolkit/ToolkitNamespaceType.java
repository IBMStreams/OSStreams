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
 * A representation of the model object '<em><b>Namespace Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getGroup <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getPrimitiveOp
 *       <em>Primitive Op</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getCompositeOp
 *       <em>Composite Op</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getFunction
 *       <em>Function</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getType <em>Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getEnumLiterals <em>Enum
 *       Literals</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getRelPathToNamespace
 *       <em>Rel Path To Namespace</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType()
 * @model extendedMetaData="name='toolkitNamespaceType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitNamespaceType extends ToolkitElementType {
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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_Group()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='group' name='group:3'"
   * @generated
   */
  FeatureMap getGroup();

  /**
   * Returns the value of the '<em><b>Primitive Op</b></em>' containment reference list. The list
   * contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Primitive Op</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Primitive Op</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_PrimitiveOp()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='primitiveOp' namespace='##targetNamespace'
   *     group='#group:3'"
   * @generated
   */
  EList<PrimitiveToolkitOperatorType> getPrimitiveOp();

  /**
   * Returns the value of the '<em><b>Composite Op</b></em>' containment reference list. The list
   * contents are of type {@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Composite Op</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Composite Op</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_CompositeOp()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='compositeOp' namespace='##targetNamespace'
   *     group='#group:3'"
   * @generated
   */
  EList<CompositeToolkitOperatorType> getCompositeOp();

  /**
   * Returns the value of the '<em><b>Function</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Function</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Function</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_Function()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='function' namespace='##targetNamespace'
   *     group='#group:3'"
   * @generated
   */
  EList<ToolkitFunctionType> getFunction();

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_Type()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='type' namespace='##targetNamespace'
   *     group='#group:3'"
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
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_EnumLiterals()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='enumLiterals' namespace='##targetNamespace'
   *     group='#group:3'"
   * @generated
   */
  EList<EnumLiteralsType> getEnumLiterals();

  /**
   * Returns the value of the '<em><b>Rel Path To Namespace</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Rel Path To Namespace</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Rel Path To Namespace</em>' attribute.
   * @see #setRelPathToNamespace(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitNamespaceType_RelPathToNamespace()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='relPathToNamespace'"
   * @generated
   */
  String getRelPathToNamespace();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getRelPathToNamespace <em>Rel Path
   * To Namespace</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Rel Path To Namespace</em>' attribute.
   * @see #getRelPathToNamespace()
   * @generated
   */
  void setRelPathToNamespace(String value);
} // ToolkitNamespaceType
