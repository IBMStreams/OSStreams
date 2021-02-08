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
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage
 * @generated
 */
public class CodeAdapterFactory extends AdapterFactoryImpl {
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static CodePackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeAdapterFactory() {
    if (modelPackage == null) {
      modelPackage = CodePackage.eINSTANCE;
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
  protected CodeSwitch<Adapter> modelSwitch =
      new CodeSwitch<Adapter>() {
        @Override
        public Adapter caseCompilationUnitType(CompilationUnitType object) {
          return createCompilationUnitTypeAdapter();
        }

        @Override
        public Adapter caseCompositeBodyType(CompositeBodyType object) {
          return createCompositeBodyTypeAdapter();
        }

        @Override
        public Adapter caseCompositeDefinitionType(CompositeDefinitionType object) {
          return createCompositeDefinitionTypeAdapter();
        }

        @Override
        public Adapter caseCompositeGraphType(CompositeGraphType object) {
          return createCompositeGraphTypeAdapter();
        }

        @Override
        public Adapter caseCompositeHeadType(CompositeHeadType object) {
          return createCompositeHeadTypeAdapter();
        }

        @Override
        public Adapter caseCompositeInputsType(CompositeInputsType object) {
          return createCompositeInputsTypeAdapter();
        }

        @Override
        public Adapter caseCompositeModifiersType(CompositeModifiersType object) {
          return createCompositeModifiersTypeAdapter();
        }

        @Override
        public Adapter caseCompositeModifierType(CompositeModifierType object) {
          return createCompositeModifierTypeAdapter();
        }

        @Override
        public Adapter caseCompositeOutputsType(CompositeOutputsType object) {
          return createCompositeOutputsTypeAdapter();
        }

        @Override
        public Adapter caseCompositeParameterExpressionModeType(
            CompositeParameterExpressionModeType object) {
          return createCompositeParameterExpressionModeTypeAdapter();
        }

        @Override
        public Adapter caseCompositeParametersType(CompositeParametersType object) {
          return createCompositeParametersTypeAdapter();
        }

        @Override
        public Adapter caseCompositeParameterType(CompositeParameterType object) {
          return createCompositeParameterTypeAdapter();
        }

        @Override
        public Adapter caseCompositePortType(CompositePortType object) {
          return createCompositePortTypeAdapter();
        }

        @Override
        public Adapter caseCompositeTypesType(CompositeTypesType object) {
          return createCompositeTypesTypeAdapter();
        }

        @Override
        public Adapter caseConfigOptionType(ConfigOptionType object) {
          return createConfigOptionTypeAdapter();
        }

        @Override
        public Adapter caseConfigsType(ConfigsType object) {
          return createConfigsTypeAdapter();
        }

        @Override
        public Adapter caseConfigType(ConfigType object) {
          return createConfigTypeAdapter();
        }

        @Override
        public Adapter caseConfigValueParameterType(ConfigValueParameterType object) {
          return createConfigValueParameterTypeAdapter();
        }

        @Override
        public Adapter caseDefinitionsType(DefinitionsType object) {
          return createDefinitionsTypeAdapter();
        }

        @Override
        public Adapter caseDocumentRoot(DocumentRoot object) {
          return createDocumentRootAdapter();
        }

        @Override
        public Adapter caseExpressionType(ExpressionType object) {
          return createExpressionTypeAdapter();
        }

        @Override
        public Adapter caseExtendedSourceLocationType(ExtendedSourceLocationType object) {
          return createExtendedSourceLocationTypeAdapter();
        }

        @Override
        public Adapter caseFunctionDefinitionType(FunctionDefinitionType object) {
          return createFunctionDefinitionTypeAdapter();
        }

        @Override
        public Adapter caseFunctionHeadType(FunctionHeadType object) {
          return createFunctionHeadTypeAdapter();
        }

        @Override
        public Adapter caseFunctionModifiersType(FunctionModifiersType object) {
          return createFunctionModifiersTypeAdapter();
        }

        @Override
        public Adapter caseFunctionModifierType(FunctionModifierType object) {
          return createFunctionModifierTypeAdapter();
        }

        @Override
        public Adapter caseFunctionParameterModifiersType(FunctionParameterModifiersType object) {
          return createFunctionParameterModifiersTypeAdapter();
        }

        @Override
        public Adapter caseFunctionParameterModifierType(FunctionParameterModifierType object) {
          return createFunctionParameterModifierTypeAdapter();
        }

        @Override
        public Adapter caseFunctionParametersType(FunctionParametersType object) {
          return createFunctionParametersTypeAdapter();
        }

        @Override
        public Adapter caseFunctionParameterType(FunctionParameterType object) {
          return createFunctionParameterTypeAdapter();
        }

        @Override
        public Adapter caseOnProcessType(OnProcessType object) {
          return createOnProcessTypeAdapter();
        }

        @Override
        public Adapter caseOnPunctType(OnPunctType object) {
          return createOnPunctTypeAdapter();
        }

        @Override
        public Adapter caseOnTupleType(OnTupleType object) {
          return createOnTupleTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationAttributeAssignmentType(
            OperatorInvocationAttributeAssignmentType object) {
          return createOperatorInvocationAttributeAssignmentTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationBodyType(OperatorInvocationBodyType object) {
          return createOperatorInvocationBodyTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationHeadType(OperatorInvocationHeadType object) {
          return createOperatorInvocationHeadTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationInputStreamType(
            OperatorInvocationInputStreamType object) {
          return createOperatorInvocationInputStreamTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationInputsType(OperatorInvocationInputsType object) {
          return createOperatorInvocationInputsTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationInputType(OperatorInvocationInputType object) {
          return createOperatorInvocationInputTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationLogicType(OperatorInvocationLogicType object) {
          return createOperatorInvocationLogicTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationOutputAssignmentsType(
            OperatorInvocationOutputAssignmentsType object) {
          return createOperatorInvocationOutputAssignmentsTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationOutputAssignmentType(
            OperatorInvocationOutputAssignmentType object) {
          return createOperatorInvocationOutputAssignmentTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationOutputsType(OperatorInvocationOutputsType object) {
          return createOperatorInvocationOutputsTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationOutputType(OperatorInvocationOutputType object) {
          return createOperatorInvocationOutputTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationParametersType(
            OperatorInvocationParametersType object) {
          return createOperatorInvocationParametersTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationParameterType(OperatorInvocationParameterType object) {
          return createOperatorInvocationParameterTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationType(OperatorInvocationType object) {
          return createOperatorInvocationTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationWindowsType(OperatorInvocationWindowsType object) {
          return createOperatorInvocationWindowsTypeAdapter();
        }

        @Override
        public Adapter caseOperatorInvocationWindowType(OperatorInvocationWindowType object) {
          return createOperatorInvocationWindowTypeAdapter();
        }

        @Override
        public Adapter caseSourceFileType(SourceFileType object) {
          return createSourceFileTypeAdapter();
        }

        @Override
        public Adapter caseSourceLocationType(SourceLocationType object) {
          return createSourceLocationTypeAdapter();
        }

        @Override
        public Adapter caseSourceModelType(SourceModelType object) {
          return createSourceModelTypeAdapter();
        }

        @Override
        public Adapter caseSplDocAnnotationType(SplDocAnnotationType object) {
          return createSplDocAnnotationTypeAdapter();
        }

        @Override
        public Adapter caseSplDocDescriptionType(SplDocDescriptionType object) {
          return createSplDocDescriptionTypeAdapter();
        }

        @Override
        public Adapter caseSplDocType(SplDocType object) {
          return createSplDocTypeAdapter();
        }

        @Override
        public Adapter caseSplNamespaceType(SplNamespaceType object) {
          return createSplNamespaceTypeAdapter();
        }

        @Override
        public Adapter caseTypeDefinitionType(TypeDefinitionType object) {
          return createTypeDefinitionTypeAdapter();
        }

        @Override
        public Adapter caseTypeModifiersType(TypeModifiersType object) {
          return createTypeModifiersTypeAdapter();
        }

        @Override
        public Adapter caseTypeModifierType(TypeModifierType object) {
          return createTypeModifierTypeAdapter();
        }

        @Override
        public Adapter caseUseDirectivesType(UseDirectivesType object) {
          return createUseDirectivesTypeAdapter();
        }

        @Override
        public Adapter caseUseDirectiveType(UseDirectiveType object) {
          return createUseDirectiveTypeAdapter();
        }

        @Override
        public Adapter caseWindowPolicyType(WindowPolicyType object) {
          return createWindowPolicyTypeAdapter();
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
   * com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType <em>Compilation Unit
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType
   * @generated
   */
  public Adapter createCompilationUnitTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType <em>Composite Body Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType
   * @generated
   */
  public Adapter createCompositeBodyTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType <em>Composite Definition
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType
   * @generated
   */
  public Adapter createCompositeDefinitionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType <em>Composite Graph Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType
   * @generated
   */
  public Adapter createCompositeGraphTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType <em>Composite Head Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType
   * @generated
   */
  public Adapter createCompositeHeadTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType <em>Composite Inputs
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType
   * @generated
   */
  public Adapter createCompositeInputsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType <em>Composite Modifiers
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType
   * @generated
   */
  public Adapter createCompositeModifiersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType <em>Composite Modifier
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType
   * @generated
   */
  public Adapter createCompositeModifierTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType <em>Composite Outputs
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType
   * @generated
   */
  public Adapter createCompositeOutputsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType <em>Composite
   * Parameter Expression Mode Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType
   * @generated
   */
  public Adapter createCompositeParameterExpressionModeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType <em>Composite Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType
   * @generated
   */
  public Adapter createCompositeParametersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType <em>Composite Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType
   * @generated
   */
  public Adapter createCompositeParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType <em>Composite Port Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType
   * @generated
   */
  public Adapter createCompositePortTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType <em>Composite Types Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType
   * @generated
   */
  public Adapter createCompositeTypesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType <em>Config Option Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType
   * @generated
   */
  public Adapter createConfigOptionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType <em>Configs Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType
   * @generated
   */
  public Adapter createConfigsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigType <em>Config Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigType
   * @generated
   */
  public Adapter createConfigTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType <em>Config Value Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType
   * @generated
   */
  public Adapter createConfigValueParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType <em>Definitions Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType
   * @generated
   */
  public Adapter createDefinitionsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot
   * @generated
   */
  public Adapter createDocumentRootAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType <em>Expression Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType
   * @generated
   */
  public Adapter createExpressionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType <em>Extended Source
   * Location Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType
   * @generated
   */
  public Adapter createExtendedSourceLocationTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType <em>Function Definition
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType
   * @generated
   */
  public Adapter createFunctionDefinitionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType <em>Function Head Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType
   * @generated
   */
  public Adapter createFunctionHeadTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType <em>Function Modifiers
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType
   * @generated
   */
  public Adapter createFunctionModifiersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType <em>Function Modifier
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType
   * @generated
   */
  public Adapter createFunctionModifierTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType <em>Function
   * Parameter Modifiers Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType
   * @generated
   */
  public Adapter createFunctionParameterModifiersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType <em>Function Parameter
   * Modifier Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType
   * @generated
   */
  public Adapter createFunctionParameterModifierTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType <em>Function Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType
   * @generated
   */
  public Adapter createFunctionParametersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType <em>Function Parameter
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType
   * @generated
   */
  public Adapter createFunctionParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType <em>On Process Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType
   * @generated
   */
  public Adapter createOnProcessTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType <em>On Punct Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType
   * @generated
   */
  public Adapter createOnPunctTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType <em>On Tuple Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType
   * @generated
   */
  public Adapter createOnTupleTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType
   * <em>Operator Invocation Attribute Assignment Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType
   * @generated
   */
  public Adapter createOperatorInvocationAttributeAssignmentTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType <em>Operator Invocation
   * Body Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType
   * @generated
   */
  public Adapter createOperatorInvocationBodyTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType <em>Operator Invocation
   * Head Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType
   * @generated
   */
  public Adapter createOperatorInvocationHeadTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType <em>Operator
   * Invocation Input Stream Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType
   * @generated
   */
  public Adapter createOperatorInvocationInputStreamTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType <em>Operator Invocation
   * Inputs Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType
   * @generated
   */
  public Adapter createOperatorInvocationInputsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType <em>Operator Invocation
   * Input Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType
   * @generated
   */
  public Adapter createOperatorInvocationInputTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType <em>Operator Invocation
   * Logic Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType
   * @generated
   */
  public Adapter createOperatorInvocationLogicTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType <em>Operator
   * Invocation Output Assignments Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType
   * @generated
   */
  public Adapter createOperatorInvocationOutputAssignmentsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType <em>Operator
   * Invocation Output Assignment Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType
   * @generated
   */
  public Adapter createOperatorInvocationOutputAssignmentTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType <em>Operator
   * Invocation Outputs Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType
   * @generated
   */
  public Adapter createOperatorInvocationOutputsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType <em>Operator Invocation
   * Output Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType
   * @generated
   */
  public Adapter createOperatorInvocationOutputTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType <em>Operator
   * Invocation Parameters Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType
   * @generated
   */
  public Adapter createOperatorInvocationParametersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType <em>Operator
   * Invocation Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType
   * @generated
   */
  public Adapter createOperatorInvocationParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType <em>Operator Invocation
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType
   * @generated
   */
  public Adapter createOperatorInvocationTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType <em>Operator
   * Invocation Windows Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType
   * @generated
   */
  public Adapter createOperatorInvocationWindowsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType <em>Operator Invocation
   * Window Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType
   * @generated
   */
  public Adapter createOperatorInvocationWindowTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType <em>Source File Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType
   * @generated
   */
  public Adapter createSourceFileTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType <em>Source Location Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType
   * @generated
   */
  public Adapter createSourceLocationTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType <em>Source Model Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType
   * @generated
   */
  public Adapter createSourceModelTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType <em>Spl Doc Annotation
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType
   * @generated
   */
  public Adapter createSplDocAnnotationTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType <em>Spl Doc Description
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType
   * @generated
   */
  public Adapter createSplDocDescriptionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplDocType <em>Spl Doc Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplDocType
   * @generated
   */
  public Adapter createSplDocTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType <em>Spl Namespace Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType
   * @generated
   */
  public Adapter createSplNamespaceTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType <em>Type Definition Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType
   * @generated
   */
  public Adapter createTypeDefinitionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType <em>Type Modifiers Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType
   * @generated
   */
  public Adapter createTypeModifiersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType <em>Type Modifier Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType
   * @generated
   */
  public Adapter createTypeModifierTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType <em>Use Directives Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType
   * @generated
   */
  public Adapter createUseDirectivesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType <em>Use Directive Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType
   * @generated
   */
  public Adapter createUseDirectiveTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType <em>Window Policy Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType
   * @generated
   */
  public Adapter createWindowPolicyTypeAdapter() {
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
} // CodeAdapterFactory
