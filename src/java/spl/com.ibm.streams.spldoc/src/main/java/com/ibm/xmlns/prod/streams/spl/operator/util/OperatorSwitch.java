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
package com.ibm.xmlns.prod.streams.spl.operator.util;

import com.ibm.xmlns.prod.streams.spl.operator.*;
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
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage
 * @generated
 */
public class OperatorSwitch<T> extends Switch<T> {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static OperatorPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorSwitch() {
    if (modelPackage == null) {
      modelPackage = OperatorPackage.eINSTANCE;
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
      case OperatorPackage.CODE_TEMPLATES_TYPE:
        {
          CodeTemplatesType codeTemplatesType = (CodeTemplatesType) theEObject;
          T result = caseCodeTemplatesType(codeTemplatesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.CODE_TEMPLATE_TYPE:
        {
          CodeTemplateType codeTemplateType = (CodeTemplateType) theEObject;
          T result = caseCodeTemplateType(codeTemplateType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.CONTEXT_TYPE:
        {
          ContextType contextType = (ContextType) theEObject;
          T result = caseContextType(contextType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_SET_TYPE:
        {
          CustomOutputFunctionSetType customOutputFunctionSetType =
              (CustomOutputFunctionSetType) theEObject;
          T result = caseCustomOutputFunctionSetType(customOutputFunctionSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTIONS_TYPE:
        {
          CustomOutputFunctionsType customOutputFunctionsType =
              (CustomOutputFunctionsType) theEObject;
          T result = caseCustomOutputFunctionsType(customOutputFunctionsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.CUSTOM_OUTPUT_FUNCTION_TYPE:
        {
          CustomOutputFunctionType customOutputFunctionType = (CustomOutputFunctionType) theEObject;
          T result = caseCustomOutputFunctionType(customOutputFunctionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.DOCUMENT_ROOT:
        {
          DocumentRoot documentRoot = (DocumentRoot) theEObject;
          T result = caseDocumentRoot(documentRoot);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.ENUMERATIONS_TYPE:
        {
          EnumerationsType enumerationsType = (EnumerationsType) theEObject;
          T result = caseEnumerationsType(enumerationsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.ENUMERATION_TYPE:
        {
          EnumerationType enumerationType = (EnumerationType) theEObject;
          T result = caseEnumerationType(enumerationType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.ICON_URI_TYPE:
        {
          IconUriType iconUriType = (IconUriType) theEObject;
          T result = caseIconUriType(iconUriType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.INPUT_PORT_OPEN_SET_TYPE:
        {
          InputPortOpenSetType inputPortOpenSetType = (InputPortOpenSetType) theEObject;
          T result = caseInputPortOpenSetType(inputPortOpenSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.INPUT_PORT_SET_TYPE:
        {
          InputPortSetType inputPortSetType = (InputPortSetType) theEObject;
          T result = caseInputPortSetType(inputPortSetType);
          if (result == null) result = caseInputPortOpenSetType(inputPortSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.INPUT_PORTS_TYPE:
        {
          InputPortsType inputPortsType = (InputPortsType) theEObject;
          T result = caseInputPortsType(inputPortsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_CONTEXT_TYPE:
        {
          JavaOpContextType javaOpContextType = (JavaOpContextType) theEObject;
          T result = caseJavaOpContextType(javaOpContextType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_EXECUTION_SETTINGS_TYPE:
        {
          JavaOpExecutionSettingsType javaOpExecutionSettingsType =
              (JavaOpExecutionSettingsType) theEObject;
          T result = caseJavaOpExecutionSettingsType(javaOpExecutionSettingsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_INPUT_PORT_OPEN_SET_TYPE:
        {
          JavaOpInputPortOpenSetType javaOpInputPortOpenSetType =
              (JavaOpInputPortOpenSetType) theEObject;
          T result = caseJavaOpInputPortOpenSetType(javaOpInputPortOpenSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_INPUT_PORT_SET_TYPE:
        {
          JavaOpInputPortSetType javaOpInputPortSetType = (JavaOpInputPortSetType) theEObject;
          T result = caseJavaOpInputPortSetType(javaOpInputPortSetType);
          if (result == null) result = caseJavaOpInputPortOpenSetType(javaOpInputPortSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_INPUT_PORTS_TYPE:
        {
          JavaOpInputPortsType javaOpInputPortsType = (JavaOpInputPortsType) theEObject;
          T result = caseJavaOpInputPortsType(javaOpInputPortsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_LIBRARY_DEPENDENCIES_TYPE:
        {
          JavaOpLibraryDependenciesType javaOpLibraryDependenciesType =
              (JavaOpLibraryDependenciesType) theEObject;
          T result = caseJavaOpLibraryDependenciesType(javaOpLibraryDependenciesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_MODEL_TYPE:
        {
          JavaOpModelType javaOpModelType = (JavaOpModelType) theEObject;
          T result = caseJavaOpModelType(javaOpModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE:
        {
          JavaOpOutputPortOpenSetType javaOpOutputPortOpenSetType =
              (JavaOpOutputPortOpenSetType) theEObject;
          T result = caseJavaOpOutputPortOpenSetType(javaOpOutputPortOpenSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_OUTPUT_PORT_SET_TYPE:
        {
          JavaOpOutputPortSetType javaOpOutputPortSetType = (JavaOpOutputPortSetType) theEObject;
          T result = caseJavaOpOutputPortSetType(javaOpOutputPortSetType);
          if (result == null) result = caseJavaOpOutputPortOpenSetType(javaOpOutputPortSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_OUTPUT_PORTS_TYPE:
        {
          JavaOpOutputPortsType javaOpOutputPortsType = (JavaOpOutputPortsType) theEObject;
          T result = caseJavaOpOutputPortsType(javaOpOutputPortsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_PARAMETERS_TYPE:
        {
          JavaOpParametersType javaOpParametersType = (JavaOpParametersType) theEObject;
          T result = caseJavaOpParametersType(javaOpParametersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_PARAMETER_TYPE:
        {
          JavaOpParameterType javaOpParameterType = (JavaOpParameterType) theEObject;
          T result = caseJavaOpParameterType(javaOpParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.JAVA_OP_VM_ARGS_TYPE:
        {
          JavaOpVMArgsType javaOpVMArgsType = (JavaOpVMArgsType) theEObject;
          T result = caseJavaOpVMArgsType(javaOpVMArgsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.LIBRARY_DEPENDENCIES_TYPE:
        {
          LibraryDependenciesType libraryDependenciesType = (LibraryDependenciesType) theEObject;
          T result = caseLibraryDependenciesType(libraryDependenciesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.METRICS_TYPE:
        {
          MetricsType metricsType = (MetricsType) theEObject;
          T result = caseMetricsType(metricsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.METRIC_TYPE:
        {
          MetricType metricType = (MetricType) theEObject;
          T result = caseMetricType(metricType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OPERATOR_MODEL_TYPE:
        {
          OperatorModelType operatorModelType = (OperatorModelType) theEObject;
          T result = caseOperatorModelType(operatorModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OP_MODEL_TYPE:
        {
          OpModelType opModelType = (OpModelType) theEObject;
          T result = caseOpModelType(opModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OPTIONAL_PORT_SCOPE_TYPE:
        {
          OptionalPortScopeType optionalPortScopeType = (OptionalPortScopeType) theEObject;
          T result = caseOptionalPortScopeType(optionalPortScopeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OUTPUT_FUNCTIONS_TYPE:
        {
          OutputFunctionsType outputFunctionsType = (OutputFunctionsType) theEObject;
          T result = caseOutputFunctionsType(outputFunctionsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OUTPUT_PORT_OPEN_SET_TYPE:
        {
          OutputPortOpenSetType outputPortOpenSetType = (OutputPortOpenSetType) theEObject;
          T result = caseOutputPortOpenSetType(outputPortOpenSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OUTPUT_PORT_SET_TYPE:
        {
          OutputPortSetType outputPortSetType = (OutputPortSetType) theEObject;
          T result = caseOutputPortSetType(outputPortSetType);
          if (result == null) result = caseOutputPortOpenSetType(outputPortSetType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.OUTPUT_PORTS_TYPE:
        {
          OutputPortsType outputPortsType = (OutputPortsType) theEObject;
          T result = caseOutputPortsType(outputPortsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.PARAMETERS_TYPE:
        {
          ParametersType parametersType = (ParametersType) theEObject;
          T result = caseParametersType(parametersType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.PARAMETER_TYPE:
        {
          ParameterType parameterType = (ParameterType) theEObject;
          T result = caseParameterType(parameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.PORT_SCOPE_TYPE:
        {
          PortScopeType portScopeType = (PortScopeType) theEObject;
          T result = casePortScopeType(portScopeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE:
        {
          SplExpressionTreeType splExpressionTreeType = (SplExpressionTreeType) theEObject;
          T result = caseSplExpressionTreeType(splExpressionTreeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      default:
        return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Code Templates Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Code Templates Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCodeTemplatesType(CodeTemplatesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Code Template Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Code Template Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCodeTemplateType(CodeTemplateType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Context Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Context Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseContextType(ContextType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Custom Output Function Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Custom Output Function Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCustomOutputFunctionSetType(CustomOutputFunctionSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Custom Output Functions
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Custom Output Functions
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCustomOutputFunctionsType(CustomOutputFunctionsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Custom Output Function
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Custom Output Function
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCustomOutputFunctionType(CustomOutputFunctionType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Enumerations Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Enumerations Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseEnumerationsType(EnumerationsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Enumeration Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Enumeration Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseEnumerationType(EnumerationType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Icon Uri Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Icon Uri Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseIconUriType(IconUriType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Input Port Open Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Input Port Open Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseInputPortOpenSetType(InputPortOpenSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Input Port Set Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Input Port Set Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseInputPortSetType(InputPortSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Input Ports Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Input Ports Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseInputPortsType(InputPortsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Context
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Context
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpContextType(JavaOpContextType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Execution Settings
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Execution Settings
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpExecutionSettingsType(JavaOpExecutionSettingsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Input Port Open
   * Set Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Input Port Open
   *     Set Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpInputPortOpenSetType(JavaOpInputPortOpenSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Input Port Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Input Port Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpInputPortSetType(JavaOpInputPortSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Input Ports
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Input Ports
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpInputPortsType(JavaOpInputPortsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Library
   * Dependencies Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Library
   *     Dependencies Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpLibraryDependenciesType(JavaOpLibraryDependenciesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Model Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Model Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpModelType(JavaOpModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Output Port Open
   * Set Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Output Port Open
   *     Set Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpOutputPortOpenSetType(JavaOpOutputPortOpenSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Output Port Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Output Port Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpOutputPortSetType(JavaOpOutputPortSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Output Ports
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Output Ports
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpOutputPortsType(JavaOpOutputPortsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Parameters
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Parameters
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpParametersType(JavaOpParametersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpParameterType(JavaOpParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Java Op VM Args
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Java Op VM Args
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseJavaOpVMArgsType(JavaOpVMArgsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Library Dependencies
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Library Dependencies
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseLibraryDependenciesType(LibraryDependenciesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Metrics Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Metrics Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseMetricsType(MetricsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Metric Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Metric Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseMetricType(MetricType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Model Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Model Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOperatorModelType(OperatorModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Op Model Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Op Model Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOpModelType(OpModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Optional Port Scope
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Optional Port Scope
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOptionalPortScopeType(OptionalPortScopeType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Output Functions
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Output Functions
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOutputFunctionsType(OutputFunctionsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Output Port Open Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Output Port Open Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOutputPortOpenSetType(OutputPortOpenSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Output Port Set
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Output Port Set
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOutputPortSetType(OutputPortSetType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Output Ports Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Output Ports Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOutputPortsType(OutputPortsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Parameters Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Parameters Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseParametersType(ParametersType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Parameter Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Parameter Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseParameterType(ParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Port Scope Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Port Scope Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePortScopeType(PortScopeType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Spl Expression Tree
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Spl Expression Tree
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseSplExpressionTreeType(SplExpressionTreeType object) {
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
} // OperatorSwitch
