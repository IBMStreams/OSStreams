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
package com.ibm.xmlns.prod.streams.spl.source.code.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl;
import com.ibm.xmlns.prod.streams.spl.source.code.CodeFactory;
import com.ibm.xmlns.prod.streams.spl.source.code.CodePackage;
import com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeGraphType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositePortType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeTypesType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigOptionType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigValueParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.ConfigsType;
import com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType;
import com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.source.code.ExpressionType;
import com.ibm.xmlns.prod.streams.spl.source.code.ExtendedSourceLocationType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.FunctionParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.OnProcessType;
import com.ibm.xmlns.prod.streams.spl.source.code.OnPunctType;
import com.ibm.xmlns.prod.streams.spl.source.code.OnTupleType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationAttributeAssignmentType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationHeadType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputStreamType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationInputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationLogicType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputAssignmentsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationOutputsType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParameterType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationParametersType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowType;
import com.ibm.xmlns.prod.streams.spl.source.code.OperatorInvocationWindowsType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceFileType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceLocationType;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocAnnotationType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocDescriptionType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeModifierType;
import com.ibm.xmlns.prod.streams.spl.source.code.TypeModifiersType;
import com.ibm.xmlns.prod.streams.spl.source.code.UseDirectiveType;
import com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType;
import com.ibm.xmlns.prod.streams.spl.source.code.WindowTypeEnumType;
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
public class CodePackageImpl extends EPackageImpl implements CodePackage {
  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compilationUnitTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeBodyTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeDefinitionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeGraphTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeHeadTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeInputsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeModifiersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeModifierTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeOutputsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeParameterExpressionModeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeParametersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositePortTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeTypesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass configOptionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass configsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass configTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass configValueParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass definitionsTypeEClass = null;

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
  private EClass expressionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass extendedSourceLocationTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionDefinitionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionHeadTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionModifiersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionModifierTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionParameterModifiersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionParameterModifierTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionParametersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass onProcessTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass onPunctTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass onTupleTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationAttributeAssignmentTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationBodyTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationHeadTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationInputStreamTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationInputsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationInputTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationLogicTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationOutputAssignmentsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationOutputAssignmentTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationOutputsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationOutputTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationParametersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationWindowsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass operatorInvocationWindowTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass sourceFileTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass sourceLocationTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass sourceModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass splDocAnnotationTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass splDocDescriptionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass splDocTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass splNamespaceTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass typeDefinitionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass typeModifiersTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass typeModifierTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass useDirectivesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass useDirectiveTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass windowPolicyTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum compositeModifierEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum compositeParameterExpressionModeEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum functionModifierEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum functionParameterModifierEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum typeModifierEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowPolicyKindEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum windowTypeEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType compositeModifierEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType compositeParameterExpressionModeEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType functionModifierEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType functionParameterModifierEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType typeModifierEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowPolicyKindEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType windowTypeEnumTypeObjectEDataType = null;

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
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#eNS_URI
   * @see #init()
   * @generated
   */
  private CodePackageImpl() {
    super(eNS_URI, CodeFactory.eINSTANCE);
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
   * <p>This method is used to initialize {@link CodePackage#eINSTANCE} when that field is accessed.
   * Clients should not invoke it directly. Instead, they should simply access that field to obtain
   * the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #eNS_URI
   * @see #createPackageContents()
   * @see #initializePackageContents()
   * @generated
   */
  public static CodePackage init() {
    if (isInited) return (CodePackage) EPackage.Registry.INSTANCE.getEPackage(CodePackage.eNS_URI);

    // Obtain or create and register package
    CodePackageImpl theCodePackage =
        (CodePackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof CodePackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new CodePackageImpl());

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
    theCodePackage.createPackageContents();
    theCommonPackage.createPackageContents();

    // Initialize created meta-data
    theCodePackage.initializePackageContents();
    theCommonPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theCodePackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(CodePackage.eNS_URI, theCodePackage);
    return theCodePackage;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompilationUnitType() {
    return compilationUnitTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompilationUnitType_SplDoc() {
    return (EReference) compilationUnitTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompilationUnitType_SplNamespace() {
    return (EReference) compilationUnitTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompilationUnitType_UseDirectives() {
    return (EReference) compilationUnitTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompilationUnitType_Definitions() {
    return (EReference) compilationUnitTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeBodyType() {
    return compositeBodyTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeBodyType_Types() {
    return (EReference) compositeBodyTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeBodyType_Parameters() {
    return (EReference) compositeBodyTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeBodyType_Graph() {
    return (EReference) compositeBodyTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeBodyType_Configs() {
    return (EReference) compositeBodyTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeDefinitionType() {
    return compositeDefinitionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeDefinitionType_CompositeHead() {
    return (EReference) compositeDefinitionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeDefinitionType_CompositeBody() {
    return (EReference) compositeDefinitionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeGraphType() {
    return compositeGraphTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeGraphType_OperatorInvocation() {
    return (EReference) compositeGraphTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeHeadType() {
    return compositeHeadTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeHeadType_SplDoc() {
    return (EReference) compositeHeadTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeHeadType_Modifiers() {
    return (EReference) compositeHeadTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeHeadType_Inputs() {
    return (EReference) compositeHeadTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeHeadType_Outputs() {
    return (EReference) compositeHeadTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeHeadType_Name() {
    return (EAttribute) compositeHeadTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeInputsType() {
    return compositeInputsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeInputsType_Iport() {
    return (EReference) compositeInputsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeModifiersType() {
    return compositeModifiersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeModifiersType_Modifier() {
    return (EReference) compositeModifiersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeModifierType() {
    return compositeModifierTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeModifierType_Name() {
    return (EAttribute) compositeModifierTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeOutputsType() {
    return compositeOutputsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeOutputsType_Oport() {
    return (EReference) compositeOutputsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeParameterExpressionModeType() {
    return compositeParameterExpressionModeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeParameterExpressionModeType_Mode() {
    return (EAttribute) compositeParameterExpressionModeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeParameterExpressionModeType_Type() {
    return (EAttribute) compositeParameterExpressionModeTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeParametersType() {
    return compositeParametersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeParametersType_Parameter() {
    return (EReference) compositeParametersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeParameterType() {
    return compositeParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeParameterType_ExpressionMode() {
    return (EReference) compositeParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeParameterType_DefaultValue() {
    return (EAttribute) compositeParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeParameterType_Name() {
    return (EAttribute) compositeParameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositePortType() {
    return compositePortTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositePortType_Index() {
    return (EAttribute) compositePortTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositePortType_Name() {
    return (EAttribute) compositePortTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositePortType_Type() {
    return (EAttribute) compositePortTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeTypesType() {
    return compositeTypesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeTypesType_Type() {
    return (EReference) compositeTypesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getConfigOptionType() {
    return configOptionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getConfigOptionType_Parameter() {
    return (EReference) configOptionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getConfigOptionType_Value() {
    return (EAttribute) configOptionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getConfigsType() {
    return configsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getConfigsType_Config() {
    return (EReference) configsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getConfigType() {
    return configTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getConfigType_Option() {
    return (EReference) configTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getConfigType_Name() {
    return (EAttribute) configTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getConfigValueParameterType() {
    return configValueParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getConfigValueParameterType_Value() {
    return (EAttribute) configValueParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getDefinitionsType() {
    return definitionsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDefinitionsType_TypeDefinition() {
    return (EReference) definitionsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDefinitionsType_FunctionDefinition() {
    return (EReference) definitionsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDefinitionsType_CompositeDefinition() {
    return (EReference) definitionsTypeEClass.getEStructuralFeatures().get(2);
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
  public EReference getDocumentRoot_SourceModel() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getExpressionType() {
    return expressionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExpressionType_Expr() {
    return (EAttribute) expressionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getExtendedSourceLocationType() {
    return extendedSourceLocationTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExtendedSourceLocationType_EndColumn() {
    return (EAttribute) extendedSourceLocationTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExtendedSourceLocationType_EndLine() {
    return (EAttribute) extendedSourceLocationTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExtendedSourceLocationType_StartColumn() {
    return (EAttribute) extendedSourceLocationTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExtendedSourceLocationType_StartLine() {
    return (EAttribute) extendedSourceLocationTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionDefinitionType() {
    return functionDefinitionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionDefinitionType_FunctionHead() {
    return (EReference) functionDefinitionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionDefinitionType_FunctionBody() {
    return (EAttribute) functionDefinitionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionHeadType() {
    return functionHeadTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionHeadType_SplDoc() {
    return (EReference) functionHeadTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionHeadType_Modifiers() {
    return (EReference) functionHeadTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionHeadType_Parameters() {
    return (EReference) functionHeadTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionHeadType_Name() {
    return (EAttribute) functionHeadTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionHeadType_ReturnType() {
    return (EAttribute) functionHeadTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionModifiersType() {
    return functionModifiersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionModifiersType_Modifier() {
    return (EReference) functionModifiersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionModifierType() {
    return functionModifierTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionModifierType_Name() {
    return (EAttribute) functionModifierTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionParameterModifiersType() {
    return functionParameterModifiersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionParameterModifiersType_Modifier() {
    return (EReference) functionParameterModifiersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionParameterModifierType() {
    return functionParameterModifierTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionParameterModifierType_Name() {
    return (EAttribute) functionParameterModifierTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionParametersType() {
    return functionParametersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionParametersType_Parameter() {
    return (EReference) functionParametersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionParameterType() {
    return functionParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionParameterType_Modifiers() {
    return (EReference) functionParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionParameterType_Name() {
    return (EAttribute) functionParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionParameterType_Type() {
    return (EAttribute) functionParameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOnProcessType() {
    return onProcessTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOnPunctType() {
    return onPunctTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOnPunctType_PortName() {
    return (EAttribute) onPunctTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOnTupleType() {
    return onTupleTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOnTupleType_PortName() {
    return (EAttribute) onTupleTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationAttributeAssignmentType() {
    return operatorInvocationAttributeAssignmentTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationAttributeAssignmentType_Value() {
    return (EReference)
        operatorInvocationAttributeAssignmentTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationAttributeAssignmentType_Name() {
    return (EAttribute)
        operatorInvocationAttributeAssignmentTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationAttributeAssignmentType_OutputFunction() {
    return (EAttribute)
        operatorInvocationAttributeAssignmentTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationBodyType() {
    return operatorInvocationBodyTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationBodyType_Logic() {
    return (EReference) operatorInvocationBodyTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationBodyType_Windows() {
    return (EReference) operatorInvocationBodyTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationBodyType_Parameters() {
    return (EReference) operatorInvocationBodyTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationBodyType_OutputAssignments() {
    return (EReference) operatorInvocationBodyTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationBodyType_Configs() {
    return (EReference) operatorInvocationBodyTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationHeadType() {
    return operatorInvocationHeadTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationHeadType_SplDoc() {
    return (EReference) operatorInvocationHeadTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationHeadType_Outputs() {
    return (EReference) operatorInvocationHeadTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationHeadType_Inputs() {
    return (EReference) operatorInvocationHeadTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationHeadType_InvocationAlias() {
    return (EAttribute) operatorInvocationHeadTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationHeadType_OperatorName() {
    return (EAttribute) operatorInvocationHeadTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationInputStreamType() {
    return operatorInvocationInputStreamTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationInputStreamType_Name() {
    return (EAttribute) operatorInvocationInputStreamTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationInputsType() {
    return operatorInvocationInputsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationInputsType_Input() {
    return (EReference) operatorInvocationInputsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationInputType() {
    return operatorInvocationInputTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationInputType_Istream() {
    return (EReference) operatorInvocationInputTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationInputType_Alias() {
    return (EAttribute) operatorInvocationInputTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationInputType_Index() {
    return (EAttribute) operatorInvocationInputTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationLogicType() {
    return operatorInvocationLogicTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationLogicType_OnProcess() {
    return (EReference) operatorInvocationLogicTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationLogicType_OnTuple() {
    return (EReference) operatorInvocationLogicTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationLogicType_OnPunct() {
    return (EReference) operatorInvocationLogicTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationLogicType_HasState() {
    return (EAttribute) operatorInvocationLogicTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationOutputAssignmentsType() {
    return operatorInvocationOutputAssignmentsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationOutputAssignmentsType_OutputAssignment() {
    return (EReference)
        operatorInvocationOutputAssignmentsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationOutputAssignmentType() {
    return operatorInvocationOutputAssignmentTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationOutputAssignmentType_AttributeAssignment() {
    return (EReference)
        operatorInvocationOutputAssignmentTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationOutputAssignmentType_PortName() {
    return (EAttribute)
        operatorInvocationOutputAssignmentTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationOutputsType() {
    return operatorInvocationOutputsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationOutputsType_Output() {
    return (EReference) operatorInvocationOutputsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationOutputType() {
    return operatorInvocationOutputTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationOutputType_Alias() {
    return (EAttribute) operatorInvocationOutputTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationOutputType_Index() {
    return (EAttribute) operatorInvocationOutputTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationOutputType_StreamName() {
    return (EAttribute) operatorInvocationOutputTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationOutputType_Type() {
    return (EAttribute) operatorInvocationOutputTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationParametersType() {
    return operatorInvocationParametersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationParametersType_Parameter() {
    return (EReference) operatorInvocationParametersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationParameterType() {
    return operatorInvocationParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationParameterType_Value() {
    return (EReference) operatorInvocationParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationParameterType_Name() {
    return (EAttribute) operatorInvocationParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationType() {
    return operatorInvocationTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationType_OperatorInvocationHead() {
    return (EReference) operatorInvocationTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationType_OperatorInvocationBody() {
    return (EReference) operatorInvocationTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationWindowsType() {
    return operatorInvocationWindowsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationWindowsType_Window() {
    return (EReference) operatorInvocationWindowsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOperatorInvocationWindowType() {
    return operatorInvocationWindowTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationWindowType_EvictionPolicy() {
    return (EReference) operatorInvocationWindowTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getOperatorInvocationWindowType_TriggerPolicy() {
    return (EReference) operatorInvocationWindowTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationWindowType_Partitioned() {
    return (EAttribute) operatorInvocationWindowTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationWindowType_PortName() {
    return (EAttribute) operatorInvocationWindowTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOperatorInvocationWindowType_WindowType() {
    return (EAttribute) operatorInvocationWindowTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSourceFileType() {
    return sourceFileTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSourceFileType_CompilationUnit() {
    return (EReference) sourceFileTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSourceFileType_Uri() {
    return (EAttribute) sourceFileTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSourceLocationType() {
    return sourceLocationTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSourceLocationType_Column() {
    return (EAttribute) sourceLocationTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSourceLocationType_Line() {
    return (EAttribute) sourceLocationTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSourceModelType() {
    return sourceModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSourceModelType_SourceFile() {
    return (EReference) sourceModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSplDocAnnotationType() {
    return splDocAnnotationTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplDocAnnotationType_Name() {
    return (EAttribute) splDocAnnotationTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplDocAnnotationType_Target() {
    return (EAttribute) splDocAnnotationTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSplDocDescriptionType() {
    return splDocDescriptionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplDocDescriptionType_Description() {
    return (EAttribute) splDocDescriptionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSplDocType() {
    return splDocTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSplDocType_Description() {
    return (EReference) splDocTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSplDocType_Annotation() {
    return (EReference) splDocTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSplNamespaceType() {
    return splNamespaceTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSplNamespaceType_SplDoc() {
    return (EReference) splNamespaceTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSplNamespaceType_Name() {
    return (EAttribute) splNamespaceTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getTypeDefinitionType() {
    return typeDefinitionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getTypeDefinitionType_SplDoc() {
    return (EReference) typeDefinitionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getTypeDefinitionType_Modifiers() {
    return (EReference) typeDefinitionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getTypeDefinitionType_Name() {
    return (EAttribute) typeDefinitionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getTypeDefinitionType_Value() {
    return (EAttribute) typeDefinitionTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getTypeModifiersType() {
    return typeModifiersTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getTypeModifiersType_Modifier() {
    return (EReference) typeModifiersTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getTypeModifierType() {
    return typeModifierTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getTypeModifierType_Name() {
    return (EAttribute) typeModifierTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getUseDirectivesType() {
    return useDirectivesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getUseDirectivesType_UseDirective() {
    return (EReference) useDirectivesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getUseDirectiveType() {
    return useDirectiveTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getUseDirectiveType_NamespaceName() {
    return (EAttribute) useDirectiveTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getUseDirectiveType_Tail() {
    return (EAttribute) useDirectiveTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getWindowPolicyType() {
    return windowPolicyTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getWindowPolicyType_Attribute() {
    return (EAttribute) windowPolicyTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getWindowPolicyType_Kind() {
    return (EAttribute) windowPolicyTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getWindowPolicyType_Size() {
    return (EAttribute) windowPolicyTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getCompositeModifierEnumType() {
    return compositeModifierEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getCompositeParameterExpressionModeEnumType() {
    return compositeParameterExpressionModeEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getFunctionModifierEnumType() {
    return functionModifierEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getFunctionParameterModifierEnumType() {
    return functionParameterModifierEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getTypeModifierEnumType() {
    return typeModifierEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowPolicyKindEnumType() {
    return windowPolicyKindEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getWindowTypeEnumType() {
    return windowTypeEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getCompositeModifierEnumTypeObject() {
    return compositeModifierEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getCompositeParameterExpressionModeEnumTypeObject() {
    return compositeParameterExpressionModeEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getFunctionModifierEnumTypeObject() {
    return functionModifierEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getFunctionParameterModifierEnumTypeObject() {
    return functionParameterModifierEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getTypeModifierEnumTypeObject() {
    return typeModifierEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowPolicyKindEnumTypeObject() {
    return windowPolicyKindEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getWindowTypeEnumTypeObject() {
    return windowTypeEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CodeFactory getCodeFactory() {
    return (CodeFactory) getEFactoryInstance();
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
    compilationUnitTypeEClass = createEClass(COMPILATION_UNIT_TYPE);
    createEReference(compilationUnitTypeEClass, COMPILATION_UNIT_TYPE__SPL_DOC);
    createEReference(compilationUnitTypeEClass, COMPILATION_UNIT_TYPE__SPL_NAMESPACE);
    createEReference(compilationUnitTypeEClass, COMPILATION_UNIT_TYPE__USE_DIRECTIVES);
    createEReference(compilationUnitTypeEClass, COMPILATION_UNIT_TYPE__DEFINITIONS);

    compositeBodyTypeEClass = createEClass(COMPOSITE_BODY_TYPE);
    createEReference(compositeBodyTypeEClass, COMPOSITE_BODY_TYPE__TYPES);
    createEReference(compositeBodyTypeEClass, COMPOSITE_BODY_TYPE__PARAMETERS);
    createEReference(compositeBodyTypeEClass, COMPOSITE_BODY_TYPE__GRAPH);
    createEReference(compositeBodyTypeEClass, COMPOSITE_BODY_TYPE__CONFIGS);

    compositeDefinitionTypeEClass = createEClass(COMPOSITE_DEFINITION_TYPE);
    createEReference(compositeDefinitionTypeEClass, COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD);
    createEReference(compositeDefinitionTypeEClass, COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY);

    compositeGraphTypeEClass = createEClass(COMPOSITE_GRAPH_TYPE);
    createEReference(compositeGraphTypeEClass, COMPOSITE_GRAPH_TYPE__OPERATOR_INVOCATION);

    compositeHeadTypeEClass = createEClass(COMPOSITE_HEAD_TYPE);
    createEReference(compositeHeadTypeEClass, COMPOSITE_HEAD_TYPE__SPL_DOC);
    createEReference(compositeHeadTypeEClass, COMPOSITE_HEAD_TYPE__MODIFIERS);
    createEReference(compositeHeadTypeEClass, COMPOSITE_HEAD_TYPE__INPUTS);
    createEReference(compositeHeadTypeEClass, COMPOSITE_HEAD_TYPE__OUTPUTS);
    createEAttribute(compositeHeadTypeEClass, COMPOSITE_HEAD_TYPE__NAME);

    compositeInputsTypeEClass = createEClass(COMPOSITE_INPUTS_TYPE);
    createEReference(compositeInputsTypeEClass, COMPOSITE_INPUTS_TYPE__IPORT);

    compositeModifiersTypeEClass = createEClass(COMPOSITE_MODIFIERS_TYPE);
    createEReference(compositeModifiersTypeEClass, COMPOSITE_MODIFIERS_TYPE__MODIFIER);

    compositeModifierTypeEClass = createEClass(COMPOSITE_MODIFIER_TYPE);
    createEAttribute(compositeModifierTypeEClass, COMPOSITE_MODIFIER_TYPE__NAME);

    compositeOutputsTypeEClass = createEClass(COMPOSITE_OUTPUTS_TYPE);
    createEReference(compositeOutputsTypeEClass, COMPOSITE_OUTPUTS_TYPE__OPORT);

    compositeParameterExpressionModeTypeEClass =
        createEClass(COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE);
    createEAttribute(
        compositeParameterExpressionModeTypeEClass, COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__MODE);
    createEAttribute(
        compositeParameterExpressionModeTypeEClass, COMPOSITE_PARAMETER_EXPRESSION_MODE_TYPE__TYPE);

    compositeParametersTypeEClass = createEClass(COMPOSITE_PARAMETERS_TYPE);
    createEReference(compositeParametersTypeEClass, COMPOSITE_PARAMETERS_TYPE__PARAMETER);

    compositeParameterTypeEClass = createEClass(COMPOSITE_PARAMETER_TYPE);
    createEReference(compositeParameterTypeEClass, COMPOSITE_PARAMETER_TYPE__EXPRESSION_MODE);
    createEAttribute(compositeParameterTypeEClass, COMPOSITE_PARAMETER_TYPE__DEFAULT_VALUE);
    createEAttribute(compositeParameterTypeEClass, COMPOSITE_PARAMETER_TYPE__NAME);

    compositePortTypeEClass = createEClass(COMPOSITE_PORT_TYPE);
    createEAttribute(compositePortTypeEClass, COMPOSITE_PORT_TYPE__INDEX);
    createEAttribute(compositePortTypeEClass, COMPOSITE_PORT_TYPE__NAME);
    createEAttribute(compositePortTypeEClass, COMPOSITE_PORT_TYPE__TYPE);

    compositeTypesTypeEClass = createEClass(COMPOSITE_TYPES_TYPE);
    createEReference(compositeTypesTypeEClass, COMPOSITE_TYPES_TYPE__TYPE);

    configOptionTypeEClass = createEClass(CONFIG_OPTION_TYPE);
    createEReference(configOptionTypeEClass, CONFIG_OPTION_TYPE__PARAMETER);
    createEAttribute(configOptionTypeEClass, CONFIG_OPTION_TYPE__VALUE);

    configsTypeEClass = createEClass(CONFIGS_TYPE);
    createEReference(configsTypeEClass, CONFIGS_TYPE__CONFIG);

    configTypeEClass = createEClass(CONFIG_TYPE);
    createEReference(configTypeEClass, CONFIG_TYPE__OPTION);
    createEAttribute(configTypeEClass, CONFIG_TYPE__NAME);

    configValueParameterTypeEClass = createEClass(CONFIG_VALUE_PARAMETER_TYPE);
    createEAttribute(configValueParameterTypeEClass, CONFIG_VALUE_PARAMETER_TYPE__VALUE);

    definitionsTypeEClass = createEClass(DEFINITIONS_TYPE);
    createEReference(definitionsTypeEClass, DEFINITIONS_TYPE__TYPE_DEFINITION);
    createEReference(definitionsTypeEClass, DEFINITIONS_TYPE__FUNCTION_DEFINITION);
    createEReference(definitionsTypeEClass, DEFINITIONS_TYPE__COMPOSITE_DEFINITION);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__SOURCE_MODEL);

    expressionTypeEClass = createEClass(EXPRESSION_TYPE);
    createEAttribute(expressionTypeEClass, EXPRESSION_TYPE__EXPR);

    extendedSourceLocationTypeEClass = createEClass(EXTENDED_SOURCE_LOCATION_TYPE);
    createEAttribute(extendedSourceLocationTypeEClass, EXTENDED_SOURCE_LOCATION_TYPE__END_COLUMN);
    createEAttribute(extendedSourceLocationTypeEClass, EXTENDED_SOURCE_LOCATION_TYPE__END_LINE);
    createEAttribute(extendedSourceLocationTypeEClass, EXTENDED_SOURCE_LOCATION_TYPE__START_COLUMN);
    createEAttribute(extendedSourceLocationTypeEClass, EXTENDED_SOURCE_LOCATION_TYPE__START_LINE);

    functionDefinitionTypeEClass = createEClass(FUNCTION_DEFINITION_TYPE);
    createEReference(functionDefinitionTypeEClass, FUNCTION_DEFINITION_TYPE__FUNCTION_HEAD);
    createEAttribute(functionDefinitionTypeEClass, FUNCTION_DEFINITION_TYPE__FUNCTION_BODY);

    functionHeadTypeEClass = createEClass(FUNCTION_HEAD_TYPE);
    createEReference(functionHeadTypeEClass, FUNCTION_HEAD_TYPE__SPL_DOC);
    createEReference(functionHeadTypeEClass, FUNCTION_HEAD_TYPE__MODIFIERS);
    createEReference(functionHeadTypeEClass, FUNCTION_HEAD_TYPE__PARAMETERS);
    createEAttribute(functionHeadTypeEClass, FUNCTION_HEAD_TYPE__NAME);
    createEAttribute(functionHeadTypeEClass, FUNCTION_HEAD_TYPE__RETURN_TYPE);

    functionModifiersTypeEClass = createEClass(FUNCTION_MODIFIERS_TYPE);
    createEReference(functionModifiersTypeEClass, FUNCTION_MODIFIERS_TYPE__MODIFIER);

    functionModifierTypeEClass = createEClass(FUNCTION_MODIFIER_TYPE);
    createEAttribute(functionModifierTypeEClass, FUNCTION_MODIFIER_TYPE__NAME);

    functionParameterModifiersTypeEClass = createEClass(FUNCTION_PARAMETER_MODIFIERS_TYPE);
    createEReference(
        functionParameterModifiersTypeEClass, FUNCTION_PARAMETER_MODIFIERS_TYPE__MODIFIER);

    functionParameterModifierTypeEClass = createEClass(FUNCTION_PARAMETER_MODIFIER_TYPE);
    createEAttribute(functionParameterModifierTypeEClass, FUNCTION_PARAMETER_MODIFIER_TYPE__NAME);

    functionParametersTypeEClass = createEClass(FUNCTION_PARAMETERS_TYPE);
    createEReference(functionParametersTypeEClass, FUNCTION_PARAMETERS_TYPE__PARAMETER);

    functionParameterTypeEClass = createEClass(FUNCTION_PARAMETER_TYPE);
    createEReference(functionParameterTypeEClass, FUNCTION_PARAMETER_TYPE__MODIFIERS);
    createEAttribute(functionParameterTypeEClass, FUNCTION_PARAMETER_TYPE__NAME);
    createEAttribute(functionParameterTypeEClass, FUNCTION_PARAMETER_TYPE__TYPE);

    onProcessTypeEClass = createEClass(ON_PROCESS_TYPE);

    onPunctTypeEClass = createEClass(ON_PUNCT_TYPE);
    createEAttribute(onPunctTypeEClass, ON_PUNCT_TYPE__PORT_NAME);

    onTupleTypeEClass = createEClass(ON_TUPLE_TYPE);
    createEAttribute(onTupleTypeEClass, ON_TUPLE_TYPE__PORT_NAME);

    operatorInvocationAttributeAssignmentTypeEClass =
        createEClass(OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE);
    createEReference(
        operatorInvocationAttributeAssignmentTypeEClass,
        OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__VALUE);
    createEAttribute(
        operatorInvocationAttributeAssignmentTypeEClass,
        OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__NAME);
    createEAttribute(
        operatorInvocationAttributeAssignmentTypeEClass,
        OPERATOR_INVOCATION_ATTRIBUTE_ASSIGNMENT_TYPE__OUTPUT_FUNCTION);

    operatorInvocationBodyTypeEClass = createEClass(OPERATOR_INVOCATION_BODY_TYPE);
    createEReference(operatorInvocationBodyTypeEClass, OPERATOR_INVOCATION_BODY_TYPE__LOGIC);
    createEReference(operatorInvocationBodyTypeEClass, OPERATOR_INVOCATION_BODY_TYPE__WINDOWS);
    createEReference(operatorInvocationBodyTypeEClass, OPERATOR_INVOCATION_BODY_TYPE__PARAMETERS);
    createEReference(
        operatorInvocationBodyTypeEClass, OPERATOR_INVOCATION_BODY_TYPE__OUTPUT_ASSIGNMENTS);
    createEReference(operatorInvocationBodyTypeEClass, OPERATOR_INVOCATION_BODY_TYPE__CONFIGS);

    operatorInvocationHeadTypeEClass = createEClass(OPERATOR_INVOCATION_HEAD_TYPE);
    createEReference(operatorInvocationHeadTypeEClass, OPERATOR_INVOCATION_HEAD_TYPE__SPL_DOC);
    createEReference(operatorInvocationHeadTypeEClass, OPERATOR_INVOCATION_HEAD_TYPE__OUTPUTS);
    createEReference(operatorInvocationHeadTypeEClass, OPERATOR_INVOCATION_HEAD_TYPE__INPUTS);
    createEAttribute(
        operatorInvocationHeadTypeEClass, OPERATOR_INVOCATION_HEAD_TYPE__INVOCATION_ALIAS);
    createEAttribute(
        operatorInvocationHeadTypeEClass, OPERATOR_INVOCATION_HEAD_TYPE__OPERATOR_NAME);

    operatorInvocationInputStreamTypeEClass = createEClass(OPERATOR_INVOCATION_INPUT_STREAM_TYPE);
    createEAttribute(
        operatorInvocationInputStreamTypeEClass, OPERATOR_INVOCATION_INPUT_STREAM_TYPE__NAME);

    operatorInvocationInputsTypeEClass = createEClass(OPERATOR_INVOCATION_INPUTS_TYPE);
    createEReference(operatorInvocationInputsTypeEClass, OPERATOR_INVOCATION_INPUTS_TYPE__INPUT);

    operatorInvocationInputTypeEClass = createEClass(OPERATOR_INVOCATION_INPUT_TYPE);
    createEReference(operatorInvocationInputTypeEClass, OPERATOR_INVOCATION_INPUT_TYPE__ISTREAM);
    createEAttribute(operatorInvocationInputTypeEClass, OPERATOR_INVOCATION_INPUT_TYPE__ALIAS);
    createEAttribute(operatorInvocationInputTypeEClass, OPERATOR_INVOCATION_INPUT_TYPE__INDEX);

    operatorInvocationLogicTypeEClass = createEClass(OPERATOR_INVOCATION_LOGIC_TYPE);
    createEReference(operatorInvocationLogicTypeEClass, OPERATOR_INVOCATION_LOGIC_TYPE__ON_PROCESS);
    createEReference(operatorInvocationLogicTypeEClass, OPERATOR_INVOCATION_LOGIC_TYPE__ON_TUPLE);
    createEReference(operatorInvocationLogicTypeEClass, OPERATOR_INVOCATION_LOGIC_TYPE__ON_PUNCT);
    createEAttribute(operatorInvocationLogicTypeEClass, OPERATOR_INVOCATION_LOGIC_TYPE__HAS_STATE);

    operatorInvocationOutputAssignmentsTypeEClass =
        createEClass(OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE);
    createEReference(
        operatorInvocationOutputAssignmentsTypeEClass,
        OPERATOR_INVOCATION_OUTPUT_ASSIGNMENTS_TYPE__OUTPUT_ASSIGNMENT);

    operatorInvocationOutputAssignmentTypeEClass =
        createEClass(OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE);
    createEReference(
        operatorInvocationOutputAssignmentTypeEClass,
        OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__ATTRIBUTE_ASSIGNMENT);
    createEAttribute(
        operatorInvocationOutputAssignmentTypeEClass,
        OPERATOR_INVOCATION_OUTPUT_ASSIGNMENT_TYPE__PORT_NAME);

    operatorInvocationOutputsTypeEClass = createEClass(OPERATOR_INVOCATION_OUTPUTS_TYPE);
    createEReference(operatorInvocationOutputsTypeEClass, OPERATOR_INVOCATION_OUTPUTS_TYPE__OUTPUT);

    operatorInvocationOutputTypeEClass = createEClass(OPERATOR_INVOCATION_OUTPUT_TYPE);
    createEAttribute(operatorInvocationOutputTypeEClass, OPERATOR_INVOCATION_OUTPUT_TYPE__ALIAS);
    createEAttribute(operatorInvocationOutputTypeEClass, OPERATOR_INVOCATION_OUTPUT_TYPE__INDEX);
    createEAttribute(
        operatorInvocationOutputTypeEClass, OPERATOR_INVOCATION_OUTPUT_TYPE__STREAM_NAME);
    createEAttribute(operatorInvocationOutputTypeEClass, OPERATOR_INVOCATION_OUTPUT_TYPE__TYPE);

    operatorInvocationParametersTypeEClass = createEClass(OPERATOR_INVOCATION_PARAMETERS_TYPE);
    createEReference(
        operatorInvocationParametersTypeEClass, OPERATOR_INVOCATION_PARAMETERS_TYPE__PARAMETER);

    operatorInvocationParameterTypeEClass = createEClass(OPERATOR_INVOCATION_PARAMETER_TYPE);
    createEReference(
        operatorInvocationParameterTypeEClass, OPERATOR_INVOCATION_PARAMETER_TYPE__VALUE);
    createEAttribute(
        operatorInvocationParameterTypeEClass, OPERATOR_INVOCATION_PARAMETER_TYPE__NAME);

    operatorInvocationTypeEClass = createEClass(OPERATOR_INVOCATION_TYPE);
    createEReference(
        operatorInvocationTypeEClass, OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_HEAD);
    createEReference(
        operatorInvocationTypeEClass, OPERATOR_INVOCATION_TYPE__OPERATOR_INVOCATION_BODY);

    operatorInvocationWindowsTypeEClass = createEClass(OPERATOR_INVOCATION_WINDOWS_TYPE);
    createEReference(operatorInvocationWindowsTypeEClass, OPERATOR_INVOCATION_WINDOWS_TYPE__WINDOW);

    operatorInvocationWindowTypeEClass = createEClass(OPERATOR_INVOCATION_WINDOW_TYPE);
    createEReference(
        operatorInvocationWindowTypeEClass, OPERATOR_INVOCATION_WINDOW_TYPE__EVICTION_POLICY);
    createEReference(
        operatorInvocationWindowTypeEClass, OPERATOR_INVOCATION_WINDOW_TYPE__TRIGGER_POLICY);
    createEAttribute(
        operatorInvocationWindowTypeEClass, OPERATOR_INVOCATION_WINDOW_TYPE__PARTITIONED);
    createEAttribute(
        operatorInvocationWindowTypeEClass, OPERATOR_INVOCATION_WINDOW_TYPE__PORT_NAME);
    createEAttribute(
        operatorInvocationWindowTypeEClass, OPERATOR_INVOCATION_WINDOW_TYPE__WINDOW_TYPE);

    sourceFileTypeEClass = createEClass(SOURCE_FILE_TYPE);
    createEReference(sourceFileTypeEClass, SOURCE_FILE_TYPE__COMPILATION_UNIT);
    createEAttribute(sourceFileTypeEClass, SOURCE_FILE_TYPE__URI);

    sourceLocationTypeEClass = createEClass(SOURCE_LOCATION_TYPE);
    createEAttribute(sourceLocationTypeEClass, SOURCE_LOCATION_TYPE__COLUMN);
    createEAttribute(sourceLocationTypeEClass, SOURCE_LOCATION_TYPE__LINE);

    sourceModelTypeEClass = createEClass(SOURCE_MODEL_TYPE);
    createEReference(sourceModelTypeEClass, SOURCE_MODEL_TYPE__SOURCE_FILE);

    splDocAnnotationTypeEClass = createEClass(SPL_DOC_ANNOTATION_TYPE);
    createEAttribute(splDocAnnotationTypeEClass, SPL_DOC_ANNOTATION_TYPE__NAME);
    createEAttribute(splDocAnnotationTypeEClass, SPL_DOC_ANNOTATION_TYPE__TARGET);

    splDocDescriptionTypeEClass = createEClass(SPL_DOC_DESCRIPTION_TYPE);
    createEAttribute(splDocDescriptionTypeEClass, SPL_DOC_DESCRIPTION_TYPE__DESCRIPTION);

    splDocTypeEClass = createEClass(SPL_DOC_TYPE);
    createEReference(splDocTypeEClass, SPL_DOC_TYPE__DESCRIPTION);
    createEReference(splDocTypeEClass, SPL_DOC_TYPE__ANNOTATION);

    splNamespaceTypeEClass = createEClass(SPL_NAMESPACE_TYPE);
    createEReference(splNamespaceTypeEClass, SPL_NAMESPACE_TYPE__SPL_DOC);
    createEAttribute(splNamespaceTypeEClass, SPL_NAMESPACE_TYPE__NAME);

    typeDefinitionTypeEClass = createEClass(TYPE_DEFINITION_TYPE);
    createEReference(typeDefinitionTypeEClass, TYPE_DEFINITION_TYPE__SPL_DOC);
    createEReference(typeDefinitionTypeEClass, TYPE_DEFINITION_TYPE__MODIFIERS);
    createEAttribute(typeDefinitionTypeEClass, TYPE_DEFINITION_TYPE__NAME);
    createEAttribute(typeDefinitionTypeEClass, TYPE_DEFINITION_TYPE__VALUE);

    typeModifiersTypeEClass = createEClass(TYPE_MODIFIERS_TYPE);
    createEReference(typeModifiersTypeEClass, TYPE_MODIFIERS_TYPE__MODIFIER);

    typeModifierTypeEClass = createEClass(TYPE_MODIFIER_TYPE);
    createEAttribute(typeModifierTypeEClass, TYPE_MODIFIER_TYPE__NAME);

    useDirectivesTypeEClass = createEClass(USE_DIRECTIVES_TYPE);
    createEReference(useDirectivesTypeEClass, USE_DIRECTIVES_TYPE__USE_DIRECTIVE);

    useDirectiveTypeEClass = createEClass(USE_DIRECTIVE_TYPE);
    createEAttribute(useDirectiveTypeEClass, USE_DIRECTIVE_TYPE__NAMESPACE_NAME);
    createEAttribute(useDirectiveTypeEClass, USE_DIRECTIVE_TYPE__TAIL);

    windowPolicyTypeEClass = createEClass(WINDOW_POLICY_TYPE);
    createEAttribute(windowPolicyTypeEClass, WINDOW_POLICY_TYPE__ATTRIBUTE);
    createEAttribute(windowPolicyTypeEClass, WINDOW_POLICY_TYPE__KIND);
    createEAttribute(windowPolicyTypeEClass, WINDOW_POLICY_TYPE__SIZE);

    // Create enums
    compositeModifierEnumTypeEEnum = createEEnum(COMPOSITE_MODIFIER_ENUM_TYPE);
    compositeParameterExpressionModeEnumTypeEEnum =
        createEEnum(COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE);
    functionModifierEnumTypeEEnum = createEEnum(FUNCTION_MODIFIER_ENUM_TYPE);
    functionParameterModifierEnumTypeEEnum = createEEnum(FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE);
    typeModifierEnumTypeEEnum = createEEnum(TYPE_MODIFIER_ENUM_TYPE);
    windowPolicyKindEnumTypeEEnum = createEEnum(WINDOW_POLICY_KIND_ENUM_TYPE);
    windowTypeEnumTypeEEnum = createEEnum(WINDOW_TYPE_ENUM_TYPE);

    // Create data types
    compositeModifierEnumTypeObjectEDataType = createEDataType(COMPOSITE_MODIFIER_ENUM_TYPE_OBJECT);
    compositeParameterExpressionModeEnumTypeObjectEDataType =
        createEDataType(COMPOSITE_PARAMETER_EXPRESSION_MODE_ENUM_TYPE_OBJECT);
    functionModifierEnumTypeObjectEDataType = createEDataType(FUNCTION_MODIFIER_ENUM_TYPE_OBJECT);
    functionParameterModifierEnumTypeObjectEDataType =
        createEDataType(FUNCTION_PARAMETER_MODIFIER_ENUM_TYPE_OBJECT);
    typeModifierEnumTypeObjectEDataType = createEDataType(TYPE_MODIFIER_ENUM_TYPE_OBJECT);
    windowPolicyKindEnumTypeObjectEDataType = createEDataType(WINDOW_POLICY_KIND_ENUM_TYPE_OBJECT);
    windowTypeEnumTypeObjectEDataType = createEDataType(WINDOW_TYPE_ENUM_TYPE_OBJECT);
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
    XMLTypePackage theXMLTypePackage =
        (XMLTypePackage) EPackage.Registry.INSTANCE.getEPackage(XMLTypePackage.eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    compositeDefinitionTypeEClass.getESuperTypes().add(this.getExtendedSourceLocationType());
    compositeHeadTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    compositeParameterTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    compositePortTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    compositeTypesTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    configOptionTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    configTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    functionDefinitionTypeEClass.getESuperTypes().add(this.getExtendedSourceLocationType());
    functionHeadTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    functionParameterTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    onProcessTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    onPunctTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    onTupleTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationAttributeAssignmentTypeEClass
        .getESuperTypes()
        .add(this.getSourceLocationType());
    operatorInvocationHeadTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationInputStreamTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationInputTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationOutputAssignmentTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationOutputTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationParameterTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    operatorInvocationWindowTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    splDocAnnotationTypeEClass.getESuperTypes().add(this.getSplDocDescriptionType());
    splNamespaceTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    typeDefinitionTypeEClass.getESuperTypes().add(this.getExtendedSourceLocationType());
    useDirectiveTypeEClass.getESuperTypes().add(this.getSourceLocationType());
    windowPolicyTypeEClass.getESuperTypes().add(this.getSourceLocationType());

    // Initialize classes, features, and operations; add parameters
    initEClass(
        compilationUnitTypeEClass,
        CompilationUnitType.class,
        "CompilationUnitType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompilationUnitType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        CompilationUnitType.class,
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
        getCompilationUnitType_SplNamespace(),
        this.getSplNamespaceType(),
        null,
        "splNamespace",
        null,
        0,
        1,
        CompilationUnitType.class,
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
        getCompilationUnitType_UseDirectives(),
        this.getUseDirectivesType(),
        null,
        "useDirectives",
        null,
        0,
        1,
        CompilationUnitType.class,
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
        getCompilationUnitType_Definitions(),
        this.getDefinitionsType(),
        null,
        "definitions",
        null,
        0,
        1,
        CompilationUnitType.class,
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
        compositeBodyTypeEClass,
        CompositeBodyType.class,
        "CompositeBodyType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeBodyType_Types(),
        this.getCompositeTypesType(),
        null,
        "types",
        null,
        0,
        1,
        CompositeBodyType.class,
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
        getCompositeBodyType_Parameters(),
        this.getCompositeParametersType(),
        null,
        "parameters",
        null,
        0,
        1,
        CompositeBodyType.class,
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
        getCompositeBodyType_Graph(),
        this.getCompositeGraphType(),
        null,
        "graph",
        null,
        0,
        1,
        CompositeBodyType.class,
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
        getCompositeBodyType_Configs(),
        this.getConfigsType(),
        null,
        "configs",
        null,
        0,
        1,
        CompositeBodyType.class,
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
        compositeDefinitionTypeEClass,
        CompositeDefinitionType.class,
        "CompositeDefinitionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeDefinitionType_CompositeHead(),
        this.getCompositeHeadType(),
        null,
        "compositeHead",
        null,
        1,
        1,
        CompositeDefinitionType.class,
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
        getCompositeDefinitionType_CompositeBody(),
        this.getCompositeBodyType(),
        null,
        "compositeBody",
        null,
        1,
        1,
        CompositeDefinitionType.class,
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
        compositeGraphTypeEClass,
        CompositeGraphType.class,
        "CompositeGraphType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeGraphType_OperatorInvocation(),
        this.getOperatorInvocationType(),
        null,
        "operatorInvocation",
        null,
        1,
        -1,
        CompositeGraphType.class,
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
        compositeHeadTypeEClass,
        CompositeHeadType.class,
        "CompositeHeadType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeHeadType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        CompositeHeadType.class,
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
        getCompositeHeadType_Modifiers(),
        this.getCompositeModifiersType(),
        null,
        "modifiers",
        null,
        0,
        1,
        CompositeHeadType.class,
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
        getCompositeHeadType_Inputs(),
        this.getCompositeInputsType(),
        null,
        "inputs",
        null,
        0,
        1,
        CompositeHeadType.class,
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
        getCompositeHeadType_Outputs(),
        this.getCompositeOutputsType(),
        null,
        "outputs",
        null,
        0,
        1,
        CompositeHeadType.class,
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
        getCompositeHeadType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        CompositeHeadType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeInputsTypeEClass,
        CompositeInputsType.class,
        "CompositeInputsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeInputsType_Iport(),
        this.getCompositePortType(),
        null,
        "iport",
        null,
        0,
        -1,
        CompositeInputsType.class,
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
        compositeModifiersTypeEClass,
        CompositeModifiersType.class,
        "CompositeModifiersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeModifiersType_Modifier(),
        this.getCompositeModifierType(),
        null,
        "modifier",
        null,
        1,
        -1,
        CompositeModifiersType.class,
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
        compositeModifierTypeEClass,
        CompositeModifierType.class,
        "CompositeModifierType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositeModifierType_Name(),
        this.getCompositeModifierEnumType(),
        "name",
        null,
        1,
        1,
        CompositeModifierType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeOutputsTypeEClass,
        CompositeOutputsType.class,
        "CompositeOutputsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeOutputsType_Oport(),
        this.getCompositePortType(),
        null,
        "oport",
        null,
        0,
        -1,
        CompositeOutputsType.class,
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
        compositeParameterExpressionModeTypeEClass,
        CompositeParameterExpressionModeType.class,
        "CompositeParameterExpressionModeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositeParameterExpressionModeType_Mode(),
        this.getCompositeParameterExpressionModeEnumType(),
        "mode",
        null,
        1,
        1,
        CompositeParameterExpressionModeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeParameterExpressionModeType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        0,
        1,
        CompositeParameterExpressionModeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeParametersTypeEClass,
        CompositeParametersType.class,
        "CompositeParametersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeParametersType_Parameter(),
        this.getCompositeParameterType(),
        null,
        "parameter",
        null,
        1,
        -1,
        CompositeParametersType.class,
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
        compositeParameterTypeEClass,
        CompositeParameterType.class,
        "CompositeParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeParameterType_ExpressionMode(),
        this.getCompositeParameterExpressionModeType(),
        null,
        "expressionMode",
        null,
        1,
        1,
        CompositeParameterType.class,
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
        getCompositeParameterType_DefaultValue(),
        theXMLTypePackage.getString(),
        "defaultValue",
        null,
        0,
        1,
        CompositeParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeParameterType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        CompositeParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositePortTypeEClass,
        CompositePortType.class,
        "CompositePortType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositePortType_Index(),
        theXMLTypePackage.getUnsignedLong(),
        "index",
        null,
        1,
        1,
        CompositePortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositePortType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        CompositePortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositePortType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        0,
        1,
        CompositePortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeTypesTypeEClass,
        CompositeTypesType.class,
        "CompositeTypesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCompositeTypesType_Type(),
        this.getTypeDefinitionType(),
        null,
        "type",
        null,
        1,
        -1,
        CompositeTypesType.class,
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
        configOptionTypeEClass,
        ConfigOptionType.class,
        "ConfigOptionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getConfigOptionType_Parameter(),
        this.getConfigValueParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        ConfigOptionType.class,
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
        getConfigOptionType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        1,
        1,
        ConfigOptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        configsTypeEClass,
        ConfigsType.class,
        "ConfigsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getConfigsType_Config(),
        this.getConfigType(),
        null,
        "config",
        null,
        1,
        -1,
        ConfigsType.class,
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
        configTypeEClass,
        ConfigType.class,
        "ConfigType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getConfigType_Option(),
        this.getConfigOptionType(),
        null,
        "option",
        null,
        1,
        -1,
        ConfigType.class,
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
        getConfigType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        ConfigType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        configValueParameterTypeEClass,
        ConfigValueParameterType.class,
        "ConfigValueParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getConfigValueParameterType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        1,
        1,
        ConfigValueParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        definitionsTypeEClass,
        DefinitionsType.class,
        "DefinitionsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getDefinitionsType_TypeDefinition(),
        this.getTypeDefinitionType(),
        null,
        "typeDefinition",
        null,
        0,
        -1,
        DefinitionsType.class,
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
        getDefinitionsType_FunctionDefinition(),
        this.getFunctionDefinitionType(),
        null,
        "functionDefinition",
        null,
        0,
        -1,
        DefinitionsType.class,
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
        getDefinitionsType_CompositeDefinition(),
        this.getCompositeDefinitionType(),
        null,
        "compositeDefinition",
        null,
        0,
        -1,
        DefinitionsType.class,
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
        getDocumentRoot_SourceModel(),
        this.getSourceModelType(),
        null,
        "sourceModel",
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
        expressionTypeEClass,
        ExpressionType.class,
        "ExpressionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getExpressionType_Expr(),
        theXMLTypePackage.getString(),
        "expr",
        null,
        1,
        1,
        ExpressionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        extendedSourceLocationTypeEClass,
        ExtendedSourceLocationType.class,
        "ExtendedSourceLocationType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getExtendedSourceLocationType_EndColumn(),
        theXMLTypePackage.getInteger(),
        "endColumn",
        null,
        1,
        1,
        ExtendedSourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getExtendedSourceLocationType_EndLine(),
        theXMLTypePackage.getInteger(),
        "endLine",
        null,
        1,
        1,
        ExtendedSourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getExtendedSourceLocationType_StartColumn(),
        theXMLTypePackage.getInteger(),
        "startColumn",
        null,
        1,
        1,
        ExtendedSourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getExtendedSourceLocationType_StartLine(),
        theXMLTypePackage.getInteger(),
        "startLine",
        null,
        1,
        1,
        ExtendedSourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        functionDefinitionTypeEClass,
        FunctionDefinitionType.class,
        "FunctionDefinitionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionDefinitionType_FunctionHead(),
        this.getFunctionHeadType(),
        null,
        "functionHead",
        null,
        1,
        1,
        FunctionDefinitionType.class,
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
        getFunctionDefinitionType_FunctionBody(),
        theXMLTypePackage.getString(),
        "functionBody",
        null,
        0,
        1,
        FunctionDefinitionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        functionHeadTypeEClass,
        FunctionHeadType.class,
        "FunctionHeadType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionHeadType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        FunctionHeadType.class,
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
        getFunctionHeadType_Modifiers(),
        this.getFunctionModifiersType(),
        null,
        "modifiers",
        null,
        0,
        1,
        FunctionHeadType.class,
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
        getFunctionHeadType_Parameters(),
        this.getFunctionParametersType(),
        null,
        "parameters",
        null,
        0,
        1,
        FunctionHeadType.class,
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
        getFunctionHeadType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        FunctionHeadType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getFunctionHeadType_ReturnType(),
        theXMLTypePackage.getString(),
        "returnType",
        null,
        1,
        1,
        FunctionHeadType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        functionModifiersTypeEClass,
        FunctionModifiersType.class,
        "FunctionModifiersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionModifiersType_Modifier(),
        this.getFunctionModifierType(),
        null,
        "modifier",
        null,
        0,
        -1,
        FunctionModifiersType.class,
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
        functionModifierTypeEClass,
        FunctionModifierType.class,
        "FunctionModifierType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getFunctionModifierType_Name(),
        this.getFunctionModifierEnumType(),
        "name",
        null,
        1,
        1,
        FunctionModifierType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        functionParameterModifiersTypeEClass,
        FunctionParameterModifiersType.class,
        "FunctionParameterModifiersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionParameterModifiersType_Modifier(),
        this.getFunctionParameterModifierType(),
        null,
        "modifier",
        null,
        0,
        -1,
        FunctionParameterModifiersType.class,
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
        functionParameterModifierTypeEClass,
        FunctionParameterModifierType.class,
        "FunctionParameterModifierType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getFunctionParameterModifierType_Name(),
        this.getFunctionParameterModifierEnumType(),
        "name",
        null,
        1,
        1,
        FunctionParameterModifierType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        functionParametersTypeEClass,
        FunctionParametersType.class,
        "FunctionParametersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionParametersType_Parameter(),
        this.getFunctionParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        FunctionParametersType.class,
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
        functionParameterTypeEClass,
        FunctionParameterType.class,
        "FunctionParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionParameterType_Modifiers(),
        this.getFunctionParameterModifiersType(),
        null,
        "modifiers",
        null,
        0,
        1,
        FunctionParameterType.class,
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
        getFunctionParameterType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        FunctionParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getFunctionParameterType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        1,
        1,
        FunctionParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        onProcessTypeEClass,
        OnProcessType.class,
        "OnProcessType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);

    initEClass(
        onPunctTypeEClass,
        OnPunctType.class,
        "OnPunctType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOnPunctType_PortName(),
        theXMLTypePackage.getString(),
        "portName",
        null,
        1,
        1,
        OnPunctType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        onTupleTypeEClass,
        OnTupleType.class,
        "OnTupleType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOnTupleType_PortName(),
        theXMLTypePackage.getString(),
        "portName",
        null,
        1,
        1,
        OnTupleType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationAttributeAssignmentTypeEClass,
        OperatorInvocationAttributeAssignmentType.class,
        "OperatorInvocationAttributeAssignmentType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationAttributeAssignmentType_Value(),
        this.getExpressionType(),
        null,
        "value",
        null,
        1,
        -1,
        OperatorInvocationAttributeAssignmentType.class,
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
        getOperatorInvocationAttributeAssignmentType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        OperatorInvocationAttributeAssignmentType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationAttributeAssignmentType_OutputFunction(),
        theXMLTypePackage.getString(),
        "outputFunction",
        null,
        0,
        1,
        OperatorInvocationAttributeAssignmentType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationBodyTypeEClass,
        OperatorInvocationBodyType.class,
        "OperatorInvocationBodyType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationBodyType_Logic(),
        this.getOperatorInvocationLogicType(),
        null,
        "logic",
        null,
        0,
        1,
        OperatorInvocationBodyType.class,
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
        getOperatorInvocationBodyType_Windows(),
        this.getOperatorInvocationWindowsType(),
        null,
        "windows",
        null,
        0,
        1,
        OperatorInvocationBodyType.class,
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
        getOperatorInvocationBodyType_Parameters(),
        this.getOperatorInvocationParametersType(),
        null,
        "parameters",
        null,
        0,
        1,
        OperatorInvocationBodyType.class,
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
        getOperatorInvocationBodyType_OutputAssignments(),
        this.getOperatorInvocationOutputAssignmentsType(),
        null,
        "outputAssignments",
        null,
        0,
        1,
        OperatorInvocationBodyType.class,
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
        getOperatorInvocationBodyType_Configs(),
        this.getConfigsType(),
        null,
        "configs",
        null,
        0,
        1,
        OperatorInvocationBodyType.class,
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
        operatorInvocationHeadTypeEClass,
        OperatorInvocationHeadType.class,
        "OperatorInvocationHeadType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationHeadType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        OperatorInvocationHeadType.class,
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
        getOperatorInvocationHeadType_Outputs(),
        this.getOperatorInvocationOutputsType(),
        null,
        "outputs",
        null,
        0,
        1,
        OperatorInvocationHeadType.class,
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
        getOperatorInvocationHeadType_Inputs(),
        this.getOperatorInvocationInputsType(),
        null,
        "inputs",
        null,
        0,
        1,
        OperatorInvocationHeadType.class,
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
        getOperatorInvocationHeadType_InvocationAlias(),
        theXMLTypePackage.getString(),
        "invocationAlias",
        null,
        0,
        1,
        OperatorInvocationHeadType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationHeadType_OperatorName(),
        theXMLTypePackage.getString(),
        "operatorName",
        null,
        1,
        1,
        OperatorInvocationHeadType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationInputStreamTypeEClass,
        OperatorInvocationInputStreamType.class,
        "OperatorInvocationInputStreamType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOperatorInvocationInputStreamType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        0,
        1,
        OperatorInvocationInputStreamType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationInputsTypeEClass,
        OperatorInvocationInputsType.class,
        "OperatorInvocationInputsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationInputsType_Input(),
        this.getOperatorInvocationInputType(),
        null,
        "input",
        null,
        1,
        -1,
        OperatorInvocationInputsType.class,
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
        operatorInvocationInputTypeEClass,
        OperatorInvocationInputType.class,
        "OperatorInvocationInputType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationInputType_Istream(),
        this.getOperatorInvocationInputStreamType(),
        null,
        "istream",
        null,
        1,
        -1,
        OperatorInvocationInputType.class,
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
        getOperatorInvocationInputType_Alias(),
        theXMLTypePackage.getString(),
        "alias",
        null,
        0,
        1,
        OperatorInvocationInputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationInputType_Index(),
        theXMLTypePackage.getUnsignedLong(),
        "index",
        null,
        1,
        1,
        OperatorInvocationInputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationLogicTypeEClass,
        OperatorInvocationLogicType.class,
        "OperatorInvocationLogicType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationLogicType_OnProcess(),
        this.getOnProcessType(),
        null,
        "onProcess",
        null,
        0,
        1,
        OperatorInvocationLogicType.class,
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
        getOperatorInvocationLogicType_OnTuple(),
        this.getOnTupleType(),
        null,
        "onTuple",
        null,
        0,
        -1,
        OperatorInvocationLogicType.class,
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
        getOperatorInvocationLogicType_OnPunct(),
        this.getOnPunctType(),
        null,
        "onPunct",
        null,
        0,
        -1,
        OperatorInvocationLogicType.class,
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
        getOperatorInvocationLogicType_HasState(),
        theXMLTypePackage.getBoolean(),
        "hasState",
        null,
        1,
        1,
        OperatorInvocationLogicType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationOutputAssignmentsTypeEClass,
        OperatorInvocationOutputAssignmentsType.class,
        "OperatorInvocationOutputAssignmentsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationOutputAssignmentsType_OutputAssignment(),
        this.getOperatorInvocationOutputAssignmentType(),
        null,
        "outputAssignment",
        null,
        0,
        -1,
        OperatorInvocationOutputAssignmentsType.class,
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
        operatorInvocationOutputAssignmentTypeEClass,
        OperatorInvocationOutputAssignmentType.class,
        "OperatorInvocationOutputAssignmentType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationOutputAssignmentType_AttributeAssignment(),
        this.getOperatorInvocationAttributeAssignmentType(),
        null,
        "attributeAssignment",
        null,
        1,
        -1,
        OperatorInvocationOutputAssignmentType.class,
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
        getOperatorInvocationOutputAssignmentType_PortName(),
        theXMLTypePackage.getString(),
        "portName",
        null,
        1,
        1,
        OperatorInvocationOutputAssignmentType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationOutputsTypeEClass,
        OperatorInvocationOutputsType.class,
        "OperatorInvocationOutputsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationOutputsType_Output(),
        this.getOperatorInvocationOutputType(),
        null,
        "output",
        null,
        1,
        -1,
        OperatorInvocationOutputsType.class,
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
        operatorInvocationOutputTypeEClass,
        OperatorInvocationOutputType.class,
        "OperatorInvocationOutputType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOperatorInvocationOutputType_Alias(),
        theXMLTypePackage.getString(),
        "alias",
        null,
        0,
        1,
        OperatorInvocationOutputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationOutputType_Index(),
        theXMLTypePackage.getUnsignedLong(),
        "index",
        null,
        1,
        1,
        OperatorInvocationOutputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationOutputType_StreamName(),
        theXMLTypePackage.getString(),
        "streamName",
        null,
        1,
        1,
        OperatorInvocationOutputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationOutputType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        1,
        1,
        OperatorInvocationOutputType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationParametersTypeEClass,
        OperatorInvocationParametersType.class,
        "OperatorInvocationParametersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationParametersType_Parameter(),
        this.getOperatorInvocationParameterType(),
        null,
        "parameter",
        null,
        1,
        -1,
        OperatorInvocationParametersType.class,
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
        operatorInvocationParameterTypeEClass,
        OperatorInvocationParameterType.class,
        "OperatorInvocationParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationParameterType_Value(),
        this.getExpressionType(),
        null,
        "value",
        null,
        1,
        -1,
        OperatorInvocationParameterType.class,
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
        getOperatorInvocationParameterType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        OperatorInvocationParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        operatorInvocationTypeEClass,
        OperatorInvocationType.class,
        "OperatorInvocationType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationType_OperatorInvocationHead(),
        this.getOperatorInvocationHeadType(),
        null,
        "operatorInvocationHead",
        null,
        1,
        1,
        OperatorInvocationType.class,
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
        getOperatorInvocationType_OperatorInvocationBody(),
        this.getOperatorInvocationBodyType(),
        null,
        "operatorInvocationBody",
        null,
        1,
        1,
        OperatorInvocationType.class,
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
        operatorInvocationWindowsTypeEClass,
        OperatorInvocationWindowsType.class,
        "OperatorInvocationWindowsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationWindowsType_Window(),
        this.getOperatorInvocationWindowType(),
        null,
        "window",
        null,
        1,
        -1,
        OperatorInvocationWindowsType.class,
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
        operatorInvocationWindowTypeEClass,
        OperatorInvocationWindowType.class,
        "OperatorInvocationWindowType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOperatorInvocationWindowType_EvictionPolicy(),
        this.getWindowPolicyType(),
        null,
        "evictionPolicy",
        null,
        1,
        1,
        OperatorInvocationWindowType.class,
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
        getOperatorInvocationWindowType_TriggerPolicy(),
        this.getWindowPolicyType(),
        null,
        "triggerPolicy",
        null,
        0,
        1,
        OperatorInvocationWindowType.class,
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
        getOperatorInvocationWindowType_Partitioned(),
        theXMLTypePackage.getBoolean(),
        "partitioned",
        null,
        1,
        1,
        OperatorInvocationWindowType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationWindowType_PortName(),
        theXMLTypePackage.getString(),
        "portName",
        null,
        1,
        1,
        OperatorInvocationWindowType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOperatorInvocationWindowType_WindowType(),
        this.getWindowTypeEnumType(),
        "windowType",
        null,
        1,
        1,
        OperatorInvocationWindowType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        sourceFileTypeEClass,
        SourceFileType.class,
        "SourceFileType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getSourceFileType_CompilationUnit(),
        this.getCompilationUnitType(),
        null,
        "compilationUnit",
        null,
        1,
        1,
        SourceFileType.class,
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
        getSourceFileType_Uri(),
        theXMLTypePackage.getString(),
        "uri",
        null,
        1,
        1,
        SourceFileType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        sourceLocationTypeEClass,
        SourceLocationType.class,
        "SourceLocationType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getSourceLocationType_Column(),
        theXMLTypePackage.getInteger(),
        "column",
        null,
        1,
        1,
        SourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getSourceLocationType_Line(),
        theXMLTypePackage.getInteger(),
        "line",
        null,
        1,
        1,
        SourceLocationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        sourceModelTypeEClass,
        SourceModelType.class,
        "SourceModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getSourceModelType_SourceFile(),
        this.getSourceFileType(),
        null,
        "sourceFile",
        null,
        0,
        -1,
        SourceModelType.class,
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
        splDocAnnotationTypeEClass,
        SplDocAnnotationType.class,
        "SplDocAnnotationType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getSplDocAnnotationType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        0,
        1,
        SplDocAnnotationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getSplDocAnnotationType_Target(),
        theXMLTypePackage.getString(),
        "target",
        null,
        0,
        1,
        SplDocAnnotationType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        splDocDescriptionTypeEClass,
        SplDocDescriptionType.class,
        "SplDocDescriptionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getSplDocDescriptionType_Description(),
        theXMLTypePackage.getString(),
        "description",
        null,
        0,
        1,
        SplDocDescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        splDocTypeEClass,
        SplDocType.class,
        "SplDocType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getSplDocType_Description(),
        this.getSplDocDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        SplDocType.class,
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
        getSplDocType_Annotation(),
        this.getSplDocAnnotationType(),
        null,
        "annotation",
        null,
        0,
        -1,
        SplDocType.class,
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
        splNamespaceTypeEClass,
        SplNamespaceType.class,
        "SplNamespaceType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getSplNamespaceType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        SplNamespaceType.class,
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
        getSplNamespaceType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        SplNamespaceType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        typeDefinitionTypeEClass,
        TypeDefinitionType.class,
        "TypeDefinitionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getTypeDefinitionType_SplDoc(),
        this.getSplDocType(),
        null,
        "splDoc",
        null,
        0,
        1,
        TypeDefinitionType.class,
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
        getTypeDefinitionType_Modifiers(),
        this.getTypeModifiersType(),
        null,
        "modifiers",
        null,
        0,
        1,
        TypeDefinitionType.class,
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
        getTypeDefinitionType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        TypeDefinitionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getTypeDefinitionType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        1,
        1,
        TypeDefinitionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        typeModifiersTypeEClass,
        TypeModifiersType.class,
        "TypeModifiersType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getTypeModifiersType_Modifier(),
        this.getTypeModifierType(),
        null,
        "modifier",
        null,
        0,
        -1,
        TypeModifiersType.class,
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
        typeModifierTypeEClass,
        TypeModifierType.class,
        "TypeModifierType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getTypeModifierType_Name(),
        this.getTypeModifierEnumType(),
        "name",
        null,
        1,
        1,
        TypeModifierType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        useDirectivesTypeEClass,
        UseDirectivesType.class,
        "UseDirectivesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getUseDirectivesType_UseDirective(),
        this.getUseDirectiveType(),
        null,
        "useDirective",
        null,
        1,
        -1,
        UseDirectivesType.class,
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
        useDirectiveTypeEClass,
        UseDirectiveType.class,
        "UseDirectiveType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getUseDirectiveType_NamespaceName(),
        theXMLTypePackage.getString(),
        "namespaceName",
        null,
        1,
        1,
        UseDirectiveType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getUseDirectiveType_Tail(),
        theXMLTypePackage.getString(),
        "tail",
        null,
        1,
        1,
        UseDirectiveType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        windowPolicyTypeEClass,
        WindowPolicyType.class,
        "WindowPolicyType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getWindowPolicyType_Attribute(),
        theXMLTypePackage.getString(),
        "attribute",
        null,
        0,
        1,
        WindowPolicyType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getWindowPolicyType_Kind(),
        this.getWindowPolicyKindEnumType(),
        "kind",
        null,
        1,
        1,
        WindowPolicyType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getWindowPolicyType_Size(),
        theXMLTypePackage.getString(),
        "size",
        null,
        0,
        1,
        WindowPolicyType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    // Initialize enums and add enum literals
    initEEnum(
        compositeModifierEnumTypeEEnum,
        CompositeModifierEnumType.class,
        "CompositeModifierEnumType");
    addEEnumLiteral(compositeModifierEnumTypeEEnum, CompositeModifierEnumType.PUBLIC);

    initEEnum(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.class,
        "CompositeParameterExpressionModeEnumType");
    addEEnumLiteral(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.OPERATOR);
    addEEnumLiteral(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.FUNCTION);
    addEEnumLiteral(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.ATTRIBUTE);
    addEEnumLiteral(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.EXPRESSION);
    addEEnumLiteral(
        compositeParameterExpressionModeEnumTypeEEnum,
        CompositeParameterExpressionModeEnumType.TYPE);

    initEEnum(
        functionModifierEnumTypeEEnum, FunctionModifierEnumType.class, "FunctionModifierEnumType");
    addEEnumLiteral(functionModifierEnumTypeEEnum, FunctionModifierEnumType.PUBLIC);
    addEEnumLiteral(functionModifierEnumTypeEEnum, FunctionModifierEnumType.STATEFUL);

    initEEnum(
        functionParameterModifierEnumTypeEEnum,
        FunctionParameterModifierEnumType.class,
        "FunctionParameterModifierEnumType");
    addEEnumLiteral(
        functionParameterModifierEnumTypeEEnum, FunctionParameterModifierEnumType.MUTABLE);

    initEEnum(typeModifierEnumTypeEEnum, TypeModifierEnumType.class, "TypeModifierEnumType");
    addEEnumLiteral(typeModifierEnumTypeEEnum, TypeModifierEnumType.PUBLIC);
    addEEnumLiteral(typeModifierEnumTypeEEnum, TypeModifierEnumType.STATIC);

    initEEnum(
        windowPolicyKindEnumTypeEEnum, WindowPolicyKindEnumType.class, "WindowPolicyKindEnumType");
    addEEnumLiteral(windowPolicyKindEnumTypeEEnum, WindowPolicyKindEnumType.COUNT);
    addEEnumLiteral(windowPolicyKindEnumTypeEEnum, WindowPolicyKindEnumType.TIME);
    addEEnumLiteral(windowPolicyKindEnumTypeEEnum, WindowPolicyKindEnumType.DELTA);
    addEEnumLiteral(windowPolicyKindEnumTypeEEnum, WindowPolicyKindEnumType.PUNCT);

    initEEnum(windowTypeEnumTypeEEnum, WindowTypeEnumType.class, "WindowTypeEnumType");
    addEEnumLiteral(windowTypeEnumTypeEEnum, WindowTypeEnumType.TUMBLING);
    addEEnumLiteral(windowTypeEnumTypeEEnum, WindowTypeEnumType.SLIDING);

    // Initialize data types
    initEDataType(
        compositeModifierEnumTypeObjectEDataType,
        CompositeModifierEnumType.class,
        "CompositeModifierEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        compositeParameterExpressionModeEnumTypeObjectEDataType,
        CompositeParameterExpressionModeEnumType.class,
        "CompositeParameterExpressionModeEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        functionModifierEnumTypeObjectEDataType,
        FunctionModifierEnumType.class,
        "FunctionModifierEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        functionParameterModifierEnumTypeObjectEDataType,
        FunctionParameterModifierEnumType.class,
        "FunctionParameterModifierEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        typeModifierEnumTypeObjectEDataType,
        TypeModifierEnumType.class,
        "TypeModifierEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowPolicyKindEnumTypeObjectEDataType,
        WindowPolicyKindEnumType.class,
        "WindowPolicyKindEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        windowTypeEnumTypeObjectEDataType,
        WindowTypeEnumType.class,
        "WindowTypeEnumTypeObject",
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
        compilationUnitTypeEClass,
        source,
        new String[] {
          "name", "compilationUnitType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompilationUnitType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompilationUnitType_SplNamespace(),
        source,
        new String[] {
          "kind", "element",
          "name", "splNamespace",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompilationUnitType_UseDirectives(),
        source,
        new String[] {
          "kind", "element",
          "name", "useDirectives",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompilationUnitType_Definitions(),
        source,
        new String[] {
          "kind", "element",
          "name", "definitions",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeBodyTypeEClass,
        source,
        new String[] {
          "name", "compositeBodyType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeBodyType_Types(),
        source,
        new String[] {
          "kind", "element",
          "name", "types",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeBodyType_Parameters(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameters",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeBodyType_Graph(),
        source,
        new String[] {
          "kind", "element",
          "name", "graph",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeBodyType_Configs(),
        source,
        new String[] {
          "kind", "element",
          "name", "configs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeDefinitionTypeEClass,
        source,
        new String[] {
          "name", "compositeDefinitionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeDefinitionType_CompositeHead(),
        source,
        new String[] {
          "kind", "element",
          "name", "compositeHead",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeDefinitionType_CompositeBody(),
        source,
        new String[] {
          "kind", "element",
          "name", "compositeBody",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeGraphTypeEClass,
        source,
        new String[] {
          "name", "compositeGraphType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeGraphType_OperatorInvocation(),
        source,
        new String[] {
          "kind", "element",
          "name", "operatorInvocation",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeHeadTypeEClass,
        source,
        new String[] {
          "name", "compositeHeadType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeHeadType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeHeadType_Modifiers(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifiers",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeHeadType_Inputs(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeHeadType_Outputs(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeHeadType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        compositeInputsTypeEClass,
        source,
        new String[] {
          "name", "compositeInputsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeInputsType_Iport(),
        source,
        new String[] {
          "kind", "element",
          "name", "iport",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeModifierEnumTypeEEnum, source, new String[] {"name", "compositeModifierEnumType"});
    addAnnotation(
        compositeModifierEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "compositeModifierEnumType:Object",
          "baseType", "compositeModifierEnumType"
        });
    addAnnotation(
        compositeModifiersTypeEClass,
        source,
        new String[] {
          "name", "compositeModifiersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeModifiersType_Modifier(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifier",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeModifierTypeEClass,
        source,
        new String[] {
          "name", "compositeModifierType",
          "kind", "empty"
        });
    addAnnotation(
        getCompositeModifierType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        compositeOutputsTypeEClass,
        source,
        new String[] {
          "name", "compositeOutputsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeOutputsType_Oport(),
        source,
        new String[] {
          "kind", "element",
          "name", "oport",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeParameterExpressionModeEnumTypeEEnum,
        source,
        new String[] {"name", "compositeParameterExpressionModeEnumType"});
    addAnnotation(
        compositeParameterExpressionModeEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "compositeParameterExpressionModeEnumType:Object",
          "baseType", "compositeParameterExpressionModeEnumType"
        });
    addAnnotation(
        compositeParameterExpressionModeTypeEClass,
        source,
        new String[] {
          "name", "compositeParameterExpressionModeType",
          "kind", "empty"
        });
    addAnnotation(
        getCompositeParameterExpressionModeType_Mode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "mode"
        });
    addAnnotation(
        getCompositeParameterExpressionModeType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        compositeParametersTypeEClass,
        source,
        new String[] {
          "name", "compositeParametersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeParametersType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        compositeParameterTypeEClass,
        source,
        new String[] {
          "name", "compositeParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeParameterType_ExpressionMode(),
        source,
        new String[] {
          "kind", "element",
          "name", "expressionMode",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getCompositeParameterType_DefaultValue(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "defaultValue"
        });
    addAnnotation(
        getCompositeParameterType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        compositePortTypeEClass,
        source,
        new String[] {
          "name", "compositePortType",
          "kind", "empty"
        });
    addAnnotation(
        getCompositePortType_Index(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "index"
        });
    addAnnotation(
        getCompositePortType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getCompositePortType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        compositeTypesTypeEClass,
        source,
        new String[] {
          "name", "compositeTypesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeTypesType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        configOptionTypeEClass,
        source,
        new String[] {
          "name", "configOptionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getConfigOptionType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getConfigOptionType_Value(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "value"
        });
    addAnnotation(
        configsTypeEClass,
        source,
        new String[] {
          "name", "configsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getConfigsType_Config(),
        source,
        new String[] {
          "kind", "element",
          "name", "config",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        configTypeEClass,
        source,
        new String[] {
          "name", "configType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getConfigType_Option(),
        source,
        new String[] {
          "kind", "element",
          "name", "option",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getConfigType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        configValueParameterTypeEClass,
        source,
        new String[] {
          "name", "configValueParameterType",
          "kind", "empty"
        });
    addAnnotation(
        getConfigValueParameterType_Value(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "value"
        });
    addAnnotation(
        definitionsTypeEClass,
        source,
        new String[] {
          "name", "definitionsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getDefinitionsType_TypeDefinition(),
        source,
        new String[] {
          "kind", "element",
          "name", "typeDefinition",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getDefinitionsType_FunctionDefinition(),
        source,
        new String[] {
          "kind", "element",
          "name", "functionDefinition",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getDefinitionsType_CompositeDefinition(),
        source,
        new String[] {
          "kind", "element",
          "name", "compositeDefinition",
          "namespace", "##targetNamespace"
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
        getDocumentRoot_SourceModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "sourceModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        expressionTypeEClass,
        source,
        new String[] {
          "name", "expressionType",
          "kind", "empty"
        });
    addAnnotation(
        getExpressionType_Expr(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "expr"
        });
    addAnnotation(
        extendedSourceLocationTypeEClass,
        source,
        new String[] {
          "name", "extendedSourceLocationType",
          "kind", "empty"
        });
    addAnnotation(
        getExtendedSourceLocationType_EndColumn(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "endColumn"
        });
    addAnnotation(
        getExtendedSourceLocationType_EndLine(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "endLine"
        });
    addAnnotation(
        getExtendedSourceLocationType_StartColumn(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "startColumn"
        });
    addAnnotation(
        getExtendedSourceLocationType_StartLine(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "startLine"
        });
    addAnnotation(
        functionDefinitionTypeEClass,
        source,
        new String[] {
          "name", "functionDefinitionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionDefinitionType_FunctionHead(),
        source,
        new String[] {
          "kind", "element",
          "name", "functionHead",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionDefinitionType_FunctionBody(),
        source,
        new String[] {
          "kind", "element",
          "name", "functionBody",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionHeadTypeEClass,
        source,
        new String[] {
          "name", "functionHeadType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionHeadType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionHeadType_Modifiers(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifiers",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionHeadType_Parameters(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameters",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionHeadType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getFunctionHeadType_ReturnType(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "returnType"
        });
    addAnnotation(
        functionModifierEnumTypeEEnum, source, new String[] {"name", "functionModifierEnumType"});
    addAnnotation(
        functionModifierEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "functionModifierEnumType:Object",
          "baseType", "functionModifierEnumType"
        });
    addAnnotation(
        functionModifiersTypeEClass,
        source,
        new String[] {
          "name", "functionModifiersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionModifiersType_Modifier(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifier",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionModifierTypeEClass,
        source,
        new String[] {
          "name", "functionModifierType",
          "kind", "empty"
        });
    addAnnotation(
        getFunctionModifierType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        functionParameterModifierEnumTypeEEnum,
        source,
        new String[] {"name", "functionParameterModifierEnumType"});
    addAnnotation(
        functionParameterModifierEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "functionParameterModifierEnumType:Object",
          "baseType", "functionParameterModifierEnumType"
        });
    addAnnotation(
        functionParameterModifiersTypeEClass,
        source,
        new String[] {
          "name", "functionParameterModifiersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionParameterModifiersType_Modifier(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifier",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionParameterModifierTypeEClass,
        source,
        new String[] {
          "name", "functionParameterModifierType",
          "kind", "empty"
        });
    addAnnotation(
        getFunctionParameterModifierType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        functionParametersTypeEClass,
        source,
        new String[] {
          "name", "functionParametersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionParametersType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionParameterTypeEClass,
        source,
        new String[] {
          "name", "functionParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionParameterType_Modifiers(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifiers",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionParameterType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getFunctionParameterType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        onProcessTypeEClass,
        source,
        new String[] {
          "name", "onProcessType",
          "kind", "empty"
        });
    addAnnotation(
        onPunctTypeEClass,
        source,
        new String[] {
          "name", "onPunctType",
          "kind", "empty"
        });
    addAnnotation(
        getOnPunctType_PortName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "portName"
        });
    addAnnotation(
        onTupleTypeEClass,
        source,
        new String[] {
          "name", "onTupleType",
          "kind", "empty"
        });
    addAnnotation(
        getOnTupleType_PortName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "portName"
        });
    addAnnotation(
        operatorInvocationAttributeAssignmentTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationAttributeAssignmentType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationAttributeAssignmentType_Value(),
        source,
        new String[] {
          "kind", "element",
          "name", "value",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationAttributeAssignmentType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getOperatorInvocationAttributeAssignmentType_OutputFunction(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "outputFunction"
        });
    addAnnotation(
        operatorInvocationBodyTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationBodyType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationBodyType_Logic(),
        source,
        new String[] {
          "kind", "element",
          "name", "logic",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationBodyType_Windows(),
        source,
        new String[] {
          "kind", "element",
          "name", "windows",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationBodyType_Parameters(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameters",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationBodyType_OutputAssignments(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputAssignments",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationBodyType_Configs(),
        source,
        new String[] {
          "kind", "element",
          "name", "configs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationHeadTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationHeadType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationHeadType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationHeadType_Outputs(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationHeadType_Inputs(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputs",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationHeadType_InvocationAlias(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "invocationAlias"
        });
    addAnnotation(
        getOperatorInvocationHeadType_OperatorName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "operatorName"
        });
    addAnnotation(
        operatorInvocationInputStreamTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationInputStreamType",
          "kind", "empty"
        });
    addAnnotation(
        getOperatorInvocationInputStreamType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        operatorInvocationInputsTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationInputsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationInputsType_Input(),
        source,
        new String[] {
          "kind", "element",
          "name", "input",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationInputTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationInputType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationInputType_Istream(),
        source,
        new String[] {
          "kind", "element",
          "name", "istream",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationInputType_Alias(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "alias"
        });
    addAnnotation(
        getOperatorInvocationInputType_Index(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "index"
        });
    addAnnotation(
        operatorInvocationLogicTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationLogicType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationLogicType_OnProcess(),
        source,
        new String[] {
          "kind", "element",
          "name", "onProcess",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationLogicType_OnTuple(),
        source,
        new String[] {
          "kind", "element",
          "name", "onTuple",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationLogicType_OnPunct(),
        source,
        new String[] {
          "kind", "element",
          "name", "onPunct",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationLogicType_HasState(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "hasState"
        });
    addAnnotation(
        operatorInvocationOutputAssignmentsTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationOutputAssignmentsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationOutputAssignmentsType_OutputAssignment(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputAssignment",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationOutputAssignmentTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationOutputAssignmentType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationOutputAssignmentType_AttributeAssignment(),
        source,
        new String[] {
          "kind", "element",
          "name", "attributeAssignment",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationOutputAssignmentType_PortName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "portName"
        });
    addAnnotation(
        operatorInvocationOutputsTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationOutputsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationOutputsType_Output(),
        source,
        new String[] {
          "kind", "element",
          "name", "output",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationOutputTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationOutputType",
          "kind", "empty"
        });
    addAnnotation(
        getOperatorInvocationOutputType_Alias(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "alias"
        });
    addAnnotation(
        getOperatorInvocationOutputType_Index(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "index"
        });
    addAnnotation(
        getOperatorInvocationOutputType_StreamName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "streamName"
        });
    addAnnotation(
        getOperatorInvocationOutputType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        operatorInvocationParametersTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationParametersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationParametersType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationParameterTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationParameterType_Value(),
        source,
        new String[] {
          "kind", "element",
          "name", "value",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationParameterType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        operatorInvocationTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationType_OperatorInvocationHead(),
        source,
        new String[] {
          "kind", "element",
          "name", "operatorInvocationHead",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationType_OperatorInvocationBody(),
        source,
        new String[] {
          "kind", "element",
          "name", "operatorInvocationBody",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationWindowsTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationWindowsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationWindowsType_Window(),
        source,
        new String[] {
          "kind", "element",
          "name", "window",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        operatorInvocationWindowTypeEClass,
        source,
        new String[] {
          "name", "operatorInvocationWindowType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOperatorInvocationWindowType_EvictionPolicy(),
        source,
        new String[] {
          "kind", "element",
          "name", "evictionPolicy",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationWindowType_TriggerPolicy(),
        source,
        new String[] {
          "kind", "element",
          "name", "triggerPolicy",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getOperatorInvocationWindowType_Partitioned(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "partitioned"
        });
    addAnnotation(
        getOperatorInvocationWindowType_PortName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "portName"
        });
    addAnnotation(
        getOperatorInvocationWindowType_WindowType(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "windowType"
        });
    addAnnotation(
        sourceFileTypeEClass,
        source,
        new String[] {
          "name", "sourceFileType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSourceFileType_CompilationUnit(),
        source,
        new String[] {
          "kind", "element",
          "name", "compilationUnit",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getSourceFileType_Uri(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uri"
        });
    addAnnotation(
        sourceLocationTypeEClass,
        source,
        new String[] {
          "name", "sourceLocationType",
          "kind", "empty"
        });
    addAnnotation(
        getSourceLocationType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getSourceLocationType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
    addAnnotation(
        sourceModelTypeEClass,
        source,
        new String[] {
          "name", "sourceModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSourceModelType_SourceFile(),
        source,
        new String[] {
          "kind", "element",
          "name", "sourceFile",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        splDocAnnotationTypeEClass,
        source,
        new String[] {
          "name", "splDocAnnotationType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSplDocAnnotationType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getSplDocAnnotationType_Target(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "target"
        });
    addAnnotation(
        splDocDescriptionTypeEClass,
        source,
        new String[] {
          "name", "splDocDescriptionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSplDocDescriptionType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        splDocTypeEClass,
        source,
        new String[] {
          "name", "splDocType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSplDocType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getSplDocType_Annotation(),
        source,
        new String[] {
          "kind", "element",
          "name", "annotation",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        splNamespaceTypeEClass,
        source,
        new String[] {
          "name", "splNamespaceType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSplNamespaceType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getSplNamespaceType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        typeDefinitionTypeEClass,
        source,
        new String[] {
          "name", "typeDefinitionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getTypeDefinitionType_SplDoc(),
        source,
        new String[] {
          "kind", "element",
          "name", "splDoc",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getTypeDefinitionType_Modifiers(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifiers",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getTypeDefinitionType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getTypeDefinitionType_Value(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "value"
        });
    addAnnotation(typeModifierEnumTypeEEnum, source, new String[] {"name", "typeModifierEnumType"});
    addAnnotation(
        typeModifierEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "typeModifierEnumType:Object",
          "baseType", "typeModifierEnumType"
        });
    addAnnotation(
        typeModifiersTypeEClass,
        source,
        new String[] {
          "name", "typeModifiersType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getTypeModifiersType_Modifier(),
        source,
        new String[] {
          "kind", "element",
          "name", "modifier",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        typeModifierTypeEClass,
        source,
        new String[] {
          "name", "typeModifierType",
          "kind", "empty"
        });
    addAnnotation(
        getTypeModifierType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        useDirectivesTypeEClass,
        source,
        new String[] {
          "name", "useDirectivesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getUseDirectivesType_UseDirective(),
        source,
        new String[] {
          "kind", "element",
          "name", "useDirective",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        useDirectiveTypeEClass,
        source,
        new String[] {
          "name", "useDirectiveType",
          "kind", "empty"
        });
    addAnnotation(
        getUseDirectiveType_NamespaceName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "namespaceName"
        });
    addAnnotation(
        getUseDirectiveType_Tail(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "tail"
        });
    addAnnotation(
        windowPolicyKindEnumTypeEEnum, source, new String[] {"name", "windowPolicyKindEnumType"});
    addAnnotation(
        windowPolicyKindEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowPolicyKindEnumType:Object",
          "baseType", "windowPolicyKindEnumType"
        });
    addAnnotation(
        windowPolicyTypeEClass,
        source,
        new String[] {
          "name", "windowPolicyType",
          "kind", "empty"
        });
    addAnnotation(
        getWindowPolicyType_Attribute(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "attribute"
        });
    addAnnotation(
        getWindowPolicyType_Kind(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "kind"
        });
    addAnnotation(
        getWindowPolicyType_Size(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "size"
        });
    addAnnotation(windowTypeEnumTypeEEnum, source, new String[] {"name", "windowTypeEnumType"});
    addAnnotation(
        windowTypeEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "windowTypeEnumType:Object",
          "baseType", "windowTypeEnumType"
        });
  }
} // CodePackageImpl
