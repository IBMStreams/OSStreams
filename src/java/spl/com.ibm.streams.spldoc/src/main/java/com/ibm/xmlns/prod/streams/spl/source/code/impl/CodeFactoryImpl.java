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
package com.ibm.xmlns.prod.streams.spl.source.code.impl;

import com.ibm.xmlns.prod.streams.spl.source.code.*;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.impl.EFactoryImpl;
import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public class CodeFactoryImpl extends EFactoryImpl implements CodeFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CodeFactory init() {
    try {
      CodeFactory theCodeFactory =
          (CodeFactory) EPackage.Registry.INSTANCE.getEFactory(CodePackage.eNS_URI);
      if (theCodeFactory != null) {
        return theCodeFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new CodeFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeFactoryImpl() {
    super();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public EObject create(EClass eClass) {
    switch (eClass.getClassifierID()) {
      case CodePackage.COMPILATION_UNIT_TYPE:
        return createCompilationUnitType();
      case CodePackage.COMPOSITE_BODY_TYPE:
        return createCompositeBodyType();
      case CodePackage.COMPOSITE_DEFINITION_TYPE:
        return createCompositeDefinitionType();
      case CodePackage.COMPOSITE_GRAPH_TYPE:
        return createCompositeGraphType();
      case CodePackage.COMPOSITE_HEAD_TYPE:
        return createCompositeHeadType();
      case CodePackage.COMPOSITE_INPUTS_TYPE:
        return createCompositeInputsType();
      case CodePackage.COMPOSITE_MODIFIERS_TYPE:
        return createCompositeModifiersType();
      case CodePackage.COMPOSITE_MODIFIER_TYPE:
        return createCompositeModifierType();
      case CodePackage.COMPOSITE_OUTPUTS_TYPE:
        return createCompositeOutputsType();
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE:
        return createCompositeParameterExpressionModeType();
      case CodePackage.COMPOSITE_PARAMETERS_TYPE:
        return createCompositeParametersType();
      case CodePackage.COMPOSITE_PARAMETER_TYPE:
        return createCompositeParameterType();
      case CodePackage.COMPOSITE_PORT_TYPE:
        return createCompositePortType();
      case CodePackage.COMPOSITE_TYPES_TYPE:
        return createCompositeTypesType();
      case CodePackage.CONFIG_OPTION_TYPE:
        return createConfigOptionType();
      case CodePackage.CONFIGS_TYPE:
        return createConfigsType();
      case CodePackage.CONFIG_TYPE:
        return createConfigType();
      case CodePackage.CONFIG_VALUE_PARAMETER_TYPE:
        return createConfigValueParameterType();
      case CodePackage.DEFINITIONS_TYPE:
        return createDefinitionsType();
      case CodePackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case CodePackage.EXPRESSION_TYPE:
        return createExpressionType();
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE:
        return createExtendedSourceLocationType();
      case CodePackage.FUNCTION_DEFINITION_TYPE:
        return createFunctionDefinitionType();
      case CodePackage.FUNCTION_HEAD_TYPE:
        return createFunctionHeadType();
      case CodePackage.FUNCTION_MODIFIERS_TYPE:
        return createFunctionModifiersType();
      case CodePackage.FUNCTION_MODIFIER_TYPE:
        return createFunctionModifierType();
      case CodePackage.FUNCTION_PARAMETER_MODIFIERS_TYPE:
        return createFunctionParameterModifiersType();
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_TYPE:
        return createFunctionParameterModifierType();
      case CodePackage.FUNCTION_PARAMETERS_TYPE:
        return createFunctionParametersType();
      case CodePackage.FUNCTION_PARAMETER_TYPE:
        return createFunctionParameterType();
      case CodePackage.ON_PROCESS_TYPE:
        return createOnProcessType();
      case CodePackage.ON_PUNCT_TYPE:
        return createOnPunctType();
      case CodePackage.ON_TUPLE_TYPE:
        return createOnTupleType();
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE:
        return createOperatorInvocationAttributeAssignmentType();
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE:
        return createOperatorInvocationBodyType();
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE:
        return createOperatorInvocationHeadType();
      case CodePackage.OPERATOR_INVOCATION_INPUT_STREAM_TYPE:
        return createOperatorInvocationInputStreamType();
      case CodePackage.OPERATOR_INVOCATION_INPUTS_TYPE:
        return createOperatorInvocationInputsType();
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE:
        return createOperatorInvocationInputType();
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE:
        return createOperatorInvocationLogicType();
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE:
        return createOperatorInvocationOutputAssignmentsType();
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE:
        return createOperatorInvocationOutputAssignmentType();
      case CodePackage.OPERATOR_INVOCATION_OUTPUTS_TYPE:
        return createOperatorInvocationOutputsType();
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_TYPE:
        return createOperatorInvocationOutputType();
      case CodePackage.OPERATOR_INVOCATION_PARAMETERS_TYPE:
        return createOperatorInvocationParametersType();
      case CodePackage.OPERATOR_INVOCATION_PARAMETER_TYPE:
        return createOperatorInvocationParameterType();
      case CodePackage.OPERATOR_INVOCATION_TYPE:
        return createOperatorInvocationType();
      case CodePackage.OPERATOR_INVOCATION_WINDOWS_TYPE:
        return createOperatorInvocationWindowsType();
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE:
        return createOperatorInvocationWindowType();
      case CodePackage.SOURCE_FILE_TYPE:
        return createSourceFileType();
      case CodePackage.SOURCE_LOCATION_TYPE:
        return createSourceLocationType();
      case CodePackage.SOURCE_MODEL_TYPE:
        return createSourceModelType();
      case CodePackage.SPL_DOC_ANNOTATION_TYPE:
        return createSplDocAnnotationType();
      case CodePackage.SPL_DOC_DESCRIPTION_TYPE:
        return createSplDocDescriptionType();
      case CodePackage.SPL_DOC_TYPE:
        return createSplDocType();
      case CodePackage.SPL_NAMESPACE_TYPE:
        return createSplNamespaceType();
      case CodePackage.TYPE_DEFINITION_TYPE:
        return createTypeDefinitionType();
      case CodePackage.TYPE_MODIFIERS_TYPE:
        return createTypeModifiersType();
      case CodePackage.TYPE_MODIFIER_TYPE:
        return createTypeModifierType();
      case CodePackage.USE_DIRECTIVES_TYPE:
        return createUseDirectivesType();
      case CodePackage.USE_DIRECTIVE_TYPE:
        return createUseDirectiveType();
      case CodePackage.WINDOW_POLICY_TYPE:
        return createWindowPolicyType();
      default:
        throw new IllegalArgumentException(
            "The class '" + eClass.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public Object createFromString(EDataType eDataType, String initialValue) {
    switch (eDataType.getClassifierID()) {
      case CodePackage.COMPOSITE_MODIFIER_ENUM_TYPE:
        return createCompositeModifierEnumTypeFromString(eDataType, initialValue);
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE:
        return createCompositeParameterExpressionModeEnumTypeFromString(eDataType, initialValue);
      case CodePackage.FUNCTION_MODIFIER_ENUM_TYPE:
        return createFunctionModifierEnumTypeFromString(eDataType, initialValue);
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE:
        return createFunctionParameterModifierEnumTypeFromString(eDataType, initialValue);
      case CodePackage.TYPE_MODIFIER_ENUM_TYPE:
        return createTypeModifierEnumTypeFromString(eDataType, initialValue);
      case CodePackage.WINDOW_POLICY_KIND_ENUM_TYPE:
        return createWindowPolicyKindEnumTypeFromString(eDataType, initialValue);
      case CodePackage.WINDOW_TYPE_ENUM_TYPE:
        return createWindowTypeEnumTypeFromString(eDataType, initialValue);
      case CodePackage.COMPOSITE_MODIFIER_ENUM_TYPE_OBJECT:
        return createCompositeModifierEnumTypeObjectFromString(eDataType, initialValue);
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE_OBJECT:
        return createCompositeParameterExpressionModeEnumTypeObjectFromString(
            eDataType, initialValue);
      case CodePackage.FUNCTION_MODIFIER_ENUM_TYPE_OBJECT:
        return createFunctionModifierEnumTypeObjectFromString(eDataType, initialValue);
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE_OBJECT:
        return createFunctionParameterModifierEnumTypeObjectFromString(eDataType, initialValue);
      case CodePackage.TYPE_MODIFIER_ENUM_TYPE_OBJECT:
        return createTypeModifierEnumTypeObjectFromString(eDataType, initialValue);
      case CodePackage.WINDOW_POLICY_KIND_ENUM_TYPE_OBJECT:
        return createWindowPolicyKindEnumTypeObjectFromString(eDataType, initialValue);
      case CodePackage.WINDOW_TYPE_ENUM_TYPE_OBJECT:
        return createWindowTypeEnumTypeObjectFromString(eDataType, initialValue);
      default:
        throw new IllegalArgumentException(
            "The datatype '" + eDataType.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public String convertToString(EDataType eDataType, Object instanceValue) {
    switch (eDataType.getClassifierID()) {
      case CodePackage.COMPOSITE_MODIFIER_ENUM_TYPE:
        return convertCompositeModifierEnumTypeToString(eDataType, instanceValue);
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE:
        return convertCompositeParameterExpressionModeEnumTypeToString(eDataType, instanceValue);
      case CodePackage.FUNCTION_MODIFIER_ENUM_TYPE:
        return convertFunctionModifierEnumTypeToString(eDataType, instanceValue);
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE:
        return convertFunctionParameterModifierEnumTypeToString(eDataType, instanceValue);
      case CodePackage.TYPE_MODIFIER_ENUM_TYPE:
        return convertTypeModifierEnumTypeToString(eDataType, instanceValue);
      case CodePackage.WINDOW_POLICY_KIND_ENUM_TYPE:
        return convertWindowPolicyKindEnumTypeToString(eDataType, instanceValue);
      case CodePackage.WINDOW_TYPE_ENUM_TYPE:
        return convertWindowTypeEnumTypeToString(eDataType, instanceValue);
      case CodePackage.COMPOSITE_MODIFIER_ENUM_TYPE_OBJECT:
        return convertCompositeModifierEnumTypeObjectToString(eDataType, instanceValue);
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE_OBJECT:
        return convertCompositeParameterExpressionModeEnumTypeObjectToString(
            eDataType, instanceValue);
      case CodePackage.FUNCTION_MODIFIER_ENUM_TYPE_OBJECT:
        return convertFunctionModifierEnumTypeObjectToString(eDataType, instanceValue);
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE_OBJECT:
        return convertFunctionParameterModifierEnumTypeObjectToString(eDataType, instanceValue);
      case CodePackage.TYPE_MODIFIER_ENUM_TYPE_OBJECT:
        return convertTypeModifierEnumTypeObjectToString(eDataType, instanceValue);
      case CodePackage.WINDOW_POLICY_KIND_ENUM_TYPE_OBJECT:
        return convertWindowPolicyKindEnumTypeObjectToString(eDataType, instanceValue);
      case CodePackage.WINDOW_TYPE_ENUM_TYPE_OBJECT:
        return convertWindowTypeEnumTypeObjectToString(eDataType, instanceValue);
      default:
        throw new IllegalArgumentException(
            "The datatype '" + eDataType.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompilationUnitType createCompilationUnitType() {
    CompilationUnitTypeImpl compilationUnitType = new CompilationUnitTypeImpl();
    return compilationUnitType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeBodyType createCompositeBodyType() {
    CompositeBodyTypeImpl compositeBodyType = new CompositeBodyTypeImpl();
    return compositeBodyType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeDefinitionType createCompositeDefinitionType() {
    CompositeDefinitionTypeImpl compositeDefinitionType = new CompositeDefinitionTypeImpl();
    return compositeDefinitionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeGraphType createCompositeGraphType() {
    CompositeGraphTypeImpl compositeGraphType = new CompositeGraphTypeImpl();
    return compositeGraphType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeHeadType createCompositeHeadType() {
    CompositeHeadTypeImpl compositeHeadType = new CompositeHeadTypeImpl();
    return compositeHeadType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeInputsType createCompositeInputsType() {
    CompositeInputsTypeImpl compositeInputsType = new CompositeInputsTypeImpl();
    return compositeInputsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeModifiersType createCompositeModifiersType() {
    CompositeModifiersTypeImpl compositeModifiersType = new CompositeModifiersTypeImpl();
    return compositeModifiersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeModifierType createCompositeModifierType() {
    CompositeModifierTypeImpl compositeModifierType = new CompositeModifierTypeImpl();
    return compositeModifierType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeOutputsType createCompositeOutputsType() {
    CompositeOutputsTypeImpl compositeOutputsType = new CompositeOutputsTypeImpl();
    return compositeOutputsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterExpressionModeType createCompositeParameterExpressionModeType() {
    CompositeParameterExpressionModeTypeImpl compositeParameterExpressionModeType =
        new CompositeParameterExpressionModeTypeImpl();
    return compositeParameterExpressionModeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParametersType createCompositeParametersType() {
    CompositeParametersTypeImpl compositeParametersType = new CompositeParametersTypeImpl();
    return compositeParametersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterType createCompositeParameterType() {
    CompositeParameterTypeImpl compositeParameterType = new CompositeParameterTypeImpl();
    return compositeParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositePortType createCompositePortType() {
    CompositePortTypeImpl compositePortType = new CompositePortTypeImpl();
    return compositePortType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeTypesType createCompositeTypesType() {
    CompositeTypesTypeImpl compositeTypesType = new CompositeTypesTypeImpl();
    return compositeTypesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigOptionType createConfigOptionType() {
    ConfigOptionTypeImpl configOptionType = new ConfigOptionTypeImpl();
    return configOptionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigsType createConfigsType() {
    ConfigsTypeImpl configsType = new ConfigsTypeImpl();
    return configsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigType createConfigType() {
    ConfigTypeImpl configType = new ConfigTypeImpl();
    return configType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ConfigValueParameterType createConfigValueParameterType() {
    ConfigValueParameterTypeImpl configValueParameterType = new ConfigValueParameterTypeImpl();
    return configValueParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DefinitionsType createDefinitionsType() {
    DefinitionsTypeImpl definitionsType = new DefinitionsTypeImpl();
    return definitionsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DocumentRoot createDocumentRoot() {
    DocumentRootImpl documentRoot = new DocumentRootImpl();
    return documentRoot;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ExpressionType createExpressionType() {
    ExpressionTypeImpl expressionType = new ExpressionTypeImpl();
    return expressionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ExtendedSourceLocationType createExtendedSourceLocationType() {
    ExtendedSourceLocationTypeImpl extendedSourceLocationType =
        new ExtendedSourceLocationTypeImpl();
    return extendedSourceLocationType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionDefinitionType createFunctionDefinitionType() {
    FunctionDefinitionTypeImpl functionDefinitionType = new FunctionDefinitionTypeImpl();
    return functionDefinitionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionHeadType createFunctionHeadType() {
    FunctionHeadTypeImpl functionHeadType = new FunctionHeadTypeImpl();
    return functionHeadType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionModifiersType createFunctionModifiersType() {
    FunctionModifiersTypeImpl functionModifiersType = new FunctionModifiersTypeImpl();
    return functionModifiersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionModifierType createFunctionModifierType() {
    FunctionModifierTypeImpl functionModifierType = new FunctionModifierTypeImpl();
    return functionModifierType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParameterModifiersType createFunctionParameterModifiersType() {
    FunctionParameterModifiersTypeImpl functionParameterModifiersType =
        new FunctionParameterModifiersTypeImpl();
    return functionParameterModifiersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParameterModifierType createFunctionParameterModifierType() {
    FunctionParameterModifierTypeImpl functionParameterModifierType =
        new FunctionParameterModifierTypeImpl();
    return functionParameterModifierType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParametersType createFunctionParametersType() {
    FunctionParametersTypeImpl functionParametersType = new FunctionParametersTypeImpl();
    return functionParametersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParameterType createFunctionParameterType() {
    FunctionParameterTypeImpl functionParameterType = new FunctionParameterTypeImpl();
    return functionParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OnProcessType createOnProcessType() {
    OnProcessTypeImpl onProcessType = new OnProcessTypeImpl();
    return onProcessType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OnPunctType createOnPunctType() {
    OnPunctTypeImpl onPunctType = new OnPunctTypeImpl();
    return onPunctType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OnTupleType createOnTupleType() {
    OnTupleTypeImpl onTupleType = new OnTupleTypeImpl();
    return onTupleType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationAttributeAssignmentType
      createOperatorInvocationAttributeAssignmentType() {
    OperatorInvocationAttributeAssignmentTypeImpl operatorInvocationAttributeAssignmentType =
        new OperatorInvocationAttributeAssignmentTypeImpl();
    return operatorInvocationAttributeAssignmentType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationBodyType createOperatorInvocationBodyType() {
    OperatorInvocationBodyTypeImpl operatorInvocationBodyType =
        new OperatorInvocationBodyTypeImpl();
    return operatorInvocationBodyType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationHeadType createOperatorInvocationHeadType() {
    OperatorInvocationHeadTypeImpl operatorInvocationHeadType =
        new OperatorInvocationHeadTypeImpl();
    return operatorInvocationHeadType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationInputStreamType createOperatorInvocationInputStreamType() {
    OperatorInvocationInputStreamTypeImpl operatorInvocationInputStreamType =
        new OperatorInvocationInputStreamTypeImpl();
    return operatorInvocationInputStreamType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationInputsType createOperatorInvocationInputsType() {
    OperatorInvocationInputsTypeImpl operatorInvocationInputsType =
        new OperatorInvocationInputsTypeImpl();
    return operatorInvocationInputsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationInputType createOperatorInvocationInputType() {
    OperatorInvocationInputTypeImpl operatorInvocationInputType =
        new OperatorInvocationInputTypeImpl();
    return operatorInvocationInputType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationLogicType createOperatorInvocationLogicType() {
    OperatorInvocationLogicTypeImpl operatorInvocationLogicType =
        new OperatorInvocationLogicTypeImpl();
    return operatorInvocationLogicType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationOutputAssignmentsType createOperatorInvocationOutputAssignmentsType() {
    OperatorInvocationOutputAssignmentsTypeImpl operatorInvocationOutputAssignmentsType =
        new OperatorInvocationOutputAssignmentsTypeImpl();
    return operatorInvocationOutputAssignmentsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationOutputAssignmentType createOperatorInvocationOutputAssignmentType() {
    OperatorInvocationOutputAssignmentTypeImpl operatorInvocationOutputAssignmentType =
        new OperatorInvocationOutputAssignmentTypeImpl();
    return operatorInvocationOutputAssignmentType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationOutputsType createOperatorInvocationOutputsType() {
    OperatorInvocationOutputsTypeImpl operatorInvocationOutputsType =
        new OperatorInvocationOutputsTypeImpl();
    return operatorInvocationOutputsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationOutputType createOperatorInvocationOutputType() {
    OperatorInvocationOutputTypeImpl operatorInvocationOutputType =
        new OperatorInvocationOutputTypeImpl();
    return operatorInvocationOutputType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationParametersType createOperatorInvocationParametersType() {
    OperatorInvocationParametersTypeImpl operatorInvocationParametersType =
        new OperatorInvocationParametersTypeImpl();
    return operatorInvocationParametersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationParameterType createOperatorInvocationParameterType() {
    OperatorInvocationParameterTypeImpl operatorInvocationParameterType =
        new OperatorInvocationParameterTypeImpl();
    return operatorInvocationParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationType createOperatorInvocationType() {
    OperatorInvocationTypeImpl operatorInvocationType = new OperatorInvocationTypeImpl();
    return operatorInvocationType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationWindowsType createOperatorInvocationWindowsType() {
    OperatorInvocationWindowsTypeImpl operatorInvocationWindowsType =
        new OperatorInvocationWindowsTypeImpl();
    return operatorInvocationWindowsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorInvocationWindowType createOperatorInvocationWindowType() {
    OperatorInvocationWindowTypeImpl operatorInvocationWindowType =
        new OperatorInvocationWindowTypeImpl();
    return operatorInvocationWindowType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SourceFileType createSourceFileType() {
    SourceFileTypeImpl sourceFileType = new SourceFileTypeImpl();
    return sourceFileType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SourceLocationType createSourceLocationType() {
    SourceLocationTypeImpl sourceLocationType = new SourceLocationTypeImpl();
    return sourceLocationType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SourceModelType createSourceModelType() {
    SourceModelTypeImpl sourceModelType = new SourceModelTypeImpl();
    return sourceModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocAnnotationType createSplDocAnnotationType() {
    SplDocAnnotationTypeImpl splDocAnnotationType = new SplDocAnnotationTypeImpl();
    return splDocAnnotationType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocDescriptionType createSplDocDescriptionType() {
    SplDocDescriptionTypeImpl splDocDescriptionType = new SplDocDescriptionTypeImpl();
    return splDocDescriptionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocType createSplDocType() {
    SplDocTypeImpl splDocType = new SplDocTypeImpl();
    return splDocType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplNamespaceType createSplNamespaceType() {
    SplNamespaceTypeImpl splNamespaceType = new SplNamespaceTypeImpl();
    return splNamespaceType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public TypeDefinitionType createTypeDefinitionType() {
    TypeDefinitionTypeImpl typeDefinitionType = new TypeDefinitionTypeImpl();
    return typeDefinitionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public TypeModifiersType createTypeModifiersType() {
    TypeModifiersTypeImpl typeModifiersType = new TypeModifiersTypeImpl();
    return typeModifiersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public TypeModifierType createTypeModifierType() {
    TypeModifierTypeImpl typeModifierType = new TypeModifierTypeImpl();
    return typeModifierType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UseDirectivesType createUseDirectivesType() {
    UseDirectivesTypeImpl useDirectivesType = new UseDirectivesTypeImpl();
    return useDirectivesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UseDirectiveType createUseDirectiveType() {
    UseDirectiveTypeImpl useDirectiveType = new UseDirectiveTypeImpl();
    return useDirectiveType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyType createWindowPolicyType() {
    WindowPolicyTypeImpl windowPolicyType = new WindowPolicyTypeImpl();
    return windowPolicyType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeModifierEnumType createCompositeModifierEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    CompositeModifierEnumType result = CompositeModifierEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertCompositeModifierEnumTypeToString(
      EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterExpressionModeEnumType
      createCompositeParameterExpressionModeEnumTypeFromString(
          EDataType eDataType, String initialValue) {
    CompositeParameterExpressionModeEnumType result =
        CompositeParameterExpressionModeEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertCompositeParameterExpressionModeEnumTypeToString(
      EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionModifierEnumType createFunctionModifierEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    FunctionModifierEnumType result = FunctionModifierEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertFunctionModifierEnumTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParameterModifierEnumType createFunctionParameterModifierEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    FunctionParameterModifierEnumType result = FunctionParameterModifierEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertFunctionParameterModifierEnumTypeToString(
      EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public TypeModifierEnumType createTypeModifierEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    TypeModifierEnumType result = TypeModifierEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertTypeModifierEnumTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyKindEnumType createWindowPolicyKindEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowPolicyKindEnumType result = WindowPolicyKindEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowPolicyKindEnumTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowTypeEnumType createWindowTypeEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowTypeEnumType result = WindowTypeEnumType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowTypeEnumTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeModifierEnumType createCompositeModifierEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createCompositeModifierEnumTypeFromString(
        CodePackage.Literals.COMPOSITE_MODIFIER_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertCompositeModifierEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertCompositeModifierEnumTypeToString(
        CodePackage.Literals.COMPOSITE_MODIFIER_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParameterExpressionModeEnumType
      createCompositeParameterExpressionModeEnumTypeObjectFromString(
          EDataType eDataType, String initialValue) {
    return createCompositeParameterExpressionModeEnumTypeFromString(
        CodePackage.Literals.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertCompositeParameterExpressionModeEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertCompositeParameterExpressionModeEnumTypeToString(
        CodePackage.Literals.COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionModifierEnumType createFunctionModifierEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createFunctionModifierEnumTypeFromString(
        CodePackage.Literals.FUNCTION_MODIFIER_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertFunctionModifierEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertFunctionModifierEnumTypeToString(
        CodePackage.Literals.FUNCTION_MODIFIER_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionParameterModifierEnumType createFunctionParameterModifierEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createFunctionParameterModifierEnumTypeFromString(
        CodePackage.Literals.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertFunctionParameterModifierEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertFunctionParameterModifierEnumTypeToString(
        CodePackage.Literals.FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public TypeModifierEnumType createTypeModifierEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createTypeModifierEnumTypeFromString(
        CodePackage.Literals.TYPE_MODIFIER_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertTypeModifierEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertTypeModifierEnumTypeToString(
        CodePackage.Literals.TYPE_MODIFIER_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPolicyKindEnumType createWindowPolicyKindEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowPolicyKindEnumTypeFromString(
        CodePackage.Literals.WINDOW_POLICY_KIND_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowPolicyKindEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertWindowPolicyKindEnumTypeToString(
        CodePackage.Literals.WINDOW_POLICY_KIND_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowTypeEnumType createWindowTypeEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowTypeEnumTypeFromString(
        CodePackage.Literals.WINDOW_TYPE_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowTypeEnumTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertWindowTypeEnumTypeToString(
        CodePackage.Literals.WINDOW_TYPE_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodePackage getCodePackage() {
    return (CodePackage) getEPackage();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @deprecated
   * @generated
   */
  @Deprecated
  public static CodePackage getPackage() {
    return CodePackage.eINSTANCE;
  }
} // CodeFactoryImpl
