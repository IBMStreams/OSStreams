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
package com.ibm.xmlns.prod.streams.spl.toolkit.impl;

import com.ibm.xmlns.prod.streams.spl.toolkit.*;
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
public class ToolkitFactoryImpl extends EFactoryImpl implements ToolkitFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static ToolkitFactory init() {
    try {
      ToolkitFactory theToolkitFactory =
          (ToolkitFactory)
              EPackage.Registry.INSTANCE.getEFactory(
                  "http://www.ibm.com/xmlns/prod/streams/spl/toolkit");
      if (theToolkitFactory != null) {
        return theToolkitFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new ToolkitFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitFactoryImpl() {
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
      case ToolkitPackage.BASE_FUNCTION_TYPE:
        return createBaseFunctionType();
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE:
        return createBaseToolkitOperatorType();
      case ToolkitPackage.CODE_TEMPLATE_TYPE:
        return createCodeTemplateType();
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE:
        return createCompositeOpParameterType();
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE:
        return createCompositeOpPortType();
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE:
        return createCompositeToolkitOperatorType();
      case ToolkitPackage.CUSTOM_LITERALS_TYPE:
        return createCustomLiteralsType();
      case ToolkitPackage.CUSTOM_LITERAL_VALUE_TYPE:
        return createCustomLiteralValueType();
      case ToolkitPackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case ToolkitPackage.ENUM_LITERALS_TYPE:
        return createEnumLiteralsType();
      case ToolkitPackage.IMAGES_TYPE:
        return createImagesType();
      case ToolkitPackage.IMAGE_TYPE:
        return createImageType();
      case ToolkitPackage.OUTPUT_FUNCTIONS_TYPE:
        return createOutputFunctionsType();
      case ToolkitPackage.OUTPUT_FUNCTIONS_TYPE_TYPE:
        return createOutputFunctionsTypeType();
      case ToolkitPackage.PARAMETER_TYPE:
        return createParameterType();
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE:
        return createPrimitiveOpInputPortType();
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE:
        return createPrimitiveOpOutputPortType();
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE:
        return createPrimitiveOpParameterType();
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE:
        return createPrimitivePortDescriptionType();
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE:
        return createPrimitiveToolkitOperatorType();
      case ToolkitPackage.TOOLKIT_BASE_TYPE:
        return createToolkitBaseType();
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE:
        return createToolkitElementType();
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE:
        return createToolkitFunctionType();
      case ToolkitPackage.TOOLKIT_MODEL_TYPE:
        return createToolkitModelType();
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE:
        return createToolkitNamespaceType();
      case ToolkitPackage.TOOLKIT_TYPE:
        return createToolkitType();
      case ToolkitPackage.TOOLKIT_TYPE_TYPE:
        return createToolkitTypeType();
      case ToolkitPackage.URI_TABLE_TYPE:
        return createUriTableType();
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
      case ToolkitPackage.COMPOSITE_PARAM_ENUM_TYPE:
        return createCompositeParamEnumTypeFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_EXPRESSION_MODE_TYPE:
        return createEnumExpressionModeTypeFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_POLICY_TYPE:
        return createEnumWindowPolicyTypeFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE:
        return createEnumWindowPunctInputModeTypeFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE:
        return createEnumWindowPunctOutputModeTypeFromString(eDataType, initialValue);
      case ToolkitPackage.LANGUAGE_TYPE:
        return createLanguageTypeFromString(eDataType, initialValue);
      case ToolkitPackage.COMPOSITE_PARAM_ENUM_TYPE_OBJECT:
        return createCompositeParamEnumTypeObjectFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_EXPRESSION_MODE_TYPE_OBJECT:
        return createEnumExpressionModeTypeObjectFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_POLICY_TYPE_OBJECT:
        return createEnumWindowPolicyTypeObjectFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE_OBJECT:
        return createEnumWindowPunctInputModeTypeObjectFromString(eDataType, initialValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE_OBJECT:
        return createEnumWindowPunctOutputModeTypeObjectFromString(eDataType, initialValue);
      case ToolkitPackage.LANGUAGE_TYPE_OBJECT:
        return createLanguageTypeObjectFromString(eDataType, initialValue);
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
      case ToolkitPackage.COMPOSITE_PARAM_ENUM_TYPE:
        return convertCompositeParamEnumTypeToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_EXPRESSION_MODE_TYPE:
        return convertEnumExpressionModeTypeToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_POLICY_TYPE:
        return convertEnumWindowPolicyTypeToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE:
        return convertEnumWindowPunctInputModeTypeToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE:
        return convertEnumWindowPunctOutputModeTypeToString(eDataType, instanceValue);
      case ToolkitPackage.LANGUAGE_TYPE:
        return convertLanguageTypeToString(eDataType, instanceValue);
      case ToolkitPackage.COMPOSITE_PARAM_ENUM_TYPE_OBJECT:
        return convertCompositeParamEnumTypeObjectToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_EXPRESSION_MODE_TYPE_OBJECT:
        return convertEnumExpressionModeTypeObjectToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_POLICY_TYPE_OBJECT:
        return convertEnumWindowPolicyTypeObjectToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE_OBJECT:
        return convertEnumWindowPunctInputModeTypeObjectToString(eDataType, instanceValue);
      case ToolkitPackage.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE_OBJECT:
        return convertEnumWindowPunctOutputModeTypeObjectToString(eDataType, instanceValue);
      case ToolkitPackage.LANGUAGE_TYPE_OBJECT:
        return convertLanguageTypeObjectToString(eDataType, instanceValue);
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
  public BaseFunctionType createBaseFunctionType() {
    BaseFunctionTypeImpl baseFunctionType = new BaseFunctionTypeImpl();
    return baseFunctionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public BaseToolkitOperatorType createBaseToolkitOperatorType() {
    BaseToolkitOperatorTypeImpl baseToolkitOperatorType = new BaseToolkitOperatorTypeImpl();
    return baseToolkitOperatorType;
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
  public CompositeOpParameterType createCompositeOpParameterType() {
    CompositeOpParameterTypeImpl compositeOpParameterType = new CompositeOpParameterTypeImpl();
    return compositeOpParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeOpPortType createCompositeOpPortType() {
    CompositeOpPortTypeImpl compositeOpPortType = new CompositeOpPortTypeImpl();
    return compositeOpPortType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeToolkitOperatorType createCompositeToolkitOperatorType() {
    CompositeToolkitOperatorTypeImpl compositeToolkitOperatorType =
        new CompositeToolkitOperatorTypeImpl();
    return compositeToolkitOperatorType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CustomLiteralsType createCustomLiteralsType() {
    CustomLiteralsTypeImpl customLiteralsType = new CustomLiteralsTypeImpl();
    return customLiteralsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CustomLiteralValueType createCustomLiteralValueType() {
    CustomLiteralValueTypeImpl customLiteralValueType = new CustomLiteralValueTypeImpl();
    return customLiteralValueType;
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
  public EnumLiteralsType createEnumLiteralsType() {
    EnumLiteralsTypeImpl enumLiteralsType = new EnumLiteralsTypeImpl();
    return enumLiteralsType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ImagesType createImagesType() {
    ImagesTypeImpl imagesType = new ImagesTypeImpl();
    return imagesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ImageType createImageType() {
    ImageTypeImpl imageType = new ImageTypeImpl();
    return imageType;
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
  public OutputFunctionsTypeType createOutputFunctionsTypeType() {
    OutputFunctionsTypeTypeImpl outputFunctionsTypeType = new OutputFunctionsTypeTypeImpl();
    return outputFunctionsTypeType;
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
  public PrimitiveOpInputPortType createPrimitiveOpInputPortType() {
    PrimitiveOpInputPortTypeImpl primitiveOpInputPortType = new PrimitiveOpInputPortTypeImpl();
    return primitiveOpInputPortType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PrimitiveOpOutputPortType createPrimitiveOpOutputPortType() {
    PrimitiveOpOutputPortTypeImpl primitiveOpOutputPortType = new PrimitiveOpOutputPortTypeImpl();
    return primitiveOpOutputPortType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PrimitiveOpParameterType createPrimitiveOpParameterType() {
    PrimitiveOpParameterTypeImpl primitiveOpParameterType = new PrimitiveOpParameterTypeImpl();
    return primitiveOpParameterType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PrimitivePortDescriptionType createPrimitivePortDescriptionType() {
    PrimitivePortDescriptionTypeImpl primitivePortDescriptionType =
        new PrimitivePortDescriptionTypeImpl();
    return primitivePortDescriptionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public PrimitiveToolkitOperatorType createPrimitiveToolkitOperatorType() {
    PrimitiveToolkitOperatorTypeImpl primitiveToolkitOperatorType =
        new PrimitiveToolkitOperatorTypeImpl();
    return primitiveToolkitOperatorType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitBaseType createToolkitBaseType() {
    ToolkitBaseTypeImpl toolkitBaseType = new ToolkitBaseTypeImpl();
    return toolkitBaseType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitElementType createToolkitElementType() {
    ToolkitElementTypeImpl toolkitElementType = new ToolkitElementTypeImpl();
    return toolkitElementType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitFunctionType createToolkitFunctionType() {
    ToolkitFunctionTypeImpl toolkitFunctionType = new ToolkitFunctionTypeImpl();
    return toolkitFunctionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitModelType createToolkitModelType() {
    ToolkitModelTypeImpl toolkitModelType = new ToolkitModelTypeImpl();
    return toolkitModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitNamespaceType createToolkitNamespaceType() {
    ToolkitNamespaceTypeImpl toolkitNamespaceType = new ToolkitNamespaceTypeImpl();
    return toolkitNamespaceType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitType createToolkitType() {
    ToolkitTypeImpl toolkitType = new ToolkitTypeImpl();
    return toolkitType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitTypeType createToolkitTypeType() {
    ToolkitTypeTypeImpl toolkitTypeType = new ToolkitTypeTypeImpl();
    return toolkitTypeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UriTableType createUriTableType() {
    UriTableTypeImpl uriTableType = new UriTableTypeImpl();
    return uriTableType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParamEnumType createCompositeParamEnumTypeFromString(
      EDataType eDataType, String initialValue) {
    CompositeParamEnumType result = CompositeParamEnumType.get(initialValue);
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
  public String convertCompositeParamEnumTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumExpressionModeType createEnumExpressionModeTypeFromString(
      EDataType eDataType, String initialValue) {
    EnumExpressionModeType result = EnumExpressionModeType.get(initialValue);
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
  public String convertEnumExpressionModeTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPolicyType createEnumWindowPolicyTypeFromString(
      EDataType eDataType, String initialValue) {
    EnumWindowPolicyType result = EnumWindowPolicyType.get(initialValue);
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
  public String convertEnumWindowPolicyTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPunctInputModeType createEnumWindowPunctInputModeTypeFromString(
      EDataType eDataType, String initialValue) {
    EnumWindowPunctInputModeType result = EnumWindowPunctInputModeType.get(initialValue);
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
  public String convertEnumWindowPunctInputModeTypeToString(
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
  public EnumWindowPunctOutputModeType createEnumWindowPunctOutputModeTypeFromString(
      EDataType eDataType, String initialValue) {
    EnumWindowPunctOutputModeType result = EnumWindowPunctOutputModeType.get(initialValue);
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
  public String convertEnumWindowPunctOutputModeTypeToString(
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
  public LanguageType createLanguageTypeFromString(EDataType eDataType, String initialValue) {
    LanguageType result = LanguageType.get(initialValue);
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
  public String convertLanguageTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeParamEnumType createCompositeParamEnumTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createCompositeParamEnumTypeFromString(
        ToolkitPackage.Literals.COMPOSITE_PARAM_ENUM_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertCompositeParamEnumTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertCompositeParamEnumTypeToString(
        ToolkitPackage.Literals.COMPOSITE_PARAM_ENUM_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumExpressionModeType createEnumExpressionModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createEnumExpressionModeTypeFromString(
        ToolkitPackage.Literals.ENUM_EXPRESSION_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertEnumExpressionModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertEnumExpressionModeTypeToString(
        ToolkitPackage.Literals.ENUM_EXPRESSION_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPolicyType createEnumWindowPolicyTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createEnumWindowPolicyTypeFromString(
        ToolkitPackage.Literals.ENUM_WINDOW_POLICY_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertEnumWindowPolicyTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertEnumWindowPolicyTypeToString(
        ToolkitPackage.Literals.ENUM_WINDOW_POLICY_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPunctInputModeType createEnumWindowPunctInputModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createEnumWindowPunctInputModeTypeFromString(
        ToolkitPackage.Literals.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertEnumWindowPunctInputModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertEnumWindowPunctInputModeTypeToString(
        ToolkitPackage.Literals.ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EnumWindowPunctOutputModeType createEnumWindowPunctOutputModeTypeObjectFromString(
      EDataType eDataType, String initialValue) {
    return createEnumWindowPunctOutputModeTypeFromString(
        ToolkitPackage.Literals.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertEnumWindowPunctOutputModeTypeObjectToString(
      EDataType eDataType, Object instanceValue) {
    return convertEnumWindowPunctOutputModeTypeToString(
        ToolkitPackage.Literals.ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LanguageType createLanguageTypeObjectFromString(EDataType eDataType, String initialValue) {
    return createLanguageTypeFromString(ToolkitPackage.Literals.LANGUAGE_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertLanguageTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertLanguageTypeToString(ToolkitPackage.Literals.LANGUAGE_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitPackage getToolkitPackage() {
    return (ToolkitPackage) getEPackage();
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
  public static ToolkitPackage getPackage() {
    return ToolkitPackage.eINSTANCE;
  }
} // ToolkitFactoryImpl
