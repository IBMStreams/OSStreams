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
package com.ibm.xmlns.prod.streams.spl.toolkit.util;

import com.ibm.xmlns.prod.streams.spl.toolkit.*;
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
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage
 * @generated
 */
public class ToolkitSwitch<T> extends Switch<T> {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static ToolkitPackage modelPackage;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitSwitch() {
    if (modelPackage == null) {
      modelPackage = ToolkitPackage.eINSTANCE;
    }
  }

  /**
   * Checks whether this is a switch for the given package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @parameter ePackage the package in question.
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
      case ToolkitPackage.BASE_FUNCTION_TYPE:
        {
          BaseFunctionType baseFunctionType = (BaseFunctionType) theEObject;
          T result = caseBaseFunctionType(baseFunctionType);
          if (result == null) result = caseToolkitElementType(baseFunctionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.BASE_TOOLKIT_OPERATOR_TYPE:
        {
          BaseToolkitOperatorType baseToolkitOperatorType = (BaseToolkitOperatorType) theEObject;
          T result = caseBaseToolkitOperatorType(baseToolkitOperatorType);
          if (result == null) result = caseToolkitElementType(baseToolkitOperatorType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.CODE_TEMPLATE_TYPE:
        {
          CodeTemplateType codeTemplateType = (CodeTemplateType) theEObject;
          T result = caseCodeTemplateType(codeTemplateType);
          if (result == null) result = caseToolkitElementType(codeTemplateType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.COMPOSITE_OP_PARAMETER_TYPE:
        {
          CompositeOpParameterType compositeOpParameterType = (CompositeOpParameterType) theEObject;
          T result = caseCompositeOpParameterType(compositeOpParameterType);
          if (result == null) result = caseParameterType(compositeOpParameterType);
          if (result == null) result = caseToolkitElementType(compositeOpParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.COMPOSITE_OP_PORT_TYPE:
        {
          CompositeOpPortType compositeOpPortType = (CompositeOpPortType) theEObject;
          T result = caseCompositeOpPortType(compositeOpPortType);
          if (result == null) result = caseToolkitElementType(compositeOpPortType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.COMPOSITE_TOOLKIT_OPERATOR_TYPE:
        {
          CompositeToolkitOperatorType compositeToolkitOperatorType =
              (CompositeToolkitOperatorType) theEObject;
          T result = caseCompositeToolkitOperatorType(compositeToolkitOperatorType);
          if (result == null) result = caseBaseToolkitOperatorType(compositeToolkitOperatorType);
          if (result == null) result = caseToolkitElementType(compositeToolkitOperatorType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.CUSTOM_LITERALS_TYPE:
        {
          CustomLiteralsType customLiteralsType = (CustomLiteralsType) theEObject;
          T result = caseCustomLiteralsType(customLiteralsType);
          if (result == null) result = caseToolkitElementType(customLiteralsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.CUSTOM_LITERAL_VALUE_TYPE:
        {
          CustomLiteralValueType customLiteralValueType = (CustomLiteralValueType) theEObject;
          T result = caseCustomLiteralValueType(customLiteralValueType);
          if (result == null) result = caseToolkitElementType(customLiteralValueType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.DOCUMENT_ROOT:
        {
          DocumentRoot documentRoot = (DocumentRoot) theEObject;
          T result = caseDocumentRoot(documentRoot);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.ENUM_LITERALS_TYPE:
        {
          EnumLiteralsType enumLiteralsType = (EnumLiteralsType) theEObject;
          T result = caseEnumLiteralsType(enumLiteralsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.IMAGES_TYPE:
        {
          ImagesType imagesType = (ImagesType) theEObject;
          T result = caseImagesType(imagesType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.IMAGE_TYPE:
        {
          ImageType imageType = (ImageType) theEObject;
          T result = caseImageType(imageType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.OUTPUT_FUNCTIONS_TYPE:
        {
          OutputFunctionsType outputFunctionsType = (OutputFunctionsType) theEObject;
          T result = caseOutputFunctionsType(outputFunctionsType);
          if (result == null) result = caseToolkitElementType(outputFunctionsType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.OUTPUT_FUNCTIONS_TYPE_TYPE:
        {
          OutputFunctionsTypeType outputFunctionsTypeType = (OutputFunctionsTypeType) theEObject;
          T result = caseOutputFunctionsTypeType(outputFunctionsTypeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PARAMETER_TYPE:
        {
          ParameterType parameterType = (ParameterType) theEObject;
          T result = caseParameterType(parameterType);
          if (result == null) result = caseToolkitElementType(parameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PRIMITIVE_OP_INPUT_PORT_TYPE:
        {
          PrimitiveOpInputPortType primitiveOpInputPortType = (PrimitiveOpInputPortType) theEObject;
          T result = casePrimitiveOpInputPortType(primitiveOpInputPortType);
          if (result == null) result = casePrimitivePortDescriptionType(primitiveOpInputPortType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PRIMITIVE_OP_OUTPUT_PORT_TYPE:
        {
          PrimitiveOpOutputPortType primitiveOpOutputPortType =
              (PrimitiveOpOutputPortType) theEObject;
          T result = casePrimitiveOpOutputPortType(primitiveOpOutputPortType);
          if (result == null) result = casePrimitivePortDescriptionType(primitiveOpOutputPortType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PRIMITIVE_OP_PARAMETER_TYPE:
        {
          PrimitiveOpParameterType primitiveOpParameterType = (PrimitiveOpParameterType) theEObject;
          T result = casePrimitiveOpParameterType(primitiveOpParameterType);
          if (result == null) result = caseParameterType(primitiveOpParameterType);
          if (result == null) result = caseToolkitElementType(primitiveOpParameterType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PRIMITIVE_PORT_DESCRIPTION_TYPE:
        {
          PrimitivePortDescriptionType primitivePortDescriptionType =
              (PrimitivePortDescriptionType) theEObject;
          T result = casePrimitivePortDescriptionType(primitivePortDescriptionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.PRIMITIVE_TOOLKIT_OPERATOR_TYPE:
        {
          PrimitiveToolkitOperatorType primitiveToolkitOperatorType =
              (PrimitiveToolkitOperatorType) theEObject;
          T result = casePrimitiveToolkitOperatorType(primitiveToolkitOperatorType);
          if (result == null) result = caseBaseToolkitOperatorType(primitiveToolkitOperatorType);
          if (result == null) result = caseToolkitElementType(primitiveToolkitOperatorType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_BASE_TYPE:
        {
          ToolkitBaseType toolkitBaseType = (ToolkitBaseType) theEObject;
          T result = caseToolkitBaseType(toolkitBaseType);
          if (result == null) result = caseToolkitElementType(toolkitBaseType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_ELEMENT_TYPE:
        {
          ToolkitElementType toolkitElementType = (ToolkitElementType) theEObject;
          T result = caseToolkitElementType(toolkitElementType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_FUNCTION_TYPE:
        {
          ToolkitFunctionType toolkitFunctionType = (ToolkitFunctionType) theEObject;
          T result = caseToolkitFunctionType(toolkitFunctionType);
          if (result == null) result = caseBaseFunctionType(toolkitFunctionType);
          if (result == null) result = caseToolkitElementType(toolkitFunctionType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_MODEL_TYPE:
        {
          ToolkitModelType toolkitModelType = (ToolkitModelType) theEObject;
          T result = caseToolkitModelType(toolkitModelType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_NAMESPACE_TYPE:
        {
          ToolkitNamespaceType toolkitNamespaceType = (ToolkitNamespaceType) theEObject;
          T result = caseToolkitNamespaceType(toolkitNamespaceType);
          if (result == null) result = caseToolkitElementType(toolkitNamespaceType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_TYPE:
        {
          ToolkitType toolkitType = (ToolkitType) theEObject;
          T result = caseToolkitType(toolkitType);
          if (result == null) result = caseToolkitBaseType(toolkitType);
          if (result == null) result = caseToolkitElementType(toolkitType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.TOOLKIT_TYPE_TYPE:
        {
          ToolkitTypeType toolkitTypeType = (ToolkitTypeType) theEObject;
          T result = caseToolkitTypeType(toolkitTypeType);
          if (result == null) result = caseToolkitElementType(toolkitTypeType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      case ToolkitPackage.URI_TABLE_TYPE:
        {
          UriTableType uriTableType = (UriTableType) theEObject;
          T result = caseUriTableType(uriTableType);
          if (result == null) result = defaultCase(theEObject);
          return result;
        }
      default:
        return defaultCase(theEObject);
    }
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Base Function Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Base Function Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseBaseFunctionType(BaseFunctionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Base Toolkit Operator
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Base Toolkit Operator
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseBaseToolkitOperatorType(BaseToolkitOperatorType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Composite Op Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Op Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeOpParameterType(CompositeOpParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Op Port
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Op Port
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeOpPortType(CompositeOpPortType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Composite Toolkit Operator
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Composite Toolkit Operator
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCompositeToolkitOperatorType(CompositeToolkitOperatorType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Custom Literals
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Custom Literals
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCustomLiteralsType(CustomLiteralsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Custom Literal Value
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Custom Literal Value
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseCustomLiteralValueType(CustomLiteralValueType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Enum Literals Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Enum Literals Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseEnumLiteralsType(EnumLiteralsType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Images Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Images Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseImagesType(ImagesType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Image Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Image Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseImageType(ImageType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Output Functions Type
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Output Functions Type
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseOutputFunctionsTypeType(OutputFunctionsTypeType object) {
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
   * Returns the result of interpreting the object as an instance of '<em>Primitive Op Input Port
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Primitive Op Input Port
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrimitiveOpInputPortType(PrimitiveOpInputPortType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Primitive Op Output Port
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Primitive Op Output Port
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrimitiveOpOutputPortType(PrimitiveOpOutputPortType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Primitive Op Parameter
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Primitive Op Parameter
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrimitiveOpParameterType(PrimitiveOpParameterType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Primitive Port Description
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Primitive Port Description
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrimitivePortDescriptionType(PrimitivePortDescriptionType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Primitive Toolkit Operator
   * Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Primitive Toolkit Operator
   *     Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T casePrimitiveToolkitOperatorType(PrimitiveToolkitOperatorType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Base Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Base Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitBaseType(ToolkitBaseType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Element Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Element Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitElementType(ToolkitElementType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Function Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Function Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitFunctionType(ToolkitFunctionType object) {
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
  public T caseToolkitModelType(ToolkitModelType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Namespace Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Namespace Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitNamespaceType(ToolkitNamespaceType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitType(ToolkitType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Type Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Type Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseToolkitTypeType(ToolkitTypeType object) {
    return null;
  }

  /**
   * Returns the result of interpreting the object as an instance of '<em>Uri Table Type</em>'.
   * <!-- begin-user-doc -->
   * This implementation returns null; returning a non-null result will terminate the switch.
   * <!-- end-user-doc -->
   *
   * @param object the target of the switch.
   * @return the result of interpreting the object as an instance of '<em>Uri Table Type</em>'.
   * @see #doSwitch(org.eclipse.emf.ecore.EObject) doSwitch(EObject)
   * @generated
   */
  public T caseUriTableType(UriTableType object) {
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
} // ToolkitSwitch
