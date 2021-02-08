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
package com.ibm.xmlns.prod.streams.spl.operator;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Java Op Context Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getIconUri <em>Icon
 *       Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getMetrics
 *       <em>Metrics</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCustomLiterals
 *       <em>Custom Literals</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getExecutionSettings
 *       <em>Execution Settings</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getLibraryDependencies
 *       <em>Library Dependencies</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCodeTemplates <em>Code
 *       Templates</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType()
 * @model extendedMetaData="name='javaOpContextType' kind='elementOnly'"
 * @generated
 */
public interface JavaOpContextType extends EObject {
  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getDescription <em>Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Icon Uri</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Icon Uri</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Icon Uri</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_IconUri()
   * @model containment="true" extendedMetaData="kind='element' name='iconUri'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<IconUriType> getIconUri();

  /**
   * Returns the value of the '<em><b>Metrics</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Metrics</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Metrics</em>' containment reference.
   * @see #setMetrics(MetricsType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_Metrics()
   * @model containment="true" extendedMetaData="kind='element' name='metrics'
   *     namespace='##targetNamespace'"
   * @generated
   */
  MetricsType getMetrics();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getMetrics <em>Metrics</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Metrics</em>' containment reference.
   * @see #getMetrics()
   * @generated
   */
  void setMetrics(MetricsType value);

  /**
   * Returns the value of the '<em><b>Custom Literals</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Custom Literals</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Custom Literals</em>' containment reference.
   * @see #setCustomLiterals(EnumerationsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_CustomLiterals()
   * @model containment="true" extendedMetaData="kind='element' name='customLiterals'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EnumerationsType getCustomLiterals();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCustomLiterals <em>Custom
   * Literals</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Custom Literals</em>' containment reference.
   * @see #getCustomLiterals()
   * @generated
   */
  void setCustomLiterals(EnumerationsType value);

  /**
   * Returns the value of the '<em><b>Execution Settings</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Execution Settings</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Execution Settings</em>' containment reference.
   * @see #setExecutionSettings(JavaOpExecutionSettingsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_ExecutionSettings()
   * @model containment="true" required="true" extendedMetaData="kind='element'
   *     name='executionSettings' namespace='##targetNamespace'"
   * @generated
   */
  JavaOpExecutionSettingsType getExecutionSettings();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getExecutionSettings <em>Execution
   * Settings</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Execution Settings</em>' containment reference.
   * @see #getExecutionSettings()
   * @generated
   */
  void setExecutionSettings(JavaOpExecutionSettingsType value);

  /**
   * Returns the value of the '<em><b>Library Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Library Dependencies</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Library Dependencies</em>' containment reference.
   * @see #setLibraryDependencies(JavaOpLibraryDependenciesType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_LibraryDependencies()
   * @model containment="true" extendedMetaData="kind='element' name='libraryDependencies'
   *     namespace='##targetNamespace'"
   * @generated
   */
  JavaOpLibraryDependenciesType getLibraryDependencies();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getLibraryDependencies <em>Library
   * Dependencies</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Library Dependencies</em>' containment reference.
   * @see #getLibraryDependencies()
   * @generated
   */
  void setLibraryDependencies(JavaOpLibraryDependenciesType value);

  /**
   * Returns the value of the '<em><b>Code Templates</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Code Templates</em>' containment reference isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Code Templates</em>' containment reference.
   * @see #setCodeTemplates(CodeTemplatesType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getJavaOpContextType_CodeTemplates()
   * @model containment="true" extendedMetaData="kind='element' name='codeTemplates'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CodeTemplatesType getCodeTemplates();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCodeTemplates <em>Code
   * Templates</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Code Templates</em>' containment reference.
   * @see #getCodeTemplates()
   * @generated
   */
  void setCodeTemplates(CodeTemplatesType value);
} // JavaOpContextType
