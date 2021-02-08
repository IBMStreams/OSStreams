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

import org.eclipse.emf.ecore.EFactory;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Factory</b> for the model. It provides a create method for each non-abstract class of the
 * model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage
 * @generated
 */
public interface ToolkitFactory extends EFactory {
  /**
   * The singleton instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  ToolkitFactory eINSTANCE = com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitFactoryImpl.init();

  /**
   * Returns a new object of class '<em>Base Function Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Base Function Type</em>'.
   * @generated
   */
  BaseFunctionType createBaseFunctionType();

  /**
   * Returns a new object of class '<em>Base Toolkit Operator Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Base Toolkit Operator Type</em>'.
   * @generated
   */
  BaseToolkitOperatorType createBaseToolkitOperatorType();

  /**
   * Returns a new object of class '<em>Code Template Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Code Template Type</em>'.
   * @generated
   */
  CodeTemplateType createCodeTemplateType();

  /**
   * Returns a new object of class '<em>Composite Op Parameter Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Composite Op Parameter Type</em>'.
   * @generated
   */
  CompositeOpParameterType createCompositeOpParameterType();

  /**
   * Returns a new object of class '<em>Composite Op Port Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Composite Op Port Type</em>'.
   * @generated
   */
  CompositeOpPortType createCompositeOpPortType();

  /**
   * Returns a new object of class '<em>Composite Toolkit Operator Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Composite Toolkit Operator Type</em>'.
   * @generated
   */
  CompositeToolkitOperatorType createCompositeToolkitOperatorType();

  /**
   * Returns a new object of class '<em>Custom Literals Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Custom Literals Type</em>'.
   * @generated
   */
  CustomLiteralsType createCustomLiteralsType();

  /**
   * Returns a new object of class '<em>Custom Literal Value Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Custom Literal Value Type</em>'.
   * @generated
   */
  CustomLiteralValueType createCustomLiteralValueType();

  /**
   * Returns a new object of class '<em>Document Root</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Document Root</em>'.
   * @generated
   */
  DocumentRoot createDocumentRoot();

  /**
   * Returns a new object of class '<em>Enum Literals Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Enum Literals Type</em>'.
   * @generated
   */
  EnumLiteralsType createEnumLiteralsType();

  /**
   * Returns a new object of class '<em>Images Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Images Type</em>'.
   * @generated
   */
  ImagesType createImagesType();

  /**
   * Returns a new object of class '<em>Image Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Image Type</em>'.
   * @generated
   */
  ImageType createImageType();

  /**
   * Returns a new object of class '<em>Output Functions Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Output Functions Type</em>'.
   * @generated
   */
  OutputFunctionsType createOutputFunctionsType();

  /**
   * Returns a new object of class '<em>Output Functions Type Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Output Functions Type Type</em>'.
   * @generated
   */
  OutputFunctionsTypeType createOutputFunctionsTypeType();

  /**
   * Returns a new object of class '<em>Parameter Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Parameter Type</em>'.
   * @generated
   */
  ParameterType createParameterType();

  /**
   * Returns a new object of class '<em>Primitive Op Input Port Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Primitive Op Input Port Type</em>'.
   * @generated
   */
  PrimitiveOpInputPortType createPrimitiveOpInputPortType();

  /**
   * Returns a new object of class '<em>Primitive Op Output Port Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Primitive Op Output Port Type</em>'.
   * @generated
   */
  PrimitiveOpOutputPortType createPrimitiveOpOutputPortType();

  /**
   * Returns a new object of class '<em>Primitive Op Parameter Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Primitive Op Parameter Type</em>'.
   * @generated
   */
  PrimitiveOpParameterType createPrimitiveOpParameterType();

  /**
   * Returns a new object of class '<em>Primitive Port Description Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Primitive Port Description Type</em>'.
   * @generated
   */
  PrimitivePortDescriptionType createPrimitivePortDescriptionType();

  /**
   * Returns a new object of class '<em>Primitive Toolkit Operator Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Primitive Toolkit Operator Type</em>'.
   * @generated
   */
  PrimitiveToolkitOperatorType createPrimitiveToolkitOperatorType();

  /**
   * Returns a new object of class '<em>Base Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Base Type</em>'.
   * @generated
   */
  ToolkitBaseType createToolkitBaseType();

  /**
   * Returns a new object of class '<em>Element Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Element Type</em>'.
   * @generated
   */
  ToolkitElementType createToolkitElementType();

  /**
   * Returns a new object of class '<em>Function Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Function Type</em>'.
   * @generated
   */
  ToolkitFunctionType createToolkitFunctionType();

  /**
   * Returns a new object of class '<em>Model Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Model Type</em>'.
   * @generated
   */
  ToolkitModelType createToolkitModelType();

  /**
   * Returns a new object of class '<em>Namespace Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Namespace Type</em>'.
   * @generated
   */
  ToolkitNamespaceType createToolkitNamespaceType();

  /**
   * Returns a new object of class '<em>Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Type</em>'.
   * @generated
   */
  ToolkitType createToolkitType();

  /**
   * Returns a new object of class '<em>Type Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Type Type</em>'.
   * @generated
   */
  ToolkitTypeType createToolkitTypeType();

  /**
   * Returns a new object of class '<em>Uri Table Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return a new object of class '<em>Uri Table Type</em>'.
   * @generated
   */
  UriTableType createUriTableType();

  /**
   * Returns the package supported by this factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the package supported by this factory.
   * @generated
   */
  ToolkitPackage getToolkitPackage();
} // ToolkitFactory
