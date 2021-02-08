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
import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;
import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model. It provides an adapter <code>createXXX</code> method
 * for each class of the model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage
 * @generated
 */
public class ToolkitAdapterFactory extends AdapterFactoryImpl {
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static ToolkitPackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitAdapterFactory() {
    if (modelPackage == null) {
      modelPackage = ToolkitPackage.eINSTANCE;
    }
  }

  /**
   * Returns whether this factory is applicable for the type of the object.
   * <!-- begin-user-doc -->
   * This implementation returns <code>true</code> if the object is either the model's package or is
   * an instance object of the model.
   * <!-- end-user-doc -->
   *
   * @return whether this factory is applicable for the type of the object.
   * @generated
   */
  @Override
  public boolean isFactoryForType(Object object) {
    if (object == modelPackage) {
      return true;
    }
    if (object instanceof EObject) {
      return ((EObject) object).eClass().getEPackage() == modelPackage;
    }
    return false;
  }

  /**
   * The switch that delegates to the <code>createXXX</code> methods.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected ToolkitSwitch<Adapter> modelSwitch =
      new ToolkitSwitch<Adapter>() {
        @Override
        public Adapter caseBaseFunctionType(BaseFunctionType object) {
          return createBaseFunctionTypeAdapter();
        }

        @Override
        public Adapter caseBaseToolkitOperatorType(BaseToolkitOperatorType object) {
          return createBaseToolkitOperatorTypeAdapter();
        }

        @Override
        public Adapter caseCodeTemplateType(CodeTemplateType object) {
          return createCodeTemplateTypeAdapter();
        }

        @Override
        public Adapter caseCompositeOpParameterType(CompositeOpParameterType object) {
          return createCompositeOpParameterTypeAdapter();
        }

        @Override
        public Adapter caseCompositeOpPortType(CompositeOpPortType object) {
          return createCompositeOpPortTypeAdapter();
        }

        @Override
        public Adapter caseCompositeToolkitOperatorType(CompositeToolkitOperatorType object) {
          return createCompositeToolkitOperatorTypeAdapter();
        }

        @Override
        public Adapter caseCustomLiteralsType(CustomLiteralsType object) {
          return createCustomLiteralsTypeAdapter();
        }

        @Override
        public Adapter caseCustomLiteralValueType(CustomLiteralValueType object) {
          return createCustomLiteralValueTypeAdapter();
        }

        @Override
        public Adapter caseDocumentRoot(DocumentRoot object) {
          return createDocumentRootAdapter();
        }

        @Override
        public Adapter caseEnumLiteralsType(EnumLiteralsType object) {
          return createEnumLiteralsTypeAdapter();
        }

        @Override
        public Adapter caseImagesType(ImagesType object) {
          return createImagesTypeAdapter();
        }

        @Override
        public Adapter caseImageType(ImageType object) {
          return createImageTypeAdapter();
        }

        @Override
        public Adapter caseOutputFunctionsType(OutputFunctionsType object) {
          return createOutputFunctionsTypeAdapter();
        }

        @Override
        public Adapter caseOutputFunctionsTypeType(OutputFunctionsTypeType object) {
          return createOutputFunctionsTypeTypeAdapter();
        }

        @Override
        public Adapter caseParameterType(ParameterType object) {
          return createParameterTypeAdapter();
        }

        @Override
        public Adapter casePrimitiveOpInputPortType(PrimitiveOpInputPortType object) {
          return createPrimitiveOpInputPortTypeAdapter();
        }

        @Override
        public Adapter casePrimitiveOpOutputPortType(PrimitiveOpOutputPortType object) {
          return createPrimitiveOpOutputPortTypeAdapter();
        }

        @Override
        public Adapter casePrimitiveOpParameterType(PrimitiveOpParameterType object) {
          return createPrimitiveOpParameterTypeAdapter();
        }

        @Override
        public Adapter casePrimitivePortDescriptionType(PrimitivePortDescriptionType object) {
          return createPrimitivePortDescriptionTypeAdapter();
        }

        @Override
        public Adapter casePrimitiveToolkitOperatorType(PrimitiveToolkitOperatorType object) {
          return createPrimitiveToolkitOperatorTypeAdapter();
        }

        @Override
        public Adapter caseToolkitBaseType(ToolkitBaseType object) {
          return createToolkitBaseTypeAdapter();
        }

        @Override
        public Adapter caseToolkitElementType(ToolkitElementType object) {
          return createToolkitElementTypeAdapter();
        }

        @Override
        public Adapter caseToolkitFunctionType(ToolkitFunctionType object) {
          return createToolkitFunctionTypeAdapter();
        }

        @Override
        public Adapter caseToolkitModelType(ToolkitModelType object) {
          return createToolkitModelTypeAdapter();
        }

        @Override
        public Adapter caseToolkitNamespaceType(ToolkitNamespaceType object) {
          return createToolkitNamespaceTypeAdapter();
        }

        @Override
        public Adapter caseToolkitType(ToolkitType object) {
          return createToolkitTypeAdapter();
        }

        @Override
        public Adapter caseToolkitTypeType(ToolkitTypeType object) {
          return createToolkitTypeTypeAdapter();
        }

        @Override
        public Adapter caseUriTableType(UriTableType object) {
          return createUriTableTypeAdapter();
        }

        @Override
        public Adapter defaultCase(EObject object) {
          return createEObjectAdapter();
        }
      };

  /**
   * Creates an adapter for the <code>target</code>.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param target the object to adapt.
   * @return the adapter for the <code>target</code>.
   * @generated
   */
  @Override
  public Adapter createAdapter(Notifier target) {
    return modelSwitch.doSwitch((EObject) target);
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType <em>Base Function Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType
   * @generated
   */
  public Adapter createBaseFunctionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType <em>Base Toolkit Operator
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType
   * @generated
   */
  public Adapter createBaseToolkitOperatorTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType <em>Code Template Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType
   * @generated
   */
  public Adapter createCodeTemplateTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType <em>Composite Op Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType
   * @generated
   */
  public Adapter createCompositeOpParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType <em>Composite Op Port Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType
   * @generated
   */
  public Adapter createCompositeOpPortTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType <em>Composite Toolkit
   * Operator Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType
   * @generated
   */
  public Adapter createCompositeToolkitOperatorTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType <em>Custom Literals Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType
   * @generated
   */
  public Adapter createCustomLiteralsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralValueType <em>Custom Literal Value
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralValueType
   * @generated
   */
  public Adapter createCustomLiteralValueTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot
   * @generated
   */
  public Adapter createDocumentRootAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType <em>Enum Literals Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType
   * @generated
   */
  public Adapter createEnumLiteralsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType <em>Images Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType
   * @generated
   */
  public Adapter createImagesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ImageType <em>Image Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ImageType
   * @generated
   */
  public Adapter createImageTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType <em>Output Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType
   * @generated
   */
  public Adapter createOutputFunctionsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType <em>Output Functions Type
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType
   * @generated
   */
  public Adapter createOutputFunctionsTypeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType <em>Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType
   * @generated
   */
  public Adapter createParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType <em>Primitive Op Input Port
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType
   * @generated
   */
  public Adapter createPrimitiveOpInputPortTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType <em>Primitive Op Output Port
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType
   * @generated
   */
  public Adapter createPrimitiveOpOutputPortTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType <em>Primitive Op Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType
   * @generated
   */
  public Adapter createPrimitiveOpParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType <em>Primitive Port
   * Description Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType
   * @generated
   */
  public Adapter createPrimitivePortDescriptionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType <em>Primitive Toolkit
   * Operator Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType
   * @generated
   */
  public Adapter createPrimitiveToolkitOperatorTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType <em>Base Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType
   * @generated
   */
  public Adapter createToolkitBaseTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType <em>Element Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType
   * @generated
   */
  public Adapter createToolkitElementTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType <em>Function Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType
   * @generated
   */
  public Adapter createToolkitFunctionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType <em>Model Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType
   * @generated
   */
  public Adapter createToolkitModelTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType <em>Namespace Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType
   * @generated
   */
  public Adapter createToolkitNamespaceTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType <em>Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType
   * @generated
   */
  public Adapter createToolkitTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType <em>Type Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType
   * @generated
   */
  public Adapter createToolkitTypeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType <em>Uri Table Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType
   * @generated
   */
  public Adapter createUriTableTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for the default case.
   * <!-- begin-user-doc -->
   * This default implementation returns null.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @generated
   */
  public Adapter createEObjectAdapter() {
    return null;
  }
} // ToolkitAdapterFactory
