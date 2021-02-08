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

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EEnum;
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
 *   <li>each operation of each class,
 *   <li>each enum,
 *   <li>and each data type
 * </ul>
 *
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorFactory
 * @model kind="package"
 * @generated
 */
public interface OperatorPackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "operator";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/operator";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "operator";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  OperatorPackage eINSTANCE =
      com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplatesTypeImpl <em>Code Templates
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplatesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCodeTemplatesType()
   * @generated
   */
  int CODE_TEMPLATES_TYPE = 0;

  /**
   * The feature id for the '<em><b>Code Template</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATES_TYPE__CODE_TEMPLATE = 0;

  /**
   * The number of structural features of the '<em>Code Templates Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATES_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Code Templates Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATES_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplateTypeImpl <em>Code Template Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplateTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCodeTemplateType()
   * @generated
   */
  int CODE_TEMPLATE_TYPE = 1;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Template</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__TEMPLATE = 1;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__NAME = 2;

  /**
   * The number of structural features of the '<em>Code Template Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Code Template Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl
   * <em>Context Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getContextType()
   * @generated
   */
  int CONTEXT_TYPE = 2;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Icon Uri</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__ICON_URI = 1;

  /**
   * The feature id for the '<em><b>Metrics</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__METRICS = 2;

  /**
   * The feature id for the '<em><b>Custom Literals</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__CUSTOM_LITERALS = 3;

  /**
   * The feature id for the '<em><b>Custom Output Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS = 4;

  /**
   * The feature id for the '<em><b>Library Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__LIBRARY_DEPENDENCIES = 5;

  /**
   * The feature id for the '<em><b>Provides Single Threaded Context</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT = 6;

  /**
   * The feature id for the '<em><b>Incremental Compilation Strategy</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY = 7;

  /**
   * The feature id for the '<em><b>Allow Custom Logic</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC = 8;

  /**
   * The feature id for the '<em><b>Code Templates</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__CODE_TEMPLATES = 9;

  /**
   * The feature id for the '<em><b>Spl Expression Tree</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__SPL_EXPRESSION_TREE = 10;

  /**
   * The feature id for the '<em><b>Capability</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__CAPABILITY = 11;

  /**
   * The feature id for the '<em><b>Verification Module</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE__VERIFICATION_MODULE = 12;

  /**
   * The number of structural features of the '<em>Context Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE_FEATURE_COUNT = 13;

  /**
   * The number of operations of the '<em>Context Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONTEXT_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl <em>Custom Output
   * Function Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionSetType()
   * @generated
   */
  int CUSTOM_OUTPUT_FUNCTION_SET_TYPE = 3;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Function</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION = 1;

  /**
   * The number of structural features of the '<em>Custom Output Function Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_SET_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Custom Output Function Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_SET_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionsTypeImpl <em>Custom Output
   * Functions Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionsType()
   * @generated
   */
  int CUSTOM_OUTPUT_FUNCTIONS_TYPE = 4;

  /**
   * The feature id for the '<em><b>Custom Output Function</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION = 0;

  /**
   * The number of structural features of the '<em>Custom Output Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTIONS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Custom Output Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTIONS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl <em>Custom Output
   * Function Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionType()
   * @generated
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE = 5;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Prototype</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE = 1;

  /**
   * The feature id for the '<em><b>Pseudo Function</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION = 2;

  /**
   * The number of structural features of the '<em>Custom Output Function Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Custom Output Function Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_OUTPUT_FUNCTION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl <em>Document Root</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getDocumentRoot()
   * @generated
   */
  int DOCUMENT_ROOT = 6;

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
   * The feature id for the '<em><b>Operator Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__OPERATOR_MODEL = 3;

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
   * The number of operations of the '<em>Document Root</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationsTypeImpl <em>Enumerations Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getEnumerationsType()
   * @generated
   */
  int ENUMERATIONS_TYPE = 7;

  /**
   * The feature id for the '<em><b>Enumeration</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATIONS_TYPE__ENUMERATION = 0;

  /**
   * The number of structural features of the '<em>Enumerations Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATIONS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Enumerations Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATIONS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationTypeImpl <em>Enumeration Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getEnumerationType()
   * @generated
   */
  int ENUMERATION_TYPE = 8;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATION_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATION_TYPE__VALUE = 1;

  /**
   * The number of structural features of the '<em>Enumeration Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATION_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Enumeration Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUMERATION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.impl.IconUriTypeImpl
   * <em>Icon Uri Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.IconUriTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIconUriType()
   * @generated
   */
  int ICON_URI_TYPE = 9;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ICON_URI_TYPE__VALUE = 0;

  /**
   * The feature id for the '<em><b>Size</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ICON_URI_TYPE__SIZE = 1;

  /**
   * The number of structural features of the '<em>Icon Uri Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ICON_URI_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Icon Uri Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ICON_URI_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl <em>Input Port Open Set
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortOpenSetType()
   * @generated
   */
  int INPUT_PORT_OPEN_SET_TYPE = 10;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Windowing Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Tuple Mutation Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED = 2;

  /**
   * The feature id for the '<em><b>Windowing Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE = 3;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE = 4;

  /**
   * The feature id for the '<em><b>Control Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT = 5;

  /**
   * The feature id for the '<em><b>Window Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE = 6;

  /**
   * The feature id for the '<em><b>Rewrite Allowed For Window Expression</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION = 7;

  /**
   * The number of structural features of the '<em>Input Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT = 8;

  /**
   * The number of operations of the '<em>Input Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl <em>Input Port Set
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortSetType()
   * @generated
   */
  int INPUT_PORT_SET_TYPE = 11;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__DESCRIPTION = INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Windowing Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__WINDOWING_DESCRIPTION = INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION;

  /**
   * The feature id for the '<em><b>Tuple Mutation Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__TUPLE_MUTATION_ALLOWED =
      INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED;

  /**
   * The feature id for the '<em><b>Windowing Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__WINDOWING_MODE = INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE =
      INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE;

  /**
   * The feature id for the '<em><b>Control Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__CONTROL_PORT = INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT;

  /**
   * The feature id for the '<em><b>Window Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__WINDOW_EXPRESSION_MODE =
      INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE;

  /**
   * The feature id for the '<em><b>Rewrite Allowed For Window Expression</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION =
      INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__CARDINALITY = INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE__OPTIONAL = INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Input Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE_FEATURE_COUNT = INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Input Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORT_SET_TYPE_OPERATION_COUNT = INPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortsTypeImpl <em>Input Ports Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortsType()
   * @generated
   */
  int INPUT_PORTS_TYPE = 12;

  /**
   * The feature id for the '<em><b>Input Port Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORTS_TYPE__INPUT_PORT_SET = 0;

  /**
   * The feature id for the '<em><b>Input Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET = 1;

  /**
   * The number of structural features of the '<em>Input Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORTS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Input Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INPUT_PORTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl <em>Java Op Context
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpContextType()
   * @generated
   */
  int JAVA_OP_CONTEXT_TYPE = 13;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Icon Uri</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__ICON_URI = 1;

  /**
   * The feature id for the '<em><b>Metrics</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__METRICS = 2;

  /**
   * The feature id for the '<em><b>Custom Literals</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS = 3;

  /**
   * The feature id for the '<em><b>Execution Settings</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS = 4;

  /**
   * The feature id for the '<em><b>Library Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES = 5;

  /**
   * The feature id for the '<em><b>Code Templates</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES = 6;

  /**
   * The number of structural features of the '<em>Java Op Context Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE_FEATURE_COUNT = 7;

  /**
   * The number of operations of the '<em>Java Op Context Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_CONTEXT_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl <em>Java Op
   * Execution Settings Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExecutionSettingsType()
   * @generated
   */
  int JAVA_OP_EXECUTION_SETTINGS_TYPE = 14;

  /**
   * The feature id for the '<em><b>Class Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME = 0;

  /**
   * The feature id for the '<em><b>Vm Args</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS = 1;

  /**
   * The number of structural features of the '<em>Java Op Execution Settings Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_EXECUTION_SETTINGS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Execution Settings Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_EXECUTION_SETTINGS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl <em>Java Op Input
   * Port Open Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortOpenSetType()
   * @generated
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE = 15;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Windowing Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Windowing Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE = 2;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE = 3;

  /**
   * The feature id for the '<em><b>Control Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT = 4;

  /**
   * The number of structural features of the '<em>Java Op Input Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT = 5;

  /**
   * The number of operations of the '<em>Java Op Input Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortSetTypeImpl <em>Java Op Input Port
   * Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortSetType()
   * @generated
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE = 16;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__DESCRIPTION = JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Windowing Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__WINDOWING_DESCRIPTION =
      JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION;

  /**
   * The feature id for the '<em><b>Windowing Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__WINDOWING_MODE =
      JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE =
      JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE;

  /**
   * The feature id for the '<em><b>Control Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__CONTROL_PORT = JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__CARDINALITY = JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE__OPTIONAL = JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Java Op Input Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE_FEATURE_COUNT =
      JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Java Op Input Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORT_SET_TYPE_OPERATION_COUNT =
      JAVA_OP_INPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortsTypeImpl <em>Java Op Input Ports
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortsType()
   * @generated
   */
  int JAVA_OP_INPUT_PORTS_TYPE = 17;

  /**
   * The feature id for the '<em><b>Input Port Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_SET = 0;

  /**
   * The feature id for the '<em><b>Input Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET = 1;

  /**
   * The number of structural features of the '<em>Java Op Input Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORTS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Input Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_INPUT_PORTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpLibraryDependenciesTypeImpl <em>Java Op
   * Library Dependencies Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpLibraryDependenciesTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpLibraryDependenciesType()
   * @generated
   */
  int JAVA_OP_LIBRARY_DEPENDENCIES_TYPE = 18;

  /**
   * The feature id for the '<em><b>Library</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_DEPENDENCIES_TYPE__LIBRARY = 0;

  /**
   * The number of structural features of the '<em>Java Op Library Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_DEPENDENCIES_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Java Op Library Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_DEPENDENCIES_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl <em>Java Op Model Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpModelType()
   * @generated
   */
  int JAVA_OP_MODEL_TYPE = 19;

  /**
   * The feature id for the '<em><b>Context</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE__CONTEXT = 0;

  /**
   * The feature id for the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE__PARAMETERS = 1;

  /**
   * The feature id for the '<em><b>Input Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE__INPUT_PORTS = 2;

  /**
   * The feature id for the '<em><b>Output Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE__OUTPUT_PORTS = 3;

  /**
   * The number of structural features of the '<em>Java Op Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Java Op Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MODEL_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl <em>Java Op Output
   * Port Open Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortOpenSetType()
   * @generated
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE = 20;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Window Punctuation Output Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE = 1;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT = 2;

  /**
   * The feature id for the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE = 3;

  /**
   * The number of structural features of the '<em>Java Op Output Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Java Op Output Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortSetTypeImpl <em>Java Op Output
   * Port Set Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortSetType()
   * @generated
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE = 21;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__DESCRIPTION = JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Window Punctuation Output Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT;

  /**
   * The feature id for the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__CARDINALITY =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE__OPTIONAL = JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Java Op Output Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE_FEATURE_COUNT =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Java Op Output Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORT_SET_TYPE_OPERATION_COUNT =
      JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl <em>Java Op Output Ports
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortsType()
   * @generated
   */
  int JAVA_OP_OUTPUT_PORTS_TYPE = 22;

  /**
   * The feature id for the '<em><b>Output Port Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET = 0;

  /**
   * The feature id for the '<em><b>Output Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET = 1;

  /**
   * The number of structural features of the '<em>Java Op Output Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORTS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Output Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_OUTPUT_PORTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParametersTypeImpl <em>Java Op Parameters
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParametersTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpParametersType()
   * @generated
   */
  int JAVA_OP_PARAMETERS_TYPE = 23;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETERS_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETERS_TYPE__PARAMETER = 1;

  /**
   * The number of structural features of the '<em>Java Op Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETERS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl <em>Java Op Parameter
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpParameterType()
   * @generated
   */
  int JAVA_OP_PARAMETER_TYPE = 24;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__OPTIONAL = 2;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE = 3;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__TYPE = 4;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE__CARDINALITY = 5;

  /**
   * The number of structural features of the '<em>Java Op Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE_FEATURE_COUNT = 6;

  /**
   * The number of operations of the '<em>Java Op Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_PARAMETER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpVMArgsTypeImpl <em>Java Op VM Args
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpVMArgsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpVMArgsType()
   * @generated
   */
  int JAVA_OP_VM_ARGS_TYPE = 25;

  /**
   * The feature id for the '<em><b>Vm Arg</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_VM_ARGS_TYPE__VM_ARG = 0;

  /**
   * The number of structural features of the '<em>Java Op VM Args Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_VM_ARGS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Java Op VM Args Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_VM_ARGS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.LibraryDependenciesTypeImpl <em>Library
   * Dependencies Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.LibraryDependenciesTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getLibraryDependenciesType()
   * @generated
   */
  int LIBRARY_DEPENDENCIES_TYPE = 26;

  /**
   * The feature id for the '<em><b>Library</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_DEPENDENCIES_TYPE__LIBRARY = 0;

  /**
   * The number of structural features of the '<em>Library Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_DEPENDENCIES_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Library Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_DEPENDENCIES_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.impl.MetricsTypeImpl
   * <em>Metrics Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.MetricsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricsType()
   * @generated
   */
  int METRICS_TYPE = 27;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRICS_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Metric</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRICS_TYPE__METRIC = 1;

  /**
   * The number of structural features of the '<em>Metrics Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRICS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Metrics Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRICS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.impl.MetricTypeImpl
   * <em>Metric Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.MetricTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricType()
   * @generated
   */
  int METRIC_TYPE = 28;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE__DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Kind</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE__KIND = 2;

  /**
   * The feature id for the '<em><b>Dynamic</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE__DYNAMIC = 3;

  /**
   * The number of structural features of the '<em>Metric Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Metric Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int METRIC_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl <em>Model Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOperatorModelType()
   * @generated
   */
  int OPERATOR_MODEL_TYPE = 29;

  /**
   * The feature id for the '<em><b>Cpp Operator Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL = 0;

  /**
   * The feature id for the '<em><b>Java Operator Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL = 1;

  /**
   * The number of structural features of the '<em>Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_MODEL_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_MODEL_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl
   * <em>Op Model Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOpModelType()
   * @generated
   */
  int OP_MODEL_TYPE = 30;

  /**
   * The feature id for the '<em><b>Context</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE__CONTEXT = 0;

  /**
   * The feature id for the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE__PARAMETERS = 1;

  /**
   * The feature id for the '<em><b>Input Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE__INPUT_PORTS = 2;

  /**
   * The feature id for the '<em><b>Output Ports</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE__OUTPUT_PORTS = 3;

  /**
   * The number of structural features of the '<em>Op Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Op Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OP_MODEL_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OptionalPortScopeTypeImpl <em>Optional Port Scope
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OptionalPortScopeTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOptionalPortScopeType()
   * @generated
   */
  int OPTIONAL_PORT_SCOPE_TYPE = 31;

  /**
   * The feature id for the '<em><b>Port</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPTIONAL_PORT_SCOPE_TYPE__PORT = 0;

  /**
   * The number of structural features of the '<em>Optional Port Scope Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPTIONAL_PORT_SCOPE_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Optional Port Scope Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPTIONAL_PORT_SCOPE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl <em>Output Functions
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputFunctionsType()
   * @generated
   */
  int OUTPUT_FUNCTIONS_TYPE = 32;

  /**
   * The feature id for the '<em><b>Default</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__DEFAULT = 0;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__TYPE = 1;

  /**
   * The number of structural features of the '<em>Output Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Output Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl <em>Output Port Open Set
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortOpenSetType()
   * @generated
   */
  int OUTPUT_PORT_OPEN_SET_TYPE = 33;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE = 1;

  /**
   * The feature id for the '<em><b>Auto Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT = 2;

  /**
   * The feature id for the '<em><b>Complete Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT = 3;

  /**
   * The feature id for the '<em><b>Rewrite Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED = 4;

  /**
   * The feature id for the '<em><b>Output Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS = 5;

  /**
   * The feature id for the '<em><b>Window Punctuation Output Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE = 6;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT = 7;

  /**
   * The feature id for the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE = 8;

  /**
   * The feature id for the '<em><b>Tuple Mutation Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED = 9;

  /**
   * The feature id for the '<em><b>Output Assignment Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE = 10;

  /**
   * The feature id for the '<em><b>Allow Nested Custom Output Functions</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS = 11;

  /**
   * The number of structural features of the '<em>Output Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT = 12;

  /**
   * The number of operations of the '<em>Output Port Open Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortSetTypeImpl <em>Output Port Set
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortSetTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortSetType()
   * @generated
   */
  int OUTPUT_PORT_SET_TYPE = 34;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__DESCRIPTION = OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__EXPRESSION_MODE = OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE;

  /**
   * The feature id for the '<em><b>Auto Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__AUTO_ASSIGNMENT = OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT;

  /**
   * The feature id for the '<em><b>Complete Assignment</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__COMPLETE_ASSIGNMENT = OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT;

  /**
   * The feature id for the '<em><b>Rewrite Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__REWRITE_ALLOWED = OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED;

  /**
   * The feature id for the '<em><b>Output Functions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__OUTPUT_FUNCTIONS = OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS;

  /**
   * The feature id for the '<em><b>Window Punctuation Output Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE =
      OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE;

  /**
   * The feature id for the '<em><b>Window Punctuation Input Port</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT =
      OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT;

  /**
   * The feature id for the '<em><b>Final Punctuation Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE =
      OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE;

  /**
   * The feature id for the '<em><b>Tuple Mutation Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__TUPLE_MUTATION_ALLOWED =
      OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED;

  /**
   * The feature id for the '<em><b>Output Assignment Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE =
      OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE;

  /**
   * The feature id for the '<em><b>Allow Nested Custom Output Functions</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS =
      OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__CARDINALITY = OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE__OPTIONAL = OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Output Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE_FEATURE_COUNT = OUTPUT_PORT_OPEN_SET_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Output Port Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORT_SET_TYPE_OPERATION_COUNT = OUTPUT_PORT_OPEN_SET_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortsTypeImpl <em>Output Ports Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortsType()
   * @generated
   */
  int OUTPUT_PORTS_TYPE = 35;

  /**
   * The feature id for the '<em><b>Output Port Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET = 0;

  /**
   * The feature id for the '<em><b>Output Port Open Set</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET = 1;

  /**
   * The number of structural features of the '<em>Output Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORTS_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Output Ports Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_PORTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl <em>Parameters Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getParametersType()
   * @generated
   */
  int PARAMETERS_TYPE = 36;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETERS_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Allow Any</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETERS_TYPE__ALLOW_ANY = 1;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETERS_TYPE__PARAMETER = 2;

  /**
   * The number of structural features of the '<em>Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETERS_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl <em>Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getParameterType()
   * @generated
   */
  int PARAMETER_TYPE = 37;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__OPTIONAL = 2;

  /**
   * The feature id for the '<em><b>Rewrite Allowed</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__REWRITE_ALLOWED = 3;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__EXPRESSION_MODE = 4;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__TYPE = 5;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__CARDINALITY = 6;

  /**
   * The feature id for the '<em><b>Port Scope</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__PORT_SCOPE = 7;

  /**
   * The feature id for the '<em><b>Custom Output Function</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION = 8;

  /**
   * The number of structural features of the '<em>Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE_FEATURE_COUNT = 9;

  /**
   * The number of operations of the '<em>Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.PortScopeTypeImpl <em>Port Scope Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.PortScopeTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getPortScopeType()
   * @generated
   */
  int PORT_SCOPE_TYPE = 38;

  /**
   * The feature id for the '<em><b>Port</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PORT_SCOPE_TYPE__PORT = 0;

  /**
   * The number of structural features of the '<em>Port Scope Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PORT_SCOPE_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Port Scope Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PORT_SCOPE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl <em>Spl Expression Tree
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSplExpressionTreeType()
   * @generated
   */
  int SPL_EXPRESSION_TREE_TYPE = 39;

  /**
   * The feature id for the '<em><b>Cpp Code</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_EXPRESSION_TREE_TYPE__CPP_CODE = 0;

  /**
   * The feature id for the '<em><b>Output</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_EXPRESSION_TREE_TYPE__OUTPUT = 1;

  /**
   * The feature id for the '<em><b>Param</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_EXPRESSION_TREE_TYPE__PARAM = 2;

  /**
   * The number of structural features of the '<em>Spl Expression Tree Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_EXPRESSION_TREE_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Spl Expression Tree Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_EXPRESSION_TREE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * <em>Expression Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getExpressionModeType()
   * @generated
   */
  int EXPRESSION_MODE_TYPE = 40;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType <em>Incremental
   * Compilation Strategy Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIncrementalCompilationStrategyType()
   * @generated
   */
  int INCREMENTAL_COMPILATION_STRATEGY_TYPE = 41;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType <em>Java Op Expression Mode
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExpressionModeType()
   * @generated
   */
  int JAVA_OP_EXPRESSION_MODE_TYPE = 42;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * <em>Metric Kind Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricKindType()
   * @generated
   */
  int METRIC_KIND_TYPE = 43;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType <em>Single Threaded Context
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSingleThreadedContextType()
   * @generated
   */
  int SINGLE_THREADED_CONTEXT_TYPE = 44;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType <em>Window Expression Mode
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowExpressionModeType()
   * @generated
   */
  int WINDOW_EXPRESSION_MODE_TYPE = 45;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * <em>Windowing Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowingModeType()
   * @generated
   */
  int WINDOWING_MODE_TYPE = 46;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType <em>Window Punctuation
   * Input Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationInputModeType()
   * @generated
   */
  int WINDOW_PUNCTUATION_INPUT_MODE_TYPE = 47;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType <em>Window Punctuation
   * Output Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationOutputModeType()
   * @generated
   */
  int WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE = 48;

  /**
   * The meta object id for the '<em>Expression Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getExpressionModeTypeObject()
   * @generated
   */
  int EXPRESSION_MODE_TYPE_OBJECT = 49;

  /**
   * The meta object id for the '<em>Incremental Compilation Strategy Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIncrementalCompilationStrategyTypeObject()
   * @generated
   */
  int INCREMENTAL_COMPILATION_STRATEGY_TYPE_OBJECT = 50;

  /**
   * The meta object id for the '<em>Java Op Expression Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExpressionModeTypeObject()
   * @generated
   */
  int JAVA_OP_EXPRESSION_MODE_TYPE_OBJECT = 51;

  /**
   * The meta object id for the '<em>Metric Kind Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricKindTypeObject()
   * @generated
   */
  int METRIC_KIND_TYPE_OBJECT = 52;

  /**
   * The meta object id for the '<em>Single Threaded Context Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSingleThreadedContextTypeObject()
   * @generated
   */
  int SINGLE_THREADED_CONTEXT_TYPE_OBJECT = 53;

  /**
   * The meta object id for the '<em>Window Expression Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowExpressionModeTypeObject()
   * @generated
   */
  int WINDOW_EXPRESSION_MODE_TYPE_OBJECT = 54;

  /**
   * The meta object id for the '<em>Windowing Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowingModeTypeObject()
   * @generated
   */
  int WINDOWING_MODE_TYPE_OBJECT = 55;

  /**
   * The meta object id for the '<em>Window Punctuation Input Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationInputModeTypeObject()
   * @generated
   */
  int WINDOW_PUNCTUATION_INPUT_MODE_TYPE_OBJECT = 56;

  /**
   * The meta object id for the '<em>Window Punctuation Output Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationOutputModeTypeObject()
   * @generated
   */
  int WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE_OBJECT = 57;

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType <em>Code Templates Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Code Templates Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType
   * @generated
   */
  EClass getCodeTemplatesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType#getCodeTemplate <em>Code
   * Template</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Code Template</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType#getCodeTemplate()
   * @see #getCodeTemplatesType()
   * @generated
   */
  EReference getCodeTemplatesType_CodeTemplate();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType <em>Code Template Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Code Template Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType
   * @generated
   */
  EClass getCodeTemplateType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getDescription()
   * @see #getCodeTemplateType()
   * @generated
   */
  EReference getCodeTemplateType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getTemplate <em>Template</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Template</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getTemplate()
   * @see #getCodeTemplateType()
   * @generated
   */
  EAttribute getCodeTemplateType_Template();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getName()
   * @see #getCodeTemplateType()
   * @generated
   */
  EAttribute getCodeTemplateType_Name();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.ContextType
   * <em>Context Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Context Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType
   * @generated
   */
  EClass getContextType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getDescription()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_Description();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIconUri <em>Icon Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Icon Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIconUri()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_IconUri();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getMetrics <em>Metrics</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Metrics</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getMetrics()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_Metrics();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomLiterals <em>Custom
   * Literals</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Custom Literals</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomLiterals()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_CustomLiterals();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomOutputFunctions <em>Custom Output
   * Functions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Custom Output Functions</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCustomOutputFunctions()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_CustomOutputFunctions();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getLibraryDependencies <em>Library
   * Dependencies</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Library Dependencies</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getLibraryDependencies()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_LibraryDependencies();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext
   * <em>Provides Single Threaded Context</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Provides Single Threaded Context</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getProvidesSingleThreadedContext()
   * @see #getContextType()
   * @generated
   */
  EAttribute getContextType_ProvidesSingleThreadedContext();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy
   * <em>Incremental Compilation Strategy</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Incremental Compilation Strategy</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getIncrementalCompilationStrategy()
   * @see #getContextType()
   * @generated
   */
  EAttribute getContextType_IncrementalCompilationStrategy();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic <em>Allow Custom
   * Logic</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Allow Custom Logic</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#isAllowCustomLogic()
   * @see #getContextType()
   * @generated
   */
  EAttribute getContextType_AllowCustomLogic();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCodeTemplates <em>Code Templates</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Code Templates</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCodeTemplates()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_CodeTemplates();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getSplExpressionTree <em>Spl Expression
   * Tree</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Expression Tree</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getSplExpressionTree()
   * @see #getContextType()
   * @generated
   */
  EReference getContextType_SplExpressionTree();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCapability <em>Capability</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Capability</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getCapability()
   * @see #getContextType()
   * @generated
   */
  EAttribute getContextType_Capability();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType#getVerificationModule <em>Verification
   * Module</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Verification Module</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType#getVerificationModule()
   * @see #getContextType()
   * @generated
   */
  EAttribute getContextType_VerificationModule();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType <em>Custom Output Function
   * Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Custom Output Function Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType
   * @generated
   */
  EClass getCustomOutputFunctionSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType#getName()
   * @see #getCustomOutputFunctionSetType()
   * @generated
   */
  EAttribute getCustomOutputFunctionSetType_Name();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType#getFunction
   * <em>Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType#getFunction()
   * @see #getCustomOutputFunctionSetType()
   * @generated
   */
  EReference getCustomOutputFunctionSetType_Function();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType <em>Custom Output Functions
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Custom Output Functions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType
   * @generated
   */
  EClass getCustomOutputFunctionsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType#getCustomOutputFunction
   * <em>Custom Output Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Custom Output Function</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType#getCustomOutputFunction()
   * @see #getCustomOutputFunctionsType()
   * @generated
   */
  EReference getCustomOutputFunctionsType_CustomOutputFunction();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType <em>Custom Output Function
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Custom Output Function Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType
   * @generated
   */
  EClass getCustomOutputFunctionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getDescription()
   * @see #getCustomOutputFunctionType()
   * @generated
   */
  EReference getCustomOutputFunctionType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getPrototype
   * <em>Prototype</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Prototype</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getPrototype()
   * @see #getCustomOutputFunctionType()
   * @generated
   */
  EAttribute getCustomOutputFunctionType_Prototype();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction <em>Pseudo
   * Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Pseudo Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction()
   * @see #getCustomOutputFunctionType()
   * @generated
   */
  EAttribute getCustomOutputFunctionType_PseudoFunction();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot
   * <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getOperatorModel <em>Operator
   * Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Operator Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot#getOperatorModel()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_OperatorModel();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType <em>Enumerations Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Enumerations Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType
   * @generated
   */
  EClass getEnumerationsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType#getEnumeration <em>Enumeration</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Enumeration</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType#getEnumeration()
   * @see #getEnumerationsType()
   * @generated
   */
  EReference getEnumerationsType_Enumeration();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationType <em>Enumeration Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Enumeration Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationType
   * @generated
   */
  EClass getEnumerationType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationType#getName()
   * @see #getEnumerationType()
   * @generated
   */
  EAttribute getEnumerationType_Name();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationType#getValue()
   * @see #getEnumerationType()
   * @generated
   */
  EAttribute getEnumerationType_Value();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.IconUriType
   * <em>Icon Uri Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Icon Uri Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IconUriType
   * @generated
   */
  EClass getIconUriType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getValue()
   * @see #getIconUriType()
   * @generated
   */
  EAttribute getIconUriType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize <em>Size</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Size</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IconUriType#getSize()
   * @see #getIconUriType()
   * @generated
   */
  EAttribute getIconUriType_Size();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType <em>Input Port Open Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Input Port Open Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType
   * @generated
   */
  EClass getInputPortOpenSetType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getDescription()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EReference getInputPortOpenSetType_Description();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowingDescription
   * <em>Windowing Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Windowing Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowingDescription()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EReference getInputPortOpenSetType_WindowingDescription();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isTupleMutationAllowed <em>Tuple
   * Mutation Allowed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Tuple Mutation Allowed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isTupleMutationAllowed()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_TupleMutationAllowed();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowingMode <em>Windowing
   * Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Windowing Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowingMode()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_WindowingMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowPunctuationInputMode
   * <em>Window Punctuation Input Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Input Mode</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowPunctuationInputMode()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_WindowPunctuationInputMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isControlPort <em>Control
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Control Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isControlPort()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_ControlPort();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowExpressionMode <em>Window
   * Expression Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#getWindowExpressionMode()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_WindowExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isRewriteAllowedForWindowExpression
   * <em>Rewrite Allowed For Window Expression</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Rewrite Allowed For Window Expression</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType#isRewriteAllowedForWindowExpression()
   * @see #getInputPortOpenSetType()
   * @generated
   */
  EAttribute getInputPortOpenSetType_RewriteAllowedForWindowExpression();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType <em>Input Port Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Input Port Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType
   * @generated
   */
  EClass getInputPortSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType#getCardinality <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType#getCardinality()
   * @see #getInputPortSetType()
   * @generated
   */
  EAttribute getInputPortSetType_Cardinality();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType#isOptional()
   * @see #getInputPortSetType()
   * @generated
   */
  EAttribute getInputPortSetType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortsType <em>Input Ports Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Input Ports Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortsType
   * @generated
   */
  EClass getInputPortsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortsType#getInputPortSet <em>Input Port
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Input Port Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortsType#getInputPortSet()
   * @see #getInputPortsType()
   * @generated
   */
  EReference getInputPortsType_InputPortSet();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortsType#getInputPortOpenSet <em>Input Port Open
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Input Port Open Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortsType#getInputPortOpenSet()
   * @see #getInputPortsType()
   * @generated
   */
  EReference getInputPortsType_InputPortOpenSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType <em>Java Op Context Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Context Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType
   * @generated
   */
  EClass getJavaOpContextType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getDescription()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_Description();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getIconUri <em>Icon Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Icon Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getIconUri()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_IconUri();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getMetrics <em>Metrics</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Metrics</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getMetrics()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_Metrics();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCustomLiterals <em>Custom
   * Literals</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Custom Literals</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCustomLiterals()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_CustomLiterals();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getExecutionSettings <em>Execution
   * Settings</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Execution Settings</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getExecutionSettings()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_ExecutionSettings();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getLibraryDependencies <em>Library
   * Dependencies</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Library Dependencies</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getLibraryDependencies()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_LibraryDependencies();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCodeTemplates <em>Code
   * Templates</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Code Templates</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType#getCodeTemplates()
   * @see #getJavaOpContextType()
   * @generated
   */
  EReference getJavaOpContextType_CodeTemplates();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType <em>Java Op Execution
   * Settings Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Execution Settings Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType
   * @generated
   */
  EClass getJavaOpExecutionSettingsType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getClassName <em>Class
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Class Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getClassName()
   * @see #getJavaOpExecutionSettingsType()
   * @generated
   */
  EAttribute getJavaOpExecutionSettingsType_ClassName();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getVmArgs <em>Vm
   * Args</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Vm Args</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType#getVmArgs()
   * @see #getJavaOpExecutionSettingsType()
   * @generated
   */
  EReference getJavaOpExecutionSettingsType_VmArgs();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType <em>Java Op Input Port Open
   * Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Input Port Open Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType
   * @generated
   */
  EClass getJavaOpInputPortOpenSetType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getDescription()
   * @see #getJavaOpInputPortOpenSetType()
   * @generated
   */
  EReference getJavaOpInputPortOpenSetType_Description();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingDescription
   * <em>Windowing Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Windowing Description</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingDescription()
   * @see #getJavaOpInputPortOpenSetType()
   * @generated
   */
  EReference getJavaOpInputPortOpenSetType_WindowingDescription();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode
   * <em>Windowing Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Windowing Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowingMode()
   * @see #getJavaOpInputPortOpenSetType()
   * @generated
   */
  EAttribute getJavaOpInputPortOpenSetType_WindowingMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode
   * <em>Window Punctuation Input Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Input Mode</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#getWindowPunctuationInputMode()
   * @see #getJavaOpInputPortOpenSetType()
   * @generated
   */
  EAttribute getJavaOpInputPortOpenSetType_WindowPunctuationInputMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort <em>Control
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Control Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType#isControlPort()
   * @see #getJavaOpInputPortOpenSetType()
   * @generated
   */
  EAttribute getJavaOpInputPortOpenSetType_ControlPort();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType <em>Java Op Input Port Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Input Port Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType
   * @generated
   */
  EClass getJavaOpInputPortSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType#getCardinality
   * <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType#getCardinality()
   * @see #getJavaOpInputPortSetType()
   * @generated
   */
  EAttribute getJavaOpInputPortSetType_Cardinality();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType#isOptional()
   * @see #getJavaOpInputPortSetType()
   * @generated
   */
  EAttribute getJavaOpInputPortSetType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType <em>Java Op Input Ports
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Input Ports Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType
   * @generated
   */
  EClass getJavaOpInputPortsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortSet <em>Input Port
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Input Port Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortSet()
   * @see #getJavaOpInputPortsType()
   * @generated
   */
  EReference getJavaOpInputPortsType_InputPortSet();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortOpenSet <em>Input Port
   * Open Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Input Port Open Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType#getInputPortOpenSet()
   * @see #getJavaOpInputPortsType()
   * @generated
   */
  EReference getJavaOpInputPortsType_InputPortOpenSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType <em>Java Op Library
   * Dependencies Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Library Dependencies Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType
   * @generated
   */
  EClass getJavaOpLibraryDependenciesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType#getLibrary
   * <em>Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType#getLibrary()
   * @see #getJavaOpLibraryDependenciesType()
   * @generated
   */
  EReference getJavaOpLibraryDependenciesType_Library();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType <em>Java Op Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType
   * @generated
   */
  EClass getJavaOpModelType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getContext <em>Context</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Context</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getContext()
   * @see #getJavaOpModelType()
   * @generated
   */
  EReference getJavaOpModelType_Context();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getParameters <em>Parameters</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Parameters</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getParameters()
   * @see #getJavaOpModelType()
   * @generated
   */
  EReference getJavaOpModelType_Parameters();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getInputPorts <em>Input Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Input Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getInputPorts()
   * @see #getJavaOpModelType()
   * @generated
   */
  EReference getJavaOpModelType_InputPorts();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getOutputPorts <em>Output Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType#getOutputPorts()
   * @see #getJavaOpModelType()
   * @generated
   */
  EReference getJavaOpModelType_OutputPorts();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType <em>Java Op Output Port
   * Open Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Output Port Open Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType
   * @generated
   */
  EClass getJavaOpOutputPortOpenSetType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getDescription()
   * @see #getJavaOpOutputPortOpenSetType()
   * @generated
   */
  EReference getJavaOpOutputPortOpenSetType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Output Mode</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationOutputMode()
   * @see #getJavaOpOutputPortOpenSetType()
   * @generated
   */
  EAttribute getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationInputPort
   * <em>Window Punctuation Input Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Input Port</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getWindowPunctuationInputPort()
   * @see #getJavaOpOutputPortOpenSetType()
   * @generated
   */
  EAttribute getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getFinalPunctuationPortScope
   * <em>Final Punctuation Port Scope</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Final Punctuation Port Scope</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType#getFinalPunctuationPortScope()
   * @see #getJavaOpOutputPortOpenSetType()
   * @generated
   */
  EReference getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType <em>Java Op Output Port Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Output Port Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType
   * @generated
   */
  EClass getJavaOpOutputPortSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType#getCardinality
   * <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType#getCardinality()
   * @see #getJavaOpOutputPortSetType()
   * @generated
   */
  EAttribute getJavaOpOutputPortSetType_Cardinality();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType#isOptional()
   * @see #getJavaOpOutputPortSetType()
   * @generated
   */
  EAttribute getJavaOpOutputPortSetType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType <em>Java Op Output Ports
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Output Ports Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType
   * @generated
   */
  EClass getJavaOpOutputPortsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortSet <em>Output Port
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Port Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortSet()
   * @see #getJavaOpOutputPortsType()
   * @generated
   */
  EReference getJavaOpOutputPortsType_OutputPortSet();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortOpenSet <em>Output
   * Port Open Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Port Open Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType#getOutputPortOpenSet()
   * @see #getJavaOpOutputPortsType()
   * @generated
   */
  EReference getJavaOpOutputPortsType_OutputPortOpenSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType <em>Java Op Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Parameters Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType
   * @generated
   */
  EClass getJavaOpParametersType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType#getDescription()
   * @see #getJavaOpParametersType()
   * @generated
   */
  EReference getJavaOpParametersType_Description();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType#getParameter <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType#getParameter()
   * @see #getJavaOpParametersType()
   * @generated
   */
  EReference getJavaOpParametersType_Parameter();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType <em>Java Op Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType
   * @generated
   */
  EClass getJavaOpParameterType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getName()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EAttribute getJavaOpParameterType_Name();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getDescription()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EReference getJavaOpParameterType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#isOptional()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EAttribute getJavaOpParameterType_Optional();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getExpressionMode <em>Expression
   * Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getExpressionMode()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EAttribute getJavaOpParameterType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getType()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EAttribute getJavaOpParameterType_Type();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getCardinality
   * <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType#getCardinality()
   * @see #getJavaOpParameterType()
   * @generated
   */
  EAttribute getJavaOpParameterType_Cardinality();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType <em>Java Op VM Args Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op VM Args Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType
   * @generated
   */
  EClass getJavaOpVMArgsType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType#getVmArg <em>Vm Arg</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Vm Arg</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType#getVmArg()
   * @see #getJavaOpVMArgsType()
   * @generated
   */
  EAttribute getJavaOpVMArgsType_VmArg();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType <em>Library Dependencies
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Library Dependencies Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType
   * @generated
   */
  EClass getLibraryDependenciesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType#getLibrary <em>Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType#getLibrary()
   * @see #getLibraryDependenciesType()
   * @generated
   */
  EReference getLibraryDependenciesType_Library();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricsType
   * <em>Metrics Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Metrics Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricsType
   * @generated
   */
  EClass getMetricsType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricsType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricsType#getDescription()
   * @see #getMetricsType()
   * @generated
   */
  EReference getMetricsType_Description();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricsType#getMetric <em>Metric</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Metric</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricsType#getMetric()
   * @see #getMetricsType()
   * @generated
   */
  EReference getMetricsType_Metric();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricType
   * <em>Metric Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Metric Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType
   * @generated
   */
  EClass getMetricType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType#getName()
   * @see #getMetricType()
   * @generated
   */
  EAttribute getMetricType_Name();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType#getDescription()
   * @see #getMetricType()
   * @generated
   */
  EReference getMetricType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind <em>Kind</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Kind</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType#getKind()
   * @see #getMetricType()
   * @generated
   */
  EAttribute getMetricType_Kind();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic <em>Dynamic</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Dynamic</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType#isDynamic()
   * @see #getMetricType()
   * @generated
   */
  EAttribute getMetricType_Dynamic();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType <em>Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType
   * @generated
   */
  EClass getOperatorModelType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getCppOperatorModel <em>Cpp Operator
   * Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Cpp Operator Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getCppOperatorModel()
   * @see #getOperatorModelType()
   * @generated
   */
  EReference getOperatorModelType_CppOperatorModel();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getJavaOperatorModel <em>Java
   * Operator Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Java Operator Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType#getJavaOperatorModel()
   * @see #getOperatorModelType()
   * @generated
   */
  EReference getOperatorModelType_JavaOperatorModel();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.OpModelType
   * <em>Op Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Op Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType
   * @generated
   */
  EClass getOpModelType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getContext <em>Context</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Context</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getContext()
   * @see #getOpModelType()
   * @generated
   */
  EReference getOpModelType_Context();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getParameters <em>Parameters</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Parameters</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getParameters()
   * @see #getOpModelType()
   * @generated
   */
  EReference getOpModelType_Parameters();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getInputPorts <em>Input Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Input Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getInputPorts()
   * @see #getOpModelType()
   * @generated
   */
  EReference getOpModelType_InputPorts();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getOutputPorts <em>Output Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType#getOutputPorts()
   * @see #getOpModelType()
   * @generated
   */
  EReference getOpModelType_OutputPorts();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType <em>Optional Port Scope
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Optional Port Scope Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType
   * @generated
   */
  EClass getOptionalPortScopeType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType#getPort <em>Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType#getPort()
   * @see #getOptionalPortScopeType()
   * @generated
   */
  EAttribute getOptionalPortScopeType_Port();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType <em>Output Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Functions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType
   * @generated
   */
  EClass getOutputFunctionsType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType#getDefault <em>Default</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Default</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType#getDefault()
   * @see #getOutputFunctionsType()
   * @generated
   */
  EAttribute getOutputFunctionsType_Default();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType#getType()
   * @see #getOutputFunctionsType()
   * @generated
   */
  EAttribute getOutputFunctionsType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType <em>Output Port Open Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Port Open Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType
   * @generated
   */
  EClass getOutputPortOpenSetType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getDescription()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EReference getOutputPortOpenSetType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode <em>Expression
   * Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getExpressionMode()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment <em>Auto
   * Assignment</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Auto Assignment</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAutoAssignment()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_AutoAssignment();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment <em>Complete
   * Assignment</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Complete Assignment</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isCompleteAssignment()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_CompleteAssignment();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed <em>Rewrite
   * Allowed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Rewrite Allowed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isRewriteAllowed()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_RewriteAllowed();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputFunctions <em>Output
   * Functions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Functions</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputFunctions()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EReference getOutputPortOpenSetType_OutputFunctions();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode
   * <em>Window Punctuation Output Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Output Mode</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationOutputMode()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_WindowPunctuationOutputMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationInputPort
   * <em>Window Punctuation Input Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punctuation Input Port</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getWindowPunctuationInputPort()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_WindowPunctuationInputPort();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getFinalPunctuationPortScope
   * <em>Final Punctuation Port Scope</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Final Punctuation Port Scope</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getFinalPunctuationPortScope()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EReference getOutputPortOpenSetType_FinalPunctuationPortScope();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed <em>Tuple
   * Mutation Allowed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Tuple Mutation Allowed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isTupleMutationAllowed()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_TupleMutationAllowed();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputAssignmentPortScope
   * <em>Output Assignment Port Scope</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Assignment Port Scope</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#getOutputAssignmentPortScope()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EReference getOutputPortOpenSetType_OutputAssignmentPortScope();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions
   * <em>Allow Nested Custom Output Functions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Allow Nested Custom Output Functions</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType#isAllowNestedCustomOutputFunctions()
   * @see #getOutputPortOpenSetType()
   * @generated
   */
  EAttribute getOutputPortOpenSetType_AllowNestedCustomOutputFunctions();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType <em>Output Port Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Port Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType
   * @generated
   */
  EClass getOutputPortSetType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#getCardinality
   * <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#getCardinality()
   * @see #getOutputPortSetType()
   * @generated
   */
  EAttribute getOutputPortSetType_Cardinality();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType#isOptional()
   * @see #getOutputPortSetType()
   * @generated
   */
  EAttribute getOutputPortSetType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType <em>Output Ports Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Ports Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType
   * @generated
   */
  EClass getOutputPortsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType#getOutputPortSet <em>Output Port
   * Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Port Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType#getOutputPortSet()
   * @see #getOutputPortsType()
   * @generated
   */
  EReference getOutputPortsType_OutputPortSet();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType#getOutputPortOpenSet <em>Output Port
   * Open Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Port Open Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType#getOutputPortOpenSet()
   * @see #getOutputPortsType()
   * @generated
   */
  EReference getOutputPortsType_OutputPortOpenSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType <em>Parameters Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Parameters Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParametersType
   * @generated
   */
  EClass getParametersType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getDescription()
   * @see #getParametersType()
   * @generated
   */
  EReference getParametersType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny <em>Allow Any</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Allow Any</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParametersType#isAllowAny()
   * @see #getParametersType()
   * @generated
   */
  EAttribute getParametersType_AllowAny();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getParameter <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParametersType#getParameter()
   * @see #getParametersType()
   * @generated
   */
  EReference getParametersType_Parameter();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.ParameterType
   * <em>Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType
   * @generated
   */
  EClass getParameterType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getName()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Name();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getDescription()
   * @see #getParameterType()
   * @generated
   */
  EReference getParameterType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isOptional()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Optional();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed <em>Rewrite
   * Allowed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Rewrite Allowed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#isRewriteAllowed()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_RewriteAllowed();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode <em>Expression
   * Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getExpressionMode()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getType()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Type();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCardinality <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCardinality()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Cardinality();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getPortScope <em>Port Scope</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Port Scope</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getPortScope()
   * @see #getParameterType()
   * @generated
   */
  EReference getParameterType_PortScope();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCustomOutputFunction <em>Custom Output
   * Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Custom Output Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType#getCustomOutputFunction()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_CustomOutputFunction();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.operator.PortScopeType
   * <em>Port Scope Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Port Scope Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.PortScopeType
   * @generated
   */
  EClass getPortScopeType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.PortScopeType#getPort <em>Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.PortScopeType#getPort()
   * @see #getPortScopeType()
   * @generated
   */
  EAttribute getPortScopeType_Port();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType <em>Spl Expression Tree
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Spl Expression Tree Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType
   * @generated
   */
  EClass getSplExpressionTreeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode <em>Cpp Code</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cpp Code</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isCppCode()
   * @see #getSplExpressionTreeType()
   * @generated
   */
  EAttribute getSplExpressionTreeType_CppCode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput <em>Output</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Output</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isOutput()
   * @see #getSplExpressionTreeType()
   * @generated
   */
  EAttribute getSplExpressionTreeType_Output();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam <em>Param</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Param</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType#isParam()
   * @see #getSplExpressionTreeType()
   * @generated
   */
  EAttribute getSplExpressionTreeType_Param();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType <em>Expression Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Expression Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @generated
   */
  EEnum getExpressionModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType <em>Incremental
   * Compilation Strategy Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Incremental Compilation Strategy Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @generated
   */
  EEnum getIncrementalCompilationStrategyType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType <em>Java Op Expression Mode
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Java Op Expression Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
   * @generated
   */
  EEnum getJavaOpExpressionModeType();

  /**
   * Returns the meta object for enum '{@link com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * <em>Metric Kind Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Metric Kind Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @generated
   */
  EEnum getMetricKindType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType <em>Single Threaded Context
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Single Threaded Context Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @generated
   */
  EEnum getSingleThreadedContextType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType <em>Window Expression Mode
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Window Expression Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
   * @generated
   */
  EEnum getWindowExpressionModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType <em>Windowing Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Windowing Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @generated
   */
  EEnum getWindowingModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType <em>Window Punctuation
   * Input Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Window Punctuation Input Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @generated
   */
  EEnum getWindowPunctuationInputModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType <em>Window Punctuation
   * Output Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Window Punctuation Output Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @generated
   */
  EEnum getWindowPunctuationOutputModeType();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType <em>Expression Mode Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Expression Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType"
   *     extendedMetaData="name='expressionModeType:Object' baseType='expressionModeType'"
   * @generated
   */
  EDataType getExpressionModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType <em>Incremental
   * Compilation Strategy Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Incremental Compilation Strategy Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
   * @model
   *     instanceClass="com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType"
   *     extendedMetaData="name='incrementalCompilationStrategyType:Object'
   *     baseType='incrementalCompilationStrategyType'"
   * @generated
   */
  EDataType getIncrementalCompilationStrategyTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType <em>Java Op Expression Mode
   * Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Java Op Expression Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType"
   *     extendedMetaData="name='javaOpExpressionModeType:Object'
   *     baseType='javaOpExpressionModeType'"
   * @generated
   */
  EDataType getJavaOpExpressionModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricKindType <em>Metric Kind Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Metric Kind Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.MetricKindType"
   *     extendedMetaData="name='metricKindType:Object' baseType='metricKindType'"
   * @generated
   */
  EDataType getMetricKindTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType <em>Single Threaded Context
   * Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Single Threaded Context Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType"
   *     extendedMetaData="name='singleThreadedContextType:Object'
   *     baseType='singleThreadedContextType'"
   * @generated
   */
  EDataType getSingleThreadedContextTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType <em>Window Expression Mode
   * Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Window Expression Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType"
   *     extendedMetaData="name='windowExpressionModeType:Object'
   *     baseType='windowExpressionModeType'"
   * @generated
   */
  EDataType getWindowExpressionModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType <em>Windowing Mode Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Windowing Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType"
   *     extendedMetaData="name='windowingModeType:Object' baseType='windowingModeType'"
   * @generated
   */
  EDataType getWindowingModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType <em>Window Punctuation
   * Input Mode Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Window Punctuation Input Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType"
   *     extendedMetaData="name='windowPunctuationInputModeType:Object'
   *     baseType='windowPunctuationInputModeType'"
   * @generated
   */
  EDataType getWindowPunctuationInputModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType <em>Window Punctuation
   * Output Mode Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Window Punctuation Output Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType"
   *     extendedMetaData="name='windowPunctuationOutputModeType:Object'
   *     baseType='windowPunctuationOutputModeType'"
   * @generated
   */
  EDataType getWindowPunctuationOutputModeTypeObject();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  OperatorFactory getOperatorFactory();

  /**
   *
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   *
   * <ul>
   *   <li>each class,
   *   <li>each feature of each class,
   *   <li>each operation of each class,
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
     * com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplatesTypeImpl <em>Code Templates
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplatesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCodeTemplatesType()
     * @generated
     */
    EClass CODE_TEMPLATES_TYPE = eINSTANCE.getCodeTemplatesType();

    /**
     * The meta object literal for the '<em><b>Code Template</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CODE_TEMPLATES_TYPE__CODE_TEMPLATE = eINSTANCE.getCodeTemplatesType_CodeTemplate();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplateTypeImpl <em>Code Template
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CodeTemplateTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCodeTemplateType()
     * @generated
     */
    EClass CODE_TEMPLATE_TYPE = eINSTANCE.getCodeTemplateType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CODE_TEMPLATE_TYPE__DESCRIPTION = eINSTANCE.getCodeTemplateType_Description();

    /**
     * The meta object literal for the '<em><b>Template</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CODE_TEMPLATE_TYPE__TEMPLATE = eINSTANCE.getCodeTemplateType_Template();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CODE_TEMPLATE_TYPE__NAME = eINSTANCE.getCodeTemplateType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl <em>Context Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ContextTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getContextType()
     * @generated
     */
    EClass CONTEXT_TYPE = eINSTANCE.getContextType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__DESCRIPTION = eINSTANCE.getContextType_Description();

    /**
     * The meta object literal for the '<em><b>Icon Uri</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__ICON_URI = eINSTANCE.getContextType_IconUri();

    /**
     * The meta object literal for the '<em><b>Metrics</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__METRICS = eINSTANCE.getContextType_Metrics();

    /**
     * The meta object literal for the '<em><b>Custom Literals</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__CUSTOM_LITERALS = eINSTANCE.getContextType_CustomLiterals();

    /**
     * The meta object literal for the '<em><b>Custom Output Functions</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS =
        eINSTANCE.getContextType_CustomOutputFunctions();

    /**
     * The meta object literal for the '<em><b>Library Dependencies</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__LIBRARY_DEPENDENCIES = eINSTANCE.getContextType_LibraryDependencies();

    /**
     * The meta object literal for the '<em><b>Provides Single Threaded Context</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT =
        eINSTANCE.getContextType_ProvidesSingleThreadedContext();

    /**
     * The meta object literal for the '<em><b>Incremental Compilation Strategy</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY =
        eINSTANCE.getContextType_IncrementalCompilationStrategy();

    /**
     * The meta object literal for the '<em><b>Allow Custom Logic</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC = eINSTANCE.getContextType_AllowCustomLogic();

    /**
     * The meta object literal for the '<em><b>Code Templates</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__CODE_TEMPLATES = eINSTANCE.getContextType_CodeTemplates();

    /**
     * The meta object literal for the '<em><b>Spl Expression Tree</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONTEXT_TYPE__SPL_EXPRESSION_TREE = eINSTANCE.getContextType_SplExpressionTree();

    /**
     * The meta object literal for the '<em><b>Capability</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONTEXT_TYPE__CAPABILITY = eINSTANCE.getContextType_Capability();

    /**
     * The meta object literal for the '<em><b>Verification Module</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONTEXT_TYPE__VERIFICATION_MODULE = eINSTANCE.getContextType_VerificationModule();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl <em>Custom
     * Output Function Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionSetType()
     * @generated
     */
    EClass CUSTOM_OUTPUT_FUNCTION_SET_TYPE = eINSTANCE.getCustomOutputFunctionSetType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME =
        eINSTANCE.getCustomOutputFunctionSetType_Name();

    /**
     * The meta object literal for the '<em><b>Function</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION =
        eINSTANCE.getCustomOutputFunctionSetType_Function();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionsTypeImpl <em>Custom Output
     * Functions Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionsType()
     * @generated
     */
    EClass CUSTOM_OUTPUT_FUNCTIONS_TYPE = eINSTANCE.getCustomOutputFunctionsType();

    /**
     * The meta object literal for the '<em><b>Custom Output Function</b></em>' containment
     * reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION =
        eINSTANCE.getCustomOutputFunctionsType_CustomOutputFunction();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl <em>Custom Output
     * Function Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.CustomOutputFunctionTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getCustomOutputFunctionType()
     * @generated
     */
    EClass CUSTOM_OUTPUT_FUNCTION_TYPE = eINSTANCE.getCustomOutputFunctionType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION =
        eINSTANCE.getCustomOutputFunctionType_Description();

    /**
     * The meta object literal for the '<em><b>Prototype</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE =
        eINSTANCE.getCustomOutputFunctionType_Prototype();

    /**
     * The meta object literal for the '<em><b>Pseudo Function</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION =
        eINSTANCE.getCustomOutputFunctionType_PseudoFunction();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl <em>Document Root</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getDocumentRoot()
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
     * The meta object literal for the '<em><b>Operator Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__OPERATOR_MODEL = eINSTANCE.getDocumentRoot_OperatorModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationsTypeImpl <em>Enumerations
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getEnumerationsType()
     * @generated
     */
    EClass ENUMERATIONS_TYPE = eINSTANCE.getEnumerationsType();

    /**
     * The meta object literal for the '<em><b>Enumeration</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference ENUMERATIONS_TYPE__ENUMERATION = eINSTANCE.getEnumerationsType_Enumeration();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationTypeImpl <em>Enumeration Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.EnumerationTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getEnumerationType()
     * @generated
     */
    EClass ENUMERATION_TYPE = eINSTANCE.getEnumerationType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUMERATION_TYPE__NAME = eINSTANCE.getEnumerationType_Name();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUMERATION_TYPE__VALUE = eINSTANCE.getEnumerationType_Value();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.IconUriTypeImpl <em>Icon Uri Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.IconUriTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIconUriType()
     * @generated
     */
    EClass ICON_URI_TYPE = eINSTANCE.getIconUriType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ICON_URI_TYPE__VALUE = eINSTANCE.getIconUriType_Value();

    /**
     * The meta object literal for the '<em><b>Size</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ICON_URI_TYPE__SIZE = eINSTANCE.getIconUriType_Size();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl <em>Input Port Open Set
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortOpenSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortOpenSetType()
     * @generated
     */
    EClass INPUT_PORT_OPEN_SET_TYPE = eINSTANCE.getInputPortOpenSetType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION =
        eINSTANCE.getInputPortOpenSetType_Description();

    /**
     * The meta object literal for the '<em><b>Windowing Description</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION =
        eINSTANCE.getInputPortOpenSetType_WindowingDescription();

    /**
     * The meta object literal for the '<em><b>Tuple Mutation Allowed</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED =
        eINSTANCE.getInputPortOpenSetType_TupleMutationAllowed();

    /**
     * The meta object literal for the '<em><b>Windowing Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE =
        eINSTANCE.getInputPortOpenSetType_WindowingMode();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Input Mode</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE =
        eINSTANCE.getInputPortOpenSetType_WindowPunctuationInputMode();

    /**
     * The meta object literal for the '<em><b>Control Port</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT =
        eINSTANCE.getInputPortOpenSetType_ControlPort();

    /**
     * The meta object literal for the '<em><b>Window Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE =
        eINSTANCE.getInputPortOpenSetType_WindowExpressionMode();

    /**
     * The meta object literal for the '<em><b>Rewrite Allowed For Window Expression</b></em>'
     * attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION =
        eINSTANCE.getInputPortOpenSetType_RewriteAllowedForWindowExpression();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl <em>Input Port Set
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortSetTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortSetType()
     * @generated
     */
    EClass INPUT_PORT_SET_TYPE = eINSTANCE.getInputPortSetType();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_SET_TYPE__CARDINALITY = eINSTANCE.getInputPortSetType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INPUT_PORT_SET_TYPE__OPTIONAL = eINSTANCE.getInputPortSetType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortsTypeImpl <em>Input Ports Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.InputPortsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getInputPortsType()
     * @generated
     */
    EClass INPUT_PORTS_TYPE = eINSTANCE.getInputPortsType();

    /**
     * The meta object literal for the '<em><b>Input Port Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference INPUT_PORTS_TYPE__INPUT_PORT_SET = eINSTANCE.getInputPortsType_InputPortSet();

    /**
     * The meta object literal for the '<em><b>Input Port Open Set</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET =
        eINSTANCE.getInputPortsType_InputPortOpenSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl <em>Java Op Context
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpContextTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpContextType()
     * @generated
     */
    EClass JAVA_OP_CONTEXT_TYPE = eINSTANCE.getJavaOpContextType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__DESCRIPTION = eINSTANCE.getJavaOpContextType_Description();

    /**
     * The meta object literal for the '<em><b>Icon Uri</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__ICON_URI = eINSTANCE.getJavaOpContextType_IconUri();

    /**
     * The meta object literal for the '<em><b>Metrics</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__METRICS = eINSTANCE.getJavaOpContextType_Metrics();

    /**
     * The meta object literal for the '<em><b>Custom Literals</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS =
        eINSTANCE.getJavaOpContextType_CustomLiterals();

    /**
     * The meta object literal for the '<em><b>Execution Settings</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS =
        eINSTANCE.getJavaOpContextType_ExecutionSettings();

    /**
     * The meta object literal for the '<em><b>Library Dependencies</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES =
        eINSTANCE.getJavaOpContextType_LibraryDependencies();

    /**
     * The meta object literal for the '<em><b>Code Templates</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES =
        eINSTANCE.getJavaOpContextType_CodeTemplates();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl <em>Java Op
     * Execution Settings Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpExecutionSettingsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExecutionSettingsType()
     * @generated
     */
    EClass JAVA_OP_EXECUTION_SETTINGS_TYPE = eINSTANCE.getJavaOpExecutionSettingsType();

    /**
     * The meta object literal for the '<em><b>Class Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME =
        eINSTANCE.getJavaOpExecutionSettingsType_ClassName();

    /**
     * The meta object literal for the '<em><b>Vm Args</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS =
        eINSTANCE.getJavaOpExecutionSettingsType_VmArgs();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl <em>Java Op Input
     * Port Open Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortOpenSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortOpenSetType()
     * @generated
     */
    EClass JAVA_OP_INPUT_PORT_OPEN_SET_TYPE = eINSTANCE.getJavaOpInputPortOpenSetType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION =
        eINSTANCE.getJavaOpInputPortOpenSetType_Description();

    /**
     * The meta object literal for the '<em><b>Windowing Description</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION =
        eINSTANCE.getJavaOpInputPortOpenSetType_WindowingDescription();

    /**
     * The meta object literal for the '<em><b>Windowing Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE =
        eINSTANCE.getJavaOpInputPortOpenSetType_WindowingMode();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Input Mode</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE =
        eINSTANCE.getJavaOpInputPortOpenSetType_WindowPunctuationInputMode();

    /**
     * The meta object literal for the '<em><b>Control Port</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT =
        eINSTANCE.getJavaOpInputPortOpenSetType_ControlPort();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortSetTypeImpl <em>Java Op Input
     * Port Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortSetType()
     * @generated
     */
    EClass JAVA_OP_INPUT_PORT_SET_TYPE = eINSTANCE.getJavaOpInputPortSetType();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_INPUT_PORT_SET_TYPE__CARDINALITY =
        eINSTANCE.getJavaOpInputPortSetType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_INPUT_PORT_SET_TYPE__OPTIONAL =
        eINSTANCE.getJavaOpInputPortSetType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortsTypeImpl <em>Java Op Input Ports
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpInputPortsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpInputPortsType()
     * @generated
     */
    EClass JAVA_OP_INPUT_PORTS_TYPE = eINSTANCE.getJavaOpInputPortsType();

    /**
     * The meta object literal for the '<em><b>Input Port Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_SET =
        eINSTANCE.getJavaOpInputPortsType_InputPortSet();

    /**
     * The meta object literal for the '<em><b>Input Port Open Set</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET =
        eINSTANCE.getJavaOpInputPortsType_InputPortOpenSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpLibraryDependenciesTypeImpl <em>Java Op
     * Library Dependencies Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpLibraryDependenciesTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpLibraryDependenciesType()
     * @generated
     */
    EClass JAVA_OP_LIBRARY_DEPENDENCIES_TYPE = eINSTANCE.getJavaOpLibraryDependenciesType();

    /**
     * The meta object literal for the '<em><b>Library</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_LIBRARY_DEPENDENCIES_TYPE__LIBRARY =
        eINSTANCE.getJavaOpLibraryDependenciesType_Library();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl <em>Java Op Model
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpModelType()
     * @generated
     */
    EClass JAVA_OP_MODEL_TYPE = eINSTANCE.getJavaOpModelType();

    /**
     * The meta object literal for the '<em><b>Context</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_MODEL_TYPE__CONTEXT = eINSTANCE.getJavaOpModelType_Context();

    /**
     * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_MODEL_TYPE__PARAMETERS = eINSTANCE.getJavaOpModelType_Parameters();

    /**
     * The meta object literal for the '<em><b>Input Ports</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_MODEL_TYPE__INPUT_PORTS = eINSTANCE.getJavaOpModelType_InputPorts();

    /**
     * The meta object literal for the '<em><b>Output Ports</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_MODEL_TYPE__OUTPUT_PORTS = eINSTANCE.getJavaOpModelType_OutputPorts();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl <em>Java Op
     * Output Port Open Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortOpenSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortOpenSetType()
     * @generated
     */
    EClass JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE = eINSTANCE.getJavaOpOutputPortOpenSetType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION =
        eINSTANCE.getJavaOpOutputPortOpenSetType_Description();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Output Mode</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE =
        eINSTANCE.getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Input Port</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT =
        eINSTANCE.getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort();

    /**
     * The meta object literal for the '<em><b>Final Punctuation Port Scope</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE =
        eINSTANCE.getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortSetTypeImpl <em>Java Op Output
     * Port Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortSetType()
     * @generated
     */
    EClass JAVA_OP_OUTPUT_PORT_SET_TYPE = eINSTANCE.getJavaOpOutputPortSetType();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_OUTPUT_PORT_SET_TYPE__CARDINALITY =
        eINSTANCE.getJavaOpOutputPortSetType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_OUTPUT_PORT_SET_TYPE__OPTIONAL =
        eINSTANCE.getJavaOpOutputPortSetType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl <em>Java Op Output
     * Ports Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpOutputPortsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpOutputPortsType()
     * @generated
     */
    EClass JAVA_OP_OUTPUT_PORTS_TYPE = eINSTANCE.getJavaOpOutputPortsType();

    /**
     * The meta object literal for the '<em><b>Output Port Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET =
        eINSTANCE.getJavaOpOutputPortsType_OutputPortSet();

    /**
     * The meta object literal for the '<em><b>Output Port Open Set</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET =
        eINSTANCE.getJavaOpOutputPortsType_OutputPortOpenSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParametersTypeImpl <em>Java Op Parameters
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParametersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpParametersType()
     * @generated
     */
    EClass JAVA_OP_PARAMETERS_TYPE = eINSTANCE.getJavaOpParametersType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_PARAMETERS_TYPE__DESCRIPTION =
        eINSTANCE.getJavaOpParametersType_Description();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_PARAMETERS_TYPE__PARAMETER = eINSTANCE.getJavaOpParametersType_Parameter();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl <em>Java Op Parameter
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpParameterType()
     * @generated
     */
    EClass JAVA_OP_PARAMETER_TYPE = eINSTANCE.getJavaOpParameterType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_PARAMETER_TYPE__NAME = eINSTANCE.getJavaOpParameterType_Name();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_PARAMETER_TYPE__DESCRIPTION = eINSTANCE.getJavaOpParameterType_Description();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_PARAMETER_TYPE__OPTIONAL = eINSTANCE.getJavaOpParameterType_Optional();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE =
        eINSTANCE.getJavaOpParameterType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_PARAMETER_TYPE__TYPE = eINSTANCE.getJavaOpParameterType_Type();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_PARAMETER_TYPE__CARDINALITY = eINSTANCE.getJavaOpParameterType_Cardinality();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpVMArgsTypeImpl <em>Java Op VM Args
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.JavaOpVMArgsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpVMArgsType()
     * @generated
     */
    EClass JAVA_OP_VM_ARGS_TYPE = eINSTANCE.getJavaOpVMArgsType();

    /**
     * The meta object literal for the '<em><b>Vm Arg</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_VM_ARGS_TYPE__VM_ARG = eINSTANCE.getJavaOpVMArgsType_VmArg();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.LibraryDependenciesTypeImpl <em>Library
     * Dependencies Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.LibraryDependenciesTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getLibraryDependenciesType()
     * @generated
     */
    EClass LIBRARY_DEPENDENCIES_TYPE = eINSTANCE.getLibraryDependenciesType();

    /**
     * The meta object literal for the '<em><b>Library</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference LIBRARY_DEPENDENCIES_TYPE__LIBRARY = eINSTANCE.getLibraryDependenciesType_Library();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.MetricsTypeImpl <em>Metrics Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.MetricsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricsType()
     * @generated
     */
    EClass METRICS_TYPE = eINSTANCE.getMetricsType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference METRICS_TYPE__DESCRIPTION = eINSTANCE.getMetricsType_Description();

    /**
     * The meta object literal for the '<em><b>Metric</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference METRICS_TYPE__METRIC = eINSTANCE.getMetricsType_Metric();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.MetricTypeImpl <em>Metric Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.MetricTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricType()
     * @generated
     */
    EClass METRIC_TYPE = eINSTANCE.getMetricType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute METRIC_TYPE__NAME = eINSTANCE.getMetricType_Name();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference METRIC_TYPE__DESCRIPTION = eINSTANCE.getMetricType_Description();

    /**
     * The meta object literal for the '<em><b>Kind</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute METRIC_TYPE__KIND = eINSTANCE.getMetricType_Kind();

    /**
     * The meta object literal for the '<em><b>Dynamic</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute METRIC_TYPE__DYNAMIC = eINSTANCE.getMetricType_Dynamic();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl <em>Model Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOperatorModelType()
     * @generated
     */
    EClass OPERATOR_MODEL_TYPE = eINSTANCE.getOperatorModelType();

    /**
     * The meta object literal for the '<em><b>Cpp Operator Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL =
        eINSTANCE.getOperatorModelType_CppOperatorModel();

    /**
     * The meta object literal for the '<em><b>Java Operator Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL =
        eINSTANCE.getOperatorModelType_JavaOperatorModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl <em>Op Model Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OpModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOpModelType()
     * @generated
     */
    EClass OP_MODEL_TYPE = eINSTANCE.getOpModelType();

    /**
     * The meta object literal for the '<em><b>Context</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OP_MODEL_TYPE__CONTEXT = eINSTANCE.getOpModelType_Context();

    /**
     * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OP_MODEL_TYPE__PARAMETERS = eINSTANCE.getOpModelType_Parameters();

    /**
     * The meta object literal for the '<em><b>Input Ports</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OP_MODEL_TYPE__INPUT_PORTS = eINSTANCE.getOpModelType_InputPorts();

    /**
     * The meta object literal for the '<em><b>Output Ports</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OP_MODEL_TYPE__OUTPUT_PORTS = eINSTANCE.getOpModelType_OutputPorts();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OptionalPortScopeTypeImpl <em>Optional Port
     * Scope Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OptionalPortScopeTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOptionalPortScopeType()
     * @generated
     */
    EClass OPTIONAL_PORT_SCOPE_TYPE = eINSTANCE.getOptionalPortScopeType();

    /**
     * The meta object literal for the '<em><b>Port</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPTIONAL_PORT_SCOPE_TYPE__PORT = eINSTANCE.getOptionalPortScopeType_Port();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl <em>Output Functions
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputFunctionsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputFunctionsType()
     * @generated
     */
    EClass OUTPUT_FUNCTIONS_TYPE = eINSTANCE.getOutputFunctionsType();

    /**
     * The meta object literal for the '<em><b>Default</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_FUNCTIONS_TYPE__DEFAULT = eINSTANCE.getOutputFunctionsType_Default();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_FUNCTIONS_TYPE__TYPE = eINSTANCE.getOutputFunctionsType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl <em>Output Port Open
     * Set Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortOpenSetTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortOpenSetType()
     * @generated
     */
    EClass OUTPUT_PORT_OPEN_SET_TYPE = eINSTANCE.getOutputPortOpenSetType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION =
        eINSTANCE.getOutputPortOpenSetType_Description();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE =
        eINSTANCE.getOutputPortOpenSetType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Auto Assignment</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT =
        eINSTANCE.getOutputPortOpenSetType_AutoAssignment();

    /**
     * The meta object literal for the '<em><b>Complete Assignment</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT =
        eINSTANCE.getOutputPortOpenSetType_CompleteAssignment();

    /**
     * The meta object literal for the '<em><b>Rewrite Allowed</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED =
        eINSTANCE.getOutputPortOpenSetType_RewriteAllowed();

    /**
     * The meta object literal for the '<em><b>Output Functions</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS =
        eINSTANCE.getOutputPortOpenSetType_OutputFunctions();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Output Mode</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE =
        eINSTANCE.getOutputPortOpenSetType_WindowPunctuationOutputMode();

    /**
     * The meta object literal for the '<em><b>Window Punctuation Input Port</b></em>' attribute
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT =
        eINSTANCE.getOutputPortOpenSetType_WindowPunctuationInputPort();

    /**
     * The meta object literal for the '<em><b>Final Punctuation Port Scope</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE =
        eINSTANCE.getOutputPortOpenSetType_FinalPunctuationPortScope();

    /**
     * The meta object literal for the '<em><b>Tuple Mutation Allowed</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED =
        eINSTANCE.getOutputPortOpenSetType_TupleMutationAllowed();

    /**
     * The meta object literal for the '<em><b>Output Assignment Port Scope</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE =
        eINSTANCE.getOutputPortOpenSetType_OutputAssignmentPortScope();

    /**
     * The meta object literal for the '<em><b>Allow Nested Custom Output Functions</b></em>'
     * attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS =
        eINSTANCE.getOutputPortOpenSetType_AllowNestedCustomOutputFunctions();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortSetTypeImpl <em>Output Port Set
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortSetTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortSetType()
     * @generated
     */
    EClass OUTPUT_PORT_SET_TYPE = eINSTANCE.getOutputPortSetType();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_SET_TYPE__CARDINALITY = eINSTANCE.getOutputPortSetType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_PORT_SET_TYPE__OPTIONAL = eINSTANCE.getOutputPortSetType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortsTypeImpl <em>Output Ports Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OutputPortsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getOutputPortsType()
     * @generated
     */
    EClass OUTPUT_PORTS_TYPE = eINSTANCE.getOutputPortsType();

    /**
     * The meta object literal for the '<em><b>Output Port Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET = eINSTANCE.getOutputPortsType_OutputPortSet();

    /**
     * The meta object literal for the '<em><b>Output Port Open Set</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET =
        eINSTANCE.getOutputPortsType_OutputPortOpenSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl <em>Parameters Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ParametersTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getParametersType()
     * @generated
     */
    EClass PARAMETERS_TYPE = eINSTANCE.getParametersType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PARAMETERS_TYPE__DESCRIPTION = eINSTANCE.getParametersType_Description();

    /**
     * The meta object literal for the '<em><b>Allow Any</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETERS_TYPE__ALLOW_ANY = eINSTANCE.getParametersType_AllowAny();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PARAMETERS_TYPE__PARAMETER = eINSTANCE.getParametersType_Parameter();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl <em>Parameter Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.ParameterTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getParameterType()
     * @generated
     */
    EClass PARAMETER_TYPE = eINSTANCE.getParameterType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__NAME = eINSTANCE.getParameterType_Name();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PARAMETER_TYPE__DESCRIPTION = eINSTANCE.getParameterType_Description();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__OPTIONAL = eINSTANCE.getParameterType_Optional();

    /**
     * The meta object literal for the '<em><b>Rewrite Allowed</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__REWRITE_ALLOWED = eINSTANCE.getParameterType_RewriteAllowed();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__EXPRESSION_MODE = eINSTANCE.getParameterType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__TYPE = eINSTANCE.getParameterType_Type();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__CARDINALITY = eINSTANCE.getParameterType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Port Scope</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PARAMETER_TYPE__PORT_SCOPE = eINSTANCE.getParameterType_PortScope();

    /**
     * The meta object literal for the '<em><b>Custom Output Function</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION =
        eINSTANCE.getParameterType_CustomOutputFunction();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.PortScopeTypeImpl <em>Port Scope Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.PortScopeTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getPortScopeType()
     * @generated
     */
    EClass PORT_SCOPE_TYPE = eINSTANCE.getPortScopeType();

    /**
     * The meta object literal for the '<em><b>Port</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PORT_SCOPE_TYPE__PORT = eINSTANCE.getPortScopeType_Port();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl <em>Spl Expression
     * Tree Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSplExpressionTreeType()
     * @generated
     */
    EClass SPL_EXPRESSION_TREE_TYPE = eINSTANCE.getSplExpressionTreeType();

    /**
     * The meta object literal for the '<em><b>Cpp Code</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_EXPRESSION_TREE_TYPE__CPP_CODE = eINSTANCE.getSplExpressionTreeType_CppCode();

    /**
     * The meta object literal for the '<em><b>Output</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_EXPRESSION_TREE_TYPE__OUTPUT = eINSTANCE.getSplExpressionTreeType_Output();

    /**
     * The meta object literal for the '<em><b>Param</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_EXPRESSION_TREE_TYPE__PARAM = eINSTANCE.getSplExpressionTreeType_Param();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType <em>Expression Mode Type</em>}'
     * enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getExpressionModeType()
     * @generated
     */
    EEnum EXPRESSION_MODE_TYPE = eINSTANCE.getExpressionModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType <em>Incremental
     * Compilation Strategy Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIncrementalCompilationStrategyType()
     * @generated
     */
    EEnum INCREMENTAL_COMPILATION_STRATEGY_TYPE = eINSTANCE.getIncrementalCompilationStrategyType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType <em>Java Op Expression Mode
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExpressionModeType()
     * @generated
     */
    EEnum JAVA_OP_EXPRESSION_MODE_TYPE = eINSTANCE.getJavaOpExpressionModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.MetricKindType <em>Metric Kind Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricKindType()
     * @generated
     */
    EEnum METRIC_KIND_TYPE = eINSTANCE.getMetricKindType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType <em>Single Threaded Context
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSingleThreadedContextType()
     * @generated
     */
    EEnum SINGLE_THREADED_CONTEXT_TYPE = eINSTANCE.getSingleThreadedContextType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType <em>Window Expression Mode
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowExpressionModeType()
     * @generated
     */
    EEnum WINDOW_EXPRESSION_MODE_TYPE = eINSTANCE.getWindowExpressionModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType <em>Windowing Mode Type</em>}'
     * enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
     * @see com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowingModeType()
     * @generated
     */
    EEnum WINDOWING_MODE_TYPE = eINSTANCE.getWindowingModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType <em>Window Punctuation
     * Input Mode Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationInputModeType()
     * @generated
     */
    EEnum WINDOW_PUNCTUATION_INPUT_MODE_TYPE = eINSTANCE.getWindowPunctuationInputModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType <em>Window
     * Punctuation Output Mode Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationOutputModeType()
     * @generated
     */
    EEnum WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE = eINSTANCE.getWindowPunctuationOutputModeType();

    /**
     * The meta object literal for the '<em>Expression Mode Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getExpressionModeTypeObject()
     * @generated
     */
    EDataType EXPRESSION_MODE_TYPE_OBJECT = eINSTANCE.getExpressionModeTypeObject();

    /**
     * The meta object literal for the '<em>Incremental Compilation Strategy Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getIncrementalCompilationStrategyTypeObject()
     * @generated
     */
    EDataType INCREMENTAL_COMPILATION_STRATEGY_TYPE_OBJECT =
        eINSTANCE.getIncrementalCompilationStrategyTypeObject();

    /**
     * The meta object literal for the '<em>Java Op Expression Mode Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getJavaOpExpressionModeTypeObject()
     * @generated
     */
    EDataType JAVA_OP_EXPRESSION_MODE_TYPE_OBJECT = eINSTANCE.getJavaOpExpressionModeTypeObject();

    /**
     * The meta object literal for the '<em>Metric Kind Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.MetricKindType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getMetricKindTypeObject()
     * @generated
     */
    EDataType METRIC_KIND_TYPE_OBJECT = eINSTANCE.getMetricKindTypeObject();

    /**
     * The meta object literal for the '<em>Single Threaded Context Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getSingleThreadedContextTypeObject()
     * @generated
     */
    EDataType SINGLE_THREADED_CONTEXT_TYPE_OBJECT = eINSTANCE.getSingleThreadedContextTypeObject();

    /**
     * The meta object literal for the '<em>Window Expression Mode Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowExpressionModeTypeObject()
     * @generated
     */
    EDataType WINDOW_EXPRESSION_MODE_TYPE_OBJECT = eINSTANCE.getWindowExpressionModeTypeObject();

    /**
     * The meta object literal for the '<em>Windowing Mode Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowingModeTypeObject()
     * @generated
     */
    EDataType WINDOWING_MODE_TYPE_OBJECT = eINSTANCE.getWindowingModeTypeObject();

    /**
     * The meta object literal for the '<em>Window Punctuation Input Mode Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationInputModeTypeObject()
     * @generated
     */
    EDataType WINDOW_PUNCTUATION_INPUT_MODE_TYPE_OBJECT =
        eINSTANCE.getWindowPunctuationInputModeTypeObject();

    /**
     * The meta object literal for the '<em>Window Punctuation Output Mode Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl#getWindowPunctuationOutputModeTypeObject()
     * @generated
     */
    EDataType WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE_OBJECT =
        eINSTANCE.getWindowPunctuationOutputModeTypeObject();
  }
} // OperatorPackage
