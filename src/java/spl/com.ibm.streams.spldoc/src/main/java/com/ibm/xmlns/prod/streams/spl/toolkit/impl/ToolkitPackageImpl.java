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

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl;
import com.ibm.xmlns.prod.streams.spl.toolkit.BaseFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.BaseToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CompositeToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralValueType;
import com.ibm.xmlns.prod.streams.spl.toolkit.CustomLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumExpressionModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPolicyType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctInputModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.EnumWindowPunctOutputModeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ImageType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ImagesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.LanguageType;
import com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsType;
import com.ibm.xmlns.prod.streams.spl.toolkit.OutputFunctionsTypeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpInputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpOutputPortType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveOpParameterType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.PrimitiveToolkitOperatorType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitBaseType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitElementType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFactory;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitTypeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.UriTableType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl;
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
public class ToolkitPackageImpl extends EPackageImpl implements ToolkitPackage {
  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass baseFunctionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass baseToolkitOperatorTypeEClass = null;

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
  private EClass compositeOpParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeOpPortTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass compositeToolkitOperatorTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass customLiteralsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass customLiteralValueTypeEClass = null;

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
  private EClass enumLiteralsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass imagesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass imageTypeEClass = null;

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
  private EClass outputFunctionsTypeTypeEClass = null;

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
  private EClass primitiveOpInputPortTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass primitiveOpOutputPortTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass primitiveOpParameterTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass primitivePortDescriptionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass primitiveToolkitOperatorTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitBaseTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitElementTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitFunctionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitNamespaceTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitTypeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass uriTableTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum compositeParamEnumTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum enumExpressionModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum enumWindowPolicyTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum enumWindowPunctInputModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum enumWindowPunctOutputModeTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum languageTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType compositeParamEnumTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType enumExpressionModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType enumWindowPolicyTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType enumWindowPunctInputModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType enumWindowPunctOutputModeTypeObjectEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType languageTypeObjectEDataType = null;

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private ToolkitPackageImpl() {
    super(eNS_URI, ToolkitFactory.eINSTANCE);
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
   * <p>This method is used to initialize {@link ToolkitPackage#eINSTANCE} when that field is
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
  public static ToolkitPackage init() {
    if (isInited)
      return (ToolkitPackage) EPackage.Registry.INSTANCE.getEPackage(ToolkitPackage.eNS_URI);

    // Obtain or create and register package
    ToolkitPackageImpl theToolkitPackage =
        (ToolkitPackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof ToolkitPackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new ToolkitPackageImpl());

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
    InfoPackageImpl theInfoPackage =
        (InfoPackageImpl)
            (EPackage.Registry.INSTANCE.getEPackage(InfoPackage.eNS_URI) instanceof InfoPackageImpl
                ? EPackage.Registry.INSTANCE.getEPackage(InfoPackage.eNS_URI)
                : InfoPackage.eINSTANCE);

    // Create package meta-data objects
    theToolkitPackage.createPackageContents();
    theCommonPackage.createPackageContents();
    theInfoPackage.createPackageContents();

    // Initialize created meta-data
    theToolkitPackage.initializePackageContents();
    theCommonPackage.initializePackageContents();
    theInfoPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theToolkitPackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(ToolkitPackage.eNS_URI, theToolkitPackage);
    return theToolkitPackage;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getBaseFunctionType() {
    return baseFunctionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getBaseFunctionType_Prototype() {
    return (EAttribute) baseFunctionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getBaseFunctionType_Parameter() {
    return (EReference) baseFunctionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getBaseFunctionType_ReturnType() {
    return (EAttribute) baseFunctionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getBaseToolkitOperatorType() {
    return baseToolkitOperatorTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getBaseToolkitOperatorType_Images() {
    return (EReference) baseToolkitOperatorTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getBaseToolkitOperatorType_Public() {
    return (EAttribute) baseToolkitOperatorTypeEClass.getEStructuralFeatures().get(1);
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
  public EAttribute getCodeTemplateType_Template() {
    return (EAttribute) codeTemplateTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeOpParameterType() {
    return compositeOpParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeOpParameterType_MetaType() {
    return (EAttribute) compositeOpParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeOpParameterType_Optional() {
    return (EAttribute) compositeOpParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeOpPortType() {
    return compositeOpPortTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeOpPortType_PortIndex() {
    return (EAttribute) compositeOpPortTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeOpPortType_Type() {
    return (EAttribute) compositeOpPortTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCompositeToolkitOperatorType() {
    return compositeToolkitOperatorTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeToolkitOperatorType_Group() {
    return (EAttribute) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeToolkitOperatorType_Parameter() {
    return (EReference) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeToolkitOperatorType_InputPort() {
    return (EReference) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeToolkitOperatorType_OutputPort() {
    return (EReference) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeToolkitOperatorType_Type() {
    return (EReference) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCompositeToolkitOperatorType_EnumLiterals() {
    return (EReference) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeToolkitOperatorType_Column() {
    return (EAttribute) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeToolkitOperatorType_Line() {
    return (EAttribute) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeToolkitOperatorType_PotentialMain() {
    return (EAttribute) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(8);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getCompositeToolkitOperatorType_UriIndex() {
    return (EAttribute) compositeToolkitOperatorTypeEClass.getEStructuralFeatures().get(9);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCustomLiteralsType() {
    return customLiteralsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getCustomLiteralsType_Value() {
    return (EReference) customLiteralsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getCustomLiteralValueType() {
    return customLiteralValueTypeEClass;
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
  public EReference getDocumentRoot_ToolkitModel() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getEnumLiteralsType() {
    return enumLiteralsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getEnumLiteralsType_Description() {
    return (EReference) enumLiteralsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_Enum() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_Column() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_Line() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_Static() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_TypeName() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getEnumLiteralsType_UriIndex() {
    return (EAttribute) enumLiteralsTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getImagesType() {
    return imagesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getImagesType_Image() {
    return (EReference) imagesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getImageType() {
    return imageTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getImageType_Size() {
    return (EAttribute) imageTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getImageType_UriIndex() {
    return (EAttribute) imageTypeEClass.getEStructuralFeatures().get(1);
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
  public EReference getOutputFunctionsType_Function() {
    return (EReference) outputFunctionsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getOutputFunctionsTypeType() {
    return outputFunctionsTypeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputFunctionsTypeType_DefaultValue() {
    return (EAttribute) outputFunctionsTypeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getOutputFunctionsTypeType_Name() {
    return (EAttribute) outputFunctionsTypeTypeEClass.getEStructuralFeatures().get(1);
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
  public EAttribute getParameterType_DefaultValue() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Mutable() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getParameterType_Type() {
    return (EAttribute) parameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrimitiveOpInputPortType() {
    return primitiveOpInputPortTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpInputPortType_WindowPolicy() {
    return (EAttribute) primitiveOpInputPortTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpInputPortType_WindowPunctInputMode() {
    return (EAttribute) primitiveOpInputPortTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrimitiveOpOutputPortType() {
    return primitiveOpOutputPortTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveOpOutputPortType_OutputFunctionsType() {
    return (EReference) primitiveOpOutputPortTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpOutputPortType_ExpressionMode() {
    return (EAttribute) primitiveOpOutputPortTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpOutputPortType_WindowPunctOutputMode() {
    return (EAttribute) primitiveOpOutputPortTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrimitiveOpParameterType() {
    return primitiveOpParameterTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpParameterType_PortScope() {
    return (EAttribute) primitiveOpParameterTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpParameterType_Cardinality() {
    return (EAttribute) primitiveOpParameterTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpParameterType_ExpressionMode() {
    return (EAttribute) primitiveOpParameterTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveOpParameterType_Optional() {
    return (EAttribute) primitiveOpParameterTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrimitivePortDescriptionType() {
    return primitivePortDescriptionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitivePortDescriptionType_Description() {
    return (EReference) primitivePortDescriptionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitivePortDescriptionType_MaxNumPorts() {
    return (EAttribute) primitivePortDescriptionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitivePortDescriptionType_MinNumPorts() {
    return (EAttribute) primitivePortDescriptionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitivePortDescriptionType_Optional() {
    return (EAttribute) primitivePortDescriptionTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrimitiveToolkitOperatorType() {
    return primitiveToolkitOperatorTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveToolkitOperatorType_Group() {
    return (EAttribute) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_CustomLiterals() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_OutputFunctions() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_ParametersDescription() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_Parameter() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_InputPort() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_OutputPort() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getPrimitiveToolkitOperatorType_CodeTemplate() {
    return (EReference) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveToolkitOperatorType_Invalid() {
    return (EAttribute) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(8);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveToolkitOperatorType_Language() {
    return (EAttribute) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(9);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrimitiveToolkitOperatorType_ModelUriIndex() {
    return (EAttribute) primitiveToolkitOperatorTypeEClass.getEStructuralFeatures().get(10);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitBaseType() {
    return toolkitBaseTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitBaseType_UriTable() {
    return (EReference) toolkitBaseTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitElementType() {
    return toolkitElementTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitElementType_Description() {
    return (EReference) toolkitElementTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitElementType_Name() {
    return (EAttribute) toolkitElementTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitElementType_SampleUriIndex() {
    return (EAttribute) toolkitElementTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitFunctionType() {
    return toolkitFunctionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_Column() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_Line() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_ModelUriIndex() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_Native() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_Public() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_Stateful() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitFunctionType_UriIndex() {
    return (EAttribute) toolkitFunctionTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitModelType() {
    return toolkitModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitModelType_Toolkit() {
    return (EReference) toolkitModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitModelType_ProductVersion() {
    return (EAttribute) toolkitModelTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitNamespaceType() {
    return toolkitNamespaceTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitNamespaceType_Group() {
    return (EAttribute) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitNamespaceType_PrimitiveOp() {
    return (EReference) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitNamespaceType_CompositeOp() {
    return (EReference) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitNamespaceType_Function() {
    return (EReference) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitNamespaceType_Type() {
    return (EReference) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitNamespaceType_EnumLiterals() {
    return (EReference) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitNamespaceType_RelPathToNamespace() {
    return (EAttribute) toolkitNamespaceTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitType() {
    return toolkitTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitType_Group() {
    return (EAttribute) toolkitTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitType_Namespace() {
    return (EReference) toolkitTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitType_Dependency() {
    return (EReference) toolkitTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitType_SabFiles() {
    return (EReference) toolkitTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitType_HasNLResources() {
    return (EAttribute) toolkitTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitType_RequiredProductVersion() {
    return (EAttribute) toolkitTypeEClass.getEStructuralFeatures().get(5);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitType_Uri() {
    return (EAttribute) toolkitTypeEClass.getEStructuralFeatures().get(6);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitType_Version() {
    return (EAttribute) toolkitTypeEClass.getEStructuralFeatures().get(7);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitTypeType() {
    return toolkitTypeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitTypeType_Column() {
    return (EAttribute) toolkitTypeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitTypeType_Line() {
    return (EAttribute) toolkitTypeTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitTypeType_Static() {
    return (EAttribute) toolkitTypeTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitTypeType_Type() {
    return (EAttribute) toolkitTypeTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitTypeType_UriIndex() {
    return (EAttribute) toolkitTypeTypeEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getUriTableType() {
    return uriTableTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getUriTableType_Uri() {
    return (EReference) uriTableTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getCompositeParamEnumType() {
    return compositeParamEnumTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getEnumExpressionModeType() {
    return enumExpressionModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getEnumWindowPolicyType() {
    return enumWindowPolicyTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getEnumWindowPunctInputModeType() {
    return enumWindowPunctInputModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getEnumWindowPunctOutputModeType() {
    return enumWindowPunctOutputModeTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getLanguageType() {
    return languageTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getCompositeParamEnumTypeObject() {
    return compositeParamEnumTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getEnumExpressionModeTypeObject() {
    return enumExpressionModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getEnumWindowPolicyTypeObject() {
    return enumWindowPolicyTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getEnumWindowPunctInputModeTypeObject() {
    return enumWindowPunctInputModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getEnumWindowPunctOutputModeTypeObject() {
    return enumWindowPunctOutputModeTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getLanguageTypeObject() {
    return languageTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitFactory getToolkitFactory() {
    return (ToolkitFactory) getEFactoryInstance();
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
    baseFunctionTypeEClass = createEClass(BASE_FUNCTION_TYPE);
    createEAttribute(baseFunctionTypeEClass, BASE_FUNCTION_TYPE__PROTOTYPE);
    createEReference(baseFunctionTypeEClass, BASE_FUNCTION_TYPE__PARAMETER);
    createEAttribute(baseFunctionTypeEClass, BASE_FUNCTION_TYPE__RETURN_TYPE);

    baseToolkitOperatorTypeEClass = createEClass(BASE_TOOLKIT_OPERATOR_TYPE);
    createEReference(baseToolkitOperatorTypeEClass, BASE_TOOLKIT_OPERATOR_TYPE__IMAGES);
    createEAttribute(baseToolkitOperatorTypeEClass, BASE_TOOLKIT_OPERATOR_TYPE__PUBLIC);

    codeTemplateTypeEClass = createEClass(CODE_TEMPLATE_TYPE);
    createEAttribute(codeTemplateTypeEClass, CODE_TEMPLATE_TYPE__TEMPLATE);

    compositeOpParameterTypeEClass = createEClass(COMPOSITE_OP_PARAMETER_TYPE);
    createEAttribute(compositeOpParameterTypeEClass, COMPOSITE_OP_PARAMETER_TYPE__META_TYPE);
    createEAttribute(compositeOpParameterTypeEClass, COMPOSITE_OP_PARAMETER_TYPE__OPTIONAL);

    compositeOpPortTypeEClass = createEClass(COMPOSITE_OP_PORT_TYPE);
    createEAttribute(compositeOpPortTypeEClass, COMPOSITE_OP_PORT_TYPE__PORT_INDEX);
    createEAttribute(compositeOpPortTypeEClass, COMPOSITE_OP_PORT_TYPE__TYPE);

    compositeToolkitOperatorTypeEClass = createEClass(COMPOSITE_TOOLKIT_OPERATOR_TYPE);
    createEAttribute(compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__GROUP);
    createEReference(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__PARAMETER);
    createEReference(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT);
    createEReference(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT);
    createEReference(compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__TYPE);
    createEReference(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__ENUM_LITERALS);
    createEAttribute(compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__COLUMN);
    createEAttribute(compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__LINE);
    createEAttribute(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__POTENTIAL_MAIN);
    createEAttribute(
        compositeToolkitOperatorTypeEClass, COMPOSITE_TOOLKIT_OPERATOR_TYPE__URI_INDEX);

    customLiteralsTypeEClass = createEClass(CUSTOM_LITERALS_TYPE);
    createEReference(customLiteralsTypeEClass, CUSTOM_LITERALS_TYPE__VALUE);

    customLiteralValueTypeEClass = createEClass(CUSTOM_LITERAL_VALUE_TYPE);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__TOOLKIT_MODEL);

    enumLiteralsTypeEClass = createEClass(ENUM_LITERALS_TYPE);
    createEReference(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__DESCRIPTION);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__ENUM);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__COLUMN);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__LINE);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__STATIC);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__TYPE_NAME);
    createEAttribute(enumLiteralsTypeEClass, ENUM_LITERALS_TYPE__URI_INDEX);

    imagesTypeEClass = createEClass(IMAGES_TYPE);
    createEReference(imagesTypeEClass, IMAGES_TYPE__IMAGE);

    imageTypeEClass = createEClass(IMAGE_TYPE);
    createEAttribute(imageTypeEClass, IMAGE_TYPE__SIZE);
    createEAttribute(imageTypeEClass, IMAGE_TYPE__URI_INDEX);

    outputFunctionsTypeEClass = createEClass(OUTPUT_FUNCTIONS_TYPE);
    createEReference(outputFunctionsTypeEClass, OUTPUT_FUNCTIONS_TYPE__FUNCTION);

    outputFunctionsTypeTypeEClass = createEClass(OUTPUT_FUNCTIONS_TYPE_TYPE);
    createEAttribute(outputFunctionsTypeTypeEClass, OUTPUT_FUNCTIONS_TYPE_TYPE__DEFAULT_VALUE);
    createEAttribute(outputFunctionsTypeTypeEClass, OUTPUT_FUNCTIONS_TYPE_TYPE__NAME);

    parameterTypeEClass = createEClass(PARAMETER_TYPE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__DEFAULT_VALUE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__MUTABLE);
    createEAttribute(parameterTypeEClass, PARAMETER_TYPE__TYPE);

    primitiveOpInputPortTypeEClass = createEClass(PRIMITIVE_OP_INPUT_PORT_TYPE);
    createEAttribute(primitiveOpInputPortTypeEClass, PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_POLICY);
    createEAttribute(
        primitiveOpInputPortTypeEClass, PRIMITIVE_OP_INPUT_PORT_TYPE__WINDOW_PUNCT_INPUT_MODE);

    primitiveOpOutputPortTypeEClass = createEClass(PRIMITIVE_OP_OUTPUT_PORT_TYPE);
    createEReference(
        primitiveOpOutputPortTypeEClass, PRIMITIVE_OP_OUTPUT_PORT_TYPE__OUTPUT_FUNCTIONS_TYPE);
    createEAttribute(
        primitiveOpOutputPortTypeEClass, PRIMITIVE_OP_OUTPUT_PORT_TYPE__EXPRESSION_MODE);
    createEAttribute(
        primitiveOpOutputPortTypeEClass, PRIMITIVE_OP_OUTPUT_PORT_TYPE__WINDOW_PUNCT_OUTPUT_MODE);

    primitiveOpParameterTypeEClass = createEClass(PRIMITIVE_OP_PARAMETER_TYPE);
    createEAttribute(primitiveOpParameterTypeEClass, PRIMITIVE_OP_PARAMETER_TYPE__PORT_SCOPE);
    createEAttribute(primitiveOpParameterTypeEClass, PRIMITIVE_OP_PARAMETER_TYPE__CARDINALITY);
    createEAttribute(primitiveOpParameterTypeEClass, PRIMITIVE_OP_PARAMETER_TYPE__EXPRESSION_MODE);
    createEAttribute(primitiveOpParameterTypeEClass, PRIMITIVE_OP_PARAMETER_TYPE__OPTIONAL);

    primitivePortDescriptionTypeEClass = createEClass(PRIMITIVE_PORT_DESCRIPTION_TYPE);
    createEReference(
        primitivePortDescriptionTypeEClass, PRIMITIVE_PORT_DESCRIPTION_TYPE__DESCRIPTION);
    createEAttribute(
        primitivePortDescriptionTypeEClass, PRIMITIVE_PORT_DESCRIPTION_TYPE__MAX_NUM_PORTS);
    createEAttribute(
        primitivePortDescriptionTypeEClass, PRIMITIVE_PORT_DESCRIPTION_TYPE__MIN_NUM_PORTS);
    createEAttribute(primitivePortDescriptionTypeEClass, PRIMITIVE_PORT_DESCRIPTION_TYPE__OPTIONAL);

    primitiveToolkitOperatorTypeEClass = createEClass(PRIMITIVE_TOOLKIT_OPERATOR_TYPE);
    createEAttribute(primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__GROUP);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CUSTOM_LITERALS);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_FUNCTIONS);
    createEReference(
        primitiveToolkitOperatorTypeEClass,
        PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETERS_DESCRIPTION);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__PARAMETER);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INPUT_PORT);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__OUTPUT_PORT);
    createEReference(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__CODE_TEMPLATE);
    createEAttribute(primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__INVALID);
    createEAttribute(primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__LANGUAGE);
    createEAttribute(
        primitiveToolkitOperatorTypeEClass, PRIMITIVE_TOOLKIT_OPERATOR_TYPE__MODEL_URI_INDEX);

    toolkitBaseTypeEClass = createEClass(TOOLKIT_BASE_TYPE);
    createEReference(toolkitBaseTypeEClass, TOOLKIT_BASE_TYPE__URI_TABLE);

    toolkitElementTypeEClass = createEClass(TOOLKIT_ELEMENT_TYPE);
    createEReference(toolkitElementTypeEClass, TOOLKIT_ELEMENT_TYPE__DESCRIPTION);
    createEAttribute(toolkitElementTypeEClass, TOOLKIT_ELEMENT_TYPE__NAME);
    createEAttribute(toolkitElementTypeEClass, TOOLKIT_ELEMENT_TYPE__SAMPLE_URI_INDEX);

    toolkitFunctionTypeEClass = createEClass(TOOLKIT_FUNCTION_TYPE);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__COLUMN);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__LINE);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__MODEL_URI_INDEX);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__NATIVE);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__PUBLIC);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__STATEFUL);
    createEAttribute(toolkitFunctionTypeEClass, TOOLKIT_FUNCTION_TYPE__URI_INDEX);

    toolkitModelTypeEClass = createEClass(TOOLKIT_MODEL_TYPE);
    createEReference(toolkitModelTypeEClass, TOOLKIT_MODEL_TYPE__TOOLKIT);
    createEAttribute(toolkitModelTypeEClass, TOOLKIT_MODEL_TYPE__PRODUCT_VERSION);

    toolkitNamespaceTypeEClass = createEClass(TOOLKIT_NAMESPACE_TYPE);
    createEAttribute(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__GROUP);
    createEReference(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__PRIMITIVE_OP);
    createEReference(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__COMPOSITE_OP);
    createEReference(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__FUNCTION);
    createEReference(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__TYPE);
    createEReference(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__ENUM_LITERALS);
    createEAttribute(toolkitNamespaceTypeEClass, TOOLKIT_NAMESPACE_TYPE__REL_PATH_TO_NAMESPACE);

    toolkitTypeEClass = createEClass(TOOLKIT_TYPE);
    createEAttribute(toolkitTypeEClass, TOOLKIT_TYPE__GROUP);
    createEReference(toolkitTypeEClass, TOOLKIT_TYPE__NAMESPACE);
    createEReference(toolkitTypeEClass, TOOLKIT_TYPE__DEPENDENCY);
    createEReference(toolkitTypeEClass, TOOLKIT_TYPE__SAB_FILES);
    createEAttribute(toolkitTypeEClass, TOOLKIT_TYPE__HAS_NL_RESOURCES);
    createEAttribute(toolkitTypeEClass, TOOLKIT_TYPE__REQUIRED_PRODUCT_VERSION);
    createEAttribute(toolkitTypeEClass, TOOLKIT_TYPE__URI);
    createEAttribute(toolkitTypeEClass, TOOLKIT_TYPE__VERSION);

    toolkitTypeTypeEClass = createEClass(TOOLKIT_TYPE_TYPE);
    createEAttribute(toolkitTypeTypeEClass, TOOLKIT_TYPE_TYPE__COLUMN);
    createEAttribute(toolkitTypeTypeEClass, TOOLKIT_TYPE_TYPE__LINE);
    createEAttribute(toolkitTypeTypeEClass, TOOLKIT_TYPE_TYPE__STATIC);
    createEAttribute(toolkitTypeTypeEClass, TOOLKIT_TYPE_TYPE__TYPE);
    createEAttribute(toolkitTypeTypeEClass, TOOLKIT_TYPE_TYPE__URI_INDEX);

    uriTableTypeEClass = createEClass(URI_TABLE_TYPE);
    createEReference(uriTableTypeEClass, URI_TABLE_TYPE__URI);

    // Create enums
    compositeParamEnumTypeEEnum = createEEnum(COMPOSITE_PARAM_ENUM_TYPE);
    enumExpressionModeTypeEEnum = createEEnum(ENUM_EXPRESSION_MODE_TYPE);
    enumWindowPolicyTypeEEnum = createEEnum(ENUM_WINDOW_POLICY_TYPE);
    enumWindowPunctInputModeTypeEEnum = createEEnum(ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE);
    enumWindowPunctOutputModeTypeEEnum = createEEnum(ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE);
    languageTypeEEnum = createEEnum(LANGUAGE_TYPE);

    // Create data types
    compositeParamEnumTypeObjectEDataType = createEDataType(COMPOSITE_PARAM_ENUM_TYPE_OBJECT);
    enumExpressionModeTypeObjectEDataType = createEDataType(ENUM_EXPRESSION_MODE_TYPE_OBJECT);
    enumWindowPolicyTypeObjectEDataType = createEDataType(ENUM_WINDOW_POLICY_TYPE_OBJECT);
    enumWindowPunctInputModeTypeObjectEDataType =
        createEDataType(ENUM_WINDOW_PUNCT_INPUT_MODE_TYPE_OBJECT);
    enumWindowPunctOutputModeTypeObjectEDataType =
        createEDataType(ENUM_WINDOW_PUNCT_OUTPUT_MODE_TYPE_OBJECT);
    languageTypeObjectEDataType = createEDataType(LANGUAGE_TYPE_OBJECT);
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
    CommonPackage theCommonPackage =
        (CommonPackage) EPackage.Registry.INSTANCE.getEPackage(CommonPackage.eNS_URI);
    InfoPackage theInfoPackage =
        (InfoPackage) EPackage.Registry.INSTANCE.getEPackage(InfoPackage.eNS_URI);

    // Create type parameters

    // Set bounds for type parameters

    // Add supertypes to classes
    baseFunctionTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    baseToolkitOperatorTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    codeTemplateTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    compositeOpParameterTypeEClass.getESuperTypes().add(this.getParameterType());
    compositeOpPortTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    compositeToolkitOperatorTypeEClass.getESuperTypes().add(this.getBaseToolkitOperatorType());
    customLiteralsTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    customLiteralValueTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    outputFunctionsTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    parameterTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    primitiveOpInputPortTypeEClass.getESuperTypes().add(this.getPrimitivePortDescriptionType());
    primitiveOpOutputPortTypeEClass.getESuperTypes().add(this.getPrimitivePortDescriptionType());
    primitiveOpParameterTypeEClass.getESuperTypes().add(this.getParameterType());
    primitiveToolkitOperatorTypeEClass.getESuperTypes().add(this.getBaseToolkitOperatorType());
    toolkitBaseTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    toolkitFunctionTypeEClass.getESuperTypes().add(this.getBaseFunctionType());
    toolkitNamespaceTypeEClass.getESuperTypes().add(this.getToolkitElementType());
    toolkitTypeEClass.getESuperTypes().add(this.getToolkitBaseType());
    toolkitTypeTypeEClass.getESuperTypes().add(this.getToolkitElementType());

    // Initialize classes and features; add operations and parameters
    initEClass(
        baseFunctionTypeEClass,
        BaseFunctionType.class,
        "BaseFunctionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getBaseFunctionType_Prototype(),
        theXMLTypePackage.getString(),
        "prototype",
        null,
        1,
        1,
        BaseFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getBaseFunctionType_Parameter(),
        this.getParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        BaseFunctionType.class,
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
        getBaseFunctionType_ReturnType(),
        theXMLTypePackage.getString(),
        "returnType",
        null,
        0,
        1,
        BaseFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        baseToolkitOperatorTypeEClass,
        BaseToolkitOperatorType.class,
        "BaseToolkitOperatorType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getBaseToolkitOperatorType_Images(),
        this.getImagesType(),
        null,
        "images",
        null,
        0,
        1,
        BaseToolkitOperatorType.class,
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
        getBaseToolkitOperatorType_Public(),
        theXMLTypePackage.getBoolean(),
        "public",
        null,
        0,
        1,
        BaseToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
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

    initEClass(
        compositeOpParameterTypeEClass,
        CompositeOpParameterType.class,
        "CompositeOpParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositeOpParameterType_MetaType(),
        this.getCompositeParamEnumType(),
        "metaType",
        null,
        1,
        1,
        CompositeOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeOpParameterType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        CompositeOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeOpPortTypeEClass,
        CompositeOpPortType.class,
        "CompositeOpPortType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositeOpPortType_PortIndex(),
        theXMLTypePackage.getInt(),
        "portIndex",
        null,
        1,
        1,
        CompositeOpPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeOpPortType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        0,
        1,
        CompositeOpPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        compositeToolkitOperatorTypeEClass,
        CompositeToolkitOperatorType.class,
        "CompositeToolkitOperatorType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getCompositeToolkitOperatorType_Group(),
        ecorePackage.getEFeatureMapEntry(),
        "group",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCompositeToolkitOperatorType_Parameter(),
        this.getCompositeOpParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCompositeToolkitOperatorType_InputPort(),
        this.getCompositeOpPortType(),
        null,
        "inputPort",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCompositeToolkitOperatorType_OutputPort(),
        this.getCompositeOpPortType(),
        null,
        "outputPort",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCompositeToolkitOperatorType_Type(),
        this.getToolkitTypeType(),
        null,
        "type",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getCompositeToolkitOperatorType_EnumLiterals(),
        this.getEnumLiteralsType(),
        null,
        "enumLiterals",
        null,
        0,
        -1,
        CompositeToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeToolkitOperatorType_Column(),
        theXMLTypePackage.getInt(),
        "column",
        null,
        1,
        1,
        CompositeToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeToolkitOperatorType_Line(),
        theXMLTypePackage.getInt(),
        "line",
        null,
        1,
        1,
        CompositeToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeToolkitOperatorType_PotentialMain(),
        theXMLTypePackage.getBoolean(),
        "potentialMain",
        null,
        0,
        1,
        CompositeToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getCompositeToolkitOperatorType_UriIndex(),
        theXMLTypePackage.getInt(),
        "uriIndex",
        null,
        1,
        1,
        CompositeToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        customLiteralsTypeEClass,
        CustomLiteralsType.class,
        "CustomLiteralsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getCustomLiteralsType_Value(),
        this.getCustomLiteralValueType(),
        null,
        "value",
        null,
        1,
        -1,
        CustomLiteralsType.class,
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
        customLiteralValueTypeEClass,
        CustomLiteralValueType.class,
        "CustomLiteralValueType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);

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
        getDocumentRoot_ToolkitModel(),
        this.getToolkitModelType(),
        null,
        "toolkitModel",
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
        enumLiteralsTypeEClass,
        EnumLiteralsType.class,
        "EnumLiteralsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getEnumLiteralsType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        EnumLiteralsType.class,
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
        getEnumLiteralsType_Enum(),
        theXMLTypePackage.getToken(),
        "enum",
        null,
        0,
        -1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumLiteralsType_Column(),
        theXMLTypePackage.getInt(),
        "column",
        null,
        1,
        1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumLiteralsType_Line(),
        theXMLTypePackage.getInt(),
        "line",
        null,
        1,
        1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumLiteralsType_Static(),
        theXMLTypePackage.getBoolean(),
        "static",
        null,
        0,
        1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumLiteralsType_TypeName(),
        theXMLTypePackage.getToken(),
        "typeName",
        null,
        0,
        1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getEnumLiteralsType_UriIndex(),
        theXMLTypePackage.getInt(),
        "uriIndex",
        null,
        1,
        1,
        EnumLiteralsType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        imagesTypeEClass,
        ImagesType.class,
        "ImagesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getImagesType_Image(),
        this.getImageType(),
        null,
        "image",
        null,
        1,
        -1,
        ImagesType.class,
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
        imageTypeEClass,
        ImageType.class,
        "ImageType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getImageType_Size(),
        theXMLTypePackage.getInt(),
        "size",
        null,
        1,
        1,
        ImageType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getImageType_UriIndex(),
        theXMLTypePackage.getInt(),
        "uriIndex",
        null,
        1,
        1,
        ImageType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        outputFunctionsTypeEClass,
        OutputFunctionsType.class,
        "OutputFunctionsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getOutputFunctionsType_Function(),
        this.getBaseFunctionType(),
        null,
        "function",
        null,
        1,
        -1,
        OutputFunctionsType.class,
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
        outputFunctionsTypeTypeEClass,
        OutputFunctionsTypeType.class,
        "OutputFunctionsTypeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getOutputFunctionsTypeType_DefaultValue(),
        theXMLTypePackage.getString(),
        "defaultValue",
        null,
        1,
        1,
        OutputFunctionsTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getOutputFunctionsTypeType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        OutputFunctionsTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
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
        getParameterType_DefaultValue(),
        theXMLTypePackage.getString(),
        "defaultValue",
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
        getParameterType_Mutable(),
        theXMLTypePackage.getBoolean(),
        "mutable",
        null,
        0,
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

    initEClass(
        primitiveOpInputPortTypeEClass,
        PrimitiveOpInputPortType.class,
        "PrimitiveOpInputPortType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getPrimitiveOpInputPortType_WindowPolicy(),
        this.getEnumWindowPolicyType(),
        "windowPolicy",
        null,
        0,
        1,
        PrimitiveOpInputPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveOpInputPortType_WindowPunctInputMode(),
        this.getEnumWindowPunctInputModeType(),
        "windowPunctInputMode",
        null,
        1,
        1,
        PrimitiveOpInputPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        primitiveOpOutputPortTypeEClass,
        PrimitiveOpOutputPortType.class,
        "PrimitiveOpOutputPortType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getPrimitiveOpOutputPortType_OutputFunctionsType(),
        this.getOutputFunctionsTypeType(),
        null,
        "outputFunctionsType",
        null,
        0,
        1,
        PrimitiveOpOutputPortType.class,
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
        getPrimitiveOpOutputPortType_ExpressionMode(),
        this.getEnumExpressionModeType(),
        "expressionMode",
        null,
        1,
        1,
        PrimitiveOpOutputPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveOpOutputPortType_WindowPunctOutputMode(),
        this.getEnumWindowPunctOutputModeType(),
        "windowPunctOutputMode",
        null,
        1,
        1,
        PrimitiveOpOutputPortType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        primitiveOpParameterTypeEClass,
        PrimitiveOpParameterType.class,
        "PrimitiveOpParameterType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getPrimitiveOpParameterType_PortScope(),
        theXMLTypePackage.getInt(),
        "portScope",
        null,
        0,
        -1,
        PrimitiveOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveOpParameterType_Cardinality(),
        theXMLTypePackage.getInt(),
        "cardinality",
        null,
        0,
        1,
        PrimitiveOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveOpParameterType_ExpressionMode(),
        this.getEnumExpressionModeType(),
        "expressionMode",
        null,
        0,
        1,
        PrimitiveOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveOpParameterType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        PrimitiveOpParameterType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        primitivePortDescriptionTypeEClass,
        PrimitivePortDescriptionType.class,
        "PrimitivePortDescriptionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getPrimitivePortDescriptionType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        PrimitivePortDescriptionType.class,
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
        getPrimitivePortDescriptionType_MaxNumPorts(),
        theXMLTypePackage.getInt(),
        "maxNumPorts",
        null,
        0,
        1,
        PrimitivePortDescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitivePortDescriptionType_MinNumPorts(),
        theXMLTypePackage.getInt(),
        "minNumPorts",
        null,
        0,
        1,
        PrimitivePortDescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitivePortDescriptionType_Optional(),
        theXMLTypePackage.getBoolean(),
        "optional",
        null,
        1,
        1,
        PrimitivePortDescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        primitiveToolkitOperatorTypeEClass,
        PrimitiveToolkitOperatorType.class,
        "PrimitiveToolkitOperatorType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getPrimitiveToolkitOperatorType_Group(),
        ecorePackage.getEFeatureMapEntry(),
        "group",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_CustomLiterals(),
        this.getCustomLiteralsType(),
        null,
        "customLiterals",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_OutputFunctions(),
        this.getOutputFunctionsType(),
        null,
        "outputFunctions",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_ParametersDescription(),
        theCommonPackage.getDescriptionType(),
        null,
        "parametersDescription",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_Parameter(),
        this.getPrimitiveOpParameterType(),
        null,
        "parameter",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_InputPort(),
        this.getPrimitiveOpInputPortType(),
        null,
        "inputPort",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_OutputPort(),
        this.getPrimitiveOpOutputPortType(),
        null,
        "outputPort",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getPrimitiveToolkitOperatorType_CodeTemplate(),
        this.getCodeTemplateType(),
        null,
        "codeTemplate",
        null,
        0,
        -1,
        PrimitiveToolkitOperatorType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveToolkitOperatorType_Invalid(),
        theXMLTypePackage.getBoolean(),
        "invalid",
        null,
        0,
        1,
        PrimitiveToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveToolkitOperatorType_Language(),
        this.getLanguageType(),
        "language",
        null,
        0,
        1,
        PrimitiveToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrimitiveToolkitOperatorType_ModelUriIndex(),
        theXMLTypePackage.getInt(),
        "modelUriIndex",
        null,
        1,
        1,
        PrimitiveToolkitOperatorType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitBaseTypeEClass,
        ToolkitBaseType.class,
        "ToolkitBaseType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getToolkitBaseType_UriTable(),
        this.getUriTableType(),
        null,
        "uriTable",
        null,
        0,
        1,
        ToolkitBaseType.class,
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
        toolkitElementTypeEClass,
        ToolkitElementType.class,
        "ToolkitElementType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getToolkitElementType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        ToolkitElementType.class,
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
        getToolkitElementType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        ToolkitElementType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitElementType_SampleUriIndex(),
        theXMLTypePackage.getInt(),
        "sampleUriIndex",
        null,
        0,
        1,
        ToolkitElementType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitFunctionTypeEClass,
        ToolkitFunctionType.class,
        "ToolkitFunctionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getToolkitFunctionType_Column(),
        theXMLTypePackage.getInt(),
        "column",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_Line(),
        theXMLTypePackage.getInt(),
        "line",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_ModelUriIndex(),
        theXMLTypePackage.getInt(),
        "modelUriIndex",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_Native(),
        theXMLTypePackage.getBoolean(),
        "native",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_Public(),
        theXMLTypePackage.getBoolean(),
        "public",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_Stateful(),
        theXMLTypePackage.getBoolean(),
        "stateful",
        null,
        0,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitFunctionType_UriIndex(),
        theXMLTypePackage.getInt(),
        "uriIndex",
        null,
        1,
        1,
        ToolkitFunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitModelTypeEClass,
        ToolkitModelType.class,
        "ToolkitModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getToolkitModelType_Toolkit(),
        this.getToolkitType(),
        null,
        "toolkit",
        null,
        0,
        -1,
        ToolkitModelType.class,
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
        getToolkitModelType_ProductVersion(),
        theXMLTypePackage.getToken(),
        "productVersion",
        null,
        1,
        1,
        ToolkitModelType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitNamespaceTypeEClass,
        ToolkitNamespaceType.class,
        "ToolkitNamespaceType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getToolkitNamespaceType_Group(),
        ecorePackage.getEFeatureMapEntry(),
        "group",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitNamespaceType_PrimitiveOp(),
        this.getPrimitiveToolkitOperatorType(),
        null,
        "primitiveOp",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitNamespaceType_CompositeOp(),
        this.getCompositeToolkitOperatorType(),
        null,
        "compositeOp",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitNamespaceType_Function(),
        this.getToolkitFunctionType(),
        null,
        "function",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitNamespaceType_Type(),
        this.getToolkitTypeType(),
        null,
        "type",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitNamespaceType_EnumLiterals(),
        this.getEnumLiteralsType(),
        null,
        "enumLiterals",
        null,
        0,
        -1,
        ToolkitNamespaceType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitNamespaceType_RelPathToNamespace(),
        theXMLTypePackage.getString(),
        "relPathToNamespace",
        null,
        0,
        1,
        ToolkitNamespaceType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitTypeEClass,
        ToolkitType.class,
        "ToolkitType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getToolkitType_Group(),
        ecorePackage.getEFeatureMapEntry(),
        "group",
        null,
        0,
        -1,
        ToolkitType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitType_Namespace(),
        this.getToolkitNamespaceType(),
        null,
        "namespace",
        null,
        0,
        -1,
        ToolkitType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitType_Dependency(),
        theCommonPackage.getToolkitDependencyType(),
        null,
        "dependency",
        null,
        0,
        -1,
        ToolkitType.class,
        IS_TRANSIENT,
        IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getToolkitType_SabFiles(),
        theInfoPackage.getSabFilesType(),
        null,
        "sabFiles",
        null,
        0,
        1,
        ToolkitType.class,
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
        getToolkitType_HasNLResources(),
        theXMLTypePackage.getBoolean(),
        "hasNLResources",
        null,
        0,
        1,
        ToolkitType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitType_RequiredProductVersion(),
        theCommonPackage.getVersionRangeType(),
        "requiredProductVersion",
        null,
        1,
        1,
        ToolkitType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitType_Uri(),
        theXMLTypePackage.getString(),
        "uri",
        null,
        0,
        1,
        ToolkitType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitType_Version(),
        theCommonPackage.getVersionType(),
        "version",
        null,
        1,
        1,
        ToolkitType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitTypeTypeEClass,
        ToolkitTypeType.class,
        "ToolkitTypeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getToolkitTypeType_Column(),
        theXMLTypePackage.getInt(),
        "column",
        null,
        1,
        1,
        ToolkitTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitTypeType_Line(),
        theXMLTypePackage.getInt(),
        "line",
        null,
        1,
        1,
        ToolkitTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitTypeType_Static(),
        theXMLTypePackage.getBoolean(),
        "static",
        null,
        0,
        1,
        ToolkitTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitTypeType_Type(),
        theXMLTypePackage.getString(),
        "type",
        null,
        1,
        1,
        ToolkitTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitTypeType_UriIndex(),
        theXMLTypePackage.getInt(),
        "uriIndex",
        null,
        1,
        1,
        ToolkitTypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        uriTableTypeEClass,
        UriTableType.class,
        "UriTableType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getUriTableType_Uri(),
        theCommonPackage.getUriType(),
        null,
        "uri",
        null,
        1,
        -1,
        UriTableType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_COMPOSITE,
        !IS_RESOLVE_PROXIES,
        !IS_UNSETTABLE,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    // Initialize enums and add enum literals
    initEEnum(compositeParamEnumTypeEEnum, CompositeParamEnumType.class, "CompositeParamEnumType");
    addEEnumLiteral(compositeParamEnumTypeEEnum, CompositeParamEnumType.ATTRIBUTE);
    addEEnumLiteral(compositeParamEnumTypeEEnum, CompositeParamEnumType.EXPRESSION);
    addEEnumLiteral(compositeParamEnumTypeEEnum, CompositeParamEnumType.FUNCTION);
    addEEnumLiteral(compositeParamEnumTypeEEnum, CompositeParamEnumType.OPERATOR);
    addEEnumLiteral(compositeParamEnumTypeEEnum, CompositeParamEnumType.TYPE);

    initEEnum(enumExpressionModeTypeEEnum, EnumExpressionModeType.class, "EnumExpressionModeType");
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.ATTRIBUTE);
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.ATTRIBUTE_FREE);
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.CONSTANT);
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.CUSTOM_LITERAL);
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.EXPRESSION);
    addEEnumLiteral(enumExpressionModeTypeEEnum, EnumExpressionModeType.NONEXISTENT);

    initEEnum(enumWindowPolicyTypeEEnum, EnumWindowPolicyType.class, "EnumWindowPolicyType");
    addEEnumLiteral(enumWindowPolicyTypeEEnum, EnumWindowPolicyType.NON_WINDOWED);
    addEEnumLiteral(enumWindowPolicyTypeEEnum, EnumWindowPolicyType.WINDOWED);
    addEEnumLiteral(enumWindowPolicyTypeEEnum, EnumWindowPolicyType.OPTIONALLY_WINDOWED);

    initEEnum(
        enumWindowPunctInputModeTypeEEnum,
        EnumWindowPunctInputModeType.class,
        "EnumWindowPunctInputModeType");
    addEEnumLiteral(enumWindowPunctInputModeTypeEEnum, EnumWindowPunctInputModeType.EXPECTING);
    addEEnumLiteral(enumWindowPunctInputModeTypeEEnum, EnumWindowPunctInputModeType.OBLIVIOUS);
    addEEnumLiteral(enumWindowPunctInputModeTypeEEnum, EnumWindowPunctInputModeType.WINDOW_BOUND);

    initEEnum(
        enumWindowPunctOutputModeTypeEEnum,
        EnumWindowPunctOutputModeType.class,
        "EnumWindowPunctOutputModeType");
    addEEnumLiteral(enumWindowPunctOutputModeTypeEEnum, EnumWindowPunctOutputModeType.GENERATING);
    addEEnumLiteral(enumWindowPunctOutputModeTypeEEnum, EnumWindowPunctOutputModeType.FREE);
    addEEnumLiteral(enumWindowPunctOutputModeTypeEEnum, EnumWindowPunctOutputModeType.PRESERVING);

    initEEnum(languageTypeEEnum, LanguageType.class, "LanguageType");
    addEEnumLiteral(languageTypeEEnum, LanguageType.CPP);
    addEEnumLiteral(languageTypeEEnum, LanguageType.JAVA);

    // Initialize data types
    initEDataType(
        compositeParamEnumTypeObjectEDataType,
        CompositeParamEnumType.class,
        "CompositeParamEnumTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        enumExpressionModeTypeObjectEDataType,
        EnumExpressionModeType.class,
        "EnumExpressionModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        enumWindowPolicyTypeObjectEDataType,
        EnumWindowPolicyType.class,
        "EnumWindowPolicyTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        enumWindowPunctInputModeTypeObjectEDataType,
        EnumWindowPunctInputModeType.class,
        "EnumWindowPunctInputModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        enumWindowPunctOutputModeTypeObjectEDataType,
        EnumWindowPunctOutputModeType.class,
        "EnumWindowPunctOutputModeTypeObject",
        IS_SERIALIZABLE,
        IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        languageTypeObjectEDataType,
        LanguageType.class,
        "LanguageTypeObject",
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
        baseFunctionTypeEClass,
        source,
        new String[] {
          "name", "baseFunctionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getBaseFunctionType_Prototype(),
        source,
        new String[] {
          "kind", "element",
          "name", "prototype",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getBaseFunctionType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getBaseFunctionType_ReturnType(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "returnType"
        });
    addAnnotation(
        baseToolkitOperatorTypeEClass,
        source,
        new String[] {
          "name", "baseToolkitOperatorType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getBaseToolkitOperatorType_Images(),
        source,
        new String[] {
          "kind", "element",
          "name", "images",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getBaseToolkitOperatorType_Public(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "public"
        });
    addAnnotation(
        codeTemplateTypeEClass,
        source,
        new String[] {
          "name", "codeTemplateType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCodeTemplateType_Template(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "template"
        });
    addAnnotation(
        compositeOpParameterTypeEClass,
        source,
        new String[] {
          "name", "compositeOpParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeOpParameterType_MetaType(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "metaType"
        });
    addAnnotation(
        getCompositeOpParameterType_Optional(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "optional"
        });
    addAnnotation(
        compositeOpPortTypeEClass,
        source,
        new String[] {
          "name", "compositeOpPortType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeOpPortType_PortIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "portIndex"
        });
    addAnnotation(
        getCompositeOpPortType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        compositeParamEnumTypeEEnum, source, new String[] {"name", "compositeParamEnumType"});
    addAnnotation(
        compositeParamEnumTypeObjectEDataType,
        source,
        new String[] {
          "name", "compositeParamEnumType:Object",
          "baseType", "compositeParamEnumType"
        });
    addAnnotation(
        compositeToolkitOperatorTypeEClass,
        source,
        new String[] {
          "name", "compositeToolkitOperatorType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_Group(),
        source,
        new String[] {
          "kind", "group",
          "name", "group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_InputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPort",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_OutputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPort",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_EnumLiterals(),
        source,
        new String[] {
          "kind", "element",
          "name", "enumLiterals",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_PotentialMain(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "potentialMain"
        });
    addAnnotation(
        getCompositeToolkitOperatorType_UriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uriIndex"
        });
    addAnnotation(
        customLiteralsTypeEClass,
        source,
        new String[] {
          "name", "customLiteralsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getCustomLiteralsType_Value(),
        source,
        new String[] {
          "kind", "element",
          "name", "value",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        customLiteralValueTypeEClass,
        source,
        new String[] {
          "name", "customLiteralValueType",
          "kind", "elementOnly"
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
        getDocumentRoot_ToolkitModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "toolkitModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        enumExpressionModeTypeEEnum, source, new String[] {"name", "enumExpressionModeType"});
    addAnnotation(
        enumExpressionModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "enumExpressionModeType:Object",
          "baseType", "enumExpressionModeType"
        });
    addAnnotation(
        enumLiteralsTypeEClass,
        source,
        new String[] {
          "name", "enumLiteralsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getEnumLiteralsType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getEnumLiteralsType_Enum(),
        source,
        new String[] {
          "kind", "element",
          "name", "enum",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getEnumLiteralsType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getEnumLiteralsType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
    addAnnotation(
        getEnumLiteralsType_Static(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "static"
        });
    addAnnotation(
        getEnumLiteralsType_TypeName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "typeName"
        });
    addAnnotation(
        getEnumLiteralsType_UriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uriIndex"
        });
    addAnnotation(enumWindowPolicyTypeEEnum, source, new String[] {"name", "enumWindowPolicyType"});
    addAnnotation(
        enumWindowPolicyTypeObjectEDataType,
        source,
        new String[] {
          "name", "enumWindowPolicyType:Object",
          "baseType", "enumWindowPolicyType"
        });
    addAnnotation(
        enumWindowPunctInputModeTypeEEnum,
        source,
        new String[] {"name", "enumWindowPunctInputModeType"});
    addAnnotation(
        enumWindowPunctInputModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "enumWindowPunctInputModeType:Object",
          "baseType", "enumWindowPunctInputModeType"
        });
    addAnnotation(
        enumWindowPunctOutputModeTypeEEnum,
        source,
        new String[] {"name", "enumWindowPunctOutputModeType"});
    addAnnotation(
        enumWindowPunctOutputModeTypeObjectEDataType,
        source,
        new String[] {
          "name", "enumWindowPunctOutputModeType:Object",
          "baseType", "enumWindowPunctOutputModeType"
        });
    addAnnotation(
        imagesTypeEClass,
        source,
        new String[] {
          "name", "imagesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getImagesType_Image(),
        source,
        new String[] {
          "kind", "element",
          "name", "image",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        imageTypeEClass,
        source,
        new String[] {
          "name", "imageType",
          "kind", "empty"
        });
    addAnnotation(
        getImageType_Size(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "size"
        });
    addAnnotation(
        getImageType_UriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uriIndex"
        });
    addAnnotation(languageTypeEEnum, source, new String[] {"name", "languageType"});
    addAnnotation(
        languageTypeObjectEDataType,
        source,
        new String[] {
          "name", "languageType:Object",
          "baseType", "languageType"
        });
    addAnnotation(
        outputFunctionsTypeEClass,
        source,
        new String[] {
          "name", "outputFunctionsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getOutputFunctionsType_Function(),
        source,
        new String[] {
          "kind", "element",
          "name", "function",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        outputFunctionsTypeTypeEClass,
        source,
        new String[] {
          "name", "outputFunctionsTypeType",
          "kind", "empty"
        });
    addAnnotation(
        getOutputFunctionsTypeType_DefaultValue(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "defaultValue"
        });
    addAnnotation(
        getOutputFunctionsTypeType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        parameterTypeEClass,
        source,
        new String[] {
          "name", "parameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getParameterType_DefaultValue(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "defaultValue"
        });
    addAnnotation(
        getParameterType_Mutable(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "mutable"
        });
    addAnnotation(
        getParameterType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        primitiveOpInputPortTypeEClass,
        source,
        new String[] {
          "name", "primitiveOpInputPortType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPrimitiveOpInputPortType_WindowPolicy(),
        source,
        new String[] {
          "kind", "element",
          "name", "windowPolicy",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getPrimitiveOpInputPortType_WindowPunctInputMode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "windowPunctInputMode"
        });
    addAnnotation(
        primitiveOpOutputPortTypeEClass,
        source,
        new String[] {
          "name", "primitiveOpOutputPortType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPrimitiveOpOutputPortType_OutputFunctionsType(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputFunctionsType",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getPrimitiveOpOutputPortType_ExpressionMode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "expressionMode"
        });
    addAnnotation(
        getPrimitiveOpOutputPortType_WindowPunctOutputMode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "windowPunctOutputMode"
        });
    addAnnotation(
        primitiveOpParameterTypeEClass,
        source,
        new String[] {
          "name", "primitiveOpParameterType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPrimitiveOpParameterType_PortScope(),
        source,
        new String[] {
          "kind", "element",
          "name", "portScope",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getPrimitiveOpParameterType_Cardinality(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "cardinality"
        });
    addAnnotation(
        getPrimitiveOpParameterType_ExpressionMode(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "expressionMode"
        });
    addAnnotation(
        getPrimitiveOpParameterType_Optional(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "optional"
        });
    addAnnotation(
        primitivePortDescriptionTypeEClass,
        source,
        new String[] {
          "name", "primitivePortDescriptionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPrimitivePortDescriptionType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getPrimitivePortDescriptionType_MaxNumPorts(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "maxNumPorts"
        });
    addAnnotation(
        getPrimitivePortDescriptionType_MinNumPorts(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "minNumPorts"
        });
    addAnnotation(
        getPrimitivePortDescriptionType_Optional(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "optional"
        });
    addAnnotation(
        primitiveToolkitOperatorTypeEClass,
        source,
        new String[] {
          "name", "primitiveToolkitOperatorType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_Group(),
        source,
        new String[] {
          "kind", "group",
          "name", "group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_CustomLiterals(),
        source,
        new String[] {
          "kind", "element",
          "name", "customLiterals",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_OutputFunctions(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputFunctions",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_ParametersDescription(),
        source,
        new String[] {
          "kind", "element",
          "name", "parametersDescription",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_Parameter(),
        source,
        new String[] {
          "kind", "element",
          "name", "parameter",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_InputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "inputPort",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_OutputPort(),
        source,
        new String[] {
          "kind", "element",
          "name", "outputPort",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_CodeTemplate(),
        source,
        new String[] {
          "kind", "element",
          "name", "codeTemplate",
          "namespace", "##targetNamespace",
          "group", "#group:5"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_Invalid(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "invalid"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_Language(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "language"
        });
    addAnnotation(
        getPrimitiveToolkitOperatorType_ModelUriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "modelUriIndex"
        });
    addAnnotation(
        toolkitBaseTypeEClass,
        source,
        new String[] {
          "name", "toolkitBaseType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitBaseType_UriTable(),
        source,
        new String[] {
          "kind", "element",
          "name", "uriTable",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        toolkitElementTypeEClass,
        source,
        new String[] {
          "name", "toolkitElementType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitElementType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitElementType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        getToolkitElementType_SampleUriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "sampleUriIndex"
        });
    addAnnotation(
        toolkitFunctionTypeEClass,
        source,
        new String[] {
          "name", "toolkitFunctionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitFunctionType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getToolkitFunctionType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
    addAnnotation(
        getToolkitFunctionType_ModelUriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "modelUriIndex"
        });
    addAnnotation(
        getToolkitFunctionType_Native(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "native"
        });
    addAnnotation(
        getToolkitFunctionType_Public(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "public"
        });
    addAnnotation(
        getToolkitFunctionType_Stateful(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "stateful"
        });
    addAnnotation(
        getToolkitFunctionType_UriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uriIndex"
        });
    addAnnotation(
        toolkitModelTypeEClass,
        source,
        new String[] {
          "name", "toolkitModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitModelType_Toolkit(),
        source,
        new String[] {
          "kind", "element",
          "name", "toolkit",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitModelType_ProductVersion(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "productVersion"
        });
    addAnnotation(
        toolkitNamespaceTypeEClass,
        source,
        new String[] {
          "name", "toolkitNamespaceType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitNamespaceType_Group(),
        source,
        new String[] {
          "kind", "group",
          "name", "group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_PrimitiveOp(),
        source,
        new String[] {
          "kind", "element",
          "name", "primitiveOp",
          "namespace", "##targetNamespace",
          "group", "#group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_CompositeOp(),
        source,
        new String[] {
          "kind", "element",
          "name", "compositeOp",
          "namespace", "##targetNamespace",
          "group", "#group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_Function(),
        source,
        new String[] {
          "kind", "element",
          "name", "function",
          "namespace", "##targetNamespace",
          "group", "#group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_Type(),
        source,
        new String[] {
          "kind", "element",
          "name", "type",
          "namespace", "##targetNamespace",
          "group", "#group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_EnumLiterals(),
        source,
        new String[] {
          "kind", "element",
          "name", "enumLiterals",
          "namespace", "##targetNamespace",
          "group", "#group:3"
        });
    addAnnotation(
        getToolkitNamespaceType_RelPathToNamespace(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "relPathToNamespace"
        });
    addAnnotation(
        toolkitTypeEClass,
        source,
        new String[] {
          "name", "toolkitType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitType_Group(),
        source,
        new String[] {
          "kind", "group",
          "name", "group:4"
        });
    addAnnotation(
        getToolkitType_Namespace(),
        source,
        new String[] {
          "kind", "element",
          "name", "namespace",
          "namespace", "##targetNamespace",
          "group", "#group:4"
        });
    addAnnotation(
        getToolkitType_Dependency(),
        source,
        new String[] {
          "kind", "element",
          "name", "dependency",
          "namespace", "##targetNamespace",
          "group", "#group:4"
        });
    addAnnotation(
        getToolkitType_SabFiles(),
        source,
        new String[] {
          "kind", "element",
          "name", "sabFiles",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitType_HasNLResources(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "hasNLResources"
        });
    addAnnotation(
        getToolkitType_RequiredProductVersion(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "requiredProductVersion"
        });
    addAnnotation(
        getToolkitType_Uri(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uri"
        });
    addAnnotation(
        getToolkitType_Version(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "version"
        });
    addAnnotation(
        toolkitTypeTypeEClass,
        source,
        new String[] {
          "name", "toolkitTypeType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitTypeType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getToolkitTypeType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
    addAnnotation(
        getToolkitTypeType_Static(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "static"
        });
    addAnnotation(
        getToolkitTypeType_Type(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "type"
        });
    addAnnotation(
        getToolkitTypeType_UriIndex(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "uriIndex"
        });
    addAnnotation(
        uriTableTypeEClass,
        source,
        new String[] {
          "name", "uriTableType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getUriTableType_Uri(),
        source,
        new String[] {
          "kind", "element",
          "name", "uri",
          "namespace", "##targetNamespace"
        });
  }
} // ToolkitPackageImpl
