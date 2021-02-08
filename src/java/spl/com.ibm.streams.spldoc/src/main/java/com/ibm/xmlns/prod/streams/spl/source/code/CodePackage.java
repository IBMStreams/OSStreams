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
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodeFactory
 * @model kind="package"
 * @generated
 */
public interface CodePackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "code";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/sourceCode";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "code";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  CodePackage eINSTANCE = com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl <em>Compilation Unit
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompilationUnitType()
   * @generated
   */
  int COMPILATION_UNIT_TYPE = 0;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE__SPL_DOC = 0;

  /**
   * The feature id for the '<em><b>Spl Namespace</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE__SPL_NAMESPACE = 1;

  /**
   * The feature id for the '<em><b>Use Directives</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE__USE_DIRECTIVES = 2;

  /**
   * The feature id for the '<em><b>Definitions</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE__DEFINITIONS = 3;

  /**
   * The number of structural features of the '<em>Compilation Unit Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Compilation Unit Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPILATION_UNIT_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl <em>Composite Body
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeBodyType()
   * @generated
   */
  int COMPOSITE_BODY_TYPE = 1;

  /**
   * The feature id for the '<em><b>Types</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE__TYPES = 0;

  /**
   * The feature id for the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE__PARAMETERS = 1;

  /**
   * The feature id for the '<em><b>Graph</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE__GRAPH = 2;

  /**
   * The feature id for the '<em><b>Configs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE__CONFIGS = 3;

  /**
   * The number of structural features of the '<em>Composite Body Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Composite Body Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_BODY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl <em>Extended
   * Source Location Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getExtendedSourceLocationType()
   * @generated
   */
  int EXTENDED_SOURCE_LOCATION_TYPE = 21;

  /**
   * The feature id for the '<em><b>End Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN = 0;

  /**
   * The feature id for the '<em><b>End Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE__END_LINE = 1;

  /**
   * The feature id for the '<em><b>Start Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN = 2;

  /**
   * The feature id for the '<em><b>Start Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE__START_LINE = 3;

  /**
   * The number of structural features of the '<em>Extended Source Location Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Extended Source Location Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXTENDED_SOURCE_LOCATION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl <em>Composite
   * Definition Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeDefinitionType()
   * @generated
   */
  int COMPOSITE_DEFINITION_TYPE = 2;

  /**
   * The feature id for the '<em><b>End Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__END_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN;

  /**
   * The feature id for the '<em><b>End Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__END_LINE = EXTENDED_SOURCE_LOCATION_TYPE__END_LINE;

  /**
   * The feature id for the '<em><b>Start Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__START_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN;

  /**
   * The feature id for the '<em><b>Start Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__START_LINE = EXTENDED_SOURCE_LOCATION_TYPE__START_LINE;

  /**
   * The feature id for the '<em><b>Composite Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Composite Body</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Composite Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE_FEATURE_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Composite Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_DEFINITION_TYPE_OPERATION_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeGraphTypeImpl <em>Composite Graph
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeGraphTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeGraphType()
   * @generated
   */
  int COMPOSITE_GRAPH_TYPE = 3;

  /**
   * The feature id for the '<em><b>Operator Invocation</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_GRAPH_TYPE__OPERATOR_INVOCATION = 0;

  /**
   * The number of structural features of the '<em>Composite Graph Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_GRAPH_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Graph Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_GRAPH_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceLocationTypeImpl <em>Source Location
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceLocationTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceLocationType()
   * @generated
   */
  int SOURCE_LOCATION_TYPE = 50;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_LOCATION_TYPE__COLUMN = 0;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_LOCATION_TYPE__LINE = 1;

  /**
   * The number of structural features of the '<em>Source Location Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_LOCATION_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Source Location Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_LOCATION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl <em>Composite Head
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeHeadType()
   * @generated
   */
  int COMPOSITE_HEAD_TYPE = 4;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__SPL_DOC = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__MODIFIERS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Inputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__INPUTS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Outputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__OUTPUTS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of structural features of the '<em>Composite Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 5;

  /**
   * The number of operations of the '<em>Composite Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_HEAD_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeInputsTypeImpl <em>Composite Inputs
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeInputsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeInputsType()
   * @generated
   */
  int COMPOSITE_INPUTS_TYPE = 5;

  /**
   * The feature id for the '<em><b>Iport</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_INPUTS_TYPE__IPORT = 0;

  /**
   * The number of structural features of the '<em>Composite Inputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_INPUTS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Inputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_INPUTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifiersTypeImpl <em>Composite
   * Modifiers Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifiersTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifiersType()
   * @generated
   */
  int COMPOSITE_MODIFIERS_TYPE = 6;

  /**
   * The feature id for the '<em><b>Modifier</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIERS_TYPE__MODIFIER = 0;

  /**
   * The number of structural features of the '<em>Composite Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifierTypeImpl <em>Composite
   * Modifier Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifierTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierType()
   * @generated
   */
  int COMPOSITE_MODIFIER_TYPE = 7;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIER_TYPE__NAME = 0;

  /**
   * The number of structural features of the '<em>Composite Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIER_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_MODIFIER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeOutputsTypeImpl <em>Composite Outputs
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeOutputsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeOutputsType()
   * @generated
   */
  int COMPOSITE_OUTPUTS_TYPE = 8;

  /**
   * The feature id for the '<em><b>Oport</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OUTPUTS_TYPE__OPORT = 0;

  /**
   * The number of structural features of the '<em>Composite Outputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OUTPUTS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Outputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_OUTPUTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl
   * <em>Composite Parameter Expression Mode Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeType()
   * @generated
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE = 9;

  /**
   * The feature id for the '<em><b>Mode</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE = 0;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE = 1;

  /**
   * The number of structural features of the '<em>Composite Parameter Expression Mode Type</em>'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Composite Parameter Expression Mode Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParametersTypeImpl <em>Composite
   * Parameters Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParametersTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParametersType()
   * @generated
   */
  int COMPOSITE_PARAMETERS_TYPE = 10;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETERS_TYPE__PARAMETER = 0;

  /**
   * The number of structural features of the '<em>Composite Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Composite Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl <em>Composite
   * Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterType()
   * @generated
   */
  int COMPOSITE_PARAMETER_TYPE = 11;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Expression Mode</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Composite Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Composite Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PARAMETER_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl <em>Composite Port
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositePortType()
   * @generated
   */
  int COMPOSITE_PORT_TYPE = 12;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE__INDEX = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE__TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Composite Port Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Composite Port Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_PORT_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeTypesTypeImpl <em>Composite Types
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeTypesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeTypesType()
   * @generated
   */
  int COMPOSITE_TYPES_TYPE = 13;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TYPES_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TYPES_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Type</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TYPES_TYPE__TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Composite Types Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TYPES_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of operations of the '<em>Composite Types Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int COMPOSITE_TYPES_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigOptionTypeImpl <em>Config Option
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigOptionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigOptionType()
   * @generated
   */
  int CONFIG_OPTION_TYPE = 14;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE__PARAMETER = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE__VALUE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Config Option Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Config Option Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_OPTION_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigsTypeImpl <em>Configs Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigsType()
   * @generated
   */
  int CONFIGS_TYPE = 15;

  /**
   * The feature id for the '<em><b>Config</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIGS_TYPE__CONFIG = 0;

  /**
   * The number of structural features of the '<em>Configs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIGS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Configs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIGS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigTypeImpl <em>Config Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigType()
   * @generated
   */
  int CONFIG_TYPE = 16;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Option</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE__OPTION = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Config Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Config Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigValueParameterTypeImpl <em>Config Value
   * Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigValueParameterTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigValueParameterType()
   * @generated
   */
  int CONFIG_VALUE_PARAMETER_TYPE = 17;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_VALUE_PARAMETER_TYPE__VALUE = 0;

  /**
   * The number of structural features of the '<em>Config Value Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_VALUE_PARAMETER_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Config Value Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int CONFIG_VALUE_PARAMETER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl <em>Definitions Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getDefinitionsType()
   * @generated
   */
  int DEFINITIONS_TYPE = 18;

  /**
   * The feature id for the '<em><b>Type Definition</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEFINITIONS_TYPE__TYPE_DEFINITION = 0;

  /**
   * The feature id for the '<em><b>Function Definition</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEFINITIONS_TYPE__FUNCTION_DEFINITION = 1;

  /**
   * The feature id for the '<em><b>Composite Definition</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEFINITIONS_TYPE__COMPOSITE_DEFINITION = 2;

  /**
   * The number of structural features of the '<em>Definitions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEFINITIONS_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Definitions Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEFINITIONS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.DocumentRootImpl <em>Document Root</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getDocumentRoot()
   * @generated
   */
  int DOCUMENT_ROOT = 19;

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
   * The feature id for the '<em><b>Source Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__SOURCE_MODEL = 3;

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
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.ExpressionTypeImpl <em>Expression Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ExpressionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getExpressionType()
   * @generated
   */
  int EXPRESSION_TYPE = 20;

  /**
   * The feature id for the '<em><b>Expr</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXPRESSION_TYPE__EXPR = 0;

  /**
   * The number of structural features of the '<em>Expression Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXPRESSION_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Expression Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXPRESSION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl <em>Function
   * Definition Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionDefinitionType()
   * @generated
   */
  int FUNCTION_DEFINITION_TYPE = 22;

  /**
   * The feature id for the '<em><b>End Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__END_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN;

  /**
   * The feature id for the '<em><b>End Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__END_LINE = EXTENDED_SOURCE_LOCATION_TYPE__END_LINE;

  /**
   * The feature id for the '<em><b>Start Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__START_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN;

  /**
   * The feature id for the '<em><b>Start Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__START_LINE = EXTENDED_SOURCE_LOCATION_TYPE__START_LINE;

  /**
   * The feature id for the '<em><b>Function Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Function Body</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE__FUNCTION_BODY = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Function Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE_FEATURE_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Function Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_DEFINITION_TYPE_OPERATION_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionHeadTypeImpl <em>Function Head
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionHeadTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionHeadType()
   * @generated
   */
  int FUNCTION_HEAD_TYPE = 23;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__SPL_DOC = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__MODIFIERS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__PARAMETERS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Return Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE__RETURN_TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of structural features of the '<em>Function Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 5;

  /**
   * The number of operations of the '<em>Function Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_HEAD_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifiersTypeImpl <em>Function
   * Modifiers Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifiersTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifiersType()
   * @generated
   */
  int FUNCTION_MODIFIERS_TYPE = 24;

  /**
   * The feature id for the '<em><b>Modifier</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIERS_TYPE__MODIFIER = 0;

  /**
   * The number of structural features of the '<em>Function Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Function Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifierTypeImpl <em>Function Modifier
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifierTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierType()
   * @generated
   */
  int FUNCTION_MODIFIER_TYPE = 25;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIER_TYPE__NAME = 0;

  /**
   * The number of structural features of the '<em>Function Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIER_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Function Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_MODIFIER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifiersTypeImpl <em>Function
   * Parameter Modifiers Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifiersTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifiersType()
   * @generated
   */
  int FUNCTION_PARAMETER_MODIFIERS_TYPE = 26;

  /**
   * The feature id for the '<em><b>Modifier</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIERS_TYPE__MODIFIER = 0;

  /**
   * The number of structural features of the '<em>Function Parameter Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Function Parameter Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifierTypeImpl <em>Function
   * Parameter Modifier Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifierTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierType()
   * @generated
   */
  int FUNCTION_PARAMETER_MODIFIER_TYPE = 27;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIER_TYPE__NAME = 0;

  /**
   * The number of structural features of the '<em>Function Parameter Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIER_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Function Parameter Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_MODIFIER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParametersTypeImpl <em>Function
   * Parameters Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParametersTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParametersType()
   * @generated
   */
  int FUNCTION_PARAMETERS_TYPE = 28;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETERS_TYPE__PARAMETER = 0;

  /**
   * The number of structural features of the '<em>Function Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Function Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterTypeImpl <em>Function
   * Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterType()
   * @generated
   */
  int FUNCTION_PARAMETER_TYPE = 29;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE__MODIFIERS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE__TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Function Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Function Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int FUNCTION_PARAMETER_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnProcessTypeImpl <em>On Process Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnProcessTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnProcessType()
   * @generated
   */
  int ON_PROCESS_TYPE = 30;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PROCESS_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PROCESS_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The number of structural features of the '<em>On Process Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PROCESS_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of operations of the '<em>On Process Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PROCESS_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnPunctTypeImpl <em>On Punct Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnPunctTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnPunctType()
   * @generated
   */
  int ON_PUNCT_TYPE = 31;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PUNCT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PUNCT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Port Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PUNCT_TYPE__PORT_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>On Punct Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PUNCT_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of operations of the '<em>On Punct Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_PUNCT_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnTupleTypeImpl <em>On Tuple Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnTupleTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnTupleType()
   * @generated
   */
  int ON_TUPLE_TYPE = 32;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_TUPLE_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_TUPLE_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Port Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_TUPLE_TYPE__PORT_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>On Tuple Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_TUPLE_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of operations of the '<em>On Tuple Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int ON_TUPLE_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;
  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl
   * <em>Operator Invocation Attribute Assignment Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationAttributeAssignmentType()
   * @generated
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE = 33;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Value</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Output Function</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION =
      SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Operator Invocation Attribute Assignment
   * Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE_FEATURE_COUNT =
      SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Operator Invocation Attribute Assignment Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE_OPERATION_COUNT =
      SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl <em>Operator
   * Invocation Body Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationBodyType()
   * @generated
   */
  int OPERATOR_INVOCATION_BODY_TYPE = 34;

  /**
   * The feature id for the '<em><b>Logic</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE__LOGIC = 0;

  /**
   * The feature id for the '<em><b>Windows</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE__WINDOWS = 1;

  /**
   * The feature id for the '<em><b>Parameters</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS = 2;

  /**
   * The feature id for the '<em><b>Output Assignments</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS = 3;

  /**
   * The feature id for the '<em><b>Configs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE__CONFIGS = 4;

  /**
   * The number of structural features of the '<em>Operator Invocation Body Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE_FEATURE_COUNT = 5;

  /**
   * The number of operations of the '<em>Operator Invocation Body Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_BODY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl <em>Operator
   * Invocation Head Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationHeadType()
   * @generated
   */
  int OPERATOR_INVOCATION_HEAD_TYPE = 35;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Outputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Inputs</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__INPUTS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Invocation Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Operator Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of structural features of the '<em>Operator Invocation Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 5;

  /**
   * The number of operations of the '<em>Operator Invocation Head Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_HEAD_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputStreamTypeImpl
   * <em>Operator Invocation Input Stream Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputStreamTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputStreamType()
   * @generated
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE = 36;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Input Stream Type</em>'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of operations of the '<em>Operator Invocation Input Stream Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_STREAM_TYPE_OPERATION_COUNT =
      SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputsTypeImpl <em>Operator
   * Invocation Inputs Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputsType()
   * @generated
   */
  int OPERATOR_INVOCATION_INPUTS_TYPE = 37;

  /**
   * The feature id for the '<em><b>Input</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUTS_TYPE__INPUT = 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Inputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUTS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Operator Invocation Inputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl <em>Operator
   * Invocation Input Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputType()
   * @generated
   */
  int OPERATOR_INVOCATION_INPUT_TYPE = 38;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Istream</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE__ALIAS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE__INDEX = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Operator Invocation Input Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Operator Invocation Input Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_INPUT_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl <em>Operator
   * Invocation Logic Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationLogicType()
   * @generated
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE = 39;

  /**
   * The feature id for the '<em><b>On Process</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS = 0;

  /**
   * The feature id for the '<em><b>On Tuple</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE = 1;

  /**
   * The feature id for the '<em><b>On Punct</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT = 2;

  /**
   * The feature id for the '<em><b>Has State</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE = 3;

  /**
   * The number of structural features of the '<em>Operator Invocation Logic Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Operator Invocation Logic Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_LOGIC_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentsTypeImpl
   * <em>Operator Invocation Output Assignments Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputAssignmentsType()
   * @generated
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE = 40;

  /**
   * The feature id for the '<em><b>Output Assignment</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE__OUTPUT_ASSIGNMENT = 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Output Assignments Type</em>'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Operator Invocation Output Assignments Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl
   * <em>Operator Invocation Output Assignment Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputAssignmentType()
   * @generated
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE = 41;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Attribute Assignment</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT =
      SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Port Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME =
      SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Operator Invocation Output Assignment Type</em>'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE_FEATURE_COUNT =
      SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Operator Invocation Output Assignment Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE_OPERATION_COUNT =
      SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputsTypeImpl <em>Operator
   * Invocation Outputs Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputsType()
   * @generated
   */
  int OPERATOR_INVOCATION_OUTPUTS_TYPE = 42;

  /**
   * The feature id for the '<em><b>Output</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUTS_TYPE__OUTPUT = 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Outputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUTS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Operator Invocation Outputs Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUTS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputTypeImpl <em>Operator
   * Invocation Output Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputType()
   * @generated
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE = 43;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Alias</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__ALIAS = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__INDEX = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Stream Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__STREAM_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE__TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of structural features of the '<em>Operator Invocation Output Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of operations of the '<em>Operator Invocation Output Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_OUTPUT_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParametersTypeImpl
   * <em>Operator Invocation Parameters Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParametersTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationParametersType()
   * @generated
   */
  int OPERATOR_INVOCATION_PARAMETERS_TYPE = 44;

  /**
   * The feature id for the '<em><b>Parameter</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETERS_TYPE__PARAMETER = 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Operator Invocation Parameters Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParameterTypeImpl
   * <em>Operator Invocation Parameter Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParameterTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationParameterType()
   * @generated
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE = 45;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Value</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE__VALUE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Operator Invocation Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Operator Invocation Parameter Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_PARAMETER_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl <em>Operator
   * Invocation Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationType()
   * @generated
   */
  int OPERATOR_INVOCATION_TYPE = 46;

  /**
   * The feature id for the '<em><b>Operator Invocation Head</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD = 0;

  /**
   * The feature id for the '<em><b>Operator Invocation Body</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY = 1;

  /**
   * The number of structural features of the '<em>Operator Invocation Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Operator Invocation Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowsTypeImpl <em>Operator
   * Invocation Windows Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowsTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationWindowsType()
   * @generated
   */
  int OPERATOR_INVOCATION_WINDOWS_TYPE = 47;

  /**
   * The feature id for the '<em><b>Window</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOWS_TYPE__WINDOW = 0;

  /**
   * The number of structural features of the '<em>Operator Invocation Windows Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOWS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Operator Invocation Windows Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOWS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl <em>Operator
   * Invocation Window Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationWindowType()
   * @generated
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE = 48;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Eviction Policy</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Trigger Policy</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Partitioned</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Port Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The feature id for the '<em><b>Window Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of structural features of the '<em>Operator Invocation Window Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 5;

  /**
   * The number of operations of the '<em>Operator Invocation Window Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int OPERATOR_INVOCATION_WINDOW_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl <em>Source File Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceFileType()
   * @generated
   */
  int SOURCE_FILE_TYPE = 49;

  /**
   * The feature id for the '<em><b>Compilation Unit</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_FILE_TYPE__COMPILATION_UNIT = 0;

  /**
   * The feature id for the '<em><b>Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_FILE_TYPE__URI = 1;

  /**
   * The number of structural features of the '<em>Source File Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_FILE_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Source File Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_FILE_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceModelTypeImpl <em>Source Model
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceModelType()
   * @generated
   */
  int SOURCE_MODEL_TYPE = 51;

  /**
   * The feature id for the '<em><b>Source File</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_MODEL_TYPE__SOURCE_FILE = 0;

  /**
   * The number of structural features of the '<em>Source Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_MODEL_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Source Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SOURCE_MODEL_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocDescriptionTypeImpl <em>Spl Doc
   * Description Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocDescriptionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocDescriptionType()
   * @generated
   */
  int SPL_DOC_DESCRIPTION_TYPE = 53;

  /**
   * The feature id for the '<em><b>Description</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_DESCRIPTION_TYPE__DESCRIPTION = 0;

  /**
   * The number of structural features of the '<em>Spl Doc Description Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_DESCRIPTION_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Spl Doc Description Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_DESCRIPTION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocAnnotationTypeImpl <em>Spl Doc Annotation
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocAnnotationTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocAnnotationType()
   * @generated
   */
  int SPL_DOC_ANNOTATION_TYPE = 52;

  /**
   * The feature id for the '<em><b>Description</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_ANNOTATION_TYPE__DESCRIPTION = SPL_DOC_DESCRIPTION_TYPE__DESCRIPTION;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_ANNOTATION_TYPE__NAME = SPL_DOC_DESCRIPTION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Target</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_ANNOTATION_TYPE__TARGET = SPL_DOC_DESCRIPTION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Spl Doc Annotation Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_ANNOTATION_TYPE_FEATURE_COUNT = SPL_DOC_DESCRIPTION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Spl Doc Annotation Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_ANNOTATION_TYPE_OPERATION_COUNT = SPL_DOC_DESCRIPTION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocTypeImpl <em>Spl Doc Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocType()
   * @generated
   */
  int SPL_DOC_TYPE = 54;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Annotation</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_TYPE__ANNOTATION = 1;

  /**
   * The number of structural features of the '<em>Spl Doc Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Spl Doc Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_DOC_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplNamespaceTypeImpl <em>Spl Namespace
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplNamespaceTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplNamespaceType()
   * @generated
   */
  int SPL_NAMESPACE_TYPE = 55;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE__SPL_DOC = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE__NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Spl Namespace Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Spl Namespace Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SPL_NAMESPACE_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeDefinitionTypeImpl <em>Type Definition
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeDefinitionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeDefinitionType()
   * @generated
   */
  int TYPE_DEFINITION_TYPE = 56;

  /**
   * The feature id for the '<em><b>End Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__END_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN;

  /**
   * The feature id for the '<em><b>End Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__END_LINE = EXTENDED_SOURCE_LOCATION_TYPE__END_LINE;

  /**
   * The feature id for the '<em><b>Start Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__START_COLUMN = EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN;

  /**
   * The feature id for the '<em><b>Start Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__START_LINE = EXTENDED_SOURCE_LOCATION_TYPE__START_LINE;

  /**
   * The feature id for the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__SPL_DOC = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__MODIFIERS = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__NAME = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE__VALUE = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of structural features of the '<em>Type Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE_FEATURE_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_FEATURE_COUNT + 4;

  /**
   * The number of operations of the '<em>Type Definition Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_DEFINITION_TYPE_OPERATION_COUNT = EXTENDED_SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifiersTypeImpl <em>Type Modifiers
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifiersTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifiersType()
   * @generated
   */
  int TYPE_MODIFIERS_TYPE = 57;

  /**
   * The feature id for the '<em><b>Modifier</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIERS_TYPE__MODIFIER = 0;

  /**
   * The number of structural features of the '<em>Type Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIERS_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Type Modifiers Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIERS_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifierTypeImpl <em>Type Modifier
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifierTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierType()
   * @generated
   */
  int TYPE_MODIFIER_TYPE = 58;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIER_TYPE__NAME = 0;

  /**
   * The number of structural features of the '<em>Type Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIER_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Type Modifier Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TYPE_MODIFIER_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectivesTypeImpl <em>Use Directives
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectivesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getUseDirectivesType()
   * @generated
   */
  int USE_DIRECTIVES_TYPE = 59;

  /**
   * The feature id for the '<em><b>Use Directive</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVES_TYPE__USE_DIRECTIVE = 0;

  /**
   * The number of structural features of the '<em>Use Directives Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVES_TYPE_FEATURE_COUNT = 1;

  /**
   * The number of operations of the '<em>Use Directives Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVES_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl <em>Use Directive
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getUseDirectiveType()
   * @generated
   */
  int USE_DIRECTIVE_TYPE = 60;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Namespace Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE__NAMESPACE_NAME = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Tail</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE__TAIL = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The number of structural features of the '<em>Use Directive Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of operations of the '<em>Use Directive Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int USE_DIRECTIVE_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl <em>Window Policy
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyType()
   * @generated
   */
  int WINDOW_POLICY_TYPE = 61;

  /**
   * The feature id for the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE__COLUMN = SOURCE_LOCATION_TYPE__COLUMN;

  /**
   * The feature id for the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE__LINE = SOURCE_LOCATION_TYPE__LINE;

  /**
   * The feature id for the '<em><b>Attribute</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE__ATTRIBUTE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 0;

  /**
   * The feature id for the '<em><b>Kind</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE__KIND = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 1;

  /**
   * The feature id for the '<em><b>Size</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE__SIZE = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 2;

  /**
   * The number of structural features of the '<em>Window Policy Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE_FEATURE_COUNT = SOURCE_LOCATION_TYPE_FEATURE_COUNT + 3;

  /**
   * The number of operations of the '<em>Window Policy Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int WINDOW_POLICY_TYPE_OPERATION_COUNT = SOURCE_LOCATION_TYPE_OPERATION_COUNT + 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType <em>Composite Modifier
   * Enum Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierEnumType()
   * @generated
   */
  int COMPOSITE_MODIFIER_ENUM_TYPE = 62;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * <em>Composite Parameter Expression Mode Enum Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeEnumType()
   * @generated
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE = 63;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType <em>Function Modifier Enum
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierEnumType()
   * @generated
   */
  int FUNCTION_MODIFIER_ENUM_TYPE = 64;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType <em>Function
   * Parameter Modifier Enum Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierEnumType()
   * @generated
   */
  int FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE = 65;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType <em>Type Modifier Enum
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierEnumType()
   * @generated
   */
  int TYPE_MODIFIER_ENUM_TYPE = 66;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType <em>Window Policy Kind Enum
   * Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyKindEnumType()
   * @generated
   */
  int WINDOW_POLICY_KIND_ENUM_TYPE = 67;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType <em>Window Type Enum Type</em>}'
   * enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowTypeEnumType()
   * @generated
   */
  int WINDOW_TYPE_ENUM_TYPE = 68;

  /**
   * The meta object id for the '<em>Composite Modifier Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierEnumTypeObject()
   * @generated
   */
  int COMPOSITE_MODIFIER_ENUM_TYPE_OBJECT = 69;

  /**
   * The meta object id for the '<em>Composite Parameter Expression Mode Enum Type Object</em>' data
   * type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeEnumTypeObject()
   * @generated
   */
  int COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE_OBJECT = 70;

  /**
   * The meta object id for the '<em>Function Modifier Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierEnumTypeObject()
   * @generated
   */
  int FUNCTION_MODIFIER_ENUM_TYPE_OBJECT = 71;

  /**
   * The meta object id for the '<em>Function Parameter Modifier Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierEnumTypeObject()
   * @generated
   */
  int FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE_OBJECT = 72;

  /**
   * The meta object id for the '<em>Type Modifier Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierEnumTypeObject()
   * @generated
   */
  int TYPE_MODIFIER_ENUM_TYPE_OBJECT = 73;

  /**
   * The meta object id for the '<em>Window Policy Kind Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyKindEnumTypeObject()
   * @generated
   */
  int WINDOW_POLICY_KIND_ENUM_TYPE_OBJECT = 74;

  /**
   * The meta object id for the '<em>Window Type Enum Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowTypeEnumTypeObject()
   * @generated
   */
  int WINDOW_TYPE_ENUM_TYPE_OBJECT = 75;

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType <em>Compilation Unit
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Compilation Unit Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType
   * @generated
   */
  EClass getCompilationUnitType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplDoc <em>Spl Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplDoc()
   * @see #getCompilationUnitType()
   * @generated
   */
  EReference getCompilationUnitType_SplDoc();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplNamespace <em>Spl
   * Namespace</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Namespace</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getSplNamespace()
   * @see #getCompilationUnitType()
   * @generated
   */
  EReference getCompilationUnitType_SplNamespace();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getUseDirectives <em>Use
   * Directives</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Use Directives</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getUseDirectives()
   * @see #getCompilationUnitType()
   * @generated
   */
  EReference getCompilationUnitType_UseDirectives();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getDefinitions
   * <em>Definitions</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Definitions</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType#getDefinitions()
   * @see #getCompilationUnitType()
   * @generated
   */
  EReference getCompilationUnitType_Definitions();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType <em>Composite Body Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Body Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType
   * @generated
   */
  EClass getCompositeBodyType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getTypes <em>Types</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Types</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getTypes()
   * @see #getCompositeBodyType()
   * @generated
   */
  EReference getCompositeBodyType_Types();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getParameters
   * <em>Parameters</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Parameters</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getParameters()
   * @see #getCompositeBodyType()
   * @generated
   */
  EReference getCompositeBodyType_Parameters();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getGraph <em>Graph</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Graph</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getGraph()
   * @see #getCompositeBodyType()
   * @generated
   */
  EReference getCompositeBodyType_Graph();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getConfigs <em>Configs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Configs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType#getConfigs()
   * @see #getCompositeBodyType()
   * @generated
   */
  EReference getCompositeBodyType_Configs();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType <em>Composite Definition
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Definition Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType
   * @generated
   */
  EClass getCompositeDefinitionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeHead
   * <em>Composite Head</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Composite Head</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeHead()
   * @see #getCompositeDefinitionType()
   * @generated
   */
  EReference getCompositeDefinitionType_CompositeHead();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeBody
   * <em>Composite Body</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Composite Body</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType#getCompositeBody()
   * @see #getCompositeDefinitionType()
   * @generated
   */
  EReference getCompositeDefinitionType_CompositeBody();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType <em>Composite Graph Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Graph Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType
   * @generated
   */
  EClass getCompositeGraphType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType#getOperatorInvocation
   * <em>Operator Invocation</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Operator Invocation</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType#getOperatorInvocation()
   * @see #getCompositeGraphType()
   * @generated
   */
  EReference getCompositeGraphType_OperatorInvocation();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType <em>Composite Head Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Head Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType
   * @generated
   */
  EClass getCompositeHeadType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getSplDoc <em>Spl Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getSplDoc()
   * @see #getCompositeHeadType()
   * @generated
   */
  EReference getCompositeHeadType_SplDoc();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getModifiers <em>Modifiers</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Modifiers</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getModifiers()
   * @see #getCompositeHeadType()
   * @generated
   */
  EReference getCompositeHeadType_Modifiers();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getInputs <em>Inputs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Inputs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getInputs()
   * @see #getCompositeHeadType()
   * @generated
   */
  EReference getCompositeHeadType_Inputs();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getOutputs <em>Outputs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Outputs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getOutputs()
   * @see #getCompositeHeadType()
   * @generated
   */
  EReference getCompositeHeadType_Outputs();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType#getName()
   * @see #getCompositeHeadType()
   * @generated
   */
  EAttribute getCompositeHeadType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType <em>Composite Inputs
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Inputs Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType
   * @generated
   */
  EClass getCompositeInputsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType#getIport <em>Iport</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Iport</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType#getIport()
   * @see #getCompositeInputsType()
   * @generated
   */
  EReference getCompositeInputsType_Iport();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType <em>Composite Modifiers
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Modifiers Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType
   * @generated
   */
  EClass getCompositeModifiersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType#getModifier
   * <em>Modifier</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Modifier</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType#getModifier()
   * @see #getCompositeModifiersType()
   * @generated
   */
  EReference getCompositeModifiersType_Modifier();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType <em>Composite Modifier
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Modifier Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType
   * @generated
   */
  EClass getCompositeModifierType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType#getName()
   * @see #getCompositeModifierType()
   * @generated
   */
  EAttribute getCompositeModifierType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType <em>Composite Outputs
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Outputs Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType
   * @generated
   */
  EClass getCompositeOutputsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType#getOport <em>Oport</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Oport</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType#getOport()
   * @see #getCompositeOutputsType()
   * @generated
   */
  EReference getCompositeOutputsType_Oport();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType <em>Composite
   * Parameter Expression Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Parameter Expression Mode Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType
   * @generated
   */
  EClass getCompositeParameterExpressionModeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode
   * <em>Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getMode()
   * @see #getCompositeParameterExpressionModeType()
   * @generated
   */
  EAttribute getCompositeParameterExpressionModeType_Mode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getType
   * <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType#getType()
   * @see #getCompositeParameterExpressionModeType()
   * @generated
   */
  EAttribute getCompositeParameterExpressionModeType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType <em>Composite Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Parameters Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType
   * @generated
   */
  EClass getCompositeParametersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType#getParameter
   * <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType#getParameter()
   * @see #getCompositeParametersType()
   * @generated
   */
  EReference getCompositeParametersType_Parameter();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType <em>Composite Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType
   * @generated
   */
  EClass getCompositeParameterType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getExpressionMode
   * <em>Expression Mode</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Expression Mode</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getExpressionMode()
   * @see #getCompositeParameterType()
   * @generated
   */
  EReference getCompositeParameterType_ExpressionMode();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getDefaultValue <em>Default
   * Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Default Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getDefaultValue()
   * @see #getCompositeParameterType()
   * @generated
   */
  EAttribute getCompositeParameterType_DefaultValue();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType#getName()
   * @see #getCompositeParameterType()
   * @generated
   */
  EAttribute getCompositeParameterType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType <em>Composite Port Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Port Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType
   * @generated
   */
  EClass getCompositePortType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getIndex <em>Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getIndex()
   * @see #getCompositePortType()
   * @generated
   */
  EAttribute getCompositePortType_Index();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getName()
   * @see #getCompositePortType()
   * @generated
   */
  EAttribute getCompositePortType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType#getType()
   * @see #getCompositePortType()
   * @generated
   */
  EAttribute getCompositePortType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType <em>Composite Types Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Composite Types Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType
   * @generated
   */
  EClass getCompositeTypesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType#getType()
   * @see #getCompositeTypesType()
   * @generated
   */
  EReference getCompositeTypesType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType <em>Config Option Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Config Option Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType
   * @generated
   */
  EClass getConfigOptionType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getParameter <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getParameter()
   * @see #getConfigOptionType()
   * @generated
   */
  EReference getConfigOptionType_Parameter();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType#getValue()
   * @see #getConfigOptionType()
   * @generated
   */
  EAttribute getConfigOptionType_Value();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType <em>Configs Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Configs Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType
   * @generated
   */
  EClass getConfigsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType#getConfig <em>Config</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Config</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType#getConfig()
   * @see #getConfigsType()
   * @generated
   */
  EReference getConfigsType_Config();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.source.code.ConfigType
   * <em>Config Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Config Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigType
   * @generated
   */
  EClass getConfigType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigType#getOption <em>Option</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Option</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigType#getOption()
   * @see #getConfigType()
   * @generated
   */
  EReference getConfigType_Option();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigType#getName()
   * @see #getConfigType()
   * @generated
   */
  EAttribute getConfigType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType <em>Config Value Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Config Value Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType
   * @generated
   */
  EClass getConfigValueParameterType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType#getValue()
   * @see #getConfigValueParameterType()
   * @generated
   */
  EAttribute getConfigValueParameterType_Value();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType <em>Definitions Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Definitions Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType
   * @generated
   */
  EClass getDefinitionsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getTypeDefinition <em>Type
   * Definition</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Type Definition</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getTypeDefinition()
   * @see #getDefinitionsType()
   * @generated
   */
  EReference getDefinitionsType_TypeDefinition();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getFunctionDefinition <em>Function
   * Definition</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Function Definition</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getFunctionDefinition()
   * @see #getDefinitionsType()
   * @generated
   */
  EReference getDefinitionsType_FunctionDefinition();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getCompositeDefinition <em>Composite
   * Definition</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Composite Definition</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType#getCompositeDefinition()
   * @see #getDefinitionsType()
   * @generated
   */
  EReference getDefinitionsType_CompositeDefinition();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getSourceModel <em>Source Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Source Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot#getSourceModel()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_SourceModel();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType <em>Expression Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Expression Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType
   * @generated
   */
  EClass getExpressionType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType#getExpr <em>Expr</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Expr</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType#getExpr()
   * @see #getExpressionType()
   * @generated
   */
  EAttribute getExpressionType_Expr();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType <em>Extended Source
   * Location Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Extended Source Location Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType
   * @generated
   */
  EClass getExtendedSourceLocationType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndColumn <em>End
   * Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>End Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndColumn()
   * @see #getExtendedSourceLocationType()
   * @generated
   */
  EAttribute getExtendedSourceLocationType_EndColumn();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndLine <em>End
   * Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>End Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getEndLine()
   * @see #getExtendedSourceLocationType()
   * @generated
   */
  EAttribute getExtendedSourceLocationType_EndLine();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartColumn <em>Start
   * Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Start Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartColumn()
   * @see #getExtendedSourceLocationType()
   * @generated
   */
  EAttribute getExtendedSourceLocationType_StartColumn();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartLine <em>Start
   * Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Start Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType#getStartLine()
   * @see #getExtendedSourceLocationType()
   * @generated
   */
  EAttribute getExtendedSourceLocationType_StartLine();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType <em>Function Definition
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Definition Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType
   * @generated
   */
  EClass getFunctionDefinitionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionHead <em>Function
   * Head</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Function Head</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionHead()
   * @see #getFunctionDefinitionType()
   * @generated
   */
  EReference getFunctionDefinitionType_FunctionHead();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionBody <em>Function
   * Body</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Function Body</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType#getFunctionBody()
   * @see #getFunctionDefinitionType()
   * @generated
   */
  EAttribute getFunctionDefinitionType_FunctionBody();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType <em>Function Head Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Head Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType
   * @generated
   */
  EClass getFunctionHeadType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getSplDoc <em>Spl Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getSplDoc()
   * @see #getFunctionHeadType()
   * @generated
   */
  EReference getFunctionHeadType_SplDoc();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getModifiers <em>Modifiers</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Modifiers</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getModifiers()
   * @see #getFunctionHeadType()
   * @generated
   */
  EReference getFunctionHeadType_Modifiers();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getParameters
   * <em>Parameters</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Parameters</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getParameters()
   * @see #getFunctionHeadType()
   * @generated
   */
  EReference getFunctionHeadType_Parameters();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getName()
   * @see #getFunctionHeadType()
   * @generated
   */
  EAttribute getFunctionHeadType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getReturnType <em>Return
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Return Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType#getReturnType()
   * @see #getFunctionHeadType()
   * @generated
   */
  EAttribute getFunctionHeadType_ReturnType();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType <em>Function Modifiers
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Modifiers Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType
   * @generated
   */
  EClass getFunctionModifiersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType#getModifier
   * <em>Modifier</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Modifier</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType#getModifier()
   * @see #getFunctionModifiersType()
   * @generated
   */
  EReference getFunctionModifiersType_Modifier();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType <em>Function Modifier
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Modifier Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType
   * @generated
   */
  EClass getFunctionModifierType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType#getName()
   * @see #getFunctionModifierType()
   * @generated
   */
  EAttribute getFunctionModifierType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType <em>Function
   * Parameter Modifiers Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Parameter Modifiers Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType
   * @generated
   */
  EClass getFunctionParameterModifiersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType#getModifier
   * <em>Modifier</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Modifier</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType#getModifier()
   * @see #getFunctionParameterModifiersType()
   * @generated
   */
  EReference getFunctionParameterModifiersType_Modifier();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType <em>Function Parameter
   * Modifier Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Parameter Modifier Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType
   * @generated
   */
  EClass getFunctionParameterModifierType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType#getName
   * <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType#getName()
   * @see #getFunctionParameterModifierType()
   * @generated
   */
  EAttribute getFunctionParameterModifierType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType <em>Function Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Parameters Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType
   * @generated
   */
  EClass getFunctionParametersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType#getParameter
   * <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType#getParameter()
   * @see #getFunctionParametersType()
   * @generated
   */
  EReference getFunctionParametersType_Parameter();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType <em>Function Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Function Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType
   * @generated
   */
  EClass getFunctionParameterType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getModifiers
   * <em>Modifiers</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Modifiers</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getModifiers()
   * @see #getFunctionParameterType()
   * @generated
   */
  EReference getFunctionParameterType_Modifiers();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getName()
   * @see #getFunctionParameterType()
   * @generated
   */
  EAttribute getFunctionParameterType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType#getType()
   * @see #getFunctionParameterType()
   * @generated
   */
  EAttribute getFunctionParameterType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType <em>On Process Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>On Process Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType
   * @generated
   */
  EClass getOnProcessType();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType <em>On Punct Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>On Punct Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType
   * @generated
   */
  EClass getOnPunctType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType#getPortName <em>Port Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Port Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType#getPortName()
   * @see #getOnPunctType()
   * @generated
   */
  EAttribute getOnPunctType_PortName();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType <em>On Tuple Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>On Tuple Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType
   * @generated
   */
  EClass getOnTupleType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType#getPortName <em>Port Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Port Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType#getPortName()
   * @see #getOnTupleType()
   * @generated
   */
  EAttribute getOnTupleType_PortName();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType
   * <em>Operator Invocation Attribute Assignment Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Attribute Assignment Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType
   * @generated
   */
  EClass getOperatorInvocationAttributeAssignmentType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getValue
   * <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Value</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getValue()
   * @see #getOperatorInvocationAttributeAssignmentType()
   * @generated
   */
  EReference getOperatorInvocationAttributeAssignmentType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getName
   * <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getName()
   * @see #getOperatorInvocationAttributeAssignmentType()
   * @generated
   */
  EAttribute getOperatorInvocationAttributeAssignmentType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getOutputFunction
   * <em>Output Function</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Output Function</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType#getOutputFunction()
   * @see #getOperatorInvocationAttributeAssignmentType()
   * @generated
   */
  EAttribute getOperatorInvocationAttributeAssignmentType_OutputFunction();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType <em>Operator Invocation
   * Body Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Body Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType
   * @generated
   */
  EClass getOperatorInvocationBodyType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getLogic
   * <em>Logic</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Logic</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getLogic()
   * @see #getOperatorInvocationBodyType()
   * @generated
   */
  EReference getOperatorInvocationBodyType_Logic();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getWindows
   * <em>Windows</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Windows</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getWindows()
   * @see #getOperatorInvocationBodyType()
   * @generated
   */
  EReference getOperatorInvocationBodyType_Windows();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getParameters
   * <em>Parameters</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Parameters</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getParameters()
   * @see #getOperatorInvocationBodyType()
   * @generated
   */
  EReference getOperatorInvocationBodyType_Parameters();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getOutputAssignments
   * <em>Output Assignments</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Output Assignments</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getOutputAssignments()
   * @see #getOperatorInvocationBodyType()
   * @generated
   */
  EReference getOperatorInvocationBodyType_OutputAssignments();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getConfigs
   * <em>Configs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Configs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType#getConfigs()
   * @see #getOperatorInvocationBodyType()
   * @generated
   */
  EReference getOperatorInvocationBodyType_Configs();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType <em>Operator Invocation
   * Head Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Head Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType
   * @generated
   */
  EClass getOperatorInvocationHeadType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getSplDoc <em>Spl
   * Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getSplDoc()
   * @see #getOperatorInvocationHeadType()
   * @generated
   */
  EReference getOperatorInvocationHeadType_SplDoc();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOutputs
   * <em>Outputs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Outputs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOutputs()
   * @see #getOperatorInvocationHeadType()
   * @generated
   */
  EReference getOperatorInvocationHeadType_Outputs();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInputs
   * <em>Inputs</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Inputs</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInputs()
   * @see #getOperatorInvocationHeadType()
   * @generated
   */
  EReference getOperatorInvocationHeadType_Inputs();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInvocationAlias
   * <em>Invocation Alias</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Invocation Alias</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getInvocationAlias()
   * @see #getOperatorInvocationHeadType()
   * @generated
   */
  EAttribute getOperatorInvocationHeadType_InvocationAlias();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOperatorName
   * <em>Operator Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Operator Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType#getOperatorName()
   * @see #getOperatorInvocationHeadType()
   * @generated
   */
  EAttribute getOperatorInvocationHeadType_OperatorName();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType <em>Operator
   * Invocation Input Stream Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Input Stream Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType
   * @generated
   */
  EClass getOperatorInvocationInputStreamType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType#getName
   * <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType#getName()
   * @see #getOperatorInvocationInputStreamType()
   * @generated
   */
  EAttribute getOperatorInvocationInputStreamType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType <em>Operator Invocation
   * Inputs Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Inputs Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType
   * @generated
   */
  EClass getOperatorInvocationInputsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType#getInput
   * <em>Input</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Input</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType#getInput()
   * @see #getOperatorInvocationInputsType()
   * @generated
   */
  EReference getOperatorInvocationInputsType_Input();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType <em>Operator Invocation
   * Input Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Input Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType
   * @generated
   */
  EClass getOperatorInvocationInputType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIstream
   * <em>Istream</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Istream</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIstream()
   * @see #getOperatorInvocationInputType()
   * @generated
   */
  EReference getOperatorInvocationInputType_Istream();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getAlias
   * <em>Alias</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Alias</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getAlias()
   * @see #getOperatorInvocationInputType()
   * @generated
   */
  EAttribute getOperatorInvocationInputType_Alias();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIndex
   * <em>Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType#getIndex()
   * @see #getOperatorInvocationInputType()
   * @generated
   */
  EAttribute getOperatorInvocationInputType_Index();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType <em>Operator Invocation
   * Logic Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Logic Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType
   * @generated
   */
  EClass getOperatorInvocationLogicType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnProcess <em>On
   * Process</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>On Process</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnProcess()
   * @see #getOperatorInvocationLogicType()
   * @generated
   */
  EReference getOperatorInvocationLogicType_OnProcess();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnTuple <em>On
   * Tuple</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>On Tuple</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnTuple()
   * @see #getOperatorInvocationLogicType()
   * @generated
   */
  EReference getOperatorInvocationLogicType_OnTuple();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnPunct <em>On
   * Punct</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>On Punct</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#getOnPunct()
   * @see #getOperatorInvocationLogicType()
   * @generated
   */
  EReference getOperatorInvocationLogicType_OnPunct();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState <em>Has
   * State</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Has State</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType#isHasState()
   * @see #getOperatorInvocationLogicType()
   * @generated
   */
  EAttribute getOperatorInvocationLogicType_HasState();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType <em>Operator
   * Invocation Output Assignments Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Output Assignments Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType
   * @generated
   */
  EClass getOperatorInvocationOutputAssignmentsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType#getOutputAssignment
   * <em>Output Assignment</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output Assignment</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType#getOutputAssignment()
   * @see #getOperatorInvocationOutputAssignmentsType()
   * @generated
   */
  EReference getOperatorInvocationOutputAssignmentsType_OutputAssignment();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType <em>Operator
   * Invocation Output Assignment Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Output Assignment Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType
   * @generated
   */
  EClass getOperatorInvocationOutputAssignmentType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getAttributeAssignment
   * <em>Attribute Assignment</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Attribute Assignment</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getAttributeAssignment()
   * @see #getOperatorInvocationOutputAssignmentType()
   * @generated
   */
  EReference getOperatorInvocationOutputAssignmentType_AttributeAssignment();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getPortName
   * <em>Port Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Port Name</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType#getPortName()
   * @see #getOperatorInvocationOutputAssignmentType()
   * @generated
   */
  EAttribute getOperatorInvocationOutputAssignmentType_PortName();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType <em>Operator
   * Invocation Outputs Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Outputs Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType
   * @generated
   */
  EClass getOperatorInvocationOutputsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType#getOutput
   * <em>Output</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Output</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType#getOutput()
   * @see #getOperatorInvocationOutputsType()
   * @generated
   */
  EReference getOperatorInvocationOutputsType_Output();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType <em>Operator Invocation
   * Output Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Output Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType
   * @generated
   */
  EClass getOperatorInvocationOutputType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getAlias
   * <em>Alias</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Alias</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getAlias()
   * @see #getOperatorInvocationOutputType()
   * @generated
   */
  EAttribute getOperatorInvocationOutputType_Alias();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getIndex
   * <em>Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getIndex()
   * @see #getOperatorInvocationOutputType()
   * @generated
   */
  EAttribute getOperatorInvocationOutputType_Index();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getStreamName
   * <em>Stream Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Stream Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getStreamName()
   * @see #getOperatorInvocationOutputType()
   * @generated
   */
  EAttribute getOperatorInvocationOutputType_StreamName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getType
   * <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType#getType()
   * @see #getOperatorInvocationOutputType()
   * @generated
   */
  EAttribute getOperatorInvocationOutputType_Type();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType <em>Operator
   * Invocation Parameters Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Parameters Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType
   * @generated
   */
  EClass getOperatorInvocationParametersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType#getParameter
   * <em>Parameter</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Parameter</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType#getParameter()
   * @see #getOperatorInvocationParametersType()
   * @generated
   */
  EReference getOperatorInvocationParametersType_Parameter();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType <em>Operator
   * Invocation Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Parameter Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType
   * @generated
   */
  EClass getOperatorInvocationParameterType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType#getValue
   * <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType#getValue()
   * @see #getOperatorInvocationParameterType()
   * @generated
   */
  EReference getOperatorInvocationParameterType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType#getName
   * <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType#getName()
   * @see #getOperatorInvocationParameterType()
   * @generated
   */
  EAttribute getOperatorInvocationParameterType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType <em>Operator Invocation
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType
   * @generated
   */
  EClass getOperatorInvocationType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationHead
   * <em>Operator Invocation Head</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Operator Invocation Head</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationHead()
   * @see #getOperatorInvocationType()
   * @generated
   */
  EReference getOperatorInvocationType_OperatorInvocationHead();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationBody
   * <em>Operator Invocation Body</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Operator Invocation Body</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType#getOperatorInvocationBody()
   * @see #getOperatorInvocationType()
   * @generated
   */
  EReference getOperatorInvocationType_OperatorInvocationBody();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType <em>Operator
   * Invocation Windows Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Windows Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType
   * @generated
   */
  EClass getOperatorInvocationWindowsType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType#getWindow
   * <em>Window</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Window</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType#getWindow()
   * @see #getOperatorInvocationWindowsType()
   * @generated
   */
  EReference getOperatorInvocationWindowsType_Window();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType <em>Operator Invocation
   * Window Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Operator Invocation Window Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType
   * @generated
   */
  EClass getOperatorInvocationWindowType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getEvictionPolicy
   * <em>Eviction Policy</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Eviction Policy</em>'.
   * @see
   *     com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getEvictionPolicy()
   * @see #getOperatorInvocationWindowType()
   * @generated
   */
  EReference getOperatorInvocationWindowType_EvictionPolicy();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getTriggerPolicy
   * <em>Trigger Policy</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Trigger Policy</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getTriggerPolicy()
   * @see #getOperatorInvocationWindowType()
   * @generated
   */
  EReference getOperatorInvocationWindowType_TriggerPolicy();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned
   * <em>Partitioned</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Partitioned</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#isPartitioned()
   * @see #getOperatorInvocationWindowType()
   * @generated
   */
  EAttribute getOperatorInvocationWindowType_Partitioned();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getPortName <em>Port
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Port Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getPortName()
   * @see #getOperatorInvocationWindowType()
   * @generated
   */
  EAttribute getOperatorInvocationWindowType_PortName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType
   * <em>Window Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Window Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType#getWindowType()
   * @see #getOperatorInvocationWindowType()
   * @generated
   */
  EAttribute getOperatorInvocationWindowType_WindowType();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType <em>Source File Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Source File Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType
   * @generated
   */
  EClass getSourceFileType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getCompilationUnit <em>Compilation
   * Unit</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Compilation Unit</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getCompilationUnit()
   * @see #getSourceFileType()
   * @generated
   */
  EReference getSourceFileType_CompilationUnit();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getUri <em>Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType#getUri()
   * @see #getSourceFileType()
   * @generated
   */
  EAttribute getSourceFileType_Uri();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType <em>Source Location Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Source Location Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType
   * @generated
   */
  EClass getSourceLocationType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getColumn <em>Column</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Column</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getColumn()
   * @see #getSourceLocationType()
   * @generated
   */
  EAttribute getSourceLocationType_Column();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getLine <em>Line</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Line</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType#getLine()
   * @see #getSourceLocationType()
   * @generated
   */
  EAttribute getSourceLocationType_Line();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType <em>Source Model Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Source Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType
   * @generated
   */
  EClass getSourceModelType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType#getSourceFile <em>Source
   * File</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Source File</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType#getSourceFile()
   * @see #getSourceModelType()
   * @generated
   */
  EReference getSourceModelType_SourceFile();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType <em>Spl Doc Annotation
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Spl Doc Annotation Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType
   * @generated
   */
  EClass getSplDocAnnotationType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType#getName()
   * @see #getSplDocAnnotationType()
   * @generated
   */
  EAttribute getSplDocAnnotationType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType#getTarget <em>Target</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Target</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType#getTarget()
   * @see #getSplDocAnnotationType()
   * @generated
   */
  EAttribute getSplDocAnnotationType_Target();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType <em>Spl Doc Description
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Spl Doc Description Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType
   * @generated
   */
  EClass getSplDocDescriptionType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType#getDescription
   * <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType#getDescription()
   * @see #getSplDocDescriptionType()
   * @generated
   */
  EAttribute getSplDocDescriptionType_Description();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.source.code.SplDocType
   * <em>Spl Doc Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Spl Doc Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocType
   * @generated
   */
  EClass getSplDocType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocType#getDescription()
   * @see #getSplDocType()
   * @generated
   */
  EReference getSplDocType_Description();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocType#getAnnotation <em>Annotation</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Annotation</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocType#getAnnotation()
   * @see #getSplDocType()
   * @generated
   */
  EReference getSplDocType_Annotation();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType <em>Spl Namespace Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Spl Namespace Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType
   * @generated
   */
  EClass getSplNamespaceType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType#getSplDoc <em>Spl Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType#getSplDoc()
   * @see #getSplNamespaceType()
   * @generated
   */
  EReference getSplNamespaceType_SplDoc();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType#getName()
   * @see #getSplNamespaceType()
   * @generated
   */
  EAttribute getSplNamespaceType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType <em>Type Definition Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type Definition Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType
   * @generated
   */
  EClass getTypeDefinitionType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getSplDoc <em>Spl Doc</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Spl Doc</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getSplDoc()
   * @see #getTypeDefinitionType()
   * @generated
   */
  EReference getTypeDefinitionType_SplDoc();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getModifiers
   * <em>Modifiers</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Modifiers</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getModifiers()
   * @see #getTypeDefinitionType()
   * @generated
   */
  EReference getTypeDefinitionType_Modifiers();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getName()
   * @see #getTypeDefinitionType()
   * @generated
   */
  EAttribute getTypeDefinitionType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getValue()
   * @see #getTypeDefinitionType()
   * @generated
   */
  EAttribute getTypeDefinitionType_Value();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType <em>Type Modifiers Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type Modifiers Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType
   * @generated
   */
  EClass getTypeModifiersType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType#getModifier <em>Modifier</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Modifier</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType#getModifier()
   * @see #getTypeModifiersType()
   * @generated
   */
  EReference getTypeModifiersType_Modifier();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType <em>Type Modifier Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Type Modifier Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType
   * @generated
   */
  EClass getTypeModifierType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType#getName()
   * @see #getTypeModifierType()
   * @generated
   */
  EAttribute getTypeModifierType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType <em>Use Directives Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Use Directives Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType
   * @generated
   */
  EClass getUseDirectivesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType#getUseDirective <em>Use
   * Directive</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Use Directive</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType#getUseDirective()
   * @see #getUseDirectivesType()
   * @generated
   */
  EReference getUseDirectivesType_UseDirective();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType <em>Use Directive Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Use Directive Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType
   * @generated
   */
  EClass getUseDirectiveType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getNamespaceName <em>Namespace
   * Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Namespace Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getNamespaceName()
   * @see #getUseDirectiveType()
   * @generated
   */
  EAttribute getUseDirectiveType_NamespaceName();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getTail <em>Tail</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Tail</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType#getTail()
   * @see #getUseDirectiveType()
   * @generated
   */
  EAttribute getUseDirectiveType_Tail();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType <em>Window Policy Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Window Policy Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType
   * @generated
   */
  EClass getWindowPolicyType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getAttribute <em>Attribute</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Attribute</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getAttribute()
   * @see #getWindowPolicyType()
   * @generated
   */
  EAttribute getWindowPolicyType_Attribute();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind <em>Kind</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Kind</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind()
   * @see #getWindowPolicyType()
   * @generated
   */
  EAttribute getWindowPolicyType_Kind();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getSize <em>Size</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Size</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getSize()
   * @see #getWindowPolicyType()
   * @generated
   */
  EAttribute getWindowPolicyType_Size();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType <em>Composite Modifier
   * Enum Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Composite Modifier Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
   * @generated
   */
  EEnum getCompositeModifierEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * <em>Composite Parameter Expression Mode Enum Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Composite Parameter Expression Mode Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @generated
   */
  EEnum getCompositeParameterExpressionModeEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType <em>Function Modifier Enum
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Function Modifier Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @generated
   */
  EEnum getFunctionModifierEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType <em>Function
   * Parameter Modifier Enum Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Function Parameter Modifier Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
   * @generated
   */
  EEnum getFunctionParameterModifierEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType <em>Type Modifier Enum
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Type Modifier Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
   * @generated
   */
  EEnum getTypeModifierEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType <em>Window Policy Kind Enum
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Window Policy Kind Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @generated
   */
  EEnum getWindowPolicyKindEnumType();

  /**
   * Returns the meta object for enum '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType <em>Window Type Enum Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Window Type Enum Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @generated
   */
  EEnum getWindowTypeEnumType();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType <em>Composite Modifier
   * Enum Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Composite Modifier Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType"
   *     extendedMetaData="name='compositeModifierEnumType:Object'
   *     baseType='compositeModifierEnumType'"
   * @generated
   */
  EDataType getCompositeModifierEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * <em>Composite Parameter Expression Mode Enum Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Composite Parameter Expression Mode Enum Type
   *     Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
   * @model
   *     instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType"
   *     extendedMetaData="name='compositeParameterExpressionModeEnumType:Object'
   *     baseType='compositeParameterExpressionModeEnumType'"
   * @generated
   */
  EDataType getCompositeParameterExpressionModeEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType <em>Function Modifier Enum
   * Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Function Modifier Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType"
   *     extendedMetaData="name='functionModifierEnumType:Object'
   *     baseType='functionModifierEnumType'"
   * @generated
   */
  EDataType getFunctionModifierEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType <em>Function
   * Parameter Modifier Enum Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Function Parameter Modifier Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
   * @model
   *     instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType"
   *     extendedMetaData="name='functionParameterModifierEnumType:Object'
   *     baseType='functionParameterModifierEnumType'"
   * @generated
   */
  EDataType getFunctionParameterModifierEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType <em>Type Modifier Enum Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Type Modifier Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType"
   *     extendedMetaData="name='typeModifierEnumType:Object' baseType='typeModifierEnumType'"
   * @generated
   */
  EDataType getTypeModifierEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType <em>Window Policy Kind Enum
   * Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Window Policy Kind Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType"
   *     extendedMetaData="name='windowPolicyKindEnumType:Object'
   *     baseType='windowPolicyKindEnumType'"
   * @generated
   */
  EDataType getWindowPolicyKindEnumTypeObject();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType <em>Window Type Enum Type
   * Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Window Type Enum Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType"
   *     extendedMetaData="name='windowTypeEnumType:Object' baseType='windowTypeEnumType'"
   * @generated
   */
  EDataType getWindowTypeEnumTypeObject();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  CodeFactory getCodeFactory();

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
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl <em>Compilation Unit
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompilationUnitType()
     * @generated
     */
    EClass COMPILATION_UNIT_TYPE = eINSTANCE.getCompilationUnitType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPILATION_UNIT_TYPE__SPL_DOC = eINSTANCE.getCompilationUnitType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Spl Namespace</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPILATION_UNIT_TYPE__SPL_NAMESPACE =
        eINSTANCE.getCompilationUnitType_SplNamespace();

    /**
     * The meta object literal for the '<em><b>Use Directives</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPILATION_UNIT_TYPE__USE_DIRECTIVES =
        eINSTANCE.getCompilationUnitType_UseDirectives();

    /**
     * The meta object literal for the '<em><b>Definitions</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPILATION_UNIT_TYPE__DEFINITIONS = eINSTANCE.getCompilationUnitType_Definitions();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl <em>Composite Body
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeBodyTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeBodyType()
     * @generated
     */
    EClass COMPOSITE_BODY_TYPE = eINSTANCE.getCompositeBodyType();

    /**
     * The meta object literal for the '<em><b>Types</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_BODY_TYPE__TYPES = eINSTANCE.getCompositeBodyType_Types();

    /**
     * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_BODY_TYPE__PARAMETERS = eINSTANCE.getCompositeBodyType_Parameters();

    /**
     * The meta object literal for the '<em><b>Graph</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_BODY_TYPE__GRAPH = eINSTANCE.getCompositeBodyType_Graph();

    /**
     * The meta object literal for the '<em><b>Configs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_BODY_TYPE__CONFIGS = eINSTANCE.getCompositeBodyType_Configs();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl <em>Composite
     * Definition Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeDefinitionType()
     * @generated
     */
    EClass COMPOSITE_DEFINITION_TYPE = eINSTANCE.getCompositeDefinitionType();

    /**
     * The meta object literal for the '<em><b>Composite Head</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD =
        eINSTANCE.getCompositeDefinitionType_CompositeHead();

    /**
     * The meta object literal for the '<em><b>Composite Body</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY =
        eINSTANCE.getCompositeDefinitionType_CompositeBody();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeGraphTypeImpl <em>Composite Graph
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeGraphTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeGraphType()
     * @generated
     */
    EClass COMPOSITE_GRAPH_TYPE = eINSTANCE.getCompositeGraphType();

    /**
     * The meta object literal for the '<em><b>Operator Invocation</b></em>' containment reference
     * list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_GRAPH_TYPE__OPERATOR_INVOCATION =
        eINSTANCE.getCompositeGraphType_OperatorInvocation();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl <em>Composite Head
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeHeadTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeHeadType()
     * @generated
     */
    EClass COMPOSITE_HEAD_TYPE = eINSTANCE.getCompositeHeadType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_HEAD_TYPE__SPL_DOC = eINSTANCE.getCompositeHeadType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Modifiers</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_HEAD_TYPE__MODIFIERS = eINSTANCE.getCompositeHeadType_Modifiers();

    /**
     * The meta object literal for the '<em><b>Inputs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_HEAD_TYPE__INPUTS = eINSTANCE.getCompositeHeadType_Inputs();

    /**
     * The meta object literal for the '<em><b>Outputs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_HEAD_TYPE__OUTPUTS = eINSTANCE.getCompositeHeadType_Outputs();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_HEAD_TYPE__NAME = eINSTANCE.getCompositeHeadType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeInputsTypeImpl <em>Composite Inputs
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeInputsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeInputsType()
     * @generated
     */
    EClass COMPOSITE_INPUTS_TYPE = eINSTANCE.getCompositeInputsType();

    /**
     * The meta object literal for the '<em><b>Iport</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_INPUTS_TYPE__IPORT = eINSTANCE.getCompositeInputsType_Iport();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifiersTypeImpl <em>Composite
     * Modifiers Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifiersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifiersType()
     * @generated
     */
    EClass COMPOSITE_MODIFIERS_TYPE = eINSTANCE.getCompositeModifiersType();

    /**
     * The meta object literal for the '<em><b>Modifier</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_MODIFIERS_TYPE__MODIFIER = eINSTANCE.getCompositeModifiersType_Modifier();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifierTypeImpl <em>Composite
     * Modifier Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeModifierTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierType()
     * @generated
     */
    EClass COMPOSITE_MODIFIER_TYPE = eINSTANCE.getCompositeModifierType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_MODIFIER_TYPE__NAME = eINSTANCE.getCompositeModifierType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeOutputsTypeImpl <em>Composite
     * Outputs Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeOutputsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeOutputsType()
     * @generated
     */
    EClass COMPOSITE_OUTPUTS_TYPE = eINSTANCE.getCompositeOutputsType();

    /**
     * The meta object literal for the '<em><b>Oport</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_OUTPUTS_TYPE__OPORT = eINSTANCE.getCompositeOutputsType_Oport();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl
     * <em>Composite Parameter Expression Mode Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterExpressionModeTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeType()
     * @generated
     */
    EClass COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE =
        eINSTANCE.getCompositeParameterExpressionModeType();

    /**
     * The meta object literal for the '<em><b>Mode</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE =
        eINSTANCE.getCompositeParameterExpressionModeType_Mode();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE =
        eINSTANCE.getCompositeParameterExpressionModeType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParametersTypeImpl <em>Composite
     * Parameters Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParametersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParametersType()
     * @generated
     */
    EClass COMPOSITE_PARAMETERS_TYPE = eINSTANCE.getCompositeParametersType();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_PARAMETERS_TYPE__PARAMETER =
        eINSTANCE.getCompositeParametersType_Parameter();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl <em>Composite
     * Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterType()
     * @generated
     */
    EClass COMPOSITE_PARAMETER_TYPE = eINSTANCE.getCompositeParameterType();

    /**
     * The meta object literal for the '<em><b>Expression Mode</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE =
        eINSTANCE.getCompositeParameterType_ExpressionMode();

    /**
     * The meta object literal for the '<em><b>Default Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE =
        eINSTANCE.getCompositeParameterType_DefaultValue();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PARAMETER_TYPE__NAME = eINSTANCE.getCompositeParameterType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl <em>Composite Port
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositePortTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositePortType()
     * @generated
     */
    EClass COMPOSITE_PORT_TYPE = eINSTANCE.getCompositePortType();

    /**
     * The meta object literal for the '<em><b>Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PORT_TYPE__INDEX = eINSTANCE.getCompositePortType_Index();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PORT_TYPE__NAME = eINSTANCE.getCompositePortType_Name();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute COMPOSITE_PORT_TYPE__TYPE = eINSTANCE.getCompositePortType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeTypesTypeImpl <em>Composite Types
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeTypesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeTypesType()
     * @generated
     */
    EClass COMPOSITE_TYPES_TYPE = eINSTANCE.getCompositeTypesType();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference COMPOSITE_TYPES_TYPE__TYPE = eINSTANCE.getCompositeTypesType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigOptionTypeImpl <em>Config Option
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigOptionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigOptionType()
     * @generated
     */
    EClass CONFIG_OPTION_TYPE = eINSTANCE.getConfigOptionType();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONFIG_OPTION_TYPE__PARAMETER = eINSTANCE.getConfigOptionType_Parameter();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONFIG_OPTION_TYPE__VALUE = eINSTANCE.getConfigOptionType_Value();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigsTypeImpl <em>Configs Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigsType()
     * @generated
     */
    EClass CONFIGS_TYPE = eINSTANCE.getConfigsType();

    /**
     * The meta object literal for the '<em><b>Config</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONFIGS_TYPE__CONFIG = eINSTANCE.getConfigsType_Config();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigTypeImpl <em>Config Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigType()
     * @generated
     */
    EClass CONFIG_TYPE = eINSTANCE.getConfigType();

    /**
     * The meta object literal for the '<em><b>Option</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference CONFIG_TYPE__OPTION = eINSTANCE.getConfigType_Option();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONFIG_TYPE__NAME = eINSTANCE.getConfigType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigValueParameterTypeImpl <em>Config Value
     * Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ConfigValueParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getConfigValueParameterType()
     * @generated
     */
    EClass CONFIG_VALUE_PARAMETER_TYPE = eINSTANCE.getConfigValueParameterType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute CONFIG_VALUE_PARAMETER_TYPE__VALUE = eINSTANCE.getConfigValueParameterType_Value();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl <em>Definitions
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.DefinitionsTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getDefinitionsType()
     * @generated
     */
    EClass DEFINITIONS_TYPE = eINSTANCE.getDefinitionsType();

    /**
     * The meta object literal for the '<em><b>Type Definition</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DEFINITIONS_TYPE__TYPE_DEFINITION = eINSTANCE.getDefinitionsType_TypeDefinition();

    /**
     * The meta object literal for the '<em><b>Function Definition</b></em>' containment reference
     * list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DEFINITIONS_TYPE__FUNCTION_DEFINITION =
        eINSTANCE.getDefinitionsType_FunctionDefinition();

    /**
     * The meta object literal for the '<em><b>Composite Definition</b></em>' containment reference
     * list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DEFINITIONS_TYPE__COMPOSITE_DEFINITION =
        eINSTANCE.getDefinitionsType_CompositeDefinition();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.DocumentRootImpl <em>Document Root</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getDocumentRoot()
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
     * The meta object literal for the '<em><b>Source Model</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__SOURCE_MODEL = eINSTANCE.getDocumentRoot_SourceModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ExpressionTypeImpl <em>Expression Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ExpressionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getExpressionType()
     * @generated
     */
    EClass EXPRESSION_TYPE = eINSTANCE.getExpressionType();

    /**
     * The meta object literal for the '<em><b>Expr</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXPRESSION_TYPE__EXPR = eINSTANCE.getExpressionType_Expr();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl <em>Extended
     * Source Location Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.ExtendedSourceLocationTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getExtendedSourceLocationType()
     * @generated
     */
    EClass EXTENDED_SOURCE_LOCATION_TYPE = eINSTANCE.getExtendedSourceLocationType();

    /**
     * The meta object literal for the '<em><b>End Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN =
        eINSTANCE.getExtendedSourceLocationType_EndColumn();

    /**
     * The meta object literal for the '<em><b>End Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXTENDED_SOURCE_LOCATION_TYPE__END_LINE =
        eINSTANCE.getExtendedSourceLocationType_EndLine();

    /**
     * The meta object literal for the '<em><b>Start Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN =
        eINSTANCE.getExtendedSourceLocationType_StartColumn();

    /**
     * The meta object literal for the '<em><b>Start Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXTENDED_SOURCE_LOCATION_TYPE__START_LINE =
        eINSTANCE.getExtendedSourceLocationType_StartLine();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl <em>Function
     * Definition Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionDefinitionTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionDefinitionType()
     * @generated
     */
    EClass FUNCTION_DEFINITION_TYPE = eINSTANCE.getFunctionDefinitionType();

    /**
     * The meta object literal for the '<em><b>Function Head</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD =
        eINSTANCE.getFunctionDefinitionType_FunctionHead();

    /**
     * The meta object literal for the '<em><b>Function Body</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_DEFINITION_TYPE__FUNCTION_BODY =
        eINSTANCE.getFunctionDefinitionType_FunctionBody();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionHeadTypeImpl <em>Function Head
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionHeadTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionHeadType()
     * @generated
     */
    EClass FUNCTION_HEAD_TYPE = eINSTANCE.getFunctionHeadType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_HEAD_TYPE__SPL_DOC = eINSTANCE.getFunctionHeadType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Modifiers</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_HEAD_TYPE__MODIFIERS = eINSTANCE.getFunctionHeadType_Modifiers();

    /**
     * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_HEAD_TYPE__PARAMETERS = eINSTANCE.getFunctionHeadType_Parameters();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_HEAD_TYPE__NAME = eINSTANCE.getFunctionHeadType_Name();

    /**
     * The meta object literal for the '<em><b>Return Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_HEAD_TYPE__RETURN_TYPE = eINSTANCE.getFunctionHeadType_ReturnType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifiersTypeImpl <em>Function
     * Modifiers Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifiersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifiersType()
     * @generated
     */
    EClass FUNCTION_MODIFIERS_TYPE = eINSTANCE.getFunctionModifiersType();

    /**
     * The meta object literal for the '<em><b>Modifier</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_MODIFIERS_TYPE__MODIFIER = eINSTANCE.getFunctionModifiersType_Modifier();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifierTypeImpl <em>Function
     * Modifier Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionModifierTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierType()
     * @generated
     */
    EClass FUNCTION_MODIFIER_TYPE = eINSTANCE.getFunctionModifierType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_MODIFIER_TYPE__NAME = eINSTANCE.getFunctionModifierType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifiersTypeImpl
     * <em>Function Parameter Modifiers Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifiersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifiersType()
     * @generated
     */
    EClass FUNCTION_PARAMETER_MODIFIERS_TYPE = eINSTANCE.getFunctionParameterModifiersType();

    /**
     * The meta object literal for the '<em><b>Modifier</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_PARAMETER_MODIFIERS_TYPE__MODIFIER =
        eINSTANCE.getFunctionParameterModifiersType_Modifier();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifierTypeImpl
     * <em>Function Parameter Modifier Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterModifierTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierType()
     * @generated
     */
    EClass FUNCTION_PARAMETER_MODIFIER_TYPE = eINSTANCE.getFunctionParameterModifierType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_PARAMETER_MODIFIER_TYPE__NAME =
        eINSTANCE.getFunctionParameterModifierType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParametersTypeImpl <em>Function
     * Parameters Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParametersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParametersType()
     * @generated
     */
    EClass FUNCTION_PARAMETERS_TYPE = eINSTANCE.getFunctionParametersType();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_PARAMETERS_TYPE__PARAMETER =
        eINSTANCE.getFunctionParametersType_Parameter();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterTypeImpl <em>Function
     * Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.FunctionParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterType()
     * @generated
     */
    EClass FUNCTION_PARAMETER_TYPE = eINSTANCE.getFunctionParameterType();

    /**
     * The meta object literal for the '<em><b>Modifiers</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference FUNCTION_PARAMETER_TYPE__MODIFIERS = eINSTANCE.getFunctionParameterType_Modifiers();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_PARAMETER_TYPE__NAME = eINSTANCE.getFunctionParameterType_Name();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute FUNCTION_PARAMETER_TYPE__TYPE = eINSTANCE.getFunctionParameterType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnProcessTypeImpl <em>On Process Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnProcessTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnProcessType()
     * @generated
     */
    EClass ON_PROCESS_TYPE = eINSTANCE.getOnProcessType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnPunctTypeImpl <em>On Punct Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnPunctTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnPunctType()
     * @generated
     */
    EClass ON_PUNCT_TYPE = eINSTANCE.getOnPunctType();

    /**
     * The meta object literal for the '<em><b>Port Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ON_PUNCT_TYPE__PORT_NAME = eINSTANCE.getOnPunctType_PortName();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OnTupleTypeImpl <em>On Tuple Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OnTupleTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOnTupleType()
     * @generated
     */
    EClass ON_TUPLE_TYPE = eINSTANCE.getOnTupleType();

    /**
     * The meta object literal for the '<em><b>Port Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute ON_TUPLE_TYPE__PORT_NAME = eINSTANCE.getOnTupleType_PortName();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl
     * <em>Operator Invocation Attribute Assignment Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationAttributeAssignmentTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationAttributeAssignmentType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE =
        eINSTANCE.getOperatorInvocationAttributeAssignmentType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE =
        eINSTANCE.getOperatorInvocationAttributeAssignmentType_Value();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME =
        eINSTANCE.getOperatorInvocationAttributeAssignmentType_Name();

    /**
     * The meta object literal for the '<em><b>Output Function</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION =
        eINSTANCE.getOperatorInvocationAttributeAssignmentType_OutputFunction();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl <em>Operator
     * Invocation Body Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationBodyTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationBodyType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_BODY_TYPE = eINSTANCE.getOperatorInvocationBodyType();

    /**
     * The meta object literal for the '<em><b>Logic</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_BODY_TYPE__LOGIC =
        eINSTANCE.getOperatorInvocationBodyType_Logic();

    /**
     * The meta object literal for the '<em><b>Windows</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_BODY_TYPE__WINDOWS =
        eINSTANCE.getOperatorInvocationBodyType_Windows();

    /**
     * The meta object literal for the '<em><b>Parameters</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS =
        eINSTANCE.getOperatorInvocationBodyType_Parameters();

    /**
     * The meta object literal for the '<em><b>Output Assignments</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS =
        eINSTANCE.getOperatorInvocationBodyType_OutputAssignments();

    /**
     * The meta object literal for the '<em><b>Configs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_BODY_TYPE__CONFIGS =
        eINSTANCE.getOperatorInvocationBodyType_Configs();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl <em>Operator
     * Invocation Head Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationHeadTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationHeadType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_HEAD_TYPE = eINSTANCE.getOperatorInvocationHeadType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC =
        eINSTANCE.getOperatorInvocationHeadType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Outputs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS =
        eINSTANCE.getOperatorInvocationHeadType_Outputs();

    /**
     * The meta object literal for the '<em><b>Inputs</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_HEAD_TYPE__INPUTS =
        eINSTANCE.getOperatorInvocationHeadType_Inputs();

    /**
     * The meta object literal for the '<em><b>Invocation Alias</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS =
        eINSTANCE.getOperatorInvocationHeadType_InvocationAlias();

    /**
     * The meta object literal for the '<em><b>Operator Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME =
        eINSTANCE.getOperatorInvocationHeadType_OperatorName();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputStreamTypeImpl
     * <em>Operator Invocation Input Stream Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputStreamTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputStreamType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_INPUT_STREAM_TYPE = eINSTANCE.getOperatorInvocationInputStreamType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_INPUT_STREAM_TYPE__NAME =
        eINSTANCE.getOperatorInvocationInputStreamType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputsTypeImpl <em>Operator
     * Invocation Inputs Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputsType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_INPUTS_TYPE = eINSTANCE.getOperatorInvocationInputsType();

    /**
     * The meta object literal for the '<em><b>Input</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_INPUTS_TYPE__INPUT =
        eINSTANCE.getOperatorInvocationInputsType_Input();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl <em>Operator
     * Invocation Input Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationInputTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationInputType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_INPUT_TYPE = eINSTANCE.getOperatorInvocationInputType();

    /**
     * The meta object literal for the '<em><b>Istream</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM =
        eINSTANCE.getOperatorInvocationInputType_Istream();

    /**
     * The meta object literal for the '<em><b>Alias</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_INPUT_TYPE__ALIAS =
        eINSTANCE.getOperatorInvocationInputType_Alias();

    /**
     * The meta object literal for the '<em><b>Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_INPUT_TYPE__INDEX =
        eINSTANCE.getOperatorInvocationInputType_Index();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl <em>Operator
     * Invocation Logic Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationLogicTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationLogicType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_LOGIC_TYPE = eINSTANCE.getOperatorInvocationLogicType();

    /**
     * The meta object literal for the '<em><b>On Process</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS =
        eINSTANCE.getOperatorInvocationLogicType_OnProcess();

    /**
     * The meta object literal for the '<em><b>On Tuple</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE =
        eINSTANCE.getOperatorInvocationLogicType_OnTuple();

    /**
     * The meta object literal for the '<em><b>On Punct</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT =
        eINSTANCE.getOperatorInvocationLogicType_OnPunct();

    /**
     * The meta object literal for the '<em><b>Has State</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE =
        eINSTANCE.getOperatorInvocationLogicType_HasState();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentsTypeImpl
     * <em>Operator Invocation Output Assignments Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputAssignmentsType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE =
        eINSTANCE.getOperatorInvocationOutputAssignmentsType();

    /**
     * The meta object literal for the '<em><b>Output Assignment</b></em>' containment reference
     * list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE__OUTPUT_ASSIGNMENT =
        eINSTANCE.getOperatorInvocationOutputAssignmentsType_OutputAssignment();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl
     * <em>Operator Invocation Output Assignment Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputAssignmentTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputAssignmentType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE =
        eINSTANCE.getOperatorInvocationOutputAssignmentType();

    /**
     * The meta object literal for the '<em><b>Attribute Assignment</b></em>' containment reference
     * list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT =
        eINSTANCE.getOperatorInvocationOutputAssignmentType_AttributeAssignment();

    /**
     * The meta object literal for the '<em><b>Port Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME =
        eINSTANCE.getOperatorInvocationOutputAssignmentType_PortName();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputsTypeImpl
     * <em>Operator Invocation Outputs Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputsType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_OUTPUTS_TYPE = eINSTANCE.getOperatorInvocationOutputsType();

    /**
     * The meta object literal for the '<em><b>Output</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_OUTPUTS_TYPE__OUTPUT =
        eINSTANCE.getOperatorInvocationOutputsType_Output();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputTypeImpl <em>Operator
     * Invocation Output Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationOutputTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationOutputType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_OUTPUT_TYPE = eINSTANCE.getOperatorInvocationOutputType();

    /**
     * The meta object literal for the '<em><b>Alias</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_OUTPUT_TYPE__ALIAS =
        eINSTANCE.getOperatorInvocationOutputType_Alias();

    /**
     * The meta object literal for the '<em><b>Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_OUTPUT_TYPE__INDEX =
        eINSTANCE.getOperatorInvocationOutputType_Index();

    /**
     * The meta object literal for the '<em><b>Stream Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_OUTPUT_TYPE__STREAM_NAME =
        eINSTANCE.getOperatorInvocationOutputType_StreamName();

    /**
     * The meta object literal for the '<em><b>Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_OUTPUT_TYPE__TYPE =
        eINSTANCE.getOperatorInvocationOutputType_Type();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParametersTypeImpl
     * <em>Operator Invocation Parameters Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParametersTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationParametersType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_PARAMETERS_TYPE = eINSTANCE.getOperatorInvocationParametersType();

    /**
     * The meta object literal for the '<em><b>Parameter</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_PARAMETERS_TYPE__PARAMETER =
        eINSTANCE.getOperatorInvocationParametersType_Parameter();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParameterTypeImpl
     * <em>Operator Invocation Parameter Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationParameterTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationParameterType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_PARAMETER_TYPE = eINSTANCE.getOperatorInvocationParameterType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_PARAMETER_TYPE__VALUE =
        eINSTANCE.getOperatorInvocationParameterType_Value();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_PARAMETER_TYPE__NAME =
        eINSTANCE.getOperatorInvocationParameterType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl <em>Operator
     * Invocation Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_TYPE = eINSTANCE.getOperatorInvocationType();

    /**
     * The meta object literal for the '<em><b>Operator Invocation Head</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD =
        eINSTANCE.getOperatorInvocationType_OperatorInvocationHead();

    /**
     * The meta object literal for the '<em><b>Operator Invocation Body</b></em>' containment
     * reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY =
        eINSTANCE.getOperatorInvocationType_OperatorInvocationBody();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowsTypeImpl
     * <em>Operator Invocation Windows Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowsTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationWindowsType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_WINDOWS_TYPE = eINSTANCE.getOperatorInvocationWindowsType();

    /**
     * The meta object literal for the '<em><b>Window</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_WINDOWS_TYPE__WINDOW =
        eINSTANCE.getOperatorInvocationWindowsType_Window();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl <em>Operator
     * Invocation Window Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.OperatorInvocationWindowTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getOperatorInvocationWindowType()
     * @generated
     */
    EClass OPERATOR_INVOCATION_WINDOW_TYPE = eINSTANCE.getOperatorInvocationWindowType();

    /**
     * The meta object literal for the '<em><b>Eviction Policy</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY =
        eINSTANCE.getOperatorInvocationWindowType_EvictionPolicy();

    /**
     * The meta object literal for the '<em><b>Trigger Policy</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY =
        eINSTANCE.getOperatorInvocationWindowType_TriggerPolicy();

    /**
     * The meta object literal for the '<em><b>Partitioned</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED =
        eINSTANCE.getOperatorInvocationWindowType_Partitioned();

    /**
     * The meta object literal for the '<em><b>Port Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME =
        eINSTANCE.getOperatorInvocationWindowType_PortName();

    /**
     * The meta object literal for the '<em><b>Window Type</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE =
        eINSTANCE.getOperatorInvocationWindowType_WindowType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl <em>Source File
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceFileTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceFileType()
     * @generated
     */
    EClass SOURCE_FILE_TYPE = eINSTANCE.getSourceFileType();

    /**
     * The meta object literal for the '<em><b>Compilation Unit</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SOURCE_FILE_TYPE__COMPILATION_UNIT = eINSTANCE.getSourceFileType_CompilationUnit();

    /**
     * The meta object literal for the '<em><b>Uri</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SOURCE_FILE_TYPE__URI = eINSTANCE.getSourceFileType_Uri();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceLocationTypeImpl <em>Source Location
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceLocationTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceLocationType()
     * @generated
     */
    EClass SOURCE_LOCATION_TYPE = eINSTANCE.getSourceLocationType();

    /**
     * The meta object literal for the '<em><b>Column</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SOURCE_LOCATION_TYPE__COLUMN = eINSTANCE.getSourceLocationType_Column();

    /**
     * The meta object literal for the '<em><b>Line</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SOURCE_LOCATION_TYPE__LINE = eINSTANCE.getSourceLocationType_Line();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceModelTypeImpl <em>Source Model
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SourceModelTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSourceModelType()
     * @generated
     */
    EClass SOURCE_MODEL_TYPE = eINSTANCE.getSourceModelType();

    /**
     * The meta object literal for the '<em><b>Source File</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SOURCE_MODEL_TYPE__SOURCE_FILE = eINSTANCE.getSourceModelType_SourceFile();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocAnnotationTypeImpl <em>Spl Doc
     * Annotation Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocAnnotationTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocAnnotationType()
     * @generated
     */
    EClass SPL_DOC_ANNOTATION_TYPE = eINSTANCE.getSplDocAnnotationType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_DOC_ANNOTATION_TYPE__NAME = eINSTANCE.getSplDocAnnotationType_Name();

    /**
     * The meta object literal for the '<em><b>Target</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_DOC_ANNOTATION_TYPE__TARGET = eINSTANCE.getSplDocAnnotationType_Target();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocDescriptionTypeImpl <em>Spl Doc
     * Description Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocDescriptionTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocDescriptionType()
     * @generated
     */
    EClass SPL_DOC_DESCRIPTION_TYPE = eINSTANCE.getSplDocDescriptionType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_DOC_DESCRIPTION_TYPE__DESCRIPTION =
        eINSTANCE.getSplDocDescriptionType_Description();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocTypeImpl <em>Spl Doc Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplDocTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplDocType()
     * @generated
     */
    EClass SPL_DOC_TYPE = eINSTANCE.getSplDocType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SPL_DOC_TYPE__DESCRIPTION = eINSTANCE.getSplDocType_Description();

    /**
     * The meta object literal for the '<em><b>Annotation</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SPL_DOC_TYPE__ANNOTATION = eINSTANCE.getSplDocType_Annotation();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.SplNamespaceTypeImpl <em>Spl Namespace
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.SplNamespaceTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getSplNamespaceType()
     * @generated
     */
    EClass SPL_NAMESPACE_TYPE = eINSTANCE.getSplNamespaceType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SPL_NAMESPACE_TYPE__SPL_DOC = eINSTANCE.getSplNamespaceType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SPL_NAMESPACE_TYPE__NAME = eINSTANCE.getSplNamespaceType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeDefinitionTypeImpl <em>Type Definition
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeDefinitionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeDefinitionType()
     * @generated
     */
    EClass TYPE_DEFINITION_TYPE = eINSTANCE.getTypeDefinitionType();

    /**
     * The meta object literal for the '<em><b>Spl Doc</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TYPE_DEFINITION_TYPE__SPL_DOC = eINSTANCE.getTypeDefinitionType_SplDoc();

    /**
     * The meta object literal for the '<em><b>Modifiers</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TYPE_DEFINITION_TYPE__MODIFIERS = eINSTANCE.getTypeDefinitionType_Modifiers();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TYPE_DEFINITION_TYPE__NAME = eINSTANCE.getTypeDefinitionType_Name();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TYPE_DEFINITION_TYPE__VALUE = eINSTANCE.getTypeDefinitionType_Value();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifiersTypeImpl <em>Type Modifiers
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifiersTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifiersType()
     * @generated
     */
    EClass TYPE_MODIFIERS_TYPE = eINSTANCE.getTypeModifiersType();

    /**
     * The meta object literal for the '<em><b>Modifier</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TYPE_MODIFIERS_TYPE__MODIFIER = eINSTANCE.getTypeModifiersType_Modifier();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifierTypeImpl <em>Type Modifier
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.TypeModifierTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierType()
     * @generated
     */
    EClass TYPE_MODIFIER_TYPE = eINSTANCE.getTypeModifierType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TYPE_MODIFIER_TYPE__NAME = eINSTANCE.getTypeModifierType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectivesTypeImpl <em>Use Directives
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectivesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getUseDirectivesType()
     * @generated
     */
    EClass USE_DIRECTIVES_TYPE = eINSTANCE.getUseDirectivesType();

    /**
     * The meta object literal for the '<em><b>Use Directive</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference USE_DIRECTIVES_TYPE__USE_DIRECTIVE = eINSTANCE.getUseDirectivesType_UseDirective();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl <em>Use Directive
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.UseDirectiveTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getUseDirectiveType()
     * @generated
     */
    EClass USE_DIRECTIVE_TYPE = eINSTANCE.getUseDirectiveType();

    /**
     * The meta object literal for the '<em><b>Namespace Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute USE_DIRECTIVE_TYPE__NAMESPACE_NAME = eINSTANCE.getUseDirectiveType_NamespaceName();

    /**
     * The meta object literal for the '<em><b>Tail</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute USE_DIRECTIVE_TYPE__TAIL = eINSTANCE.getUseDirectiveType_Tail();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl <em>Window Policy
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.WindowPolicyTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyType()
     * @generated
     */
    EClass WINDOW_POLICY_TYPE = eINSTANCE.getWindowPolicyType();

    /**
     * The meta object literal for the '<em><b>Attribute</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute WINDOW_POLICY_TYPE__ATTRIBUTE = eINSTANCE.getWindowPolicyType_Attribute();

    /**
     * The meta object literal for the '<em><b>Kind</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute WINDOW_POLICY_TYPE__KIND = eINSTANCE.getWindowPolicyType_Kind();

    /**
     * The meta object literal for the '<em><b>Size</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute WINDOW_POLICY_TYPE__SIZE = eINSTANCE.getWindowPolicyType_Size();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType <em>Composite Modifier
     * Enum Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierEnumType()
     * @generated
     */
    EEnum COMPOSITE_MODIFIER_ENUM_TYPE = eINSTANCE.getCompositeModifierEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
     * <em>Composite Parameter Expression Mode Enum Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeEnumType()
     * @generated
     */
    EEnum COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE =
        eINSTANCE.getCompositeParameterExpressionModeEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType <em>Function Modifier
     * Enum Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierEnumType()
     * @generated
     */
    EEnum FUNCTION_MODIFIER_ENUM_TYPE = eINSTANCE.getFunctionModifierEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType <em>Function
     * Parameter Modifier Enum Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierEnumType()
     * @generated
     */
    EEnum FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE = eINSTANCE.getFunctionParameterModifierEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType <em>Type Modifier Enum
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierEnumType()
     * @generated
     */
    EEnum TYPE_MODIFIER_ENUM_TYPE = eINSTANCE.getTypeModifierEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType <em>Window Policy Kind
     * Enum Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyKindEnumType()
     * @generated
     */
    EEnum WINDOW_POLICY_KIND_ENUM_TYPE = eINSTANCE.getWindowPolicyKindEnumType();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType <em>Window Type Enum
     * Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
     * @see com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowTypeEnumType()
     * @generated
     */
    EEnum WINDOW_TYPE_ENUM_TYPE = eINSTANCE.getWindowTypeEnumType();

    /**
     * The meta object literal for the '<em>Composite Modifier Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeModifierEnumTypeObject()
     * @generated
     */
    EDataType COMPOSITE_MODIFIER_ENUM_TYPE_OBJECT = eINSTANCE.getCompositeModifierEnumTypeObject();

    /**
     * The meta object literal for the '<em>Composite Parameter Expression Mode Enum Type
     * Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getCompositeParameterExpressionModeEnumTypeObject()
     * @generated
     */
    EDataType COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE_OBJECT =
        eINSTANCE.getCompositeParameterExpressionModeEnumTypeObject();

    /**
     * The meta object literal for the '<em>Function Modifier Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionModifierEnumTypeObject()
     * @generated
     */
    EDataType FUNCTION_MODIFIER_ENUM_TYPE_OBJECT = eINSTANCE.getFunctionModifierEnumTypeObject();

    /**
     * The meta object literal for the '<em>Function Parameter Modifier Enum Type Object</em>' data
     * type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getFunctionParameterModifierEnumTypeObject()
     * @generated
     */
    EDataType FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE_OBJECT =
        eINSTANCE.getFunctionParameterModifierEnumTypeObject();

    /**
     * The meta object literal for the '<em>Type Modifier Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getTypeModifierEnumTypeObject()
     * @generated
     */
    EDataType TYPE_MODIFIER_ENUM_TYPE_OBJECT = eINSTANCE.getTypeModifierEnumTypeObject();

    /**
     * The meta object literal for the '<em>Window Policy Kind Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowPolicyKindEnumTypeObject()
     * @generated
     */
    EDataType WINDOW_POLICY_KIND_ENUM_TYPE_OBJECT = eINSTANCE.getWindowPolicyKindEnumTypeObject();

    /**
     * The meta object literal for the '<em>Window Type Enum Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType
     * @see
     *     com.ibm.xmlns.prod.streams.spl.source.code.impl.CodePackageImpl#getWindowTypeEnumTypeObject()
     * @generated
     */
    EDataType WINDOW_TYPE_ENUM_TYPE_OBJECT = eINSTANCE.getWindowTypeEnumTypeObject();
  }
} // CodePackage
