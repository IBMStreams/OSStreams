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
 * A representation of the model object '<em><b>Context Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIconUri <em>Icon Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getMetrics <em>Metrics</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomLiterals <em>Custom
 *       Literals</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomOutputFunctions
 *       <em>Custom Output Functions</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getLibraryDependencies
 *       <em>Library Dependencies</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext
 *       <em>Provides Single Threaded Context</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy
 *       <em>Incremental Compilation Strategy</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic <em>Allow
 *       Custom Logic</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCodeTemplates <em>Code
 *       Templates</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getSplExpressionTree <em>Spl
 *       Expression Tree</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCapability
 *       <em>Capability</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getVerificationModule
 *       <em>Verification Module</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType()
 * @model extendedMetaData="name='contextType' kind='elementOnly'"
 * @generated
 */
public interface ContextType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getDescription <em>Description</em>}'
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_IconUri()
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_Metrics()
   * @model containment="true" extendedMetaData="kind='element' name='metrics'
   *     namespace='##targetNamespace'"
   * @generated
   */
  MetricsType getMetrics();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType#getMetrics
   * <em>Metrics</em>}' containment reference.
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_CustomLiterals()
   * @model containment="true" extendedMetaData="kind='element' name='customLiterals'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EnumerationsType getCustomLiterals();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomLiterals <em>Custom
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
   * Returns the value of the '<em><b>Custom Output Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Custom Output Functions</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Custom Output Functions</em>' containment reference.
   * @see #setCustomOutputFunctions(CustomOutputFunctionsType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_CustomOutputFunctions()
   * @model containment="true" extendedMetaData="kind='element' name='customOutputFunctions'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CustomOutputFunctionsType getCustomOutputFunctions();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomOutputFunctions <em>Custom Output
   * Functions</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Custom Output Functions</em>' containment reference.
   * @see #getCustomOutputFunctions()
   * @generated
   */
  void setCustomOutputFunctions(CustomOutputFunctionsType value);

  /**
   * Returns the value of the '<em><b>Library Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Library Dependencies</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Library Dependencies</em>' containment reference.
   * @see #setLibraryDependencies(LibraryDependenciesType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_LibraryDependencies()
   * @model containment="true" extendedMetaData="kind='element' name='libraryDependencies'
   *     namespace='##targetNamespace'"
   * @generated
   */
  LibraryDependenciesType getLibraryDependencies();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getLibraryDependencies <em>Library
   * Dependencies</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Library Dependencies</em>' containment reference.
   * @see #getLibraryDependencies()
   * @generated
   */
  void setLibraryDependencies(LibraryDependenciesType value);

  /**
   * Returns the value of the '<em><b>Provides Single Threaded Context</b></em>' attribute. The
   * literals are from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Provides Single Threaded Context</em>' attribute isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Provides Single Threaded Context</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @see #isSetProvidesSingleThreadedContext()
   * @see #unsetProvidesSingleThreadedContext()
   * @see #setProvidesSingleThreadedContext(SingleThreadedContextType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_ProvidesSingleThreadedContext()
   * @model unsettable="true" required="true" extendedMetaData="kind='element'
   *     name='providesSingleThreadedContext' namespace='##targetNamespace'"
   * @generated
   */
  SingleThreadedContextType getProvidesSingleThreadedContext();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext
   * <em>Provides Single Threaded Context</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Provides Single Threaded Context</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @see #isSetProvidesSingleThreadedContext()
   * @see #unsetProvidesSingleThreadedContext()
   * @see #getProvidesSingleThreadedContext()
   * @generated
   */
  void setProvidesSingleThreadedContext(SingleThreadedContextType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext
   * <em>Provides Single Threaded Context</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetProvidesSingleThreadedContext()
   * @see #getProvidesSingleThreadedContext()
   * @see #setProvidesSingleThreadedContext(SingleThreadedContextType)
   * @generated
   */
  void unsetProvidesSingleThreadedContext();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext
   * <em>Provides Single Threaded Context</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Provides Single Threaded Context</em>' attribute is set.
   * @see #unsetProvidesSingleThreadedContext()
   * @see #getProvidesSingleThreadedContext()
   * @see #setProvidesSingleThreadedContext(SingleThreadedContextType)
   * @generated
   */
  boolean isSetProvidesSingleThreadedContext();

  /**
   * Returns the value of the '<em><b>Incremental Compilation Strategy</b></em>' attribute. The
   * literals are from the enumeration {@link
   * com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Incremental Compilation Strategy</em>' attribute isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Incremental Compilation Strategy</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @see #isSetIncrementalCompilationStrategy()
   * @see #unsetIncrementalCompilationStrategy()
   * @see #setIncrementalCompilationStrategy(IncrementalCompilationStrategyType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_IncrementalCompilationStrategy()
   * @model unsettable="true" extendedMetaData="kind='element' name='incrementalCompilationStrategy'
   *     namespace='##targetNamespace'"
   * @generated
   */
  IncrementalCompilationStrategyType getIncrementalCompilationStrategy();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy
   * <em>Incremental Compilation Strategy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Incremental Compilation Strategy</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @see #isSetIncrementalCompilationStrategy()
   * @see #unsetIncrementalCompilationStrategy()
   * @see #getIncrementalCompilationStrategy()
   * @generated
   */
  void setIncrementalCompilationStrategy(IncrementalCompilationStrategyType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy
   * <em>Incremental Compilation Strategy</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetIncrementalCompilationStrategy()
   * @see #getIncrementalCompilationStrategy()
   * @see #setIncrementalCompilationStrategy(IncrementalCompilationStrategyType)
   * @generated
   */
  void unsetIncrementalCompilationStrategy();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy
   * <em>Incremental Compilation Strategy</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Incremental Compilation Strategy</em>' attribute is set.
   * @see #unsetIncrementalCompilationStrategy()
   * @see #getIncrementalCompilationStrategy()
   * @see #setIncrementalCompilationStrategy(IncrementalCompilationStrategyType)
   * @generated
   */
  boolean isSetIncrementalCompilationStrategy();

  /**
   * Returns the value of the '<em><b>Allow Custom Logic</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Allow Custom Logic</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Allow Custom Logic</em>' attribute.
   * @see #isSetAllowCustomLogic()
   * @see #unsetAllowCustomLogic()
   * @see #setAllowCustomLogic(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_AllowCustomLogic()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='element' name='allowCustomLogic' namespace='##targetNamespace'"
   * @generated
   */
  boolean isAllowCustomLogic();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic <em>Allow Custom
   * Logic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Allow Custom Logic</em>' attribute.
   * @see #isSetAllowCustomLogic()
   * @see #unsetAllowCustomLogic()
   * @see #isAllowCustomLogic()
   * @generated
   */
  void setAllowCustomLogic(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic <em>Allow Custom
   * Logic</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetAllowCustomLogic()
   * @see #isAllowCustomLogic()
   * @see #setAllowCustomLogic(boolean)
   * @generated
   */
  void unsetAllowCustomLogic();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic <em>Allow Custom
   * Logic</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Allow Custom Logic</em>' attribute is set.
   * @see #unsetAllowCustomLogic()
   * @see #isAllowCustomLogic()
   * @see #setAllowCustomLogic(boolean)
   * @generated
   */
  boolean isSetAllowCustomLogic();

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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_CodeTemplates()
   * @model containment="true" extendedMetaData="kind='element' name='codeTemplates'
   *     namespace='##targetNamespace'"
   * @generated
   */
  CodeTemplatesType getCodeTemplates();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCodeTemplates <em>Code Templates</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Code Templates</em>' containment reference.
   * @see #getCodeTemplates()
   * @generated
   */
  void setCodeTemplates(CodeTemplatesType value);

  /**
   * Returns the value of the '<em><b>Spl Expression Tree</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Spl Expression Tree</em>' containment reference isn't clear,
   * there really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Spl Expression Tree</em>' containment reference.
   * @see #setSplExpressionTree(SplExpressionTreeType)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_SplExpressionTree()
   * @model containment="true" extendedMetaData="kind='element' name='splExpressionTree'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplExpressionTreeType getSplExpressionTree();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getSplExpressionTree <em>Spl Expression
   * Tree</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Expression Tree</em>' containment reference.
   * @see #getSplExpressionTree()
   * @generated
   */
  void setSplExpressionTree(SplExpressionTreeType value);

  /**
   * Returns the value of the '<em><b>Capability</b></em>' attribute list. The list contents are of
   * type {@link java.lang.String}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Capability</em>' attribute list isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Capability</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_Capability()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.String"
   *     extendedMetaData="kind='element' name='capability' namespace='##targetNamespace'"
   * @generated
   */
  EList<String> getCapability();

  /**
   * Returns the value of the '<em><b>Verification Module</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Verification Module</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Verification Module</em>' attribute.
   * @see #setVerificationModule(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getContextType_VerificationModule()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" extendedMetaData="kind='attribute'
   *     name='verificationModule'"
   * @generated
   */
  String getVerificationModule();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getVerificationModule <em>Verification
   * Module</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Verification Module</em>' attribute.
   * @see #getVerificationModule()
   * @generated
   */
  void setVerificationModule(String value);
} // ContextType
