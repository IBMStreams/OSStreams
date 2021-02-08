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
 *   <li>each enum,
 *   <li>and each data type
 * </ul>
 *
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFactory
 * @model kind="package"
 * @generated
 */
public interface ToolkitPackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "toolkit";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/toolkit";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "toolkit";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  ToolkitPackage eINSTANCE = com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl <em>Element Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitElementType()
   * @generated
   */
  int TOOLKIT_ELEMENT_TYPE = 21;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_ELEMENT_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_ELEMENT_TYPE__NAME = 1;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX = 2;

  /**
   * The number of structural features of the '<em>Element Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT = 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl <em>Base Function Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getBaseFunctionType()
   * @generated
   */
  int BASE_FUNCTION_TYPE = 0;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Prototype</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__PROTOTYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__PARAMETER = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Return Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE__RETURN_TYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Base Function Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_FUNCTION_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl <em>Base Toolkit
   * Operator Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getBaseToolkitOperatorType()
   * @generated
   */
  int BASE_TOOLKIT_OPERATOR_TYPE = 1;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Images</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE__IMAGES = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Base Toolkit Operator Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CodeTemplateTypeImpl <em>Code Template Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CodeTemplateTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCodeTemplateType()
   * @generated
   */
  int CODE_TEMPLATE_TYPE = 2;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Template</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE__TEMPLATE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Code Template Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CODE_TEMPLATE_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl <em>Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getParameterType()
   * @generated
   */
  int PARAMETER_TYPE = 14;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__DEFAULT_VALUE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Mutable</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__MUTABLE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE__TYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PARAMETER_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl <em>Composite Op
   * Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeOpParameterType()
   * @generated
   */
  int COMPOSITE_OP_PARAMETER_TYPE = 3;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__DESCRIPTION = PARAMETER_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__NAME = PARAMETER_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__SAMPLE_URI_INDEX = PARAMETER_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__DEFAULT_VALUE = PARAMETER_TYPE__DEFAULT_VALUE;

  /**
   * The feature id for the '<em><b>Mutable</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__MUTABLE = PARAMETER_TYPE__MUTABLE;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__TYPE = PARAMETER_TYPE__TYPE;

  /**
   * The feature id for the '<em><b>Meta Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__META_TYPE = PARAMETER_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL = PARAMETER_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Composite Op Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PARAMETER_TYPE_FEATURE_COUNT = PARAMETER_TYPE_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl <em>Composite Op Port
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeOpPortType()
   * @generated
   */
  int COMPOSITE_OP_PORT_TYPE = 4;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Port Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE__PORT_INDEX = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE__TYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Composite Op Port Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OP_PORT_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl <em>Composite
   * Toolkit Operator Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeToolkitOperatorType()
   * @generated
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE = 5;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__DESCRIPTION = BASE_TOOLKIT_OPERATOR_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__NAME = BASE_TOOLKIT_OPERATOR_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__SAMPLE_URI_INDEX =
      BASE_TOOLKIT_OPERATOR_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Images</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__IMAGES = BASE_TOOLKIT_OPERATOR_TYPE__IMAGES;

  /**
   * The feature id for the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__PUBLIC = BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC;

  /**
   * The feature id for the '<em><b>Group</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Input Port</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Output Port</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Type</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 4;

  /**
   * The feature id for the '<em><b>Enum Literals</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 5;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 6;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 7;

  /**
   * The feature id for the '<em><b>Potential Main</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN =
      BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 8;

  /**
   * The feature id for the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 9;

  /**
   * The number of structural features of the '<em>Composite Toolkit Operator Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 10;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralsTypeImpl <em>Custom Literals
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCustomLiteralsType()
   * @generated
   */
  int CUSTOM_LITERALS_TYPE = 6;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERALS_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERALS_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERALS_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Value</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERALS_TYPE__VALUE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Custom Literals Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERALS_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralValueTypeImpl <em>Custom Literal Value
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralValueTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCustomLiteralValueType()
   * @generated
   */
  int CUSTOM_LITERAL_VALUE_TYPE = 7;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERAL_VALUE_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERAL_VALUE_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERAL_VALUE_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The number of structural features of the '<em>Custom Literal Value Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CUSTOM_LITERAL_VALUE_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.DocumentRootImpl
   * <em>Document Root</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getDocumentRoot()
   * @generated
   */
  int DOCUMENT_ROOT = 8;

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
   * The feature id for the '<em><b>Toolkit Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__TOOLKIT_MODEL = 3;

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
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl <em>Enum Literals Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumLiteralsType()
   * @generated
   */
  int ENUM_LITERALS_TYPE = 9;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Enum</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__ENUM = 1;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__COLUMN = 2;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__LINE = 3;

  /**
   * The feature id for the '<em><b>Static</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__STATIC = 4;

  /**
   * The feature id for the '<em><b>Type Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__TYPE_NAME = 5;

  /**
   * The feature id for the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE__URI_INDEX = 6;

  /**
   * The number of structural features of the '<em>Enum Literals Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ENUM_LITERALS_TYPE_FEATURE_COUNT = 7;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImagesTypeImpl
   * <em>Images Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImagesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getImagesType()
   * @generated
   */
  int IMAGES_TYPE = 10;

  /**
   * The feature id for the '<em><b>Image</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IMAGES_TYPE__IMAGE = 0;

  /**
   * The number of structural features of the '<em>Images Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IMAGES_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl
   * <em>Image Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getImageType()
   * @generated
   */
  int IMAGE_TYPE = 11;

  /**
   * The feature id for the '<em><b>Size</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IMAGE_TYPE__SIZE = 0;

  /**
   * The feature id for the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IMAGE_TYPE__URI_INDEX = 1;

  /**
   * The number of structural features of the '<em>Image Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IMAGE_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeImpl <em>Output Functions
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getOutputFunctionsType()
   * @generated
   */
  int OUTPUT_FUNCTIONS_TYPE = 12;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Function</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE__FUNCTION = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Output Functions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeTypeImpl <em>Output Functions
   * Type Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getOutputFunctionsTypeType()
   * @generated
   */
  int OUTPUT_FUNCTIONS_TYPE_TYPE = 13;

  /**
   * The feature id for the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_TYPE__DEFAULT_VALUE = 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_TYPE__NAME = 1;

  /**
   * The number of structural features of the '<em>Output Functions Type Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OUTPUT_FUNCTIONS_TYPE_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl <em>Primitive Port
   * Description Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitivePortDescriptionType()
   * @generated
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE = 18;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Max Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS = 1;

  /**
   * The feature id for the '<em><b>Min Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS = 2;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL = 3;

  /**
   * The number of structural features of the '<em>Primitive Port Description Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl <em>Primitive Op Input
   * Port Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpInputPortType()
   * @generated
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE = 15;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__DESCRIPTION = PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Max Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__MAX_NUM_PORTS = PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS;

  /**
   * The feature id for the '<em><b>Min Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__MIN_NUM_PORTS = PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__OPTIONAL = PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL;

  /**
   * The feature id for the '<em><b>Window Policy</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Window Punct Input Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Primitive Op Input Port Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_INPUT_PORT_TYPE_FEATURE_COUNT =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl <em>Primitive Op
   * Output Port Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpOutputPortType()
   * @generated
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE = 16;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__DESCRIPTION = PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Max Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__MAX_NUM_PORTS = PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS;

  /**
   * The feature id for the '<em><b>Min Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__MIN_NUM_PORTS = PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__OPTIONAL = PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL;

  /**
   * The feature id for the '<em><b>Output Functions Type</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Window Punct Output Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Primitive Op Output Port Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_OUTPUT_PORT_TYPE_FEATURE_COUNT =
      PRIMITIVE_PORT_DESCRIPTION_TYPE_FEATURE_COUNT + 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl <em>Primitive Op
   * Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpParameterType()
   * @generated
   */
  int PRIMITIVE_OP_PARAMETER_TYPE = 17;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__DESCRIPTION = PARAMETER_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__NAME = PARAMETER_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__SAMPLE_URI_INDEX = PARAMETER_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__DEFAULT_VALUE = PARAMETER_TYPE__DEFAULT_VALUE;

  /**
   * The feature id for the '<em><b>Mutable</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__MUTABLE = PARAMETER_TYPE__MUTABLE;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__TYPE = PARAMETER_TYPE__TYPE;

  /**
   * The feature id for the '<em><b>Port Scope</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE = PARAMETER_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Cardinality</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY = PARAMETER_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE = PARAMETER_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL = PARAMETER_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of structural features of the '<em>Primitive Op Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_OP_PARAMETER_TYPE_FEATURE_COUNT = PARAMETER_TYPE_FEATURE_COUNT + 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl <em>Primitive
   * Toolkit Operator Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveToolkitOperatorType()
   * @generated
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE = 19;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__DESCRIPTION = BASE_TOOLKIT_OPERATOR_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__NAME = BASE_TOOLKIT_OPERATOR_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__SAMPLE_URI_INDEX =
      BASE_TOOLKIT_OPERATOR_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Images</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__IMAGES = BASE_TOOLKIT_OPERATOR_TYPE__IMAGES;

  /**
   * The feature id for the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PUBLIC = BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC;

  /**
   * The feature id for the '<em><b>Group</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Custom Literals</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS =
      BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Output Functions</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS =
      BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Parameters Description</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION =
      BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 4;

  /**
   * The feature id for the '<em><b>Input Port</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 5;

  /**
   * The feature id for the '<em><b>Output Port</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 6;

  /**
   * The feature id for the '<em><b>Code Template</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 7;

  /**
   * The feature id for the '<em><b>Invalid</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 8;

  /**
   * The feature id for the '<em><b>Language</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 9;

  /**
   * The feature id for the '<em><b>Model Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX =
      BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 10;

  /**
   * The number of structural features of the '<em>Primitive Toolkit Operator Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int PRIMITIVE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT = BASE_TOOLKIT_OPERATOR_TYPE_FEATURE_COUNT + 11;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitBaseTypeImpl <em>Base Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitBaseTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitBaseType()
   * @generated
   */
  int TOOLKIT_BASE_TYPE = 20;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_BASE_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_BASE_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_BASE_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Uri Table</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_BASE_TYPE__URI_TABLE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Base Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_BASE_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl <em>Function Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitFunctionType()
   * @generated
   */
  int TOOLKIT_FUNCTION_TYPE = 22;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__DESCRIPTION = BASE_FUNCTION_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__NAME = BASE_FUNCTION_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__SAMPLE_URI_INDEX = BASE_FUNCTION_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Prototype</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__PROTOTYPE = BASE_FUNCTION_TYPE__PROTOTYPE;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__PARAMETER = BASE_FUNCTION_TYPE__PARAMETER;

  /**
   * The feature id for the '<em><b>Return Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__RETURN_TYPE = BASE_FUNCTION_TYPE__RETURN_TYPE;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__COLUMN = BASE_FUNCTION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__LINE = BASE_FUNCTION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Model Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX = BASE_FUNCTION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Native</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__NATIVE = BASE_FUNCTION_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__PUBLIC = BASE_FUNCTION_TYPE_FEATURE_COUNT + 4;

  /**
   * The feature id for the '<em><b>Stateful</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__STATEFUL = BASE_FUNCTION_TYPE_FEATURE_COUNT + 5;

  /**
   * The feature id for the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE__URI_INDEX = BASE_FUNCTION_TYPE_FEATURE_COUNT + 6;

  /**
   * The number of structural features of the '<em>Function Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_FUNCTION_TYPE_FEATURE_COUNT = BASE_FUNCTION_TYPE_FEATURE_COUNT + 7;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl <em>Model Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitModelType()
   * @generated
   */
  int TOOLKIT_MODEL_TYPE = 23;

  /**
   * The feature id for the '<em><b>Toolkit</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_MODEL_TYPE__TOOLKIT = 0;

  /**
   * The feature id for the '<em><b>Product Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_MODEL_TYPE__PRODUCT_VERSION = 1;

  /**
   * The number of structural features of the '<em>Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_MODEL_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl <em>Namespace Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitNamespaceType()
   * @generated
   */
  int TOOLKIT_NAMESPACE_TYPE = 24;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Group</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__GROUP = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Primitive Op</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Composite Op</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Function</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__FUNCTION = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Type</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__TYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 4;

  /**
   * The feature id for the '<em><b>Enum Literals</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 5;

  /**
   * The feature id for the '<em><b>Rel Path To Namespace</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 6;

  /**
   * The number of structural features of the '<em>Namespace Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_NAMESPACE_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 7;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl
   * <em>Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitType()
   * @generated
   */
  int TOOLKIT_TYPE = 25;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__DESCRIPTION = TOOLKIT_BASE_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__NAME = TOOLKIT_BASE_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__SAMPLE_URI_INDEX = TOOLKIT_BASE_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Uri Table</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__URI_TABLE = TOOLKIT_BASE_TYPE__URI_TABLE;

  /**
   * The feature id for the '<em><b>Group</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__GROUP = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Namespace</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__NAMESPACE = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Dependency</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__DEPENDENCY = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Sab Files</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__SAB_FILES = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Has NL Resources</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__HAS_NL_RESOURCES = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 4;

  /**
   * The feature id for the '<em><b>Required Product Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 5;

  /**
   * The feature id for the '<em><b>Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__URI = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 6;

  /**
   * The feature id for the '<em><b>Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE__VERSION = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 7;

  /**
   * The number of structural features of the '<em>Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_FEATURE_COUNT = TOOLKIT_BASE_TYPE_FEATURE_COUNT + 8;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl <em>Type Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitTypeType()
   * @generated
   */
  int TOOLKIT_TYPE_TYPE = 26;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__DESCRIPTION = TOOLKIT_ELEMENT_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__NAME = TOOLKIT_ELEMENT_TYPE__NAME;

  /**
   * The feature id for the '<em><b>Sample Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__SAMPLE_URI_INDEX = TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__COLUMN = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__LINE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Static</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__STATIC = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__TYPE = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE__URI_INDEX = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of structural features of the '<em>Type Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_TYPE_TYPE_FEATURE_COUNT = TOOLKIT_ELEMENT_TYPE_FEATURE_COUNT + 5;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.impl.UriTableTypeImpl
   * <em>Uri Table Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.UriTableTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getUriTableType()
   * @generated
   */
  int URI_TABLE_TYPE = 27;

  /**
   * The feature id for the '<em><b>Uri</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TABLE_TYPE__URI = 0;

  /**
   * The number of structural features of the '<em>Uri Table Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TABLE_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType <em>Composite Param Enum
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeParamEnumType()
   * @generated
   */
  int COMPOSITE_PARAM_ENUM_TYPE = 28;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType <em>Enum Expression Mode
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumExpressionModeType()
   * @generated
   */
  int ENUM_EXPRESSION_MODE_TYPE = 29;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * <em>Enum Window Policy Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPolicyType()
   * @generated
   */
  int ENUM_WINDOW_POLICY_TYPE = 30;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType <em>Enum Window Punct Input
   * Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctInputModeType()
   * @generated
   */
  int ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE = 31;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType <em>Enum Window Punct
   * Output Mode Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctOutputModeType()
   * @generated
   */
  int ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE = 32;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * <em>Language Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getLanguageType()
   * @generated
   */
  int LANGUAGE_TYPE = 33;

  /**
   * The meta object id for the '<em>Composite Param Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeParamEnumTypeObject()
   * @generated
   */
  int COMPOSITE_PARAM_ENUM_TYPE_OBJECT = 34;

  /**
   * The meta object id for the '<em>Enum Expression Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumExpressionModeTypeObject()
   * @generated
   */
  int ENUM_EXPRESSION_MODE_TYPE_OBJECT = 35;

  /**
   * The meta object id for the '<em>Enum Window Policy Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPolicyTypeObject()
   * @generated
   */
  int ENUM_WINDOW_POLICY_TYPE_OBJECT = 36;

  /**
   * The meta object id for the '<em>Enum Window Punct Input Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctInputModeTypeObject()
   * @generated
   */
  int ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE_OBJECT = 37;

  /**
   * The meta object id for the '<em>Enum Window Punct Output Mode Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctOutputModeTypeObject()
   * @generated
   */
  int ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE_OBJECT = 38;

  /**
   * The meta object id for the '<em>Language Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getLanguageTypeObject()
   * @generated
   */
  int LANGUAGE_TYPE_OBJECT = 39;

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType <em>Base Function Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Base Function Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType
   * @generated
   */
  EClass getBaseFunctionType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getPrototype <em>Prototype</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Prototype</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getPrototype()
   * @see #getBaseFunctionType()
   * @generated
   */
  EAttribute getBaseFunctionType_Prototype();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getParameter <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getParameter()
   * @see #getBaseFunctionType()
   * @generated
   */
  EReference getBaseFunctionType_Parameter();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getReturnType <em>Return Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Return Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType#getReturnType()
   * @see #getBaseFunctionType()
   * @generated
   */
  EAttribute getBaseFunctionType_ReturnType();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType <em>Base Toolkit Operator
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Base Toolkit Operator Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType
   * @generated
   */
  EClass getBaseToolkitOperatorType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#getImages <em>Images</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Images</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#getImages()
   * @see #getBaseToolkitOperatorType()
   * @generated
   */
  EReference getBaseToolkitOperatorType_Images();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic <em>Public</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Public</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType#isPublic()
   * @see #getBaseToolkitOperatorType()
   * @generated
   */
  EAttribute getBaseToolkitOperatorType_Public();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType <em>Code Template Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Code Template Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType
   * @generated
   */
  EClass getCodeTemplateType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType#getTemplate <em>Template</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Template</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType#getTemplate()
   * @see #getCodeTemplateType()
   * @generated
   */
  EAttribute getCodeTemplateType_Template();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType <em>Composite Op Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Op Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType
   * @generated
   */
  EClass getCompositeOpParameterType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType <em>Meta
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Meta Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType()
   * @see #getCompositeOpParameterType()
   * @generated
   */
  EAttribute getCompositeOpParameterType_MetaType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional()
   * @see #getCompositeOpParameterType()
   * @generated
   */
  EAttribute getCompositeOpParameterType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType <em>Composite Op Port Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Op Port Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType
   * @generated
   */
  EClass getCompositeOpPortType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex <em>Port Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Port Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getPortIndex()
   * @see #getCompositeOpPortType()
   * @generated
   */
  EAttribute getCompositeOpPortType_PortIndex();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType#getType()
   * @see #getCompositeOpPortType()
   * @generated
   */
  EAttribute getCompositeOpPortType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType <em>Composite Toolkit
   * Operator Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Toolkit Operator Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType
   * @generated
   */
  EClass getCompositeToolkitOperatorType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getGroup <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Group</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getGroup()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EAttribute getCompositeToolkitOperatorType_Group();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getParameter
   * <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getParameter()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EReference getCompositeToolkitOperatorType_Parameter();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getInputPort <em>Input
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Input Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getInputPort()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EReference getCompositeToolkitOperatorType_InputPort();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getOutputPort <em>Output
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getOutputPort()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EReference getCompositeToolkitOperatorType_OutputPort();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getType()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EReference getCompositeToolkitOperatorType_Type();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getEnumLiterals <em>Enum
   * Literals</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Enum Literals</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getEnumLiterals()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EReference getCompositeToolkitOperatorType_EnumLiterals();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn
   * <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getColumn()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EAttribute getCompositeToolkitOperatorType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getLine()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EAttribute getCompositeToolkitOperatorType_Line();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain
   * <em>Potential Main</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Potential Main</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#isPotentialMain()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EAttribute getCompositeToolkitOperatorType_PotentialMain();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex <em>Uri
   * Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType#getUriIndex()
   * @see #getCompositeToolkitOperatorType()
   * @generated
   */
  EAttribute getCompositeToolkitOperatorType_UriIndex();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType <em>Custom Literals Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Custom Literals Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType
   * @generated
   */
  EClass getCustomLiteralsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType#getValue()
   * @see #getCustomLiteralsType()
   * @generated
   */
  EReference getCustomLiteralsType_Value();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralValueType <em>Custom Literal Value
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Custom Literal Value Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralValueType
   * @generated
   */
  EClass getCustomLiteralValueType();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot
   * <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getToolkitModel <em>Toolkit Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Toolkit Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot#getToolkitModel()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_ToolkitModel();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType <em>Enum Literals Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Enum Literals Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType
   * @generated
   */
  EClass getEnumLiteralsType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getDescription()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EReference getEnumLiteralsType_Description();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getEnum <em>Enum</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Enum</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getEnum()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_Enum();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_Line();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic <em>Static</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Static</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_Static();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getTypeName <em>Type Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getTypeName()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_TypeName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex <em>Uri Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex()
   * @see #getEnumLiteralsType()
   * @generated
   */
  EAttribute getEnumLiteralsType_UriIndex();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType
   * <em>Images Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Images Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType
   * @generated
   */
  EClass getImagesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType#getImage <em>Image</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Image</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType#getImage()
   * @see #getImagesType()
   * @generated
   */
  EReference getImagesType_Image();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ImageType
   * <em>Image Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Image Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImageType
   * @generated
   */
  EClass getImageType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ImageType#getSize <em>Size</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Size</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImageType#getSize()
   * @see #getImageType()
   * @generated
   */
  EAttribute getImageType_Size();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ImageType#getUriIndex <em>Uri Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImageType#getUriIndex()
   * @see #getImageType()
   * @generated
   */
  EAttribute getImageType_UriIndex();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType <em>Output Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Functions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType
   * @generated
   */
  EClass getOutputFunctionsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType#getFunction <em>Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType#getFunction()
   * @see #getOutputFunctionsType()
   * @generated
   */
  EReference getOutputFunctionsType_Function();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType <em>Output Functions Type
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Output Functions Type Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType
   * @generated
   */
  EClass getOutputFunctionsTypeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType#getDefaultValue <em>Default
   * Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Default Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType#getDefaultValue()
   * @see #getOutputFunctionsTypeType()
   * @generated
   */
  EAttribute getOutputFunctionsTypeType_DefaultValue();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType#getName()
   * @see #getOutputFunctionsTypeType()
   * @generated
   */
  EAttribute getOutputFunctionsTypeType_Name();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType
   * <em>Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType
   * @generated
   */
  EClass getParameterType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getDefaultValue <em>Default Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Default Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getDefaultValue()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_DefaultValue();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable <em>Mutable</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Mutable</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Mutable();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getType()
   * @see #getParameterType()
   * @generated
   */
  EAttribute getParameterType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType <em>Primitive Op Input Port
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Primitive Op Input Port Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType
   * @generated
   */
  EClass getPrimitiveOpInputPortType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy <em>Window
   * Policy</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Policy</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPolicy()
   * @see #getPrimitiveOpInputPortType()
   * @generated
   */
  EAttribute getPrimitiveOpInputPortType_WindowPolicy();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode
   * <em>Window Punct Input Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punct Input Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType#getWindowPunctInputMode()
   * @see #getPrimitiveOpInputPortType()
   * @generated
   */
  EAttribute getPrimitiveOpInputPortType_WindowPunctInputMode();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType <em>Primitive Op Output Port
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Primitive Op Output Port Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType
   * @generated
   */
  EClass getPrimitiveOpOutputPortType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getOutputFunctionsType
   * <em>Output Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Functions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getOutputFunctionsType()
   * @see #getPrimitiveOpOutputPortType()
   * @generated
   */
  EReference getPrimitiveOpOutputPortType_OutputFunctionsType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode
   * <em>Expression Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getExpressionMode()
   * @see #getPrimitiveOpOutputPortType()
   * @generated
   */
  EAttribute getPrimitiveOpOutputPortType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode
   * <em>Window Punct Output Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Punct Output Mode</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType#getWindowPunctOutputMode()
   * @see #getPrimitiveOpOutputPortType()
   * @generated
   */
  EAttribute getPrimitiveOpOutputPortType_WindowPunctOutputMode();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType <em>Primitive Op Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Primitive Op Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType
   * @generated
   */
  EClass getPrimitiveOpParameterType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getPortScope <em>Port
   * Scope</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Port Scope</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getPortScope()
   * @see #getPrimitiveOpParameterType()
   * @generated
   */
  EAttribute getPrimitiveOpParameterType_PortScope();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality
   * <em>Cardinality</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Cardinality</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getCardinality()
   * @see #getPrimitiveOpParameterType()
   * @generated
   */
  EAttribute getPrimitiveOpParameterType_Cardinality();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode
   * <em>Expression Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#getExpressionMode()
   * @see #getPrimitiveOpParameterType()
   * @generated
   */
  EAttribute getPrimitiveOpParameterType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType#isOptional()
   * @see #getPrimitiveOpParameterType()
   * @generated
   */
  EAttribute getPrimitiveOpParameterType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType <em>Primitive Port
   * Description Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Primitive Port Description Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType
   * @generated
   */
  EClass getPrimitivePortDescriptionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getDescription()
   * @see #getPrimitivePortDescriptionType()
   * @generated
   */
  EReference getPrimitivePortDescriptionType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts <em>Max Num
   * Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Max Num Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts()
   * @see #getPrimitivePortDescriptionType()
   * @generated
   */
  EAttribute getPrimitivePortDescriptionType_MaxNumPorts();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts <em>Min Num
   * Ports</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Min Num Ports</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts()
   * @see #getPrimitivePortDescriptionType()
   * @generated
   */
  EAttribute getPrimitivePortDescriptionType_MinNumPorts();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional
   * <em>Optional</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Optional</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional()
   * @see #getPrimitivePortDescriptionType()
   * @generated
   */
  EAttribute getPrimitivePortDescriptionType_Optional();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType <em>Primitive Toolkit
   * Operator Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Primitive Toolkit Operator Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType
   * @generated
   */
  EClass getPrimitiveToolkitOperatorType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getGroup <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Group</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getGroup()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EAttribute getPrimitiveToolkitOperatorType_Group();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCustomLiterals
   * <em>Custom Literals</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Custom Literals</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCustomLiterals()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_CustomLiterals();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputFunctions
   * <em>Output Functions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Functions</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputFunctions()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_OutputFunctions();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParametersDescription
   * <em>Parameters Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameters Description</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParametersDescription()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_ParametersDescription();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParameter
   * <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getParameter()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_Parameter();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getInputPort <em>Input
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Input Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getInputPort()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_InputPort();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputPort <em>Output
   * Port</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Port</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getOutputPort()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_OutputPort();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCodeTemplate <em>Code
   * Template</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Code Template</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getCodeTemplate()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EReference getPrimitiveToolkitOperatorType_CodeTemplate();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid
   * <em>Invalid</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Invalid</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#isInvalid()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EAttribute getPrimitiveToolkitOperatorType_Invalid();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage
   * <em>Language</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Language</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getLanguage()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EAttribute getPrimitiveToolkitOperatorType_Language();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex <em>Model
   * Uri Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Model Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType#getModelUriIndex()
   * @see #getPrimitiveToolkitOperatorType()
   * @generated
   */
  EAttribute getPrimitiveToolkitOperatorType_ModelUriIndex();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType <em>Base Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Base Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType
   * @generated
   */
  EClass getToolkitBaseType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType#getUriTable <em>Uri Table</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Uri Table</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType#getUriTable()
   * @see #getToolkitBaseType()
   * @generated
   */
  EReference getToolkitBaseType_UriTable();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType <em>Element Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Element Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType
   * @generated
   */
  EClass getToolkitElementType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getDescription()
   * @see #getToolkitElementType()
   * @generated
   */
  EReference getToolkitElementType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getName()
   * @see #getToolkitElementType()
   * @generated
   */
  EAttribute getToolkitElementType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getSampleUriIndex <em>Sample Uri
   * Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Sample Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType#getSampleUriIndex()
   * @see #getToolkitElementType()
   * @generated
   */
  EAttribute getToolkitElementType_SampleUriIndex();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType <em>Function Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType
   * @generated
   */
  EClass getToolkitFunctionType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_Line();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex <em>Model Uri
   * Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Model Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_ModelUriIndex();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative <em>Native</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Native</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_Native();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic <em>Public</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Public</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_Public();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful <em>Stateful</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Stateful</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_Stateful();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex <em>Uri Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex()
   * @see #getToolkitFunctionType()
   * @generated
   */
  EAttribute getToolkitFunctionType_UriIndex();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType <em>Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType
   * @generated
   */
  EClass getToolkitModelType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getToolkit <em>Toolkit</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Toolkit</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getToolkit()
   * @see #getToolkitModelType()
   * @generated
   */
  EReference getToolkitModelType_Toolkit();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getProductVersion <em>Product
   * Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Product Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType#getProductVersion()
   * @see #getToolkitModelType()
   * @generated
   */
  EAttribute getToolkitModelType_ProductVersion();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType <em>Namespace Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Namespace Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType
   * @generated
   */
  EClass getToolkitNamespaceType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getGroup <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Group</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getGroup()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EAttribute getToolkitNamespaceType_Group();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getPrimitiveOp <em>Primitive
   * Op</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Primitive Op</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getPrimitiveOp()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EReference getToolkitNamespaceType_PrimitiveOp();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getCompositeOp <em>Composite
   * Op</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Composite Op</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getCompositeOp()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EReference getToolkitNamespaceType_CompositeOp();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getFunction <em>Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getFunction()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EReference getToolkitNamespaceType_Function();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getType()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EReference getToolkitNamespaceType_Type();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getEnumLiterals <em>Enum
   * Literals</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Enum Literals</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getEnumLiterals()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EReference getToolkitNamespaceType_EnumLiterals();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getRelPathToNamespace <em>Rel Path
   * To Namespace</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Rel Path To Namespace</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType#getRelPathToNamespace()
   * @see #getToolkitNamespaceType()
   * @generated
   */
  EAttribute getToolkitNamespaceType_RelPathToNamespace();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType
   * <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType
   * @generated
   */
  EClass getToolkitType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getGroup <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Group</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getGroup()
   * @see #getToolkitType()
   * @generated
   */
  EAttribute getToolkitType_Group();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getNamespace <em>Namespace</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Namespace</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getNamespace()
   * @see #getToolkitType()
   * @generated
   */
  EReference getToolkitType_Namespace();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getDependency <em>Dependency</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Dependency</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getDependency()
   * @see #getToolkitType()
   * @generated
   */
  EReference getToolkitType_Dependency();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getSabFiles <em>Sab Files</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Sab Files</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getSabFiles()
   * @see #getToolkitType()
   * @generated
   */
  EReference getToolkitType_SabFiles();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources <em>Has NL
   * Resources</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Has NL Resources</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources()
   * @see #getToolkitType()
   * @generated
   */
  EAttribute getToolkitType_HasNLResources();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getRequiredProductVersion <em>Required
   * Product Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Required Product Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getRequiredProductVersion()
   * @see #getToolkitType()
   * @generated
   */
  EAttribute getToolkitType_RequiredProductVersion();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getUri <em>Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getUri()
   * @see #getToolkitType()
   * @generated
   */
  EAttribute getToolkitType_Uri();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getVersion <em>Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getVersion()
   * @see #getToolkitType()
   * @generated
   */
  EAttribute getToolkitType_Version();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType <em>Type Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType
   * @generated
   */
  EClass getToolkitTypeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getColumn <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getColumn()
   * @see #getToolkitTypeType()
   * @generated
   */
  EAttribute getToolkitTypeType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getLine()
   * @see #getToolkitTypeType()
   * @generated
   */
  EAttribute getToolkitTypeType_Line();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#isStatic <em>Static</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Static</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#isStatic()
   * @see #getToolkitTypeType()
   * @generated
   */
  EAttribute getToolkitTypeType_Static();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getType()
   * @see #getToolkitTypeType()
   * @generated
   */
  EAttribute getToolkitTypeType_Type();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getUriIndex <em>Uri Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType#getUriIndex()
   * @see #getToolkitTypeType()
   * @generated
   */
  EAttribute getToolkitTypeType_UriIndex();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType
   * <em>Uri Table Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Uri Table Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType
   * @generated
   */
  EClass getUriTableType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType#getUri <em>Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType#getUri()
   * @see #getUriTableType()
   * @generated
   */
  EReference getUriTableType_Uri();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType <em>Composite Param Enum
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Composite Param Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @generated
   */
  EEnum getCompositeParamEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType <em>Enum Expression Mode
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Enum Expression Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @generated
   */
  EEnum getEnumExpressionModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType <em>Enum Window Policy Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Enum Window Policy Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @generated
   */
  EEnum getEnumWindowPolicyType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType <em>Enum Window Punct Input
   * Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Enum Window Punct Input Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @generated
   */
  EEnum getEnumWindowPunctInputModeType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType <em>Enum Window Punct
   * Output Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Enum Window Punct Output Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @generated
   */
  EEnum getEnumWindowPunctOutputModeType();

  /**
   * Returns the meta object for enum '{@link com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * <em>Language Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Language Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @generated
   */
  EEnum getLanguageType();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType <em>Composite Param Enum Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Composite Param Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType"
   *     extendedMetaData="name='compositeParamEnumType:Object' baseType='compositeParamEnumType'"
   * @generated
   */
  EDataType getCompositeParamEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType <em>Enum Expression Mode Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Enum Expression Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType"
   *     extendedMetaData="name='enumExpressionModeType:Object' baseType='enumExpressionModeType'"
   * @generated
   */
  EDataType getEnumExpressionModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType <em>Enum Window Policy Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Enum Window Policy Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType"
   *     extendedMetaData="name='enumWindowPolicyType:Object' baseType='enumWindowPolicyType'"
   * @generated
   */
  EDataType getEnumWindowPolicyTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType <em>Enum Window Punct Input
   * Mode Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Enum Window Punct Input Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType"
   *     extendedMetaData="name='enumWindowPunctInputModeType:Object'
   *     baseType='enumWindowPunctInputModeType'"
   * @generated
   */
  EDataType getEnumWindowPunctInputModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType <em>Enum Window Punct
   * Output Mode Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Enum Window Punct Output Mode Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType"
   *     extendedMetaData="name='enumWindowPunctOutputModeType:Object'
   *     baseType='enumWindowPunctOutputModeType'"
   * @generated
   */
  EDataType getEnumWindowPunctOutputModeTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType <em>Language Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Language Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType"
   *     extendedMetaData="name='languageType:Object' baseType='languageType'"
   * @generated
   */
  EDataType getLanguageTypeObject();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  ToolkitFactory getToolkitFactory();

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
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl <em>Base Function
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseFunctionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getBaseFunctionType()
     * @generated
     */
    EClass BASE_FUNCTION_TYPE = eINSTANCE.getBaseFunctionType();

    /**
     * The meta object literal for the '<em><b>Prototype</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute BASE_FUNCTION_TYPE__PROTOTYPE = eINSTANCE.getBaseFunctionType_Prototype();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference BASE_FUNCTION_TYPE__PARAMETER = eINSTANCE.getBaseFunctionType_Parameter();

    /**
     * The meta object literal for the '<em><b>Return Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute BASE_FUNCTION_TYPE__RETURN_TYPE = eINSTANCE.getBaseFunctionType_ReturnType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl <em>Base Toolkit
     * Operator Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.BaseToolkitOperatorTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getBaseToolkitOperatorType()
     * @generated
     */
    EClass BASE_TOOLKIT_OPERATOR_TYPE = eINSTANCE.getBaseToolkitOperatorType();

    /**
     * The meta object literal for the '<em><b>Images</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference BASE_TOOLKIT_OPERATOR_TYPE__IMAGES = eINSTANCE.getBaseToolkitOperatorType_Images();

    /**
     * The meta object literal for the '<em><b>Public</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC = eINSTANCE.getBaseToolkitOperatorType_Public();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CodeTemplateTypeImpl <em>Code Template
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CodeTemplateTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCodeTemplateType()
     * @generated
     */
    EClass CODE_TEMPLATE_TYPE = eINSTANCE.getCodeTemplateType();

    /**
     * The meta object literal for the '<em><b>Template</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CODE_TEMPLATE_TYPE__TEMPLATE = eINSTANCE.getCodeTemplateType_Template();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl <em>Composite Op
     * Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeOpParameterType()
     * @generated
     */
    EClass COMPOSITE_OP_PARAMETER_TYPE = eINSTANCE.getCompositeOpParameterType();

    /**
     * The meta object literal for the '<em><b>Meta Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_OP_PARAMETER_TYPE__META_TYPE =
        eINSTANCE.getCompositeOpParameterType_MetaType();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL =
        eINSTANCE.getCompositeOpParameterType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl <em>Composite Op Port
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeOpPortTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeOpPortType()
     * @generated
     */
    EClass COMPOSITE_OP_PORT_TYPE = eINSTANCE.getCompositeOpPortType();

    /**
     * The meta object literal for the '<em><b>Port Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_OP_PORT_TYPE__PORT_INDEX = eINSTANCE.getCompositeOpPortType_PortIndex();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_OP_PORT_TYPE__TYPE = eINSTANCE.getCompositeOpPortType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl <em>Composite
     * Toolkit Operator Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CompositeToolkitOperatorTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeToolkitOperatorType()
     * @generated
     */
    EClass COMPOSITE_TOOLKIT_OPERATOR_TYPE = eINSTANCE.getCompositeToolkitOperatorType();

    /**
     * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP =
        eINSTANCE.getCompositeToolkitOperatorType_Group();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER =
        eINSTANCE.getCompositeToolkitOperatorType_Parameter();

    /**
     * The meta object literal for the '<em><b>Input Port</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT =
        eINSTANCE.getCompositeToolkitOperatorType_InputPort();

    /**
     * The meta object literal for the '<em><b>Output Port</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT =
        eINSTANCE.getCompositeToolkitOperatorType_OutputPort();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE =
        eINSTANCE.getCompositeToolkitOperatorType_Type();

    /**
     * The meta object literal for the '<em><b>Enum Literals</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS =
        eINSTANCE.getCompositeToolkitOperatorType_EnumLiterals();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN =
        eINSTANCE.getCompositeToolkitOperatorType_Column();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE =
        eINSTANCE.getCompositeToolkitOperatorType_Line();

    /**
     * The meta object literal for the '<em><b>Potential Main</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN =
        eINSTANCE.getCompositeToolkitOperatorType_PotentialMain();

    /**
     * The meta object literal for the '<em><b>Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX =
        eINSTANCE.getCompositeToolkitOperatorType_UriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralsTypeImpl <em>Custom Literals
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCustomLiteralsType()
     * @generated
     */
    EClass CUSTOM_LITERALS_TYPE = eINSTANCE.getCustomLiteralsType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CUSTOM_LITERALS_TYPE__VALUE = eINSTANCE.getCustomLiteralsType_Value();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralValueTypeImpl <em>Custom Literal
     * Value Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.CustomLiteralValueTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCustomLiteralValueType()
     * @generated
     */
    EClass CUSTOM_LITERAL_VALUE_TYPE = eINSTANCE.getCustomLiteralValueType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.DocumentRootImpl <em>Document Root</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getDocumentRoot()
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
     * The meta object literal for the '<em><b>Toolkit Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__TOOLKIT_MODEL = eINSTANCE.getDocumentRoot_ToolkitModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl <em>Enum Literals
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.EnumLiteralsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumLiteralsType()
     * @generated
     */
    EClass ENUM_LITERALS_TYPE = eINSTANCE.getEnumLiteralsType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference ENUM_LITERALS_TYPE__DESCRIPTION = eINSTANCE.getEnumLiteralsType_Description();

    /**
     * The meta object literal for the '<em><b>Enum</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__ENUM = eINSTANCE.getEnumLiteralsType_Enum();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__COLUMN = eINSTANCE.getEnumLiteralsType_Column();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__LINE = eINSTANCE.getEnumLiteralsType_Line();

    /**
     * The meta object literal for the '<em><b>Static</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__STATIC = eINSTANCE.getEnumLiteralsType_Static();

    /**
     * The meta object literal for the '<em><b>Type Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__TYPE_NAME = eINSTANCE.getEnumLiteralsType_TypeName();

    /**
     * The meta object literal for the '<em><b>Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ENUM_LITERALS_TYPE__URI_INDEX = eINSTANCE.getEnumLiteralsType_UriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImagesTypeImpl <em>Images Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImagesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getImagesType()
     * @generated
     */
    EClass IMAGES_TYPE = eINSTANCE.getImagesType();

    /**
     * The meta object literal for the '<em><b>Image</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference IMAGES_TYPE__IMAGE = eINSTANCE.getImagesType_Image();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl <em>Image Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ImageTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getImageType()
     * @generated
     */
    EClass IMAGE_TYPE = eINSTANCE.getImageType();

    /**
     * The meta object literal for the '<em><b>Size</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute IMAGE_TYPE__SIZE = eINSTANCE.getImageType_Size();

    /**
     * The meta object literal for the '<em><b>Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute IMAGE_TYPE__URI_INDEX = eINSTANCE.getImageType_UriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeImpl <em>Output Functions
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getOutputFunctionsType()
     * @generated
     */
    EClass OUTPUT_FUNCTIONS_TYPE = eINSTANCE.getOutputFunctionsType();

    /**
     * The meta object literal for the '<em><b>Function</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OUTPUT_FUNCTIONS_TYPE__FUNCTION = eINSTANCE.getOutputFunctionsType_Function();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeTypeImpl <em>Output Functions
     * Type Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.OutputFunctionsTypeTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getOutputFunctionsTypeType()
     * @generated
     */
    EClass OUTPUT_FUNCTIONS_TYPE_TYPE = eINSTANCE.getOutputFunctionsTypeType();

    /**
     * The meta object literal for the '<em><b>Default Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_FUNCTIONS_TYPE_TYPE__DEFAULT_VALUE =
        eINSTANCE.getOutputFunctionsTypeType_DefaultValue();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OUTPUT_FUNCTIONS_TYPE_TYPE__NAME = eINSTANCE.getOutputFunctionsTypeType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl <em>Parameter Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ParameterTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getParameterType()
     * @generated
     */
    EClass PARAMETER_TYPE = eINSTANCE.getParameterType();

    /**
     * The meta object literal for the '<em><b>Default Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__DEFAULT_VALUE = eINSTANCE.getParameterType_DefaultValue();

    /**
     * The meta object literal for the '<em><b>Mutable</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__MUTABLE = eINSTANCE.getParameterType_Mutable();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PARAMETER_TYPE__TYPE = eINSTANCE.getParameterType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl <em>Primitive Op
     * Input Port Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpInputPortTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpInputPortType()
     * @generated
     */
    EClass PRIMITIVE_OP_INPUT_PORT_TYPE = eINSTANCE.getPrimitiveOpInputPortType();

    /**
     * The meta object literal for the '<em><b>Window Policy</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY =
        eINSTANCE.getPrimitiveOpInputPortType_WindowPolicy();

    /**
     * The meta object literal for the '<em><b>Window Punct Input Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE =
        eINSTANCE.getPrimitiveOpInputPortType_WindowPunctInputMode();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl <em>Primitive Op
     * Output Port Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpOutputPortTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpOutputPortType()
     * @generated
     */
    EClass PRIMITIVE_OP_OUTPUT_PORT_TYPE = eINSTANCE.getPrimitiveOpOutputPortType();

    /**
     * The meta object literal for the '<em><b>Output Functions Type</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE =
        eINSTANCE.getPrimitiveOpOutputPortType_OutputFunctionsType();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE =
        eINSTANCE.getPrimitiveOpOutputPortType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Window Punct Output Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE =
        eINSTANCE.getPrimitiveOpOutputPortType_WindowPunctOutputMode();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl <em>Primitive Op
     * Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveOpParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveOpParameterType()
     * @generated
     */
    EClass PRIMITIVE_OP_PARAMETER_TYPE = eINSTANCE.getPrimitiveOpParameterType();

    /**
     * The meta object literal for the '<em><b>Port Scope</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE =
        eINSTANCE.getPrimitiveOpParameterType_PortScope();

    /**
     * The meta object literal for the '<em><b>Cardinality</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY =
        eINSTANCE.getPrimitiveOpParameterType_Cardinality();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE =
        eINSTANCE.getPrimitiveOpParameterType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL =
        eINSTANCE.getPrimitiveOpParameterType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl <em>Primitive
     * Port Description Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitivePortDescriptionTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitivePortDescriptionType()
     * @generated
     */
    EClass PRIMITIVE_PORT_DESCRIPTION_TYPE = eINSTANCE.getPrimitivePortDescriptionType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION =
        eINSTANCE.getPrimitivePortDescriptionType_Description();

    /**
     * The meta object literal for the '<em><b>Max Num Ports</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS =
        eINSTANCE.getPrimitivePortDescriptionType_MaxNumPorts();

    /**
     * The meta object literal for the '<em><b>Min Num Ports</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS =
        eINSTANCE.getPrimitivePortDescriptionType_MinNumPorts();

    /**
     * The meta object literal for the '<em><b>Optional</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL =
        eINSTANCE.getPrimitivePortDescriptionType_Optional();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl <em>Primitive
     * Toolkit Operator Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.PrimitiveToolkitOperatorTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getPrimitiveToolkitOperatorType()
     * @generated
     */
    EClass PRIMITIVE_TOOLKIT_OPERATOR_TYPE = eINSTANCE.getPrimitiveToolkitOperatorType();

    /**
     * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP =
        eINSTANCE.getPrimitiveToolkitOperatorType_Group();

    /**
     * The meta object literal for the '<em><b>Custom Literals</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS =
        eINSTANCE.getPrimitiveToolkitOperatorType_CustomLiterals();

    /**
     * The meta object literal for the '<em><b>Output Functions</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS =
        eINSTANCE.getPrimitiveToolkitOperatorType_OutputFunctions();

    /**
     * The meta object literal for the '<em><b>Parameters Description</b></em>' containment
     * reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION =
        eINSTANCE.getPrimitiveToolkitOperatorType_ParametersDescription();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER =
        eINSTANCE.getPrimitiveToolkitOperatorType_Parameter();

    /**
     * The meta object literal for the '<em><b>Input Port</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT =
        eINSTANCE.getPrimitiveToolkitOperatorType_InputPort();

    /**
     * The meta object literal for the '<em><b>Output Port</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT =
        eINSTANCE.getPrimitiveToolkitOperatorType_OutputPort();

    /**
     * The meta object literal for the '<em><b>Code Template</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE =
        eINSTANCE.getPrimitiveToolkitOperatorType_CodeTemplate();

    /**
     * The meta object literal for the '<em><b>Invalid</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID =
        eINSTANCE.getPrimitiveToolkitOperatorType_Invalid();

    /**
     * The meta object literal for the '<em><b>Language</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE =
        eINSTANCE.getPrimitiveToolkitOperatorType_Language();

    /**
     * The meta object literal for the '<em><b>Model Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX =
        eINSTANCE.getPrimitiveToolkitOperatorType_ModelUriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitBaseTypeImpl <em>Base Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitBaseTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitBaseType()
     * @generated
     */
    EClass TOOLKIT_BASE_TYPE = eINSTANCE.getToolkitBaseType();

    /**
     * The meta object literal for the '<em><b>Uri Table</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_BASE_TYPE__URI_TABLE = eINSTANCE.getToolkitBaseType_UriTable();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl <em>Element Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitElementTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitElementType()
     * @generated
     */
    EClass TOOLKIT_ELEMENT_TYPE = eINSTANCE.getToolkitElementType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_ELEMENT_TYPE__DESCRIPTION = eINSTANCE.getToolkitElementType_Description();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_ELEMENT_TYPE__NAME = eINSTANCE.getToolkitElementType_Name();

    /**
     * The meta object literal for the '<em><b>Sample Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX =
        eINSTANCE.getToolkitElementType_SampleUriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl <em>Function Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFunctionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitFunctionType()
     * @generated
     */
    EClass TOOLKIT_FUNCTION_TYPE = eINSTANCE.getToolkitFunctionType();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__COLUMN = eINSTANCE.getToolkitFunctionType_Column();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__LINE = eINSTANCE.getToolkitFunctionType_Line();

    /**
     * The meta object literal for the '<em><b>Model Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX =
        eINSTANCE.getToolkitFunctionType_ModelUriIndex();

    /**
     * The meta object literal for the '<em><b>Native</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__NATIVE = eINSTANCE.getToolkitFunctionType_Native();

    /**
     * The meta object literal for the '<em><b>Public</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__PUBLIC = eINSTANCE.getToolkitFunctionType_Public();

    /**
     * The meta object literal for the '<em><b>Stateful</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__STATEFUL = eINSTANCE.getToolkitFunctionType_Stateful();

    /**
     * The meta object literal for the '<em><b>Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_FUNCTION_TYPE__URI_INDEX = eINSTANCE.getToolkitFunctionType_UriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl <em>Model Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitModelType()
     * @generated
     */
    EClass TOOLKIT_MODEL_TYPE = eINSTANCE.getToolkitModelType();

    /**
     * The meta object literal for the '<em><b>Toolkit</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_MODEL_TYPE__TOOLKIT = eINSTANCE.getToolkitModelType_Toolkit();

    /**
     * The meta object literal for the '<em><b>Product Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_MODEL_TYPE__PRODUCT_VERSION = eINSTANCE.getToolkitModelType_ProductVersion();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl <em>Namespace
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitNamespaceTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitNamespaceType()
     * @generated
     */
    EClass TOOLKIT_NAMESPACE_TYPE = eINSTANCE.getToolkitNamespaceType();

    /**
     * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_NAMESPACE_TYPE__GROUP = eINSTANCE.getToolkitNamespaceType_Group();

    /**
     * The meta object literal for the '<em><b>Primitive Op</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP =
        eINSTANCE.getToolkitNamespaceType_PrimitiveOp();

    /**
     * The meta object literal for the '<em><b>Composite Op</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP =
        eINSTANCE.getToolkitNamespaceType_CompositeOp();

    /**
     * The meta object literal for the '<em><b>Function</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_NAMESPACE_TYPE__FUNCTION = eINSTANCE.getToolkitNamespaceType_Function();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_NAMESPACE_TYPE__TYPE = eINSTANCE.getToolkitNamespaceType_Type();

    /**
     * The meta object literal for the '<em><b>Enum Literals</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS =
        eINSTANCE.getToolkitNamespaceType_EnumLiterals();

    /**
     * The meta object literal for the '<em><b>Rel Path To Namespace</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE =
        eINSTANCE.getToolkitNamespaceType_RelPathToNamespace();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl <em>Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitType()
     * @generated
     */
    EClass TOOLKIT_TYPE = eINSTANCE.getToolkitType();

    /**
     * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE__GROUP = eINSTANCE.getToolkitType_Group();

    /**
     * The meta object literal for the '<em><b>Namespace</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_TYPE__NAMESPACE = eINSTANCE.getToolkitType_Namespace();

    /**
     * The meta object literal for the '<em><b>Dependency</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_TYPE__DEPENDENCY = eINSTANCE.getToolkitType_Dependency();

    /**
     * The meta object literal for the '<em><b>Sab Files</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_TYPE__SAB_FILES = eINSTANCE.getToolkitType_SabFiles();

    /**
     * The meta object literal for the '<em><b>Has NL Resources</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE__HAS_NL_RESOURCES = eINSTANCE.getToolkitType_HasNLResources();

    /**
     * The meta object literal for the '<em><b>Required Product Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION =
        eINSTANCE.getToolkitType_RequiredProductVersion();

    /**
     * The meta object literal for the '<em><b>Uri</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE__URI = eINSTANCE.getToolkitType_Uri();

    /**
     * The meta object literal for the '<em><b>Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE__VERSION = eINSTANCE.getToolkitType_Version();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl <em>Type Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitTypeTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getToolkitTypeType()
     * @generated
     */
    EClass TOOLKIT_TYPE_TYPE = eINSTANCE.getToolkitTypeType();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE_TYPE__COLUMN = eINSTANCE.getToolkitTypeType_Column();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE_TYPE__LINE = eINSTANCE.getToolkitTypeType_Line();

    /**
     * The meta object literal for the '<em><b>Static</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE_TYPE__STATIC = eINSTANCE.getToolkitTypeType_Static();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE_TYPE__TYPE = eINSTANCE.getToolkitTypeType_Type();

    /**
     * The meta object literal for the '<em><b>Uri Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_TYPE_TYPE__URI_INDEX = eINSTANCE.getToolkitTypeType_UriIndex();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.impl.UriTableTypeImpl <em>Uri Table Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.UriTableTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getUriTableType()
     * @generated
     */
    EClass URI_TABLE_TYPE = eINSTANCE.getUriTableType();

    /**
     * The meta object literal for the '<em><b>Uri</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference URI_TABLE_TYPE__URI = eINSTANCE.getUriTableType_Uri();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType <em>Composite Param Enum
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeParamEnumType()
     * @generated
     */
    EEnum COMPOSITE_PARAM_ENUM_TYPE = eINSTANCE.getCompositeParamEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType <em>Enum Expression Mode
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumExpressionModeType()
     * @generated
     */
    EEnum ENUM_EXPRESSION_MODE_TYPE = eINSTANCE.getEnumExpressionModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType <em>Enum Window Policy
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPolicyType()
     * @generated
     */
    EEnum ENUM_WINDOW_POLICY_TYPE = eINSTANCE.getEnumWindowPolicyType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType <em>Enum Window Punct
     * Input Mode Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctInputModeType()
     * @generated
     */
    EEnum ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE = eINSTANCE.getEnumWindowPunctInputModeType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType <em>Enum Window Punct
     * Output Mode Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctOutputModeType()
     * @generated
     */
    EEnum ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE = eINSTANCE.getEnumWindowPunctOutputModeType();

    /**
     * The meta object literal for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
     * <em>Language Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getLanguageType()
     * @generated
     */
    EEnum LANGUAGE_TYPE = eINSTANCE.getLanguageType();

    /**
     * The meta object literal for the '<em>Composite Param Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getCompositeParamEnumTypeObject()
     * @generated
     */
    EDataType COMPOSITE_PARAM_ENUM_TYPE_OBJECT = eINSTANCE.getCompositeParamEnumTypeObject();

    /**
     * The meta object literal for the '<em>Enum Expression Mode Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumExpressionModeTypeObject()
     * @generated
     */
    EDataType ENUM_EXPRESSION_MODE_TYPE_OBJECT = eINSTANCE.getEnumExpressionModeTypeObject();

    /**
     * The meta object literal for the '<em>Enum Window Policy Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPolicyTypeObject()
     * @generated
     */
    EDataType ENUM_WINDOW_POLICY_TYPE_OBJECT = eINSTANCE.getEnumWindowPolicyTypeObject();

    /**
     * The meta object literal for the '<em>Enum Window Punct Input Mode Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctInputModeTypeObject()
     * @generated
     */
    EDataType ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE_OBJECT =
        eINSTANCE.getEnumWindowPunctInputModeTypeObject();

    /**
     * The meta object literal for the '<em>Enum Window Punct Output Mode Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getEnumWindowPunctOutputModeTypeObject()
     * @generated
     */
    EDataType ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE_OBJECT =
        eINSTANCE.getEnumWindowPunctOutputModeTypeObject();

    /**
     * The meta object literal for the '<em>Language Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl#getLanguageTypeObject()
     * @generated
     */
    EDataType LANGUAGE_TYPE_OBJECT = eINSTANCE.getLanguageTypeObject();
  }
} // ToolkitPackage
