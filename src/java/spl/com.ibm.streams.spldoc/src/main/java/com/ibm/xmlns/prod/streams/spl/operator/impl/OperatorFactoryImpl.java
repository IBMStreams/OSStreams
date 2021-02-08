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
package com.ibm.xmlns.prod.streams.spl.operator.impl;

import com.ibm.xmlns.prod.streams.spl.operator.*;
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
public class OperatorFactoryImpl extends EFactoryImpl implements OperatorFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static OperatorFactory init() {
    try {
      OperatorFactory theOperatorFactory =
          (OperatorFactory) EPackage.Registry.INSTANCE.getEFactory(OperatorPackage.eNS_URI);
      if (theOperatorFactory != null) {
        return theOperatorFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new OperatorFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorFactoryImpl() {
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
      case OperatorPackage.CODE_TEMPLATES_TYPE:
        return createCodeTemplatesType();
      case OperatorPackage.CODE_TEMPLATE_TYPE:
        return createCodeTemplateType();
      case OperatorPackage.CONTEXT_TYPE:
        return createContextType();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE:
        return createCustomOutputFunctionSetType();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE:
        return createCustomOutputFunctionsType();
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE:
        return createCustomOutputFunctionType();
      case OperatorPackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case OperatorPackage.ENUMERATIONS_TYPE:
        return createEnumerationsType();
      case OperatorPackage.ENUMERATION_TYPE:
        return createEnumerationType();
      case OperatorPackage.ICON_URI_TYPE:
        return createIconUriType();
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE:
        return createInputPortOpenSetType();
      case OperatorPackage.INPUT_PORT_SET_TYPE:
        return createInputPortSetType();
      case OperatorPackage.INPUT_PORTS_TYPE:
        return createInputPortsType();
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE:
        return createJavaOpContextType();
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE:
        return createJavaOpExecutionSettingsType();
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE:
        return createJavaOpInputPortOpenSetType();
      case OperatorPackage.JAVA_OP_INPUT_PORT_SET_TYPE:
        return createJavaOpInputPortSetType();
      case OperatorPackage.JAVA_OP_INPUT_PORTS_TYPE:
        return createJavaOpInputPortsType();
      case OperatorPackage.JAVA_OP_LIBRARY_DEPENDENCIES_TYPE:
        return createJavaOpLibraryDependenciesType();
      case OperatorPackage.JAVA_OP_MODEL_TYPE:
        return createJavaOpModelType();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE:
        return createJavaOpOutputPortOpenSetType();
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_SET_TYPE:
        return createJavaOpOutputPortSetType();
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE:
        return createJavaOpOutputPortsType();
      case OperatorPackage.JAVA_OP_PARAMETERS_TYPE:
        return createJavaOpParametersType();
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE:
        return createJavaOpParameterType();
      case OperatorPackage.JAVA_OP_VM_ARGS_TYPE:
        return createJavaOpVMArgsType();
      case OperatorPackage.LIBRARY_DEPENDENCIES_TYPE:
        return createLibraryDependenciesType();
      case OperatorPackage.METRICS_TYPE:
        return createMetricsType();
      case OperatorPackage.METRIC_TYPE:
        return createMetricType();
      case OperatorPackage.OPERATOR_MODEL_TYPE:
        return createOperatorModelType();
      case OperatorPackage.OP_MODEL_TYPE:
        return createOpModelType();
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE:
        return createOptionalPortScopeType();
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE:
        return createOutputFunctionsType();
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE:
        return createOutputPortOpenSetType();
      case OperatorPackage.OUTPUT_PORT_SET_TYPE:
        return createOutputPortSetType();
      case OperatorPackage.OUTPUT_PORTS_TYPE:
        return createOutputPortsType();
      case OperatorPackage.PARAMETERS_TYPE:
        return createParametersType();
      case OperatorPackage.PARAMETER_TYPE:
        return createParameterType();
      case OperatorPackage.PORT_SCOPE_TYPE:
        return createPortScopeType();
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE:
        return createSplExpressionTreeType();
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
      case OperatorPackage.EXPRESSION_MODE_TYPE:
        return createExpressionModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.INCREMENTAL_COMPILATION_STRATEGY_TYPE:
        return createIncrementalCompilationStrategyTypeFromString(eDataType, initialValue);
      case OperatorPackage.JAVA_OP_EXPRESSION_MODE_TYPE:
        return createJavaOpExpressionModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.METRIC_KIND_TYPE:
        return createMetricKindTypeFromString(eDataType, initialValue);
      case OperatorPackage.SINGLE_THREADED_CONTEXT_TYPE:
        return createSingleThreadedContextTypeFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_EXPRESSION_MODE_TYPE:
        return createWindowExpressionModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.WINDOWING_MODE_TYPE:
        return createWindowingModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_PUNCTUATION_INPUT_MODE_TYPE:
        return createWindowPunctuationInputModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE:
        return createWindowPunctuationOutputModeTypeFromString(eDataType, initialValue);
      case OperatorPackage.EXPRESSION_MODE_TYPE_OBJECT:
        return createExpressionModeTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.INCREMENTAL_COMPILATION_STRATEGY_TYPE_OBJECT:
        return createIncrementalCompilationStrategyTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.JAVA_OP_EXPRESSION_MODE_TYPE_OBJECT:
        return createJavaOpExpressionModeTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.METRIC_KIND_TYPE_OBJECT:
        return createMetricKindTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.SINGLE_THREADED_CONTEXT_TYPE_OBJECT:
        return createSingleThreadedContextTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_EXPRESSION_MODE_TYPE_OBJECT:
        return createWindowExpressionModeTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.WINDOWING_MODE_TYPE_OBJECT:
        return createWindowingModeTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_PUNCTUATION_INPUT_MODE_TYPE_OBJECT:
        return createWindowPunctuationInputModeTypeObjectFromString(eDataType, initialValue);
      case OperatorPackage.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE_OBJECT:
        return createWindowPunctuationOutputModeTypeObjectFromString(eDataType, initialValue);
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
      case OperatorPackage.EXPRESSION_MODE_TYPE:
        return convertExpressionModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.INCREMENTAL_COMPILATION_STRATEGY_TYPE:
        return convertIncrementalCompilationStrategyTypeToString(eDataType, instanceValue);
      case OperatorPackage.JAVA_OP_EXPRESSION_MODE_TYPE:
        return convertJavaOpExpressionModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.METRIC_KIND_TYPE:
        return convertMetricKindTypeToString(eDataType, instanceValue);
      case OperatorPackage.SINGLE_THREADED_CONTEXT_TYPE:
        return convertSingleThreadedContextTypeToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_EXPRESSION_MODE_TYPE:
        return convertWindowExpressionModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.WINDOWING_MODE_TYPE:
        return convertWindowingModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_PUNCTUATION_INPUT_MODE_TYPE:
        return convertWindowPunctuationInputModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE:
        return convertWindowPunctuationOutputModeTypeToString(eDataType, instanceValue);
      case OperatorPackage.EXPRESSION_MODE_TYPE_OBJECT:
        return convertExpressionModeTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.INCREMENTAL_COMPILATION_STRATEGY_TYPE_OBJECT:
        return convertIncrementalCompilationStrategyTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.JAVA_OP_EXPRESSION_MODE_TYPE_OBJECT:
        return convertJavaOpExpressionModeTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.METRIC_KIND_TYPE_OBJECT:
        return convertMetricKindTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.SINGLE_THREADED_CONTEXT_TYPE_OBJECT:
        return convertSingleThreadedContextTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_EXPRESSION_MODE_TYPE_OBJECT:
        return convertWindowExpressionModeTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.WINDOWING_MODE_TYPE_OBJECT:
        return convertWindowingModeTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_PUNCTUATION_INPUT_MODE_TYPE_OBJECT:
        return convertWindowPunctuationInputModeTypeObjectToString(eDataType, instanceValue);
      case OperatorPackage.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE_OBJECT:
        return convertWindowPunctuationOutputModeTypeObjectToString(eDataType, instanceValue);
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
  public CodeTemplatesType createCodeTemplatesType() {
    CodeTemplatesTypeImpl codeTemplatesType = new CodeTemplatesTypeImpl();
    return codeTemplatesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeTemplateType createCodeTemplateType() {
    CodeTemplateTypeImpl codeTemplateType = new CodeTemplateTypeImpl();
    return codeTemplateType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ContextType createContextType() {
    ContextTypeImpl contextType = new ContextTypeImpl();
    return contextType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CustomOutputFunctionSetType createCustomOutputFunctionSetType() {
    CustomOutputFunctionSetTypeImpl customOutputFunctionSetType =
        new CustomOutputFunctionSetTypeImpl();
    return customOutputFunctionSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CustomOutputFunctionsType createCustomOutputFunctionsType() {
    CustomOutputFunctionsTypeImpl customOutputFunctionsType = new CustomOutputFunctionsTypeImpl();
    return customOutputFunctionsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CustomOutputFunctionType createCustomOutputFunctionType() {
    CustomOutputFunctionTypeImpl customOutputFunctionType = new CustomOutputFunctionTypeImpl();
    return customOutputFunctionType;
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
  public EnumerationsType createEnumerationsType() {
    EnumerationsTypeImpl enumerationsType = new EnumerationsTypeImpl();
    return enumerationsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumerationType createEnumerationType() {
    EnumerationTypeImpl enumerationType = new EnumerationTypeImpl();
    return enumerationType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IconUriType createIconUriType() {
    IconUriTypeImpl iconUriType = new IconUriTypeImpl();
    return iconUriType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InputPortOpenSetType createInputPortOpenSetType() {
    InputPortOpenSetTypeImpl inputPortOpenSetType = new InputPortOpenSetTypeImpl();
    return inputPortOpenSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InputPortSetType createInputPortSetType() {
    InputPortSetTypeImpl inputPortSetType = new InputPortSetTypeImpl();
    return inputPortSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InputPortsType createInputPortsType() {
    InputPortsTypeImpl inputPortsType = new InputPortsTypeImpl();
    return inputPortsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpContextType createJavaOpContextType() {
    JavaOpContextTypeImpl javaOpContextType = new JavaOpContextTypeImpl();
    return javaOpContextType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpExecutionSettingsType createJavaOpExecutionSettingsType() {
    JavaOpExecutionSettingsTypeImpl javaOpExecutionSettingsType =
        new JavaOpExecutionSettingsTypeImpl();
    return javaOpExecutionSettingsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpInputPortOpenSetType createJavaOpInputPortOpenSetType() {
    JavaOpInputPortOpenSetTypeImpl javaOpInputPortOpenSetType =
        new JavaOpInputPortOpenSetTypeImpl();
    return javaOpInputPortOpenSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpInputPortSetType createJavaOpInputPortSetType() {
    JavaOpInputPortSetTypeImpl javaOpInputPortSetType = new JavaOpInputPortSetTypeImpl();
    return javaOpInputPortSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpInputPortsType createJavaOpInputPortsType() {
    JavaOpInputPortsTypeImpl javaOpInputPortsType = new JavaOpInputPortsTypeImpl();
    return javaOpInputPortsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpLibraryDependenciesType createJavaOpLibraryDependenciesType() {
    JavaOpLibraryDependenciesTypeImpl javaOpLibraryDependenciesType =
        new JavaOpLibraryDependenciesTypeImpl();
    return javaOpLibraryDependenciesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpModelType createJavaOpModelType() {
    JavaOpModelTypeImpl javaOpModelType = new JavaOpModelTypeImpl();
    return javaOpModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpOutputPortOpenSetType createJavaOpOutputPortOpenSetType() {
    JavaOpOutputPortOpenSetTypeImpl javaOpOutputPortOpenSetType =
        new JavaOpOutputPortOpenSetTypeImpl();
    return javaOpOutputPortOpenSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpOutputPortSetType createJavaOpOutputPortSetType() {
    JavaOpOutputPortSetTypeImpl javaOpOutputPortSetType = new JavaOpOutputPortSetTypeImpl();
    return javaOpOutputPortSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpOutputPortsType createJavaOpOutputPortsType() {
    JavaOpOutputPortsTypeImpl javaOpOutputPortsType = new JavaOpOutputPortsTypeImpl();
    return javaOpOutputPortsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpParametersType createJavaOpParametersType() {
    JavaOpParametersTypeImpl javaOpParametersType = new JavaOpParametersTypeImpl();
    return javaOpParametersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpParameterType createJavaOpParameterType() {
    JavaOpParameterTypeImpl javaOpParameterType = new JavaOpParameterTypeImpl();
    return javaOpParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpVMArgsType createJavaOpVMArgsType() {
    JavaOpVMArgsTypeImpl javaOpVMArgsType = new JavaOpVMArgsTypeImpl();
    return javaOpVMArgsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LibraryDependenciesType createLibraryDependenciesType() {
    LibraryDependenciesTypeImpl libraryDependenciesType = new LibraryDependenciesTypeImpl();
    return libraryDependenciesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MetricsType createMetricsType() {
    MetricsTypeImpl metricsType = new MetricsTypeImpl();
    return metricsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MetricType createMetricType() {
    MetricTypeImpl metricType = new MetricTypeImpl();
    return metricType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorModelType createOperatorModelType() {
    OperatorModelTypeImpl operatorModelType = new OperatorModelTypeImpl();
    return operatorModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OpModelType createOpModelType() {
    OpModelTypeImpl opModelType = new OpModelTypeImpl();
    return opModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OptionalPortScopeType createOptionalPortScopeType() {
    OptionalPortScopeTypeImpl optionalPortScopeType = new OptionalPortScopeTypeImpl();
    return optionalPortScopeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputFunctionsType createOutputFunctionsType() {
    OutputFunctionsTypeImpl outputFunctionsType = new OutputFunctionsTypeImpl();
    return outputFunctionsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputPortOpenSetType createOutputPortOpenSetType() {
    OutputPortOpenSetTypeImpl outputPortOpenSetType = new OutputPortOpenSetTypeImpl();
    return outputPortOpenSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputPortSetType createOutputPortSetType() {
    OutputPortSetTypeImpl outputPortSetType = new OutputPortSetTypeImpl();
    return outputPortSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OutputPortsType createOutputPortsType() {
    OutputPortsTypeImpl outputPortsType = new OutputPortsTypeImpl();
    return outputPortsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ParametersType createParametersType() {
    ParametersTypeImpl parametersType = new ParametersTypeImpl();
    return parametersType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ParameterType createParameterType() {
    ParameterTypeImpl parameterType = new ParameterTypeImpl();
    return parameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PortScopeType createPortScopeType() {
    PortScopeTypeImpl portScopeType = new PortScopeTypeImpl();
    return portScopeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplExpressionTreeType createSplExpressionTreeType() {
    SplExpressionTreeTypeImpl splExpressionTreeType = new SplExpressionTreeTypeImpl();
    return splExpressionTreeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ExpressionModeType createExpressionModeTypeFromString(
      EDataType eDataType, String initialValue) {
    ExpressionModeType result = ExpressionModeType.get(initialValue);
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
  public String convertExpressionModeTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IncrementalCompilationStrategyType createIncrementalCompilationStrategyTypeFromString(
      EDataType eDataType, String initialValue) {
    IncrementalCompilationStrategyType result =
        IncrementalCompilationStrategyType.get(initialValue);
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
  public String convertIncrementalCompilationStrategyTypeToString(
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
  public JavaOpExpressionModeType createJavaOpExpressionModeTypeFromString(
      EDataType eDataType, String initialValue) {
    JavaOpExpressionModeType result = JavaOpExpressionModeType.get(initialValue);
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
  public String convertJavaOpExpressionModeTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MetricKindType createMetricKindTypeFromString(EDataType eDataType, String initialValue) {
    MetricKindType result = MetricKindType.get(initialValue);
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
  public String convertMetricKindTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SingleThreadedContextType createSingleThreadedContextTypeFromString(
      EDataType eDataType, String initialValue) {
    SingleThreadedContextType result = SingleThreadedContextType.get(initialValue);
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
  public String convertSingleThreadedContextTypeToString(
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
  public WindowExpressionModeType createWindowExpressionModeTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowExpressionModeType result = WindowExpressionModeType.get(initialValue);
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
  public String convertWindowExpressionModeTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowingModeType createWindowingModeTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowingModeType result = WindowingModeType.get(initialValue);
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
  public String convertWindowingModeTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPunctuationInputModeType createWindowPunctuationInputModeTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowPunctuationInputModeType result = WindowPunctuationInputModeType.get(initialValue);
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
  public String convertWindowPunctuationInputModeTypeToString(
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
  public WindowPunctuationOutputModeType createWindowPunctuationOutputModeTypeFromString(
      EDataType eDataType, String initialValue) {
    WindowPunctuationOutputModeType result = WindowPunctuationOutputModeType.get(initialValue);
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
  public String convertWindowPunctuationOutputModeTypeToString(
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
  public ExpressionModeType createExpressionModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createExpressionModeTypeFromString(
        OperatorPackage.Literals.EXPRESSION_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertExpressionModeTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertExpressionModeTypeToString(
        OperatorPackage.Literals.EXPRESSION_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IncrementalCompilationStrategyType
      createIncrementalCompilationStrategyTypeObjectFromString(
          EDataType eDataType, String initialValue) {
    return createIncrementalCompilationStrategyTypeFromString(
        OperatorPackage.Literals.INCREMENTAL_COMPILATION_STRATEGY_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertIncrementalCompilationStrategyTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertIncrementalCompilationStrategyTypeToString(
        OperatorPackage.Literals.INCREMENTAL_COMPILATION_STRATEGY_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpExpressionModeType createJavaOpExpressionModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createJavaOpExpressionModeTypeFromString(
        OperatorPackage.Literals.JAVA_OP_EXPRESSION_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertJavaOpExpressionModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertJavaOpExpressionModeTypeToString(
        OperatorPackage.Literals.JAVA_OP_EXPRESSION_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MetricKindType createMetricKindTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createMetricKindTypeFromString(OperatorPackage.Literals.METRIC_KIND_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertMetricKindTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertMetricKindTypeToString(OperatorPackage.Literals.METRIC_KIND_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SingleThreadedContextType createSingleThreadedContextTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createSingleThreadedContextTypeFromString(
        OperatorPackage.Literals.SINGLE_THREADED_CONTEXT_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertSingleThreadedContextTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertSingleThreadedContextTypeToString(
        OperatorPackage.Literals.SINGLE_THREADED_CONTEXT_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowExpressionModeType createWindowExpressionModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowExpressionModeTypeFromString(
        OperatorPackage.Literals.WINDOW_EXPRESSION_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowExpressionModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertWindowExpressionModeTypeToString(
        OperatorPackage.Literals.WINDOW_EXPRESSION_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowingModeType createWindowingModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowingModeTypeFromString(
        OperatorPackage.Literals.WINDOWING_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowingModeTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertWindowingModeTypeToString(
        OperatorPackage.Literals.WINDOWING_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPunctuationInputModeType createWindowPunctuationInputModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowPunctuationInputModeTypeFromString(
        OperatorPackage.Literals.WINDOW_PUNCTUATION_INPUT_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowPunctuationInputModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertWindowPunctuationInputModeTypeToString(
        OperatorPackage.Literals.WINDOW_PUNCTUATION_INPUT_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public WindowPunctuationOutputModeType createWindowPunctuationOutputModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createWindowPunctuationOutputModeTypeFromString(
        OperatorPackage.Literals.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertWindowPunctuationOutputModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertWindowPunctuationOutputModeTypeToString(
        OperatorPackage.Literals.WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorPackage getOperatorPackage() {
    return (OperatorPackage) getEPackage();
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
  public static OperatorPackage getPackage() {
    return OperatorPackage.eINSTANCE;
  }
} // OperatorFactoryImpl
