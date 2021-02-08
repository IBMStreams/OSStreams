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
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage
 * @generated
 */
public class OperatorAdapterFactory extends AdapterFactoryImpl {
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static OperatorPackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorAdapterFactory() {
    if (modelPackage == null) {
      modelPackage = OperatorPackage.eINSTANCE;
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
  protected OperatorSwitch<Adapter> modelSwitch =
      new OperatorSwitch<Adapter>() {
        @Override
        public Adapter caseCodeTemplatesType(CodeTemplatesType object) {
          return createCodeTemplatesTypeAdapter();
        }

        @Override
        public Adapter caseCodeTemplateType(CodeTemplateType object) {
          return createCodeTemplateTypeAdapter();
        }

        @Override
        public Adapter caseContextType(ContextType object) {
          return createContextTypeAdapter();
        }

        @Override
        public Adapter caseCustomOutputFunctionSetType(CustomOutputFunctionSetType object) {
          return createCustomOutputFunctionSetTypeAdapter();
        }

        @Override
        public Adapter caseCustomOutputFunctionsType(CustomOutputFunctionsType object) {
          return createCustomOutputFunctionsTypeAdapter();
        }

        @Override
        public Adapter caseCustomOutputFunctionType(CustomOutputFunctionType object) {
          return createCustomOutputFunctionTypeAdapter();
        }

        @Override
        public Adapter caseDocumentRoot(DocumentRoot object) {
          return createDocumentRootAdapter();
        }

        @Override
        public Adapter caseEnumerationsType(EnumerationsType object) {
          return createEnumerationsTypeAdapter();
        }

        @Override
        public Adapter caseEnumerationType(EnumerationType object) {
          return createEnumerationTypeAdapter();
        }

        @Override
        public Adapter caseIconUriType(IconUriType object) {
          return createIconUriTypeAdapter();
        }

        @Override
        public Adapter caseInputPortOpenSetType(InputPortOpenSetType object) {
          return createInputPortOpenSetTypeAdapter();
        }

        @Override
        public Adapter caseInputPortSetType(InputPortSetType object) {
          return createInputPortSetTypeAdapter();
        }

        @Override
        public Adapter caseInputPortsType(InputPortsType object) {
          return createInputPortsTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpContextType(JavaOpContextType object) {
          return createJavaOpContextTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpExecutionSettingsType(JavaOpExecutionSettingsType object) {
          return createJavaOpExecutionSettingsTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpInputPortOpenSetType(JavaOpInputPortOpenSetType object) {
          return createJavaOpInputPortOpenSetTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpInputPortSetType(JavaOpInputPortSetType object) {
          return createJavaOpInputPortSetTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpInputPortsType(JavaOpInputPortsType object) {
          return createJavaOpInputPortsTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpLibraryDependenciesType(JavaOpLibraryDependenciesType object) {
          return createJavaOpLibraryDependenciesTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpModelType(JavaOpModelType object) {
          return createJavaOpModelTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpOutputPortOpenSetType(JavaOpOutputPortOpenSetType object) {
          return createJavaOpOutputPortOpenSetTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpOutputPortSetType(JavaOpOutputPortSetType object) {
          return createJavaOpOutputPortSetTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpOutputPortsType(JavaOpOutputPortsType object) {
          return createJavaOpOutputPortsTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpParametersType(JavaOpParametersType object) {
          return createJavaOpParametersTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpParameterType(JavaOpParameterType object) {
          return createJavaOpParameterTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpVMArgsType(JavaOpVMArgsType object) {
          return createJavaOpVMArgsTypeAdapter();
        }

        @Override
        public Adapter caseLibraryDependenciesType(LibraryDependenciesType object) {
          return createLibraryDependenciesTypeAdapter();
        }

        @Override
        public Adapter caseMetricsType(MetricsType object) {
          return createMetricsTypeAdapter();
        }

        @Override
        public Adapter caseMetricType(MetricType object) {
          return createMetricTypeAdapter();
        }

        @Override
        public Adapter caseOperatorModelType(OperatorModelType object) {
          return createOperatorModelTypeAdapter();
        }

        @Override
        public Adapter caseOpModelType(OpModelType object) {
          return createOpModelTypeAdapter();
        }

        @Override
        public Adapter caseOptionalPortScopeType(OptionalPortScopeType object) {
          return createOptionalPortScopeTypeAdapter();
        }

        @Override
        public Adapter caseOutputFunctionsType(OutputFunctionsType object) {
          return createOutputFunctionsTypeAdapter();
        }

        @Override
        public Adapter caseOutputPortOpenSetType(OutputPortOpenSetType object) {
          return createOutputPortOpenSetTypeAdapter();
        }

        @Override
        public Adapter caseOutputPortSetType(OutputPortSetType object) {
          return createOutputPortSetTypeAdapter();
        }

        @Override
        public Adapter caseOutputPortsType(OutputPortsType object) {
          return createOutputPortsTypeAdapter();
        }

        @Override
        public Adapter caseParametersType(ParametersType object) {
          return createParametersTypeAdapter();
        }

        @Override
        public Adapter caseParameterType(ParameterType object) {
          return createParameterTypeAdapter();
        }

        @Override
        public Adapter casePortScopeType(PortScopeType object) {
          return createPortScopeTypeAdapter();
        }

        @Override
        public Adapter caseSplExpressionTreeType(SplExpressionTreeType object) {
          return createSplExpressionTreeTypeAdapter();
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
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType <em>Code Templates Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType
   * @generated
   */
  public Adapter createCodeTemplatesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType <em>Code Template Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType
   * @generated
   */
  public Adapter createCodeTemplateTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ContextType <em>Context Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ContextType
   * @generated
   */
  public Adapter createContextTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType <em>Custom Output Function
   * Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType
   * @generated
   */
  public Adapter createCustomOutputFunctionSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType <em>Custom Output Functions
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType
   * @generated
   */
  public Adapter createCustomOutputFunctionsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType <em>Custom Output Function
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType
   * @generated
   */
  public Adapter createCustomOutputFunctionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot
   * @generated
   */
  public Adapter createDocumentRootAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType <em>Enumerations Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType
   * @generated
   */
  public Adapter createEnumerationsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.EnumerationType <em>Enumeration Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.EnumerationType
   * @generated
   */
  public Adapter createEnumerationTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.IconUriType <em>Icon Uri Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.IconUriType
   * @generated
   */
  public Adapter createIconUriTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType <em>Input Port Open Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType
   * @generated
   */
  public Adapter createInputPortOpenSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType <em>Input Port Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType
   * @generated
   */
  public Adapter createInputPortSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.InputPortsType <em>Input Ports Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.InputPortsType
   * @generated
   */
  public Adapter createInputPortsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType <em>Java Op Context Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType
   * @generated
   */
  public Adapter createJavaOpContextTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType <em>Java Op Execution
   * Settings Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType
   * @generated
   */
  public Adapter createJavaOpExecutionSettingsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType <em>Java Op Input Port Open
   * Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType
   * @generated
   */
  public Adapter createJavaOpInputPortOpenSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType <em>Java Op Input Port Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType
   * @generated
   */
  public Adapter createJavaOpInputPortSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType <em>Java Op Input Ports
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType
   * @generated
   */
  public Adapter createJavaOpInputPortsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType <em>Java Op Library
   * Dependencies Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType
   * @generated
   */
  public Adapter createJavaOpLibraryDependenciesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType <em>Java Op Model Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType
   * @generated
   */
  public Adapter createJavaOpModelTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType <em>Java Op Output Port
   * Open Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType
   * @generated
   */
  public Adapter createJavaOpOutputPortOpenSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType <em>Java Op Output Port Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType
   * @generated
   */
  public Adapter createJavaOpOutputPortSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType <em>Java Op Output Ports
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType
   * @generated
   */
  public Adapter createJavaOpOutputPortsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType <em>Java Op Parameters
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType
   * @generated
   */
  public Adapter createJavaOpParametersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType <em>Java Op Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType
   * @generated
   */
  public Adapter createJavaOpParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType <em>Java Op VM Args Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType
   * @generated
   */
  public Adapter createJavaOpVMArgsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType <em>Library Dependencies
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType
   * @generated
   */
  public Adapter createLibraryDependenciesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricsType <em>Metrics Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricsType
   * @generated
   */
  public Adapter createMetricsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.MetricType <em>Metric Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.MetricType
   * @generated
   */
  public Adapter createMetricTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType <em>Model Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType
   * @generated
   */
  public Adapter createOperatorModelTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OpModelType <em>Op Model Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OpModelType
   * @generated
   */
  public Adapter createOpModelTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType <em>Optional Port Scope
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType
   * @generated
   */
  public Adapter createOptionalPortScopeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType <em>Output Functions Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType
   * @generated
   */
  public Adapter createOutputFunctionsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType <em>Output Port Open Set
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType
   * @generated
   */
  public Adapter createOutputPortOpenSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType <em>Output Port Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType
   * @generated
   */
  public Adapter createOutputPortSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType <em>Output Ports Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType
   * @generated
   */
  public Adapter createOutputPortsTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParametersType <em>Parameters Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParametersType
   * @generated
   */
  public Adapter createParametersTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.ParameterType <em>Parameter Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.ParameterType
   * @generated
   */
  public Adapter createParameterTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.PortScopeType <em>Port Scope Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.PortScopeType
   * @generated
   */
  public Adapter createPortScopeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType <em>Spl Expression Tree
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType
   * @generated
   */
  public Adapter createSplExpressionTreeTypeAdapter() {
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
} // OperatorAdapterFactory
