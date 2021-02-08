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
package com.ibm.xmlns.prod.streams.spl.function;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model. It contains accessors for the meta objects to represent
 *
 * <ul>
 *   <li>each class,
 *   <li>each feature of each class,
 *   <li>each enum,
 *   <li>and each data type
 * </ul>
 *
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionFactory
 * @model kind="package"
 * @generated
 */
public interface FunctionPackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "function";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/function";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "function";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  FunctionPackage eINSTANCE =
      com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.DependenciesTypeImpl <em>Dependencies Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.DependenciesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getDependenciesType()
   * @generated
   */
  int DEPENDENCIES_TYPE = 0;

  /**
   * The feature id for the '<em><b>Library</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEPENDENCIES_TYPE__LIBRARY = 0;

  /**
   * The number of structural features of the '<em>Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEPENDENCIES_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.DocumentRootImpl <em>Document Root</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getDocumentRoot()
   * @generated
   */
  int DOCUMENT_ROOT = 1;

  /**
   * The feature id for the '<em><b>Mixed</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__MIXED = 0;

  /**
   * The feature id for the '<em><b>XMLNS Prefix Map</b></em>' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__XMLNS_PREFIX_MAP = 1;

  /**
   * The feature id for the '<em><b>XSI Schema Location</b></em>' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__XSI_SCHEMA_LOCATION = 2;

  /**
   * The feature id for the '<em><b>Function Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__FUNCTION_MODEL = 3;

  /**
   * The number of structural features of the '<em>Document Root</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionModelTypeImpl <em>Model Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionModelType()
   * @generated
   */
  int FUNCTION_MODEL_TYPE = 2;

  /**
   * The feature id for the '<em><b>Function Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODEL_TYPE__FUNCTION_SET = 0;

  /**
   * The number of structural features of the '<em>Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODEL_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl <em>Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionSetType()
   * @generated
   */
  int FUNCTION_SET_TYPE = 3;

  /**
   * The feature id for the '<em><b>Header File Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_SET_TYPE__HEADER_FILE_NAME = 0;

  /**
   * The feature id for the '<em><b>Cpp Namespace Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME = 1;

  /**
   * The feature id for the '<em><b>Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_SET_TYPE__FUNCTIONS = 2;

  /**
   * The feature id for the '<em><b>Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_SET_TYPE__DEPENDENCIES = 3;

  /**
   * The number of structural features of the '<em>Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_SET_TYPE_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionsTypeImpl <em>Functions Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionsType()
   * @generated
   */
  int FUNCTIONS_TYPE = 4;

  /**
   * The feature id for the '<em><b>Function</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTIONS_TYPE__FUNCTION = 0;

  /**
   * The number of structural features of the '<em>Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTIONS_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl <em>Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionType()
   * @generated
   */
  int FUNCTION_TYPE = 5;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Prototype</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_TYPE__PROTOTYPE = 1;

  /**
   * The feature id for the '<em><b>Java Function Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_TYPE__JAVA_FUNCTION_NAME = 2;

  /**
   * The number of structural features of the '<em>Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_TYPE_FEATURE_COUNT = 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.impl.PrototypeTypeImpl <em>Prototype Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.PrototypeTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getPrototypeType()
   * @generated
   */
  int PROTOTYPE_TYPE = 6;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PROTOTYPE_TYPE__VALUE = 0;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PROTOTYPE_TYPE__COLUMN = 1;

  /**
   * The feature id for the '<em><b>Cpp Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PROTOTYPE_TYPE__CPP_NAME = 2;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PROTOTYPE_TYPE__LINE = 3;

  /**
   * The number of structural features of the '<em>Prototype Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PROTOTYPE_TYPE_FEATURE_COUNT = 4;

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DependenciesType <em>Dependencies Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Dependencies Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DependenciesType
   * @generated
   */
  EClass getDependenciesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DependenciesType#getLibrary <em>Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DependenciesType#getLibrary()
   * @see #getDependenciesType()
   * @generated
   */
  EReference getDependenciesType_Library();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.function.DocumentRoot
   * <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getFunctionModel <em>Function
   * Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Function Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.DocumentRoot#getFunctionModel()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_FunctionModel();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionModelType <em>Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionModelType
   * @generated
   */
  EClass getFunctionModelType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionModelType#getFunctionSet <em>Function
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionModelType#getFunctionSet()
   * @see #getFunctionModelType()
   * @generated
   */
  EReference getFunctionModelType_FunctionSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType <em>Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionSetType
   * @generated
   */
  EClass getFunctionSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getHeaderFileName <em>Header File
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Header File Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getHeaderFileName()
   * @see #getFunctionSetType()
   * @generated
   */
  EAttribute getFunctionSetType_HeaderFileName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getCppNamespaceName <em>Cpp Namespace
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cpp Namespace Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getCppNamespaceName()
   * @see #getFunctionSetType()
   * @generated
   */
  EAttribute getFunctionSetType_CppNamespaceName();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getFunctions <em>Functions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Functions</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getFunctions()
   * @see #getFunctionSetType()
   * @generated
   */
  EReference getFunctionSetType_Functions();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getDependencies
   * <em>Dependencies</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Dependencies</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionSetType#getDependencies()
   * @see #getFunctionSetType()
   * @generated
   */
  EReference getFunctionSetType_Dependencies();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.function.FunctionsType
   * <em>Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Functions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionsType
   * @generated
   */
  EClass getFunctionsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionsType#getFunction <em>Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionsType#getFunction()
   * @see #getFunctionsType()
   * @generated
   */
  EReference getFunctionsType_Function();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.function.FunctionType
   * <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionType
   * @generated
   */
  EClass getFunctionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionType#getDescription()
   * @see #getFunctionType()
   * @generated
   */
  EReference getFunctionType_Description();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionType#getPrototype <em>Prototype</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Prototype</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionType#getPrototype()
   * @see #getFunctionType()
   * @generated
   */
  EReference getFunctionType_Prototype();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionType#getJavaFunctionName <em>Java Function
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Java Function Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionType#getJavaFunctionName()
   * @see #getFunctionType()
   * @generated
   */
  EAttribute getFunctionType_JavaFunctionName();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType
   * <em>Prototype Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Prototype Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.PrototypeType
   * @generated
   */
  EClass getPrototypeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getValue()
   * @see #getPrototypeType()
   * @generated
   */
  EAttribute getPrototypeType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getColumn <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getColumn()
   * @see #getPrototypeType()
   * @generated
   */
  EAttribute getPrototypeType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getCppName <em>Cpp Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cpp Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getCppName()
   * @see #getPrototypeType()
   * @generated
   */
  EAttribute getPrototypeType_CppName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getLine()
   * @see #getPrototypeType()
   * @generated
   */
  EAttribute getPrototypeType_Line();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  FunctionFactory getFunctionFactory();

  /**
   *
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   *
   * <ul>
   *   <li>each class,
   *   <li>each feature of each class,
   *   <li>each enum,
   *   <li>and each data type
   * </ul>
   *
   * <!-- end-user-doc -->
   *
   * @generated
   */
  interface Literals {
    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.DependenciesTypeImpl <em>Dependencies
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.DependenciesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getDependenciesType()
     * @generated
     */
    EClass DEPENDENCIES_TYPE = eINSTANCE.getDependenciesType();

    /**
     * The meta object literal for the '<em><b>Library</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DEPENDENCIES_TYPE__LIBRARY = eINSTANCE.getDependenciesType_Library();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.DocumentRootImpl <em>Document Root</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getDocumentRoot()
     * @generated
     */
    EClass DOCUMENT_ROOT = eINSTANCE.getDocumentRoot();

    /**
     * The meta object literal for the '<em><b>Mixed</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute DOCUMENT_ROOT__MIXED = eINSTANCE.getDocumentRoot_Mixed();

    /**
     * The meta object literal for the '<em><b>XMLNS Prefix Map</b></em>' map feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__XMLNS_PREFIX_MAP = eINSTANCE.getDocumentRoot_XMLNSPrefixMap();

    /**
     * The meta object literal for the '<em><b>XSI Schema Location</b></em>' map feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__XSI_SCHEMA_LOCATION = eINSTANCE.getDocumentRoot_XSISchemaLocation();

    /**
     * The meta object literal for the '<em><b>Function Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__FUNCTION_MODEL = eINSTANCE.getDocumentRoot_FunctionModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionModelTypeImpl <em>Model Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionModelType()
     * @generated
     */
    EClass FUNCTION_MODEL_TYPE = eINSTANCE.getFunctionModelType();

    /**
     * The meta object literal for the '<em><b>Function Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_MODEL_TYPE__FUNCTION_SET = eINSTANCE.getFunctionModelType_FunctionSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl <em>Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionSetTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionSetType()
     * @generated
     */
    EClass FUNCTION_SET_TYPE = eINSTANCE.getFunctionSetType();

    /**
     * The meta object literal for the '<em><b>Header File Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_SET_TYPE__HEADER_FILE_NAME = eINSTANCE.getFunctionSetType_HeaderFileName();

    /**
     * The meta object literal for the '<em><b>Cpp Namespace Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME =
        eINSTANCE.getFunctionSetType_CppNamespaceName();

    /**
     * The meta object literal for the '<em><b>Functions</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_SET_TYPE__FUNCTIONS = eINSTANCE.getFunctionSetType_Functions();

    /**
     * The meta object literal for the '<em><b>Dependencies</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_SET_TYPE__DEPENDENCIES = eINSTANCE.getFunctionSetType_Dependencies();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionsTypeImpl <em>Functions Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionsType()
     * @generated
     */
    EClass FUNCTIONS_TYPE = eINSTANCE.getFunctionsType();

    /**
     * The meta object literal for the '<em><b>Function</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTIONS_TYPE__FUNCTION = eINSTANCE.getFunctionsType_Function();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl <em>Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getFunctionType()
     * @generated
     */
    EClass FUNCTION_TYPE = eINSTANCE.getFunctionType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_TYPE__DESCRIPTION = eINSTANCE.getFunctionType_Description();

    /**
     * The meta object literal for the '<em><b>Prototype</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_TYPE__PROTOTYPE = eINSTANCE.getFunctionType_Prototype();

    /**
     * The meta object literal for the '<em><b>Java Function Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_TYPE__JAVA_FUNCTION_NAME = eINSTANCE.getFunctionType_JavaFunctionName();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.function.impl.PrototypeTypeImpl <em>Prototype Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.PrototypeTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.function.impl.FunctionPackageImpl#getPrototypeType()
     * @generated
     */
    EClass PROTOTYPE_TYPE = eINSTANCE.getPrototypeType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PROTOTYPE_TYPE__VALUE = eINSTANCE.getPrototypeType_Value();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PROTOTYPE_TYPE__COLUMN = eINSTANCE.getPrototypeType_Column();

    /**
     * The meta object literal for the '<em><b>Cpp Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PROTOTYPE_TYPE__CPP_NAME = eINSTANCE.getPrototypeType_CppName();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PROTOTYPE_TYPE__LINE = eINSTANCE.getPrototypeType_Line();
  }
} // FunctionPackage
