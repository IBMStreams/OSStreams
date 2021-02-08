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
package com.ibm.xmlns.prod.streams.spl.source.code.util;

import com.ibm.xmlns.prod.streams.spl.source.code.*;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.util.Switch;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Switch</b> for the model's inheritance hierarchy. It supports the call {@link
 * #doSwitch(EObject) doSwitch(object)} to invoke the <code>caseXXX</code> method for each class of
 * the model, starting with the actual class of the object and proceeding up the inheritance
 * hierarchy until a non-null result is returned, which is the result of the switch.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage
 * @generated
 */
public class CodeSwitch<T> extends Switch<T> {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static CodePackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeSwitch() {
    if (modelPackage == null) {
      modelPackage = CodePackage.eINSTANCE;
    }
  }

  /**
   * Checks whether this is a switch for the given package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param ePackage the package in question.
   * @return whether this is a switch for the given package.
   * @generated
   */
  @Override
  protected boolean isSwitchFor(EPackage ePackage) {
    return ePackage == modelPackage;
  }

  /**
   * Calls <code>caseXXX</code> for each class of the model until one returns a non null result; it
   * yields that result.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the first non-null result returned by a <code>caseXXX</code> call.
   * @generated
   */
  @Override
  protected T doSwitch(int classifierID, EObject theEObject) {
    switch (classifierID) {
      case CodePackage.COMPILATION_UNIT_TYPE:
        {
          CompilationUnitType compilationUnitType = (CompilationUnitType) theEObject;
          T result = caseCompilationUnitType(compilationUnitType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_BODY_TYPE:
        {
          CompositeBodyType compositeBodyType = (CompositeBodyType) theEObject;
          T result = caseCompositeBodyType(compositeBodyType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_DEFINITION_TYPE:
        {
          CompositeDefinitionType compositeDefinitionType = (CompositeDefinitionType) theEObject;
          T result = caseCompositeDefinitionType(compositeDefinitionType);
          if (result == null) result = caseExtendedSourceLocationType(compositeDefinitionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_GRAPH_TYPE:
        {
          CompositeGraphType compositeGraphType = (CompositeGraphType) theEObject;
          T result = caseCompositeGraphType(compositeGraphType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_HEAD_TYPE:
        {
          CompositeHeadType compositeHeadType = (CompositeHeadType) theEObject;
          T result = caseCompositeHeadType(compositeHeadType);
          if (result == null) result = caseSourceLocationType(compositeHeadType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_INPUTS_TYPE:
        {
          CompositeInputsType compositeInputsType = (CompositeInputsType) theEObject;
          T result = caseCompositeInputsType(compositeInputsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_MODIFIERS_TYPE:
        {
          CompositeModifiersType compositeModifiersType = (CompositeModifiersType) theEObject;
          T result = caseCompositeModifiersType(compositeModifiersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_MODIFIER_TYPE:
        {
          CompositeModifierType compositeModifierType = (CompositeModifierType) theEObject;
          T result = caseCompositeModifierType(compositeModifierType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_OUTPUTS_TYPE:
        {
          CompositeOutputsType compositeOutputsType = (CompositeOutputsType) theEObject;
          T result = caseCompositeOutputsType(compositeOutputsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE:
        {
          CompositeParameterExpressionModeType compositeParameterExpressionModeType =
              (CompositeParameterExpressionModeType) theEObject;
          T result = caseCompositeParameterExpressionModeType(compositeParameterExpressionModeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_PARAMETERS_TYPE:
        {
          CompositeParametersType compositeParametersType = (CompositeParametersType) theEObject;
          T result = caseCompositeParametersType(compositeParametersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_PARAMETER_TYPE:
        {
          CompositeParameterType compositeParameterType = (CompositeParameterType) theEObject;
          T result = caseCompositeParameterType(compositeParameterType);
          if (result == null) result = caseSourceLocationType(compositeParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_PORT_TYPE:
        {
          CompositePortType compositePortType = (CompositePortType) theEObject;
          T result = caseCompositePortType(compositePortType);
          if (result == null) result = caseSourceLocationType(compositePortType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.COMPOSITE_TYPES_TYPE:
        {
          CompositeTypesType compositeTypesType = (CompositeTypesType) theEObject;
          T result = caseCompositeTypesType(compositeTypesType);
          if (result == null) result = caseSourceLocationType(compositeTypesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.CONFIG_OPTION_TYPE:
        {
          ConfigOptionType configOptionType = (ConfigOptionType) theEObject;
          T result = caseConfigOptionType(configOptionType);
          if (result == null) result = caseSourceLocationType(configOptionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.CONFIGS_TYPE:
        {
          ConfigsType configsType = (ConfigsType) theEObject;
          T result = caseConfigsType(configsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.CONFIG_TYPE:
        {
          ConfigType configType = (ConfigType) theEObject;
          T result = caseConfigType(configType);
          if (result == null) result = caseSourceLocationType(configType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.CONFIG_VALUE_PARAMETER_TYPE:
        {
          ConfigValueParameterType configValueParameterType = (ConfigValueParameterType) theEObject;
          T result = caseConfigValueParameterType(configValueParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.DEFINITIONS_TYPE:
        {
          DefinitionsType definitionsType = (DefinitionsType) theEObject;
          T result = caseDefinitionsType(definitionsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.DOCUMENT_ROOT:
        {
          DocumentRoot documentRoot = (DocumentRoot) theEObject;
          T result = caseDocumentRoot(documentRoot);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.EXPRESSION_TYPE:
        {
          ExpressionType expressionType = (ExpressionType) theEObject;
          T result = caseExpressionType(expressionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.EXTENDED_SOURCE_LOCATION_TYPE:
        {
          ExtendedSourceLocationType extendedSourceLocationType =
              (ExtendedSourceLocationType) theEObject;
          T result = caseExtendedSourceLocationType(extendedSourceLocationType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_DEFINITION_TYPE:
        {
          FunctionDefinitionType functionDefinitionType = (FunctionDefinitionType) theEObject;
          T result = caseFunctionDefinitionType(functionDefinitionType);
          if (result == null) result = caseExtendedSourceLocationType(functionDefinitionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_HEAD_TYPE:
        {
          FunctionHeadType functionHeadType = (FunctionHeadType) theEObject;
          T result = caseFunctionHeadType(functionHeadType);
          if (result == null) result = caseSourceLocationType(functionHeadType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_MODIFIERS_TYPE:
        {
          FunctionModifiersType functionModifiersType = (FunctionModifiersType) theEObject;
          T result = caseFunctionModifiersType(functionModifiersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_MODIFIER_TYPE:
        {
          FunctionModifierType functionModifierType = (FunctionModifierType) theEObject;
          T result = caseFunctionModifierType(functionModifierType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_PARAMETER_MODIFIERS_TYPE:
        {
          FunctionParameterModifiersType functionParameterModifiersType =
              (FunctionParameterModifiersType) theEObject;
          T result = caseFunctionParameterModifiersType(functionParameterModifiersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_PARAMETER_MODIFIER_TYPE:
        {
          FunctionParameterModifierType functionParameterModifierType =
              (FunctionParameterModifierType) theEObject;
          T result = caseFunctionParameterModifierType(functionParameterModifierType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_PARAMETERS_TYPE:
        {
          FunctionParametersType functionParametersType = (FunctionParametersType) theEObject;
          T result = caseFunctionParametersType(functionParametersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.FUNCTION_PARAMETER_TYPE:
        {
          FunctionParameterType functionParameterType = (FunctionParameterType) theEObject;
          T result = caseFunctionParameterType(functionParameterType);
          if (result == null) result = caseSourceLocationType(functionParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.ON_PROCESS_TYPE:
        {
          OnProcessType onProcessType = (OnProcessType) theEObject;
          T result = caseOnProcessType(onProcessType);
          if (result == null) result = caseSourceLocationType(onProcessType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.ON_PUNCT_TYPE:
        {
          OnPunctType onPunctType = (OnPunctType) theEObject;
          T result = caseOnPunctType(onPunctType);
          if (result == null) result = caseSourceLocationType(onPunctType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.ON_TUPLE_TYPE:
        {
          OnTupleType onTupleType = (OnTupleType) theEObject;
          T result = caseOnTupleType(onTupleType);
          if (result == null) result = caseSourceLocationType(onTupleType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE:
        {
          OperatorInvocationAttributeAssignmentType operatorInvocationAttributeAssignmentType =
              (OperatorInvocationAttributeAssignmentType) theEObject;
          T result =
              caseOperatorInvocationAttributeAssignmentType(
                  operatorInvocationAttributeAssignmentType);
          if (result == null)
            result = caseSourceLocationType(operatorInvocationAttributeAssignmentType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_BODY_TYPE:
        {
          OperatorInvocationBodyType operatorInvocationBodyType =
              (OperatorInvocationBodyType) theEObject;
          T result = caseOperatorInvocationBodyType(operatorInvocationBodyType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_HEAD_TYPE:
        {
          OperatorInvocationHeadType operatorInvocationHeadType =
              (OperatorInvocationHeadType) theEObject;
          T result = caseOperatorInvocationHeadType(operatorInvocationHeadType);
          if (result == null) result = caseSourceLocationType(operatorInvocationHeadType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_INPUT_STREAM_TYPE:
        {
          OperatorInvocationInputStreamType operatorInvocationInputStreamType =
              (OperatorInvocationInputStreamType) theEObject;
          T result = caseOperatorInvocationInputStreamType(operatorInvocationInputStreamType);
          if (result == null) result = caseSourceLocationType(operatorInvocationInputStreamType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_INPUTS_TYPE:
        {
          OperatorInvocationInputsType operatorInvocationInputsType =
              (OperatorInvocationInputsType) theEObject;
          T result = caseOperatorInvocationInputsType(operatorInvocationInputsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_INPUT_TYPE:
        {
          OperatorInvocationInputType operatorInvocationInputType =
              (OperatorInvocationInputType) theEObject;
          T result = caseOperatorInvocationInputType(operatorInvocationInputType);
          if (result == null) result = caseSourceLocationType(operatorInvocationInputType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_LOGIC_TYPE:
        {
          OperatorInvocationLogicType operatorInvocationLogicType =
              (OperatorInvocationLogicType) theEObject;
          T result = caseOperatorInvocationLogicType(operatorInvocationLogicType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE:
        {
          OperatorInvocationOutputAssignmentsType operatorInvocationOutputAssignmentsType =
              (OperatorInvocationOutputAssignmentsType) theEObject;
          T result =
              caseOperatorInvocationOutputAssignmentsType(operatorInvocationOutputAssignmentsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE:
        {
          OperatorInvocationOutputAssignmentType operatorInvocationOutputAssignmentType =
              (OperatorInvocationOutputAssignmentType) theEObject;
          T result =
              caseOperatorInvocationOutputAssignmentType(operatorInvocationOutputAssignmentType);
          if (result == null)
            result = caseSourceLocationType(operatorInvocationOutputAssignmentType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_OUTPUTS_TYPE:
        {
          OperatorInvocationOutputsType operatorInvocationOutputsType =
              (OperatorInvocationOutputsType) theEObject;
          T result = caseOperatorInvocationOutputsType(operatorInvocationOutputsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_OUTPUT_TYPE:
        {
          OperatorInvocationOutputType operatorInvocationOutputType =
              (OperatorInvocationOutputType) theEObject;
          T result = caseOperatorInvocationOutputType(operatorInvocationOutputType);
          if (result == null) result = caseSourceLocationType(operatorInvocationOutputType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_PARAMETERS_TYPE:
        {
          OperatorInvocationParametersType operatorInvocationParametersType =
              (OperatorInvocationParametersType) theEObject;
          T result = caseOperatorInvocationParametersType(operatorInvocationParametersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_PARAMETER_TYPE:
        {
          OperatorInvocationParameterType operatorInvocationParameterType =
              (OperatorInvocationParameterType) theEObject;
          T result = caseOperatorInvocationParameterType(operatorInvocationParameterType);
          if (result == null) result = caseSourceLocationType(operatorInvocationParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_TYPE:
        {
          OperatorInvocationType operatorInvocationType = (OperatorInvocationType) theEObject;
          T result = caseOperatorInvocationType(operatorInvocationType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_WINDOWS_TYPE:
        {
          OperatorInvocationWindowsType operatorInvocationWindowsType =
              (OperatorInvocationWindowsType) theEObject;
          T result = caseOperatorInvocationWindowsType(operatorInvocationWindowsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.OPERATOR_INVOCATION_WINDOW_TYPE:
        {
          OperatorInvocationWindowType operatorInvocationWindowType =
              (OperatorInvocationWindowType) theEObject;
          T result = caseOperatorInvocationWindowType(operatorInvocationWindowType);
          if (result == null) result = caseSourceLocationType(operatorInvocationWindowType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SOURCE_FILE_TYPE:
        {
          SourceFileType sourceFileType = (SourceFileType) theEObject;
          T result = caseSourceFileType(sourceFileType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SOURCE_LOCATION_TYPE:
        {
          SourceLocationType sourceLocationType = (SourceLocationType) theEObject;
          T result = caseSourceLocationType(sourceLocationType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SOURCE_MODEL_TYPE:
        {
          SourceModelType sourceModelType = (SourceModelType) theEObject;
          T result = caseSourceModelType(sourceModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SPL_DOC_ANNOTATION_TYPE:
        {
          SplDocAnnotationType splDocAnnotationType = (SplDocAnnotationType) theEObject;
          T result = caseSplDocAnnotationType(splDocAnnotationType);
          if (result == null) result = caseSplDocDescriptionType(splDocAnnotationType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SPL_DOC_DESCRIPTION_TYPE:
        {
          SplDocDescriptionType splDocDescriptionType = (SplDocDescriptionType) theEObject;
          T result = caseSplDocDescriptionType(splDocDescriptionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SPL_DOC_TYPE:
        {
          SplDocType splDocType = (SplDocType) theEObject;
          T result = caseSplDocType(splDocType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.SPL_NAMESPACE_TYPE:
        {
          SplNamespaceType splNamespaceType = (SplNamespaceType) theEObject;
          T result = caseSplNamespaceType(splNamespaceType);
          if (result == null) result = caseSourceLocationType(splNamespaceType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.TYPE_DEFINITION_TYPE:
        {
          TypeDefinitionType typeDefinitionType = (TypeDefinitionType) theEObject;
          T result = caseTypeDefinitionType(typeDefinitionType);
          if (result == null) result = caseExtendedSourceLocationType(typeDefinitionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.TYPE_MODIFIERS_TYPE:
        {
          TypeModifiersType typeModifiersType = (TypeModifiersType) theEObject;
          T result = caseTypeModifiersType(typeModifiersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.TYPE_MODIFIER_TYPE:
        {
          TypeModifierType typeModifierType = (TypeModifierType) theEObject;
          T result = caseTypeModifierType(typeModifierType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.USE_DIRECTIVES_TYPE:
        {
          UseDirectivesType useDirectivesType = (UseDirectivesType) theEObject;
          T result = caseUseDirectivesType(useDirectivesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.USE_DIRECTIVE_TYPE:
        {
          UseDirectiveType useDirectiveType = (UseDirectiveType) theEObject;
          T result = caseUseDirectiveType(useDirectiveType);
          if (result == null) result = caseSourceLocationType(useDirectiveType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case CodePackage.WINDOW_POLICY_TYPE:
        {
          WindowPolicyType windowPolicyType = (WindowPolicyType) theEObject;
          T result = caseWindowPolicyType(windowPolicyType);
          if (result == null) result = caseSourceLocationType(windowPolicyType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      default:
        return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Compilation Unit
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Compilation Unit
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompilationUnitType(CompilationUnitType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Body Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Body Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeBodyType(CompositeBodyType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Definition
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Definition
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeDefinitionType(CompositeDefinitionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Graph
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Graph
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeGraphType(CompositeGraphType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Head Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Head Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeHeadType(CompositeHeadType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Inputs
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Inputs
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeInputsType(CompositeInputsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Modifiers
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Modifiers
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeModifiersType(CompositeModifiersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Modifier
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Modifier
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeModifierType(CompositeModifierType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Outputs
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Outputs
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeOutputsType(CompositeOutputsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Parameter
   * Expression Mode Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Parameter
   *     Expression Mode Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeParameterExpressionModeType(CompositeParameterExpressionModeType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Parameters
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Parameters
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeParametersType(CompositeParametersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeParameterType(CompositeParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Port Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Port Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositePortType(CompositePortType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Types
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Types
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeTypesType(CompositeTypesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Config Option Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Config Option Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseConfigOptionType(ConfigOptionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Configs Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Configs Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseConfigsType(ConfigsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Config Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Config Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseConfigType(ConfigType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Config Value Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Config Value Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseConfigValueParameterType(ConfigValueParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Definitions Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Definitions Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseDefinitionsType(DefinitionsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Document Root</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Document Root</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseDocumentRoot(DocumentRoot object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Expression Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Expression Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseExpressionType(ExpressionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Extended Source Location
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Extended Source Location
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseExtendedSourceLocationType(ExtendedSourceLocationType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Definition
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Definition
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionDefinitionType(FunctionDefinitionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Head Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Head Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionHeadType(FunctionHeadType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Modifiers
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Modifiers
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionModifiersType(FunctionModifiersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Modifier
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Modifier
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionModifierType(FunctionModifierType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Parameter
   * Modifiers Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Parameter
   *     Modifiers Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionParameterModifiersType(FunctionParameterModifiersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Parameter
   * Modifier Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Parameter
   *     Modifier Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionParameterModifierType(FunctionParameterModifierType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Parameters
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Parameters
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionParametersType(FunctionParametersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseFunctionParameterType(FunctionParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>On Process Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>On Process Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOnProcessType(OnProcessType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>On Punct Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>On Punct Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOnPunctType(OnPunctType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>On Tuple Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>On Tuple Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOnTupleType(OnTupleType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Attribute Assignment Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Attribute Assignment Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationAttributeAssignmentType(
      OperatorInvocationAttributeAssignmentType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Body
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Body
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationBodyType(OperatorInvocationBodyType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Head
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Head
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationHeadType(OperatorInvocationHeadType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Input
   * Stream Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Input
   *     Stream Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationInputStreamType(OperatorInvocationInputStreamType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Inputs
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Inputs
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationInputsType(OperatorInvocationInputsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Input
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Input
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationInputType(OperatorInvocationInputType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Logic
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Logic
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationLogicType(OperatorInvocationLogicType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Output
   * Assignments Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Output
   *     Assignments Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationOutputAssignmentsType(
      OperatorInvocationOutputAssignmentsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Output
   * Assignment Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Output
   *     Assignment Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationOutputAssignmentType(
      OperatorInvocationOutputAssignmentType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Outputs Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Outputs Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationOutputsType(OperatorInvocationOutputsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Output
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Output
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationOutputType(OperatorInvocationOutputType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Parameters Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Parameters Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationParametersType(OperatorInvocationParametersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Parameter Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Parameter Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationParameterType(OperatorInvocationParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationType(OperatorInvocationType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation
   * Windows Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation
   *     Windows Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationWindowsType(OperatorInvocationWindowsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Operator Invocation Window
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Operator Invocation Window
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorInvocationWindowType(OperatorInvocationWindowType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Source File Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Source File Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSourceFileType(SourceFileType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Source Location
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Source Location
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSourceLocationType(SourceLocationType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Source Model Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Source Model Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSourceModelType(SourceModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Spl Doc Annotation
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Spl Doc Annotation
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSplDocAnnotationType(SplDocAnnotationType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Spl Doc Description
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Spl Doc Description
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSplDocDescriptionType(SplDocDescriptionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Spl Doc Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Spl Doc Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSplDocType(SplDocType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Spl Namespace Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Spl Namespace Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSplNamespaceType(SplNamespaceType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type Definition
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type Definition
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseTypeDefinitionType(TypeDefinitionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type Modifiers Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type Modifiers Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseTypeModifiersType(TypeModifiersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type Modifier Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type Modifier Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseTypeModifierType(TypeModifierType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Use Directives Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Use Directives Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseUseDirectivesType(UseDirectivesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Use Directive Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Use Directive Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseUseDirectiveType(UseDirectiveType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Window Policy Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Window Policy Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseWindowPolicyType(WindowPolicyType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>EObject</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch, but
   * this is the last case anyway.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>EObject</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject)
   * @generated
   */
  @Override
  public T defaultCase(EObject object) {
    return null;
  }
} // CodeSwitch
