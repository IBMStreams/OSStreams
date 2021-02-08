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

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl;
import com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType;
import com.ibm.xmlns.prod.streams.spl.operator.CodeTemplatesType;
import com.ibm.xmlns.prod.streams.spl.operator.ContextType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionSetType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType;
import com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationType;
import com.ibm.xmlns.prod.streams.spl.operator.EnumerationsType;
import com.ibm.xmlns.prod.streams.spl.operator.ExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.IconUriType;
import com.ibm.xmlns.prod.streams.spl.operator.IncrementalCompilationStrategyType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.InputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpContextType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpExecutionSettingsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpInputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpLibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpOutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpParametersType;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpVMArgsType;
import com.ibm.xmlns.prod.streams.spl.operator.LibraryDependenciesType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricKindType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricType;
import com.ibm.xmlns.prod.streams.spl.operator.MetricsType;
import com.ibm.xmlns.prod.streams.spl.operator.OpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorFactory;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.OptionalPortScopeType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortOpenSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortSetType;
import com.ibm.xmlns.prod.streams.spl.operator.OutputPortsType;
import com.ibm.xmlns.prod.streams.spl.operator.ParameterType;
import com.ibm.xmlns.prod.streams.spl.operator.ParametersType;
import com.ibm.xmlns.prod.streams.spl.operator.PortScopeType;
import com.ibm.xmlns.prod.streams.spl.operator.SingleThreadedContextType;
import com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationInputModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowPunctuationOutputModeType;
import com.ibm.xmlns.prod.streams.spl.operator.WindowingModeType;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.emf.ecore.impl.EPackageImpl;
import org.eclipse.emf.ecore.xml.type.XMLTypePackage;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Package</b>.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public class OperatorPackageImpl extends EPackageImpl implements OperatorPackage {
  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass codeTemplatesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass codeTemplateTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass contextTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass customOutputFunctionSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass customOutputFunctionsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass customOutputFunctionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass documentRootEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass enumerationsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass enumerationTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass iconUriTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass inputPortOpenSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass inputPortSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass inputPortsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpContextTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpExecutionSettingsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpInputPortOpenSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpInputPortSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpInputPortsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpLibraryDependenciesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpOutputPortOpenSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpOutputPortSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpOutputPortsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpParametersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpVMArgsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass libraryDependenciesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass metricsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass metricTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass opModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass optionalPortScopeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass outputFunctionsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass outputPortOpenSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass outputPortSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass outputPortsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass parametersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass parameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass portScopeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass splExpressionTreeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum expressionModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum incrementalCompilationStrategyTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum javaOpExpressionModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum metricKindTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum singleThreadedContextTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowExpressionModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowingModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowPunctuationInputModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowPunctuationOutputModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType expressionModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType incrementalCompilationStrategyTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType javaOpExpressionModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType metricKindTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType singleThreadedContextTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowExpressionModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowingModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowPunctuationInputModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowPunctuationOutputModeTypeObjectEDataType = null;

  /**
   * Creates an instance of the model <b>Package</b>, registered with {@link
   * org.eclipse.emf.ecore.EPackage.Registry EPackage.Registry} by the package package URI value.
   *
   * <p>Note: the correct way to create the package is via the static factory method {@link #init
   * init()}, which also performs initialization of the package, or returns the registered package,
   * if one already exists.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see org.eclipse.emf.ecore.EPackage.Registry
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private OperatorPackageImpl() {
    super(eNS_URI, OperatorFactory.eINSTANCE);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static boolean isInited = false;

  /**
   * Creates, registers, and initializes the <b>Package</b> for this model, and for any others upon
   * which it depends.
   *
   * <p>This method is used to initialize {@link OperatorPackage#eINSTANCE} when that field is
   * accessed. Clients should not invoke it directly. Instead, they should simply access that field
   * to obtain the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #eNS_URI
   * @see #createPackageContents()
   * @see #initializePackageContents()
   * @generated
   */
  public static OperatorPackage init() {
    if (isInited)
      return (OperatorPackage) EPackage.Registry.INSTANCE.getEPackage(OperatorPackage.eNS_URI);

    // Obtain or create and register package
    OperatorPackageImpl theOperatorPackage =
        (OperatorPackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof OperatorPackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new OperatorPackageImpl());

    isInited = true;

    // Initialize simple dependencies
    XMLTypePackage.eINSTANCE.eClass();

    // Obtain or create and register interdependencies
    CommonPackageImpl theCommonPackage =
        (CommonPackageImpl)
            (EPackage.Registry.INSTANCE.getEPackage(CommonPackage.eNS_URI)
                    instanceof CommonPackageImpl
                ? EPackage.Registry.INSTANCE.getEPackage(CommonPackage.eNS_URI)
                : CommonPackage.eINSTANCE);

    // Create package meta-data objects
    theOperatorPackage.createPackageContents();
    theCommonPackage.createPackageContents();

    // Initialize created meta-data
    theOperatorPackage.initializePackageContents();
    theCommonPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theOperatorPackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(OperatorPackage.eNS_URI, theOperatorPackage);
    return theOperatorPackage;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCodeTemplatesType() {
    return codeTemplatesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCodeTemplatesType_CodeTemplate() {
    return (EReference) codeTemplatesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCodeTemplateType() {
    return codeTemplateTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCodeTemplateType_Description() {
    return (EReference) codeTemplateTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCodeTemplateType_Template() {
    return (EAttribute) codeTemplateTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCodeTemplateType_Name() {
    return (EAttribute) codeTemplateTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getContextType() {
    return contextTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_Description() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_IconUri() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_Metrics() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_CustomLiterals() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_CustomOutputFunctions() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_LibraryDependencies() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getContextType_ProvidesSingleThreadedContext() {
    return (EAttribute) contextTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getContextType_IncrementalCompilationStrategy() {
    return (EAttribute) contextTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getContextType_AllowCustomLogic() {
    return (EAttribute) contextTypeEClass.getEStructuralFeatures().get(8);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_CodeTemplates() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(9);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getContextType_SplExpressionTree() {
    return (EReference) contextTypeEClass.getEStructuralFeatures().get(10);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getContextType_Capability() {
    return (EAttribute) contextTypeEClass.getEStructuralFeatures().get(11);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getContextType_VerificationModule() {
    return (EAttribute) contextTypeEClass.getEStructuralFeatures().get(12);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCustomOutputFunctionSetType() {
    return customOutputFunctionSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCustomOutputFunctionSetType_Name() {
    return (EAttribute) customOutputFunctionSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCustomOutputFunctionSetType_Function() {
    return (EReference) customOutputFunctionSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCustomOutputFunctionsType() {
    return customOutputFunctionsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCustomOutputFunctionsType_CustomOutputFunction() {
    return (EReference) customOutputFunctionsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCustomOutputFunctionType() {
    return customOutputFunctionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCustomOutputFunctionType_Description() {
    return (EReference) customOutputFunctionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCustomOutputFunctionType_Prototype() {
    return (EAttribute) customOutputFunctionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCustomOutputFunctionType_PseudoFunction() {
    return (EAttribute) customOutputFunctionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getDocumentRoot() {
    return documentRootEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getDocumentRoot_Mixed() {
    return (EAttribute) documentRootEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDocumentRoot_XMLNSPrefixMap() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDocumentRoot_XSISchemaLocation() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDocumentRoot_OperatorModel() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getEnumerationsType() {
    return enumerationsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getEnumerationsType_Enumeration() {
    return (EReference) enumerationsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getEnumerationType() {
    return enumerationTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumerationType_Name() {
    return (EAttribute) enumerationTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumerationType_Value() {
    return (EAttribute) enumerationTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getIconUriType() {
    return iconUriTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIconUriType_Value() {
    return (EAttribute) iconUriTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIconUriType_Size() {
    return (EAttribute) iconUriTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getInputPortOpenSetType() {
    return inputPortOpenSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getInputPortOpenSetType_Description() {
    return (EReference) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getInputPortOpenSetType_WindowingDescription() {
    return (EReference) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_TupleMutationAllowed() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_WindowingMode() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_WindowPunctuationInputMode() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_ControlPort() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_WindowExpressionMode() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortOpenSetType_RewriteAllowedForWindowExpression() {
    return (EAttribute) inputPortOpenSetTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getInputPortSetType() {
    return inputPortSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortSetType_Cardinality() {
    return (EAttribute) inputPortSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getInputPortSetType_Optional() {
    return (EAttribute) inputPortSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getInputPortsType() {
    return inputPortsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getInputPortsType_InputPortSet() {
    return (EReference) inputPortsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getInputPortsType_InputPortOpenSet() {
    return (EReference) inputPortsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpContextType() {
    return javaOpContextTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_Description() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_IconUri() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_Metrics() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_CustomLiterals() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_ExecutionSettings() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_LibraryDependencies() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpContextType_CodeTemplates() {
    return (EReference) javaOpContextTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpExecutionSettingsType() {
    return javaOpExecutionSettingsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpExecutionSettingsType_ClassName() {
    return (EAttribute) javaOpExecutionSettingsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpExecutionSettingsType_VmArgs() {
    return (EReference) javaOpExecutionSettingsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpInputPortOpenSetType() {
    return javaOpInputPortOpenSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpInputPortOpenSetType_Description() {
    return (EReference) javaOpInputPortOpenSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpInputPortOpenSetType_WindowingDescription() {
    return (EReference) javaOpInputPortOpenSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpInputPortOpenSetType_WindowingMode() {
    return (EAttribute) javaOpInputPortOpenSetTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpInputPortOpenSetType_WindowPunctuationInputMode() {
    return (EAttribute) javaOpInputPortOpenSetTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpInputPortOpenSetType_ControlPort() {
    return (EAttribute) javaOpInputPortOpenSetTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpInputPortSetType() {
    return javaOpInputPortSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpInputPortSetType_Cardinality() {
    return (EAttribute) javaOpInputPortSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpInputPortSetType_Optional() {
    return (EAttribute) javaOpInputPortSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpInputPortsType() {
    return javaOpInputPortsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpInputPortsType_InputPortSet() {
    return (EReference) javaOpInputPortsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpInputPortsType_InputPortOpenSet() {
    return (EReference) javaOpInputPortsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpLibraryDependenciesType() {
    return javaOpLibraryDependenciesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpLibraryDependenciesType_Library() {
    return (EReference) javaOpLibraryDependenciesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpModelType() {
    return javaOpModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpModelType_Context() {
    return (EReference) javaOpModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpModelType_Parameters() {
    return (EReference) javaOpModelTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpModelType_InputPorts() {
    return (EReference) javaOpModelTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpModelType_OutputPorts() {
    return (EReference) javaOpModelTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpOutputPortOpenSetType() {
    return javaOpOutputPortOpenSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpOutputPortOpenSetType_Description() {
    return (EReference) javaOpOutputPortOpenSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode() {
    return (EAttribute) javaOpOutputPortOpenSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort() {
    return (EAttribute) javaOpOutputPortOpenSetTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope() {
    return (EReference) javaOpOutputPortOpenSetTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpOutputPortSetType() {
    return javaOpOutputPortSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpOutputPortSetType_Cardinality() {
    return (EAttribute) javaOpOutputPortSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpOutputPortSetType_Optional() {
    return (EAttribute) javaOpOutputPortSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpOutputPortsType() {
    return javaOpOutputPortsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpOutputPortsType_OutputPortSet() {
    return (EReference) javaOpOutputPortsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpOutputPortsType_OutputPortOpenSet() {
    return (EReference) javaOpOutputPortsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpParametersType() {
    return javaOpParametersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpParametersType_Description() {
    return (EReference) javaOpParametersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpParametersType_Parameter() {
    return (EReference) javaOpParametersTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpParameterType() {
    return javaOpParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpParameterType_Name() {
    return (EAttribute) javaOpParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpParameterType_Description() {
    return (EReference) javaOpParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpParameterType_Optional() {
    return (EAttribute) javaOpParameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpParameterType_ExpressionMode() {
    return (EAttribute) javaOpParameterTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpParameterType_Type() {
    return (EAttribute) javaOpParameterTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpParameterType_Cardinality() {
    return (EAttribute) javaOpParameterTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpVMArgsType() {
    return javaOpVMArgsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpVMArgsType_VmArg() {
    return (EAttribute) javaOpVMArgsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getLibraryDependenciesType() {
    return libraryDependenciesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getLibraryDependenciesType_Library() {
    return (EReference) libraryDependenciesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getMetricsType() {
    return metricsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getMetricsType_Description() {
    return (EReference) metricsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getMetricsType_Metric() {
    return (EReference) metricsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getMetricType() {
    return metricTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getMetricType_Name() {
    return (EAttribute) metricTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getMetricType_Description() {
    return (EReference) metricTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getMetricType_Kind() {
    return (EAttribute) metricTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getMetricType_Dynamic() {
    return (EAttribute) metricTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorModelType() {
    return operatorModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorModelType_CppOperatorModel() {
    return (EReference) operatorModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorModelType_JavaOperatorModel() {
    return (EReference) operatorModelTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOpModelType() {
    return opModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOpModelType_Context() {
    return (EReference) opModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOpModelType_Parameters() {
    return (EReference) opModelTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOpModelType_InputPorts() {
    return (EReference) opModelTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOpModelType_OutputPorts() {
    return (EReference) opModelTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOptionalPortScopeType() {
    return optionalPortScopeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOptionalPortScopeType_Port() {
    return (EAttribute) optionalPortScopeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOutputFunctionsType() {
    return outputFunctionsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputFunctionsType_Default() {
    return (EAttribute) outputFunctionsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputFunctionsType_Type() {
    return (EAttribute) outputFunctionsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOutputPortOpenSetType() {
    return outputPortOpenSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortOpenSetType_Description() {
    return (EReference) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_ExpressionMode() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_AutoAssignment() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_CompleteAssignment() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_RewriteAllowed() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortOpenSetType_OutputFunctions() {
    return (EReference) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_WindowPunctuationOutputMode() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_WindowPunctuationInputPort() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortOpenSetType_FinalPunctuationPortScope() {
    return (EReference) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(8);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_TupleMutationAllowed() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(9);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortOpenSetType_OutputAssignmentPortScope() {
    return (EReference) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(10);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortOpenSetType_AllowNestedCustomOutputFunctions() {
    return (EAttribute) outputPortOpenSetTypeEClass.getEStructuralFeatures().get(11);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOutputPortSetType() {
    return outputPortSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortSetType_Cardinality() {
    return (EAttribute) outputPortSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputPortSetType_Optional() {
    return (EAttribute) outputPortSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOutputPortsType() {
    return outputPortsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortsType_OutputPortSet() {
    return (EReference) outputPortsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOutputPortsType_OutputPortOpenSet() {
    return (EReference) outputPortsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getParametersType() {
    return parametersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getParametersType_Description() {
    return (EReference) parametersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParametersType_AllowAny() {
    return (EAttribute) parametersTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getParametersType_Parameter() {
    return (EReference) parametersTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getParameterType() {
    return parameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Name() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getParameterType_Description() {
    return (EReference) parameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Optional() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_RewriteAllowed() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_ExpressionMode() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Type() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Cardinality() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getParameterType_PortScope() {
    return (EReference) parameterTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_CustomOutputFunction() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(8);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPortScopeType() {
    return portScopeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPortScopeType_Port() {
    return (EAttribute) portScopeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSplExpressionTreeType() {
    return splExpressionTreeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplExpressionTreeType_CppCode() {
    return (EAttribute) splExpressionTreeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplExpressionTreeType_Output() {
    return (EAttribute) splExpressionTreeTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplExpressionTreeType_Param() {
    return (EAttribute) splExpressionTreeTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getExpressionModeType() {
    return expressionModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getIncrementalCompilationStrategyType() {
    return incrementalCompilationStrategyTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getJavaOpExpressionModeType() {
    return javaOpExpressionModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getMetricKindType() {
    return metricKindTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getSingleThreadedContextType() {
    return singleThreadedContextTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowExpressionModeType() {
    return windowExpressionModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowingModeType() {
    return windowingModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowPunctuationInputModeType() {
    return windowPunctuationInputModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowPunctuationOutputModeType() {
    return windowPunctuationOutputModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getExpressionModeTypeObject() {
    return expressionModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getIncrementalCompilationStrategyTypeObject() {
    return incrementalCompilationStrategyTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getJavaOpExpressionModeTypeObject() {
    return javaOpExpressionModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getMetricKindTypeObject() {
    return metricKindTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getSingleThreadedContextTypeObject() {
    return singleThreadedContextTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowExpressionModeTypeObject() {
    return windowExpressionModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowingModeTypeObject() {
    return windowingModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowPunctuationInputModeTypeObject() {
    return windowPunctuationInputModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowPunctuationOutputModeTypeObject() {
    return windowPunctuationOutputModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public OperatorFactory getOperatorFactory() {
    return (OperatorFactory) getEFactoryInstance();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private boolean isCreated = false;

  /**
   * Creates the meta-model objects for the package. This method is guarded to have no affect on any
   * invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void createPackageContents() {
    if (isCreated) return;
    isCreated = true;

    // Create classes and their features
    codeTemplatesTypeEClass = createEClass(CODE_TEMPLATES_TYPE);
    createEReference(codeTemplatesTypeEClass, CODE_TEMPLATES_TYPE__CODE_TEMPLATE);

    codeTemplateTypeEClass = createEClass(CODE_TEMPLATE_TYPE);
    createEReference(codeTemplateTypeEClass, CODE_TEMPLATE_TYPE__DESCRIPTION);
    createEAttribute(codeTemplateTypeEClass, CODE_TEMPLATE_TYPE__TEMPLATE);
    createEAttribute(codeTemplateTypeEClass, CODE_TEMPLATE_TYPE__NAME);

    contextTypeEClass = createEClass(CONTEXT_TYPE);
    createEReference(contextTypeEClass, CONTEXT_TYPE__DESCRIPTION);
    createEReference(contextTypeEClass, CONTEXT_TYPE__ICON_URI);
    createEReference(contextTypeEClass, CONTEXT_TYPE__METRICS);
    createEReference(contextTypeEClass, CONTEXT_TYPE__CUSTOM_LITERALS);
    createEReference(contextTypeEClass, CONTEXT_TYPE__CUSTOM_OUTPUT_FUNCTIONS);
    createEReference(contextTypeEClass, CONTEXT_TYPE__LIBRARY_DEPENDENCIES);
    createEAttribute(contextTypeEClass, CONTEXT_TYPE__PROVIDES_SINGLE_THREADED_CONTEXT);
    createEAttribute(contextTypeEClass, CONTEXT_TYPE__INCREMENTAL_COMPILATION_STRATEGY);
    createEAttribute(contextTypeEClass, CONTEXT_TYPE__ALLOW_CUSTOM_LOGIC);
    createEReference(contextTypeEClass, CONTEXT_TYPE__CODE_TEMPLATES);
    createEReference(contextTypeEClass, CONTEXT_TYPE__SPL_EXPRESSION_TREE);
    createEAttribute(contextTypeEClass, CONTEXT_TYPE__CAPABILITY);
    createEAttribute(contextTypeEClass, CONTEXT_TYPE__VERIFICATION_MODULE);

    customOutputFunctionSetTypeEClass = createEClass(CUSTOM_OUTPUT_FUNCTION_SET_TYPE);
    createEAttribute(customOutputFunctionSetTypeEClass, CUSTOM_OUTPUT_FUNCTION_SET_TYPE__NAME);
    createEReference(customOutputFunctionSetTypeEClass, CUSTOM_OUTPUT_FUNCTION_SET_TYPE__FUNCTION);

    customOutputFunctionsTypeEClass = createEClass(CUSTOM_OUTPUT_FUNCTIONS_TYPE);
    createEReference(
        customOutputFunctionsTypeEClass, CUSTOM_OUTPUT_FUNCTIONS_TYPE__CUSTOM_OUTPUT_FUNCTION);

    customOutputFunctionTypeEClass = createEClass(CUSTOM_OUTPUT_FUNCTION_TYPE);
    createEReference(customOutputFunctionTypeEClass, CUSTOM_OUTPUT_FUNCTION_TYPE__DESCRIPTION);
    createEAttribute(customOutputFunctionTypeEClass, CUSTOM_OUTPUT_FUNCTION_TYPE__PROTOTYPE);
    createEAttribute(customOutputFunctionTypeEClass, CUSTOM_OUTPUT_FUNCTION_TYPE__PSEUDO_FUNCTION);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__OPERATOR_MODEL);

    enumerationsTypeEClass = createEClass(ENUMERATIONS_TYPE);
    createEReference(enumerationsTypeEClass, ENUMERATIONS_TYPE__ENUMERATION);

    enumerationTypeEClass = createEClass(ENUMERATION_TYPE);
    createEAttribute(enumerationTypeEClass, ENUMERATION_TYPE__NAME);
    createEAttribute(enumerationTypeEClass, ENUMERATION_TYPE__VALUE);

    iconUriTypeEClass = createEClass(ICON_URI_TYPE);
    createEAttribute(iconUriTypeEClass, ICON_URI_TYPE__VALUE);
    createEAttribute(iconUriTypeEClass, ICON_URI_TYPE__SIZE);

    inputPortOpenSetTypeEClass = createEClass(INPUT_PORT_OPEN_SET_TYPE);
    createEReference(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION);
    createEReference(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION);
    createEAttribute(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED);
    createEAttribute(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE);
    createEAttribute(
        inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE);
    createEAttribute(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT);
    createEAttribute(inputPortOpenSetTypeEClass, INPUT_PORT_OPEN_SET_TYPE__WINDOW_EXPRESSION_MODE);
    createEAttribute(
        inputPortOpenSetTypeEClass,
        INPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED_FOR_WINDOW_EXPRESSION);

    inputPortSetTypeEClass = createEClass(INPUT_PORT_SET_TYPE);
    createEAttribute(inputPortSetTypeEClass, INPUT_PORT_SET_TYPE__CARDINALITY);
    createEAttribute(inputPortSetTypeEClass, INPUT_PORT_SET_TYPE__OPTIONAL);

    inputPortsTypeEClass = createEClass(INPUT_PORTS_TYPE);
    createEReference(inputPortsTypeEClass, INPUT_PORTS_TYPE__INPUT_PORT_SET);
    createEReference(inputPortsTypeEClass, INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET);

    javaOpContextTypeEClass = createEClass(JAVA_OP_CONTEXT_TYPE);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__DESCRIPTION);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__ICON_URI);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__METRICS);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__CUSTOM_LITERALS);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__EXECUTION_SETTINGS);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__LIBRARY_DEPENDENCIES);
    createEReference(javaOpContextTypeEClass, JAVA_OP_CONTEXT_TYPE__CODE_TEMPLATES);

    javaOpExecutionSettingsTypeEClass = createEClass(JAVA_OP_EXECUTION_SETTINGS_TYPE);
    createEAttribute(
        javaOpExecutionSettingsTypeEClass, JAVA_OP_EXECUTION_SETTINGS_TYPE__CLASS_NAME);
    createEReference(javaOpExecutionSettingsTypeEClass, JAVA_OP_EXECUTION_SETTINGS_TYPE__VM_ARGS);

    javaOpInputPortOpenSetTypeEClass = createEClass(JAVA_OP_INPUT_PORT_OPEN_SET_TYPE);
    createEReference(
        javaOpInputPortOpenSetTypeEClass, JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__DESCRIPTION);
    createEReference(
        javaOpInputPortOpenSetTypeEClass, JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_DESCRIPTION);
    createEAttribute(
        javaOpInputPortOpenSetTypeEClass, JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOWING_MODE);
    createEAttribute(
        javaOpInputPortOpenSetTypeEClass,
        JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_MODE);
    createEAttribute(
        javaOpInputPortOpenSetTypeEClass, JAVA_OP_INPUT_PORT_OPEN_SET_TYPE__CONTROL_PORT);

    javaOpInputPortSetTypeEClass = createEClass(JAVA_OP_INPUT_PORT_SET_TYPE);
    createEAttribute(javaOpInputPortSetTypeEClass, JAVA_OP_INPUT_PORT_SET_TYPE__CARDINALITY);
    createEAttribute(javaOpInputPortSetTypeEClass, JAVA_OP_INPUT_PORT_SET_TYPE__OPTIONAL);

    javaOpInputPortsTypeEClass = createEClass(JAVA_OP_INPUT_PORTS_TYPE);
    createEReference(javaOpInputPortsTypeEClass, JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_SET);
    createEReference(javaOpInputPortsTypeEClass, JAVA_OP_INPUT_PORTS_TYPE__INPUT_PORT_OPEN_SET);

    javaOpLibraryDependenciesTypeEClass = createEClass(JAVA_OP_LIBRARY_DEPENDENCIES_TYPE);
    createEReference(
        javaOpLibraryDependenciesTypeEClass, JAVA_OP_LIBRARY_DEPENDENCIES_TYPE__LIBRARY);

    javaOpModelTypeEClass = createEClass(JAVA_OP_MODEL_TYPE);
    createEReference(javaOpModelTypeEClass, JAVA_OP_MODEL_TYPE__CONTEXT);
    createEReference(javaOpModelTypeEClass, JAVA_OP_MODEL_TYPE__PARAMETERS);
    createEReference(javaOpModelTypeEClass, JAVA_OP_MODEL_TYPE__INPUT_PORTS);
    createEReference(javaOpModelTypeEClass, JAVA_OP_MODEL_TYPE__OUTPUT_PORTS);

    javaOpOutputPortOpenSetTypeEClass = createEClass(JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE);
    createEReference(
        javaOpOutputPortOpenSetTypeEClass, JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION);
    createEAttribute(
        javaOpOutputPortOpenSetTypeEClass,
        JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE);
    createEAttribute(
        javaOpOutputPortOpenSetTypeEClass,
        JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT);
    createEReference(
        javaOpOutputPortOpenSetTypeEClass,
        JAVA_OP_OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE);

    javaOpOutputPortSetTypeEClass = createEClass(JAVA_OP_OUTPUT_PORT_SET_TYPE);
    createEAttribute(javaOpOutputPortSetTypeEClass, JAVA_OP_OUTPUT_PORT_SET_TYPE__CARDINALITY);
    createEAttribute(javaOpOutputPortSetTypeEClass, JAVA_OP_OUTPUT_PORT_SET_TYPE__OPTIONAL);

    javaOpOutputPortsTypeEClass = createEClass(JAVA_OP_OUTPUT_PORTS_TYPE);
    createEReference(javaOpOutputPortsTypeEClass, JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET);
    createEReference(javaOpOutputPortsTypeEClass, JAVA_OP_OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET);

    javaOpParametersTypeEClass = createEClass(JAVA_OP_PARAMETERS_TYPE);
    createEReference(javaOpParametersTypeEClass, JAVA_OP_PARAMETERS_TYPE__DESCRIPTION);
    createEReference(javaOpParametersTypeEClass, JAVA_OP_PARAMETERS_TYPE__PARAMETER);

    javaOpParameterTypeEClass = createEClass(JAVA_OP_PARAMETER_TYPE);
    createEAttribute(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__NAME);
    createEReference(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__DESCRIPTION);
    createEAttribute(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__OPTIONAL);
    createEAttribute(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__EXPRESSION_MODE);
    createEAttribute(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__TYPE);
    createEAttribute(javaOpParameterTypeEClass, JAVA_OP_PARAMETER_TYPE__CARDINALITY);

    javaOpVMArgsTypeEClass = createEClass(JAVA_OP_VM_ARGS_TYPE);
    createEAttribute(javaOpVMArgsTypeEClass, JAVA_OP_VM_ARGS_TYPE__VM_ARG);

    libraryDependenciesTypeEClass = createEClass(LIBRARY_DEPENDENCIES_TYPE);
    createEReference(libraryDependenciesTypeEClass, LIBRARY_DEPENDENCIES_TYPE__LIBRARY);

    metricsTypeEClass = createEClass(METRICS_TYPE);
    createEReference(metricsTypeEClass, METRICS_TYPE__DESCRIPTION);
    createEReference(metricsTypeEClass, METRICS_TYPE__METRIC);

    metricTypeEClass = createEClass(METRIC_TYPE);
    createEAttribute(metricTypeEClass, METRIC_TYPE__NAME);
    createEReference(metricTypeEClass, METRIC_TYPE__DESCRIPTION);
    createEAttribute(metricTypeEClass, METRIC_TYPE__KIND);
    createEAttribute(metricTypeEClass, METRIC_TYPE__DYNAMIC);

    operatorModelTypeEClass = createEClass(OPERATOR_MODEL_TYPE);
    createEReference(operatorModelTypeEClass, OPERATOR_MODEL_TYPE__CPP_OPERATOR_MODEL);
    createEReference(operatorModelTypeEClass, OPERATOR_MODEL_TYPE__JAVA_OPERATOR_MODEL);

    opModelTypeEClass = createEClass(OP_MODEL_TYPE);
    createEReference(opModelTypeEClass, OP_MODEL_TYPE__CONTEXT);
    createEReference(opModelTypeEClass, OP_MODEL_TYPE__PARAMETERS);
    createEReference(opModelTypeEClass, OP_MODEL_TYPE__INPUT_PORTS);
    createEReference(opModelTypeEClass, OP_MODEL_TYPE__OUTPUT_PORTS);

    optionalPortScopeTypeEClass = createEClass(OPTIONAL_PORT_SCOPE_TYPE);
    createEAttribute(optionalPortScopeTypeEClass, OPTIONAL_PORT_SCOPE_TYPE__PORT);

    outputFunctionsTypeEClass = createEClass(OUTPUT_FUNCTIONS_TYPE);
    createEAttribute(outputFunctionsTypeEClass, OUTPUT_FUNCTIONS_TYPE__DEFAULT);
    createEAttribute(outputFunctionsTypeEClass, OUTPUT_FUNCTIONS_TYPE__TYPE);

    outputPortOpenSetTypeEClass = createEClass(OUTPUT_PORT_OPEN_SET_TYPE);
    createEReference(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__DESCRIPTION);
    createEAttribute(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__EXPRESSION_MODE);
    createEAttribute(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__AUTO_ASSIGNMENT);
    createEAttribute(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__COMPLETE_ASSIGNMENT);
    createEAttribute(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__REWRITE_ALLOWED);
    createEReference(outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_FUNCTIONS);
    createEAttribute(
        outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_OUTPUT_MODE);
    createEAttribute(
        outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__WINDOW_PUNCTUATION_INPUT_PORT);
    createEReference(
        outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__FINAL_PUNCTUATION_PORT_SCOPE);
    createEAttribute(
        outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__TUPLE_MUTATION_ALLOWED);
    createEReference(
        outputPortOpenSetTypeEClass, OUTPUT_PORT_OPEN_SET_TYPE__OUTPUT_ASSIGNMENT_PORT_SCOPE);
    createEAttribute(
        outputPortOpenSetTypeEClass,
        OUTPUT_PORT_OPEN_SET_TYPE__ALLOW_NESTED_CUSTOM_OUTPUT_FUNCTIONS);

    outputPortSetTypeEClass = createEClass(OUTPUT_PORT_SET_TYPE);
    createEAttribute(outputPortSetTypeEClass, OUTPUT_PORT_SET_TYPE__CARDINALITY);
    createEAttribute(outputPortSetTypeEClass, OUTPUT_PORT_SET_TYPE__OPTIONAL);

    outputPortsTypeEClass = createEClass(OUTPUT_PORTS_TYPE);
    createEReference(outputPortsTypeEClass, OUTPUT_PORTS_TYPE__OUTPUT_PORT_SET);
    createEReference(outputPortsTypeEClass, OUTPUT_PORTS_TYPE__OUTPUT_PORT_OPEN_SET);

    parametersTypeEClass = createEClass(PARAMETERS_TYPE);
    createEReference(parametersTypeEClass, PARAMETERS_TYPE__DESCRIPTION);
    createEAttribute(parametersTypeEClass, PARAMETERS_TYPE__ALLOW_ANY);
    createEReference(parametersTypeEClass, PARAMETERS_TYPE__PARAMETER);

    parameterTypeEClass = createEClass(PARAMETER_TYPE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__NAME);
    createEReference(parameterTypeEClass, PARAMETER_TYPE__DESCRIPTION);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__OPTIONAL);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__REWRITE_ALLOWED);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__EXPRESSION_MODE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__TYPE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__CARDINALITY);
    createEReference(parameterTypeEClass, PARAMETER_TYPE__PORT_SCOPE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__CUSTOM_OUTPUT_FUNCTION);

    portScopeTypeEClass = createEClass(PORT_SCOPE_TYPE);
    createEAttribute(portScopeTypeEClass, PORT_SCOPE_TYPE__PORT);

    splExpressionTreeTypeEClass = createEClass(SPL_EXPRESSION_TREE_TYPE);
    createEAttribute(splExpressionTreeTypeEClass, SPL_EXPRESSION_TREE_TYPE__CPP_CODE);
    createEAttribute(splExpressionTreeTypeEClass, SPL_EXPRESSION_TREE_TYPE__OUTPUT);
    createEAttribute(splExpressionTreeTypeEClass, SPL_EXPRESSION_TREE_TYPE__PARAM);

    // Create enums
    expressionModeTypeEEnum = createEEnum(EXPRESSION_MODE_TYPE);
    incrementalCompilationStrategyTypeEEnum = createEEnum(INCREMENTAL_COMPILATION_STRATEGY_TYPE);
    javaOpExpressionModeTypeEEnum = createEEnum(JAVA_OP_EXPRESSION_MODE_TYPE);
    metricKindTypeEEnum = createEEnum(METRIC_KIND_TYPE);
    singleThreadedContextTypeEEnum = createEEnum(SINGLE_THREADED_CONTEXT_TYPE);
    windowExpressionModeTypeEEnum = createEEnum(WINDOW_EXPRESSION_MODE_TYPE);
    windowingModeTypeEEnum = createEEnum(WINDOWING_MODE_TYPE);
    windowPunctuationInputModeTypeEEnum = createEEnum(WINDOW_PUNCTUATION_INPUT_MODE_TYPE);
    windowPunctuationOutputModeTypeEEnum = createEEnum(WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE);

    // Create data types
    expressionModeTypeObjectEDataType = createEDataType(EXPRESSION_MODE_TYPE_OBJECT);
    incrementalCompilationStrategyTypeObjectEDataType =
        createEDataType(INCREMENTAL_COMPILATION_STRATEGY_TYPE_OBJECT);
    javaOpExpressionModeTypeObjectEDataType = createEDataType(JAVA_OP_EXPRESSION_MODE_TYPE_OBJECT);
    metricKindTypeObjectEDataType = createEDataType(METRIC_KIND_TYPE_OBJECT);
    singleThreadedContextTypeObjectEDataType = createEDataType(SINGLE_THREADED_CONTEXT_TYPE_OBJECT);
    windowExpressionModeTypeObjectEDataType = createEDataType(WINDOW_EXPRESSION_MODE_TYPE_OBJECT);
    windowingModeTypeObjectEDataType = createEDataType(WINDOWING_MODE_TYPE_OBJECT);
    windowPunctuationInputModeTypeObjectEDataType =
        createEDataType(WINDOW_PUNCTUATION_INPUT_MODE_TYPE_OBJECT);
    windowPunctuationOutputModeTypeObjectEDataType =
        createEDataType(WINDOW_PUNCTUATION_OUTPUT_MODE_TYPE_OBJECT);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private boolean isInitialized = false;

  /**
   * Complete the initialization of the package and its meta-model. This method is guarded to have
   * no affect on any invocation but its first.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void initializePackageContents() {
    if (isInitialized) return;
    isInitialized = true;

    // Initialize package
    setName(eNAME);
    setNsPrefix(eNS_PREFIX);
    setNsURI(eNS_URI);

    // Obtain other dependent packages
    CommonPackage theCommonPackage =
        (CommonPackage) EPackage.Registry.INSTANCE.getEPackage(CommonPackage.eNS_URI);
    XMLTypePackage theXMLTypePackage =
        (XMLTypePackage) EPackage.Registry.INSTANCE.getEPackage(XMLTypePackage.eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    inputPortSetTypeEClass.getESuperTypes().add(this.getInputPortOpenSetType());
    javaOpInputPortSetTypeEClass.getESuperTypes().add(this.getJavaOpInputPortOpenSetType());
    javaOpOutputPortSetTypeEClass.getESuperTypes().add(this.getJavaOpOutputPortOpenSetType());
    outputPortSetTypeEClass.getESuperTypes().add(this.getOutputPortOpenSetType());

    // Initialize classes, features, and operations; add parameters
    initEClass(
        codeTemplatesTypeEClass,
        CodeTemplatesType.class,
        "CodeTemplatesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCodeTemplatesType_CodeTemplate(),
        this.getCodeTemplateType(),
        null,
        "codeTemplate",
        null,
        0,
        -1,
        CodeTemplatesType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        codeTemplateTypeEClass,
        CodeTemplateType.class,
        "CodeTemplateType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCodeTemplateType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        CodeTemplateType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCodeTemplateType_Template(),
        theXMLTypePackage.getString(),
        "template",
        null,
        1,
        1,
        CodeTemplateType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCodeTemplateType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        CodeTemplateType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        contextTypeEClass,
        ContextType.class,
        "ContextType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getContextType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_IconUri(),
        this.getIconUriType(),
        null,
        "iconUri",
        null,
        0,
        -1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_Metrics(),
        this.getMetricsType(),
        null,
        "metrics",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_CustomLiterals(),
        this.getEnumerationsType(),
        null,
        "customLiterals",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_CustomOutputFunctions(),
        this.getCustomOutputFunctionsType(),
        null,
        "customOutputFunctions",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_LibraryDependencies(),
        this.getLibraryDependenciesType(),
        null,
        "libraryDependencies",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getContextType_ProvidesSingleThreadedContext(),
        this.getSingleThreadedContextType(),
        "providesSingleThreadedContext",
        null,
        1,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getContextType_IncrementalCompilationStrategy(),
        this.getIncrementalCompilationStrategyType(),
        "incrementalCompilationStrategy",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getContextType_AllowCustomLogic(),
        theXMLTypePackage.getBoolean(),
        "allowCustomLogic",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_CodeTemplates(),
        this.getCodeTemplatesType(),
        null,
        "codeTemplates",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getContextType_SplExpressionTree(),
        this.getSplExpressionTreeType(),
        null,
        "splExpressionTree",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getContextType_Capability(),
        theXMLTypePackage.getString(),
        "capability",
        null,
        0,
        -1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getContextType_VerificationModule(),
        theXMLTypePackage.getToken(),
        "verificationModule",
        null,
        0,
        1,
        ContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        customOutputFunctionSetTypeEClass,
        CustomOutputFunctionSetType.class,
        "CustomOutputFunctionSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCustomOutputFunctionSetType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        CustomOutputFunctionSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCustomOutputFunctionSetType_Function(),
        this.getCustomOutputFunctionType(),
        null,
        "function",
        null,
        0,
        -1,
        CustomOutputFunctionSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        customOutputFunctionsTypeEClass,
        CustomOutputFunctionsType.class,
        "CustomOutputFunctionsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCustomOutputFunctionsType_CustomOutputFunction(),
        this.getCustomOutputFunctionSetType(),
        null,
        "customOutputFunction",
        null,
        0,
        -1,
        CustomOutputFunctionsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        customOutputFunctionTypeEClass,
        CustomOutputFunctionType.class,
        "CustomOutputFunctionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCustomOutputFunctionType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        CustomOutputFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCustomOutputFunctionType_Prototype(),
        theXMLTypePackage.getString(),
        "prototype",
        null,
        1,
        1,
        CustomOutputFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCustomOutputFunctionType_PseudoFunction(),
        theXMLTypePackage.getBoolean(),
        "pseudoFunction",
        null,
        0,
        1,
        CustomOutputFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        documentRootEClass,
        DocumentRoot.class,
        "DocumentRoot",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getDocumentRoot_Mixed(),
        ecorePackage.getEFeatureMapEntry(),
        "mixed",
        null,
        0,
        -1,
        null,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getDocumentRoot_XMLNSPrefixMap(),
        ecorePackage.getEStringToStringMapEntry(),
        null,
        "xMLNSPrefixMap",
        null,
        0,
        -1,
        null,
        IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getDocumentRoot_XSISchemaLocation(),
        ecorePackage.getEStringToStringMapEntry(),
        null,
        "xSISchemaLocation",
        null,
        0,
        -1,
        null,
        IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getDocumentRoot_OperatorModel(),
        this.getOperatorModelType(),
        null,
        "operatorModel",
        null,
        0,
        -2,
        null,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);

    initEClass(
        enumerationsTypeEClass,
        EnumerationsType.class,
        "EnumerationsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getEnumerationsType_Enumeration(),
        this.getEnumerationType(),
        null,
        "enumeration",
        null,
        0,
        -1,
        EnumerationsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        enumerationTypeEClass,
        EnumerationType.class,
        "EnumerationType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getEnumerationType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        EnumerationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumerationType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        0,
        -1,
        EnumerationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        iconUriTypeEClass,
        IconUriType.class,
        "IconUriType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getIconUriType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        0,
        1,
        IconUriType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getIconUriType_Size(),
        theXMLTypePackage.getInt(),
        "size",
        null,
        1,
        1,
        IconUriType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        inputPortOpenSetTypeEClass,
        InputPortOpenSetType.class,
        "InputPortOpenSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getInputPortOpenSetType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getInputPortOpenSetType_WindowingDescription(),
        theCommonPackage.getDescriptionType(),
        null,
        "windowingDescription",
        null,
        0,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_TupleMutationAllowed(),
        theXMLTypePackage.getBoolean(),
        "tupleMutationAllowed",
        null,
        1,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_WindowingMode(),
        this.getWindowingModeType(),
        "windowingMode",
        null,
        1,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_WindowPunctuationInputMode(),
        this.getWindowPunctuationInputModeType(),
        "windowPunctuationInputMode",
        null,
        1,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_ControlPort(),
        theXMLTypePackage.getBoolean(),
        "controlPort",
        null,
        0,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_WindowExpressionMode(),
        this.getWindowExpressionModeType(),
        "windowExpressionMode",
        null,
        0,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortOpenSetType_RewriteAllowedForWindowExpression(),
        theXMLTypePackage.getBoolean(),
        "rewriteAllowedForWindowExpression",
        null,
        0,
        1,
        InputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        inputPortSetTypeEClass,
        InputPortSetType.class,
        "InputPortSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getInputPortSetType_Cardinality(),
        theXMLTypePackage.getNonNegativeInteger(),
        "cardinality",
        null,
        1,
        1,
        InputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getInputPortSetType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        InputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        inputPortsTypeEClass,
        InputPortsType.class,
        "InputPortsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getInputPortsType_InputPortSet(),
        this.getInputPortSetType(),
        null,
        "inputPortSet",
        null,
        0,
        -1,
        InputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getInputPortsType_InputPortOpenSet(),
        this.getInputPortOpenSetType(),
        null,
        "inputPortOpenSet",
        null,
        0,
        1,
        InputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpContextTypeEClass,
        JavaOpContextType.class,
        "JavaOpContextType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpContextType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_IconUri(),
        this.getIconUriType(),
        null,
        "iconUri",
        null,
        0,
        -1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_Metrics(),
        this.getMetricsType(),
        null,
        "metrics",
        null,
        0,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_CustomLiterals(),
        this.getEnumerationsType(),
        null,
        "customLiterals",
        null,
        0,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_ExecutionSettings(),
        this.getJavaOpExecutionSettingsType(),
        null,
        "executionSettings",
        null,
        1,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_LibraryDependencies(),
        this.getJavaOpLibraryDependenciesType(),
        null,
        "libraryDependencies",
        null,
        0,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpContextType_CodeTemplates(),
        this.getCodeTemplatesType(),
        null,
        "codeTemplates",
        null,
        0,
        1,
        JavaOpContextType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpExecutionSettingsTypeEClass,
        JavaOpExecutionSettingsType.class,
        "JavaOpExecutionSettingsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpExecutionSettingsType_ClassName(),
        theXMLTypePackage.getToken(),
        "className",
        null,
        1,
        1,
        JavaOpExecutionSettingsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpExecutionSettingsType_VmArgs(),
        this.getJavaOpVMArgsType(),
        null,
        "vmArgs",
        null,
        0,
        1,
        JavaOpExecutionSettingsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpInputPortOpenSetTypeEClass,
        JavaOpInputPortOpenSetType.class,
        "JavaOpInputPortOpenSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpInputPortOpenSetType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        JavaOpInputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpInputPortOpenSetType_WindowingDescription(),
        theCommonPackage.getDescriptionType(),
        null,
        "windowingDescription",
        null,
        0,
        1,
        JavaOpInputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpInputPortOpenSetType_WindowingMode(),
        this.getWindowingModeType(),
        "windowingMode",
        null,
        1,
        1,
        JavaOpInputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpInputPortOpenSetType_WindowPunctuationInputMode(),
        this.getWindowPunctuationInputModeType(),
        "windowPunctuationInputMode",
        null,
        1,
        1,
        JavaOpInputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpInputPortOpenSetType_ControlPort(),
        theXMLTypePackage.getBoolean(),
        "controlPort",
        null,
        0,
        1,
        JavaOpInputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpInputPortSetTypeEClass,
        JavaOpInputPortSetType.class,
        "JavaOpInputPortSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpInputPortSetType_Cardinality(),
        theXMLTypePackage.getNonNegativeInteger(),
        "cardinality",
        null,
        1,
        1,
        JavaOpInputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpInputPortSetType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        JavaOpInputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpInputPortsTypeEClass,
        JavaOpInputPortsType.class,
        "JavaOpInputPortsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpInputPortsType_InputPortSet(),
        this.getJavaOpInputPortSetType(),
        null,
        "inputPortSet",
        null,
        0,
        -1,
        JavaOpInputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpInputPortsType_InputPortOpenSet(),
        this.getJavaOpInputPortOpenSetType(),
        null,
        "inputPortOpenSet",
        null,
        0,
        1,
        JavaOpInputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpLibraryDependenciesTypeEClass,
        JavaOpLibraryDependenciesType.class,
        "JavaOpLibraryDependenciesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpLibraryDependenciesType_Library(),
        theCommonPackage.getJavaOpLibraryType(),
        null,
        "library",
        null,
        1,
        -1,
        JavaOpLibraryDependenciesType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpModelTypeEClass,
        JavaOpModelType.class,
        "JavaOpModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpModelType_Context(),
        this.getJavaOpContextType(),
        null,
        "context",
        null,
        1,
        1,
        JavaOpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpModelType_Parameters(),
        this.getJavaOpParametersType(),
        null,
        "parameters",
        null,
        1,
        1,
        JavaOpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpModelType_InputPorts(),
        this.getJavaOpInputPortsType(),
        null,
        "inputPorts",
        null,
        1,
        1,
        JavaOpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpModelType_OutputPorts(),
        this.getJavaOpOutputPortsType(),
        null,
        "outputPorts",
        null,
        1,
        1,
        JavaOpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpOutputPortOpenSetTypeEClass,
        JavaOpOutputPortOpenSetType.class,
        "JavaOpOutputPortOpenSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpOutputPortOpenSetType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        JavaOpOutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode(),
        this.getWindowPunctuationOutputModeType(),
        "windowPunctuationOutputMode",
        null,
        1,
        1,
        JavaOpOutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort(),
        theXMLTypePackage.getInteger(),
        "windowPunctuationInputPort",
        null,
        0,
        1,
        JavaOpOutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope(),
        this.getOptionalPortScopeType(),
        null,
        "finalPunctuationPortScope",
        null,
        0,
        1,
        JavaOpOutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpOutputPortSetTypeEClass,
        JavaOpOutputPortSetType.class,
        "JavaOpOutputPortSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpOutputPortSetType_Cardinality(),
        theXMLTypePackage.getNonNegativeInteger(),
        "cardinality",
        null,
        1,
        1,
        JavaOpOutputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpOutputPortSetType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        JavaOpOutputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpOutputPortsTypeEClass,
        JavaOpOutputPortsType.class,
        "JavaOpOutputPortsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpOutputPortsType_OutputPortSet(),
        this.getJavaOpOutputPortSetType(),
        null,
        "outputPortSet",
        null,
        0,
        -1,
        JavaOpOutputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpOutputPortsType_OutputPortOpenSet(),
        this.getJavaOpOutputPortOpenSetType(),
        null,
        "outputPortOpenSet",
        null,
        0,
        1,
        JavaOpOutputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpParametersTypeEClass,
        JavaOpParametersType.class,
        "JavaOpParametersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpParametersType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        JavaOpParametersType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpParametersType_Parameter(),
        this.getJavaOpParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        JavaOpParametersType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpParameterTypeEClass,
        JavaOpParameterType.class,
        "JavaOpParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpParameterType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getJavaOpParameterType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpParameterType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpParameterType_ExpressionMode(),
        this.getJavaOpExpressionModeType(),
        "expressionMode",
        null,
        0,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpParameterType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        0,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpParameterType_Cardinality(),
        theXMLTypePackage.getInteger(),
        "cardinality",
        null,
        0,
        1,
        JavaOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        javaOpVMArgsTypeEClass,
        JavaOpVMArgsType.class,
        "JavaOpVMArgsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpVMArgsType_VmArg(),
        theXMLTypePackage.getString(),
        "vmArg",
        null,
        0,
        -1,
        JavaOpVMArgsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        libraryDependenciesTypeEClass,
        LibraryDependenciesType.class,
        "LibraryDependenciesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getLibraryDependenciesType_Library(),
        theCommonPackage.getLibraryType(),
        null,
        "library",
        null,
        0,
        -1,
        LibraryDependenciesType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        metricsTypeEClass,
        MetricsType.class,
        "MetricsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getMetricsType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        MetricsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getMetricsType_Metric(),
        this.getMetricType(),
        null,
        "metric",
        null,
        0,
        -1,
        MetricsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        metricTypeEClass,
        MetricType.class,
        "MetricType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getMetricType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        MetricType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getMetricType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        1,
        1,
        MetricType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getMetricType_Kind(),
        this.getMetricKindType(),
        "kind",
        null,
        1,
        1,
        MetricType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getMetricType_Dynamic(),
        theXMLTypePackage.getBoolean(),
        "dynamic",
        null,
        0,
        1,
        MetricType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorModelTypeEClass,
        OperatorModelType.class,
        "OperatorModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorModelType_CppOperatorModel(),
        this.getOpModelType(),
        null,
        "cppOperatorModel",
        null,
        0,
        1,
        OperatorModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOperatorModelType_JavaOperatorModel(),
        this.getJavaOpModelType(),
        null,
        "javaOperatorModel",
        null,
        0,
        1,
        OperatorModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        opModelTypeEClass,
        OpModelType.class,
        "OpModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOpModelType_Context(),
        this.getContextType(),
        null,
        "context",
        null,
        1,
        1,
        OpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOpModelType_Parameters(),
        this.getParametersType(),
        null,
        "parameters",
        null,
        1,
        1,
        OpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOpModelType_InputPorts(),
        this.getInputPortsType(),
        null,
        "inputPorts",
        null,
        1,
        1,
        OpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOpModelType_OutputPorts(),
        this.getOutputPortsType(),
        null,
        "outputPorts",
        null,
        1,
        1,
        OpModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        optionalPortScopeTypeEClass,
        OptionalPortScopeType.class,
        "OptionalPortScopeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOptionalPortScopeType_Port(),
        theXMLTypePackage.getNonNegativeInteger(),
        "port",
        null,
        0,
        -1,
        OptionalPortScopeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        outputFunctionsTypeEClass,
        OutputFunctionsType.class,
        "OutputFunctionsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOutputFunctionsType_Default(),
        theXMLTypePackage.getString(),
        "default",
        null,
        1,
        1,
        OutputFunctionsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputFunctionsType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        1,
        1,
        OutputFunctionsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        outputPortOpenSetTypeEClass,
        OutputPortOpenSetType.class,
        "OutputPortOpenSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOutputPortOpenSetType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_ExpressionMode(),
        this.getExpressionModeType(),
        "expressionMode",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_AutoAssignment(),
        theXMLTypePackage.getBoolean(),
        "autoAssignment",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_CompleteAssignment(),
        theXMLTypePackage.getBoolean(),
        "completeAssignment",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_RewriteAllowed(),
        theXMLTypePackage.getBoolean(),
        "rewriteAllowed",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOutputPortOpenSetType_OutputFunctions(),
        this.getOutputFunctionsType(),
        null,
        "outputFunctions",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_WindowPunctuationOutputMode(),
        this.getWindowPunctuationOutputModeType(),
        "windowPunctuationOutputMode",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_WindowPunctuationInputPort(),
        theXMLTypePackage.getInteger(),
        "windowPunctuationInputPort",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOutputPortOpenSetType_FinalPunctuationPortScope(),
        this.getOptionalPortScopeType(),
        null,
        "finalPunctuationPortScope",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_TupleMutationAllowed(),
        theXMLTypePackage.getBoolean(),
        "tupleMutationAllowed",
        null,
        1,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOutputPortOpenSetType_OutputAssignmentPortScope(),
        this.getPortScopeType(),
        null,
        "outputAssignmentPortScope",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortOpenSetType_AllowNestedCustomOutputFunctions(),
        theXMLTypePackage.getBoolean(),
        "allowNestedCustomOutputFunctions",
        null,
        0,
        1,
        OutputPortOpenSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        outputPortSetTypeEClass,
        OutputPortSetType.class,
        "OutputPortSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOutputPortSetType_Cardinality(),
        theXMLTypePackage.getNonNegativeInteger(),
        "cardinality",
        null,
        1,
        1,
        OutputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputPortSetType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        OutputPortSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        outputPortsTypeEClass,
        OutputPortsType.class,
        "OutputPortsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOutputPortsType_OutputPortSet(),
        this.getOutputPortSetType(),
        null,
        "outputPortSet",
        null,
        0,
        -1,
        OutputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getOutputPortsType_OutputPortOpenSet(),
        this.getOutputPortOpenSetType(),
        null,
        "outputPortOpenSet",
        null,
        0,
        1,
        OutputPortsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        parametersTypeEClass,
        ParametersType.class,
        "ParametersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getParametersType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        ParametersType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParametersType_AllowAny(),
        theXMLTypePackage.getBoolean(),
        "allowAny",
        null,
        1,
        1,
        ParametersType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getParametersType_Parameter(),
        this.getParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        ParametersType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        parameterTypeEClass,
        ParameterType.class,
        "ParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getParameterType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getParameterType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_RewriteAllowed(),
        theXMLTypePackage.getBoolean(),
        "rewriteAllowed",
        null,
        1,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_ExpressionMode(),
        this.getExpressionModeType(),
        "expressionMode",
        null,
        1,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        0,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_Cardinality(),
        theXMLTypePackage.getInteger(),
        "cardinality",
        null,
        0,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getParameterType_PortScope(),
        this.getPortScopeType(),
        null,
        "portScope",
        null,
        0,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getParameterType_CustomOutputFunction(),
        theXMLTypePackage.getString(),
        "customOutputFunction",
        null,
        0,
        1,
        ParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        portScopeTypeEClass,
        PortScopeType.class,
        "PortScopeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getPortScopeType_Port(),
        theXMLTypePackage.getNonNegativeInteger(),
        "port",
        null,
        1,
        -1,
        PortScopeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        splExpressionTreeTypeEClass,
        SplExpressionTreeType.class,
        "SplExpressionTreeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getSplExpressionTreeType_CppCode(),
        theXMLTypePackage.getBoolean(),
        "cppCode",
        null,
        0,
        1,
        SplExpressionTreeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getSplExpressionTreeType_Output(),
        theXMLTypePackage.getBoolean(),
        "output",
        null,
        0,
        1,
        SplExpressionTreeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getSplExpressionTreeType_Param(),
        theXMLTypePackage.getBoolean(),
        "param",
        null,
        0,
        1,
        SplExpressionTreeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    // Initialize enums and add enum literals
    initEEnum(expressionModeTypeEEnum, ExpressionModeType.class, "ExpressionModeType");
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.ATTRIBUTE);
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.ATTRIBUTE_FREE);
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.CONSTANT);
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.CUSTOM_LITERAL);
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.EXPRESSION);
    addEEnumLiteral(expressionModeTypeEEnum, ExpressionModeType.NONEXISTENT);

    initEEnum(
        incrementalCompilationStrategyTypeEEnum,
        IncrementalCompilationStrategyType.class,
        "IncrementalCompilationStrategyType");
    addEEnumLiteral(
        incrementalCompilationStrategyTypeEEnum,
        IncrementalCompilationStrategyType.SOURCE_DEPENDENT);
    addEEnumLiteral(
        incrementalCompilationStrategyTypeEEnum,
        IncrementalCompilationStrategyType.RESULT_DEPENDENT);

    initEEnum(
        javaOpExpressionModeTypeEEnum, JavaOpExpressionModeType.class, "JavaOpExpressionModeType");
    addEEnumLiteral(javaOpExpressionModeTypeEEnum, JavaOpExpressionModeType.ATTRIBUTE);
    addEEnumLiteral(javaOpExpressionModeTypeEEnum, JavaOpExpressionModeType.ATTRIBUTE_FREE);
    addEEnumLiteral(javaOpExpressionModeTypeEEnum, JavaOpExpressionModeType.CUSTOM_LITERAL);

    initEEnum(metricKindTypeEEnum, MetricKindType.class, "MetricKindType");
    addEEnumLiteral(metricKindTypeEEnum, MetricKindType.GAUGE);
    addEEnumLiteral(metricKindTypeEEnum, MetricKindType.COUNTER);
    addEEnumLiteral(metricKindTypeEEnum, MetricKindType.TIME);

    initEEnum(
        singleThreadedContextTypeEEnum,
        SingleThreadedContextType.class,
        "SingleThreadedContextType");
    addEEnumLiteral(singleThreadedContextTypeEEnum, SingleThreadedContextType.NEVER);
    addEEnumLiteral(singleThreadedContextTypeEEnum, SingleThreadedContextType.ALWAYS);
    addEEnumLiteral(singleThreadedContextTypeEEnum, SingleThreadedContextType.WINDOW_BOUND);
    addEEnumLiteral(singleThreadedContextTypeEEnum, SingleThreadedContextType.WINDOW_TRIGGER_BOUND);
    addEEnumLiteral(
        singleThreadedContextTypeEEnum, SingleThreadedContextType.WINDOW_EVICTION_BOUND);
    addEEnumLiteral(
        singleThreadedContextTypeEEnum, SingleThreadedContextType.WINDOW_PARTITION_EVICTION_BOUND);

    initEEnum(
        windowExpressionModeTypeEEnum, WindowExpressionModeType.class, "WindowExpressionModeType");
    addEEnumLiteral(windowExpressionModeTypeEEnum, WindowExpressionModeType.CONSTANT);
    addEEnumLiteral(windowExpressionModeTypeEEnum, WindowExpressionModeType.ATTRIBUTE_FREE);

    initEEnum(windowingModeTypeEEnum, WindowingModeType.class, "WindowingModeType");
    addEEnumLiteral(windowingModeTypeEEnum, WindowingModeType.NON_WINDOWED);
    addEEnumLiteral(windowingModeTypeEEnum, WindowingModeType.WINDOWED);
    addEEnumLiteral(windowingModeTypeEEnum, WindowingModeType.OPTIONALLY_WINDOWED);

    initEEnum(
        windowPunctuationInputModeTypeEEnum,
        WindowPunctuationInputModeType.class,
        "WindowPunctuationInputModeType");
    addEEnumLiteral(windowPunctuationInputModeTypeEEnum, WindowPunctuationInputModeType.EXPECTING);
    addEEnumLiteral(windowPunctuationInputModeTypeEEnum, WindowPunctuationInputModeType.OBLIVIOUS);
    addEEnumLiteral(
        windowPunctuationInputModeTypeEEnum, WindowPunctuationInputModeType.WINDOW_BOUND);

    initEEnum(
        windowPunctuationOutputModeTypeEEnum,
        WindowPunctuationOutputModeType.class,
        "WindowPunctuationOutputModeType");
    addEEnumLiteral(
        windowPunctuationOutputModeTypeEEnum, WindowPunctuationOutputModeType.GENERATING);
    addEEnumLiteral(windowPunctuationOutputModeTypeEEnum, WindowPunctuationOutputModeType.FREE);
    addEEnumLiteral(
        windowPunctuationOutputModeTypeEEnum, WindowPunctuationOutputModeType.PRESERVING);

    // Initialize data types
    initEDataType(
        expressionModeTypeObjectEDataType,
        ExpressionModeType.class,
        "ExpressionModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        incrementalCompilationStrategyTypeObjectEDataType,
        IncrementalCompilationStrategyType.class,
        "IncrementalCompilationStrategyTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        javaOpExpressionModeTypeObjectEDataType,
        JavaOpExpressionModeType.class,
        "JavaOpExpressionModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        metricKindTypeObjectEDataType,
        MetricKindType.class,
        "MetricKindTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        singleThreadedContextTypeObjectEDataType,
        SingleThreadedContextType.class,
        "SingleThreadedContextTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowExpressionModeTypeObjectEDataType,
        WindowExpressionModeType.class,
        "WindowExpressionModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowingModeTypeObjectEDataType,
        WindowingModeType.class,
        "WindowingModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowPunctuationInputModeTypeObjectEDataType,
        WindowPunctuationInputModeType.class,
        "WindowPunctuationInputModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowPunctuationOutputModeTypeObjectEDataType,
        WindowPunctuationOutputModeType.class,
        "WindowPunctuationOutputModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);

    // Create resource
    createResource(eNS_URI);

    // Create annotations
    // http:///org/eclipse/emf/ecore/util/ExtendedMetaData
    createExtendedMetaDataAnnotations();
  }

  /**
   * Initializes the annotations for <b>http:///org/eclipse/emf/ecore/util/ExtendedMetaData</b>.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected void createExtendedMetaDataAnnotations() {
    String source = "http:///org/eclipse/emf/ecore/util/ExtendedMetaData";
    addAnnotation(
        codeTemplatesTypeEClass,
        source,
        new String[] {
          "name", "codeTemplatesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCodeTemplatesType_CodeTemplate(),
        source,
        new String[] {
          "kind", "element",
          "name", "codeTemplate",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        codeTemplateTypeEClass,
        source,
        new String[] {
          "name", "codeTemplateType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCodeTemplateType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCodeTemplateType_Template(),
        source,
        new String[] {
          "kind", "element",
          "name", "template",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCodeTemplateType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        contextTypeEClass,
        source,
        new String[] {
          "name", "contextType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getContextType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_IconUri(),
        source,
        new String[] {
          "kind", "element",
          "name", "iconUri",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_Metrics(),
        source,
        new String[] {
          "kind", "element",
          "name", "metrics",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_CustomLiterals(),
        source,
        new String[] {
          "kind", "element",
          "name", "customLiterals",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_CustomOutputFunctions(),
        source,
        new String[] {
          "kind", "element",
          "name", "customOutputFunctions",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_LibraryDependencies(),
        source,
        new String[] {
          "kind", "element",
          "name", "libraryDependencies",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_ProvidesSingleThreadedContext(),
        source,
        new String[] {
          "kind", "element",
          "name", "providesSingleThreadedContext",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_IncrementalCompilationStrategy(),
        source,
        new String[] {
          "kind", "element",
          "name", "incrementalCompilationStrategy",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_AllowCustomLogic(),
        source,
        new String[] {
          "kind", "element",
          "name", "allowCustomLogic",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_CodeTemplates(),
        source,
        new String[] {
          "kind", "element",
          "name", "codeTemplates",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_SplExpressionTree(),
        source,
        new String[] {
          "kind", "element",
          "name", "splExpressionTree",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_Capability(),
        source,
        new String[] {
          "kind", "element",
          "name", "capability",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getContextType_VerificationModule(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "verificationModule"
        });
    addAnnotation(
        customOutputFunctionSetTypeEClass,
        source,
        new String[] {
          "name", "customOutputFunctionSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCustomOutputFunctionSetType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCustomOutputFunctionSetType_Function(),
        source,
        new String[] {
          "kind", "element",
          "name", "function",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        customOutputFunctionsTypeEClass,
        source,
        new String[] {
          "name", "customOutputFunctionsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCustomOutputFunctionsType_CustomOutputFunction(),
        source,
        new String[] {
          "kind", "element",
          "name", "customOutputFunction",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        customOutputFunctionTypeEClass,
        source,
        new String[] {
          "name", "customOutputFunctionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCustomOutputFunctionType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCustomOutputFunctionType_Prototype(),
        source,
        new String[] {
          "kind", "element",
          "name", "prototype",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCustomOutputFunctionType_PseudoFunction(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "pseudoFunction"
        });
    addAnnotation(
        documentRootEClass,
        source,
        new String[] {
          "name", "",
          "kind", "mixed"
        });
    addAnnotation(
        getDocumentRoot_Mixed(),
        source,
        new String[] {
          "kind", "elementWildcard",
          "name", ":mixed"
        });
    addAnnotation(
        getDocumentRoot_XMLNSPrefixMap(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "xmlns:prefix"
        });
    addAnnotation(
        getDocumentRoot_XSISchemaLocation(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "xsi:schemaLocation"
        });
    addAnnotation(
        getDocumentRoot_OperatorModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "operatorModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        enumerationsTypeEClass,
        source,
        new String[] {
          "name", "enumerationsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getEnumerationsType_Enumeration(),
        source,
        new String[] {
          "kind", "element",
          "name", "enumeration",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        enumerationTypeEClass,
        source,
        new String[] {
          "name", "enumerationType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getEnumerationType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getEnumerationType_Value(),
        source,
        new String[] {
          "kind", "element",
          "name", "value",
          "namespace", "##targetNamespace"
        });
    addAnnotation(expressionModeTypeEEnum, source, new String[] {"name", "expressionModeType"});
    addAnnotation(
        expressionModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "expressionModeType:Object",
          "baseType", "expressionModeType"
        });
    addAnnotation(
        iconUriTypeEClass,
        source,
        new String[] {
          "name", "iconUriType",
          "kind", "simple"
        });
    addAnnotation(
        getIconUriType_Value(),
        source,
        new String[] {
          "name", ":0",
          "kind", "simple"
        });
    addAnnotation(
        getIconUriType_Size(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "size"
        });
    addAnnotation(
        incrementalCompilationStrategyTypeEEnum,
        source,
        new String[] {"name", "incrementalCompilationStrategyType"});
    addAnnotation(
        incrementalCompilationStrategyTypeObjectEDataType,
        source,
        new String[] {
          "name", "incrementalCompilationStrategyType:Object",
          "baseType", "incrementalCompilationStrategyType"
        });
    addAnnotation(
        inputPortOpenSetTypeEClass,
        source,
        new String[] {
          "name", "inputPortOpenSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getInputPortOpenSetType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_WindowingDescription(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowingDescription",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_TupleMutationAllowed(),
        source,
        new String[] {
          "kind", "element",
          "name", "tupleMutationAllowed",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_WindowingMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowingMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_WindowPunctuationInputMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationInputMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_ControlPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "controlPort",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_WindowExpressionMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowExpressionMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortOpenSetType_RewriteAllowedForWindowExpression(),
        source,
        new String[] {
          "kind", "element",
          "name", "rewriteAllowedForWindowExpression",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        inputPortSetTypeEClass,
        source,
        new String[] {
          "name", "inputPortSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getInputPortSetType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortSetType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        inputPortsTypeEClass,
        source,
        new String[] {
          "name", "inputPortsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getInputPortsType_InputPortSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPortSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getInputPortsType_InputPortOpenSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPortOpenSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpContextTypeEClass,
        source,
        new String[] {
          "name", "javaOpContextType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpContextType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_IconUri(),
        source,
        new String[] {
          "kind", "element",
          "name", "iconUri",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_Metrics(),
        source,
        new String[] {
          "kind", "element",
          "name", "metrics",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_CustomLiterals(),
        source,
        new String[] {
          "kind", "element",
          "name", "customLiterals",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_ExecutionSettings(),
        source,
        new String[] {
          "kind", "element",
          "name", "executionSettings",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_LibraryDependencies(),
        source,
        new String[] {
          "kind", "element",
          "name", "libraryDependencies",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpContextType_CodeTemplates(),
        source,
        new String[] {
          "kind", "element",
          "name", "codeTemplates",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpExecutionSettingsTypeEClass,
        source,
        new String[] {
          "name", "javaOpExecutionSettingsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpExecutionSettingsType_ClassName(),
        source,
        new String[] {
          "kind", "element",
          "name", "className",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpExecutionSettingsType_VmArgs(),
        source,
        new String[] {
          "kind", "element",
          "name", "vmArgs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpExpressionModeTypeEEnum, source, new String[] {"name", "javaOpExpressionModeType"});
    addAnnotation(
        javaOpExpressionModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "javaOpExpressionModeType:Object",
          "baseType", "javaOpExpressionModeType"
        });
    addAnnotation(
        javaOpInputPortOpenSetTypeEClass,
        source,
        new String[] {
          "name", "javaOpInputPortOpenSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpInputPortOpenSetType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortOpenSetType_WindowingDescription(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowingDescription",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortOpenSetType_WindowingMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowingMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortOpenSetType_WindowPunctuationInputMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationInputMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortOpenSetType_ControlPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "controlPort",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpInputPortSetTypeEClass,
        source,
        new String[] {
          "name", "javaOpInputPortSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpInputPortSetType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortSetType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpInputPortsTypeEClass,
        source,
        new String[] {
          "name", "javaOpInputPortsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpInputPortsType_InputPortSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPortSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpInputPortsType_InputPortOpenSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPortOpenSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpLibraryDependenciesTypeEClass,
        source,
        new String[] {
          "name", "javaOpLibraryDependenciesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpLibraryDependenciesType_Library(),
        source,
        new String[] {
          "kind", "element",
          "name", "library",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpModelTypeEClass,
        source,
        new String[] {
          "name", "javaOpModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpModelType_Context(),
        source,
        new String[] {
          "kind", "element",
          "name", "context",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpModelType_Parameters(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameters",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpModelType_InputPorts(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPorts",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpModelType_OutputPorts(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPorts",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpOutputPortOpenSetTypeEClass,
        source,
        new String[] {
          "name", "javaOpOutputPortOpenSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpOutputPortOpenSetType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpOutputPortOpenSetType_WindowPunctuationOutputMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationOutputMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpOutputPortOpenSetType_WindowPunctuationInputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationInputPort",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpOutputPortOpenSetType_FinalPunctuationPortScope(),
        source,
        new String[] {
          "kind", "element",
          "name", "finalPunctuationPortScope",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpOutputPortSetTypeEClass,
        source,
        new String[] {
          "name", "javaOpOutputPortSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpOutputPortSetType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpOutputPortSetType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpOutputPortsTypeEClass,
        source,
        new String[] {
          "name", "javaOpOutputPortsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpOutputPortsType_OutputPortSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPortSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpOutputPortsType_OutputPortOpenSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPortOpenSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpParametersTypeEClass,
        source,
        new String[] {
          "name", "javaOpParametersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpParametersType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParametersType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpParameterTypeEClass,
        source,
        new String[] {
          "name", "javaOpParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpParameterType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParameterType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParameterType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParameterType_ExpressionMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "expressionMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParameterType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpParameterType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpVMArgsTypeEClass,
        source,
        new String[] {
          "name", "javaOpVMArgsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpVMArgsType_VmArg(),
        source,
        new String[] {
          "kind", "element",
          "name", "vmArg",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        libraryDependenciesTypeEClass,
        source,
        new String[] {
          "name", "libraryDependenciesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getLibraryDependenciesType_Library(),
        source,
        new String[] {
          "kind", "element",
          "name", "library",
          "namespace", "##targetNamespace"
        });
    addAnnotation(metricKindTypeEEnum, source, new String[] {"name", "metricKindType"});
    addAnnotation(
        metricKindTypeObjectEDataType,
        source,
        new String[] {
          "name", "metricKindType:Object",
          "baseType", "metricKindType"
        });
    addAnnotation(
        metricsTypeEClass,
        source,
        new String[] {
          "name", "metricsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getMetricsType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getMetricsType_Metric(),
        source,
        new String[] {
          "kind", "element",
          "name", "metric",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        metricTypeEClass,
        source,
        new String[] {
          "name", "metricType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getMetricType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getMetricType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getMetricType_Kind(),
        source,
        new String[] {
          "kind", "element",
          "name", "kind",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getMetricType_Dynamic(),
        source,
        new String[] {
          "kind", "element",
          "name", "dynamic",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorModelTypeEClass,
        source,
        new String[] {
          "name", "operatorModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorModelType_CppOperatorModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "cppOperatorModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorModelType_JavaOperatorModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "javaOperatorModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        opModelTypeEClass,
        source,
        new String[] {
          "name", "opModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOpModelType_Context(),
        source,
        new String[] {
          "kind", "element",
          "name", "context",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOpModelType_Parameters(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameters",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOpModelType_InputPorts(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPorts",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOpModelType_OutputPorts(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPorts",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        optionalPortScopeTypeEClass,
        source,
        new String[] {
          "name", "optionalPortScopeType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOptionalPortScopeType_Port(),
        source,
        new String[] {
          "kind", "element",
          "name", "port",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        outputFunctionsTypeEClass,
        source,
        new String[] {
          "name", "outputFunctionsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOutputFunctionsType_Default(),
        source,
        new String[] {
          "kind", "element",
          "name", "default",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputFunctionsType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        outputPortOpenSetTypeEClass,
        source,
        new String[] {
          "name", "outputPortOpenSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOutputPortOpenSetType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_ExpressionMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "expressionMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_AutoAssignment(),
        source,
        new String[] {
          "kind", "element",
          "name", "autoAssignment",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_CompleteAssignment(),
        source,
        new String[] {
          "kind", "element",
          "name", "completeAssignment",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_RewriteAllowed(),
        source,
        new String[] {
          "kind", "element",
          "name", "rewriteAllowed",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_OutputFunctions(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputFunctions",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_WindowPunctuationOutputMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationOutputMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_WindowPunctuationInputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPunctuationInputPort",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_FinalPunctuationPortScope(),
        source,
        new String[] {
          "kind", "element",
          "name", "finalPunctuationPortScope",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_TupleMutationAllowed(),
        source,
        new String[] {
          "kind", "element",
          "name", "tupleMutationAllowed",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_OutputAssignmentPortScope(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputAssignmentPortScope",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortOpenSetType_AllowNestedCustomOutputFunctions(),
        source,
        new String[] {
          "kind", "element",
          "name", "allowNestedCustomOutputFunctions",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        outputPortSetTypeEClass,
        source,
        new String[] {
          "name", "outputPortSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOutputPortSetType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortSetType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        outputPortsTypeEClass,
        source,
        new String[] {
          "name", "outputPortsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOutputPortsType_OutputPortSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPortSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOutputPortsType_OutputPortOpenSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPortOpenSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        parametersTypeEClass,
        source,
        new String[] {
          "name", "parametersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getParametersType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParametersType_AllowAny(),
        source,
        new String[] {
          "kind", "element",
          "name", "allowAny",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParametersType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        parameterTypeEClass,
        source,
        new String[] {
          "name", "parameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getParameterType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_Optional(),
        source,
        new String[] {
          "kind", "element",
          "name", "optional",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_RewriteAllowed(),
        source,
        new String[] {
          "kind", "element",
          "name", "rewriteAllowed",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_ExpressionMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "expressionMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_Cardinality(),
        source,
        new String[] {
          "kind", "element",
          "name", "cardinality",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_PortScope(),
        source,
        new String[] {
          "kind", "element",
          "name", "portScope",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getParameterType_CustomOutputFunction(),
        source,
        new String[] {
          "kind", "element",
          "name", "customOutputFunction",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        portScopeTypeEClass,
        source,
        new String[] {
          "name", "portScopeType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPortScopeType_Port(),
        source,
        new String[] {
          "kind", "element",
          "name", "port",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        singleThreadedContextTypeEEnum, source, new String[] {"name", "singleThreadedContextType"});
    addAnnotation(
        singleThreadedContextTypeObjectEDataType,
        source,
        new String[] {
          "name", "singleThreadedContextType:Object",
          "baseType", "singleThreadedContextType"
        });
    addAnnotation(
        splExpressionTreeTypeEClass,
        source,
        new String[] {
          "name", "splExpressionTreeType",
          "kind", "empty"
        });
    addAnnotation(
        getSplExpressionTreeType_CppCode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "cppCode"
        });
    addAnnotation(
        getSplExpressionTreeType_Output(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "output"
        });
    addAnnotation(
        getSplExpressionTreeType_Param(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "param"
        });
    addAnnotation(
        windowExpressionModeTypeEEnum, source, new String[] {"name", "windowExpressionModeType"});
    addAnnotation(
        windowExpressionModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowExpressionModeType:Object",
          "baseType", "windowExpressionModeType"
        });
    addAnnotation(windowingModeTypeEEnum, source, new String[] {"name", "windowingModeType"});
    addAnnotation(
        windowingModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowingModeType:Object",
          "baseType", "windowingModeType"
        });
    addAnnotation(
        windowPunctuationInputModeTypeEEnum,
        source,
        new String[] {"name", "windowPunctuationInputModeType"});
    addAnnotation(
        windowPunctuationInputModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowPunctuationInputModeType:Object",
          "baseType", "windowPunctuationInputModeType"
        });
    addAnnotation(
        windowPunctuationOutputModeTypeEEnum,
        source,
        new String[] {"name", "windowPunctuationOutputModeType"});
    addAnnotation(
        windowPunctuationOutputModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowPunctuationOutputModeType:Object",
          "baseType", "windowPunctuationOutputModeType"
        });
  }
} // OperatorPackageImpl
