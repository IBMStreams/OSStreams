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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.util.FeatureMap;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Primitive Toolkit Operator Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getGroup
 *       <em>Group</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCustomLiterals
 *       <em>Custom Literals</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputFunctions
 *       <em>Output Functions</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParametersDescription
 *       <em>Parameters Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParameter
 *       <em>Parameter</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getInputPort
 *       <em>Input Port</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputPort
 *       <em>Output Port</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCodeTemplate
 *       <em>Code Template</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid
 *       <em>Invalid</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage
 *       <em>Language</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex
 *       <em>Model Uri Index</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType()
 * @model extendedMetaData="name='primitiveToolkitOperatorType' kind='elementOnly'"
 * @generated
 */
public interface PrimitiveToolkitOperatorType extends BaseToolkitOperatorType {
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
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_Group()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='group' name='group:5'"
   * @generated
   */
  FeatureMap getGroup();

  /**
   * Returns the value of the '<em><b>Custom Literals</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Custom Literals</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Custom Literals</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_CustomLiterals()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='customLiterals' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<CustomLiteralsType> getCustomLiterals();

  /**
   * Returns the value of the '<em><b>Output Functions</b></em>' containment reference list. The
   * list contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Functions</em>' containment reference list isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Functions</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_OutputFunctions()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='outputFunctions' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<OutputFunctionsType> getOutputFunctions();

  /**
   * Returns the value of the '<em><b>Parameters Description</b></em>' containment reference list.
   * The list contents are of type {@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameters Description</em>' containment reference list isn't
   * clear, there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameters Description</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_ParametersDescription()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='parametersDescription' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<DescriptionType> getParametersDescription();

  /**
   * Returns the value of the '<em><b>Parameter</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Parameter</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Parameter</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_Parameter()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='parameter' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<PrimitiveOpParameterType> getParameter();

  /**
   * Returns the value of the '<em><b>Input Port</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Input Port</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Input Port</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_InputPort()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='inputPort' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<PrimitiveOpInputPortType> getInputPort();

  /**
   * Returns the value of the '<em><b>Output Port</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Output Port</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Output Port</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_OutputPort()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='outputPort' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<PrimitiveOpOutputPortType> getOutputPort();

  /**
   * Returns the value of the '<em><b>Code Template</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Code Template</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Code Template</em>' containment reference list.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_CodeTemplate()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='codeTemplate' namespace='##targetNamespace'
   *     group='#group:5'"
   * @generated
   */
  EList<CodeTemplateType> getCodeTemplate();

  /**
   * Returns the value of the '<em><b>Invalid</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Invalid</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Invalid</em>' attribute.
   * @see #isSetInvalid()
   * @see #unsetInvalid()
   * @see #setInvalid(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_Invalid()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='invalid'"
   * @generated
   */
  boolean isInvalid();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid
   * <em>Invalid</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Invalid</em>' attribute.
   * @see #isSetInvalid()
   * @see #unsetInvalid()
   * @see #isInvalid()
   * @generated
   */
  void setInvalid(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid
   * <em>Invalid</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetInvalid()
   * @see #isInvalid()
   * @see #setInvalid(boolean)
   * @generated
   */
  void unsetInvalid();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid
   * <em>Invalid</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Invalid</em>' attribute is set.
   * @see #unsetInvalid()
   * @see #isInvalid()
   * @see #setInvalid(boolean)
   * @generated
   */
  boolean isSetInvalid();

  /**
   * Returns the value of the '<em><b>Language</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Language</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Language</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @see #isSetLanguage()
   * @see #unsetLanguage()
   * @see #setLanguage(LanguageType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_Language()
   * @model unsettable="true" extendedMetaData="kind='attribute' name='language'"
   * @generated
   */
  LanguageType getLanguage();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage
   * <em>Language</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Language</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @see #isSetLanguage()
   * @see #unsetLanguage()
   * @see #getLanguage()
   * @generated
   */
  void setLanguage(LanguageType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage
   * <em>Language</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetLanguage()
   * @see #getLanguage()
   * @see #setLanguage(LanguageType)
   * @generated
   */
  void unsetLanguage();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage
   * <em>Language</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Language</em>' attribute is set.
   * @see #unsetLanguage()
   * @see #getLanguage()
   * @see #setLanguage(LanguageType)
   * @generated
   */
  boolean isSetLanguage();

  /**
   * Returns the value of the '<em><b>Model Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Model Uri Index</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Model Uri Index</em>' attribute.
   * @see #isSetModelUriIndex()
   * @see #unsetModelUriIndex()
   * @see #setModelUriIndex(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitiveToolkitOperatorType_ModelUriIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='modelUriIndex'"
   * @generated
   */
  int getModelUriIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex <em>Model
   * Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Model Uri Index</em>' attribute.
   * @see #isSetModelUriIndex()
   * @see #unsetModelUriIndex()
   * @see #getModelUriIndex()
   * @generated
   */
  void setModelUriIndex(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex <em>Model
   * Uri Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetModelUriIndex()
   * @see #getModelUriIndex()
   * @see #setModelUriIndex(int)
   * @generated
   */
  void unsetModelUriIndex();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex <em>Model
   * Uri Index</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Model Uri Index</em>' attribute is set.
   * @see #unsetModelUriIndex()
   * @see #getModelUriIndex()
   * @see #setModelUriIndex(int)
   * @generated
   */
  boolean isSetModelUriIndex();
} // PrimitiveToolkitOperatorType
