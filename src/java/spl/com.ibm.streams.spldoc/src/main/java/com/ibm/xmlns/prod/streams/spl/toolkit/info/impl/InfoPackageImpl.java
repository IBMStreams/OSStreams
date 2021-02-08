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
package com.ibm.xmlns.prod.streams.spl.toolkit.info.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.impl.ToolkitPackageImpl;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoFactory;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType;
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
public class InfoPackageImpl extends EPackageImpl implements InfoPackage {
  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass dependenciesTypeEClass = null;

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
  private EClass excludeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass identityTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass includeTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass langTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass messageSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass resourcesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass sabFilesTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitInfoModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EEnum rootTypeEEnum = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType rootTypeObjectEDataType = null;

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private InfoPackageImpl() {
    super(eNS_URI, InfoFactory.eINSTANCE);
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
   * <p>This method is used to initialize {@link InfoPackage#eINSTANCE} when that field is accessed.
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
  public static InfoPackage init() {
    if (isInited) return (InfoPackage) EPackage.Registry.INSTANCE.getEPackage(InfoPackage.eNS_URI);

    // Obtain or create and register package
    InfoPackageImpl theInfoPackage =
        (InfoPackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof InfoPackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new InfoPackageImpl());

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
    ToolkitPackageImpl theToolkitPackage =
        (ToolkitPackageImpl)
            (EPackage.Registry.INSTANCE.getEPackage(ToolkitPackage.eNS_URI)
                    instanceof ToolkitPackageImpl
                ? EPackage.Registry.INSTANCE.getEPackage(ToolkitPackage.eNS_URI)
                : ToolkitPackage.eINSTANCE);

    // Create package meta-data objects
    theInfoPackage.createPackageContents();
    theCommonPackage.createPackageContents();
    theToolkitPackage.createPackageContents();

    // Initialize created meta-data
    theInfoPackage.initializePackageContents();
    theCommonPackage.initializePackageContents();
    theToolkitPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theInfoPackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(InfoPackage.eNS_URI, theInfoPackage);
    return theInfoPackage;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getDependenciesType() {
    return dependenciesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDependenciesType_Toolkit() {
    return (EReference) dependenciesTypeEClass.getEStructuralFeatures().get(0);
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
  public EReference getDocumentRoot_ToolkitInfoModel() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getExcludeType() {
    return excludeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getExcludeType_Path() {
    return (EAttribute) excludeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getIdentityType() {
    return identityTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIdentityType_Name() {
    return (EAttribute) identityTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getIdentityType_Description() {
    return (EReference) identityTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIdentityType_Version() {
    return (EAttribute) identityTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIdentityType_RequiredProductVersion() {
    return (EAttribute) identityTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getIncludeType() {
    return includeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIncludeType_Path() {
    return (EAttribute) includeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getIncludeType_Root() {
    return (EAttribute) includeTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getLangType() {
    return langTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getLangType_Value() {
    return (EAttribute) langTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getLangType_Default() {
    return (EAttribute) langTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getMessageSetType() {
    return messageSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getMessageSetType_Lang() {
    return (EReference) messageSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getMessageSetType_Name() {
    return (EAttribute) messageSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getResourcesType() {
    return resourcesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getResourcesType_MessageSet() {
    return (EReference) resourcesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getSabFilesType() {
    return sabFilesTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getSabFilesType_Group() {
    return (EAttribute) sabFilesTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSabFilesType_Include() {
    return (EReference) sabFilesTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getSabFilesType_Exclude() {
    return (EReference) sabFilesTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitInfoModelType() {
    return toolkitInfoModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitInfoModelType_Identity() {
    return (EReference) toolkitInfoModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitInfoModelType_Dependencies() {
    return (EReference) toolkitInfoModelTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitInfoModelType_Resources() {
    return (EReference) toolkitInfoModelTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getToolkitInfoModelType_SabFiles() {
    return (EReference) toolkitInfoModelTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EEnum getRootType() {
    return rootTypeEEnum;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getRootTypeObject() {
    return rootTypeObjectEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InfoFactory getInfoFactory() {
    return (InfoFactory) getEFactoryInstance();
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
    dependenciesTypeEClass = createEClass(DEPENDENCIES_TYPE);
    createEReference(dependenciesTypeEClass, DEPENDENCIES_TYPE__TOOLKIT);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__TOOLKIT_INFO_MODEL);

    excludeTypeEClass = createEClass(EXCLUDE_TYPE);
    createEAttribute(excludeTypeEClass, EXCLUDE_TYPE__PATH);

    identityTypeEClass = createEClass(IDENTITY_TYPE);
    createEAttribute(identityTypeEClass, IDENTITY_TYPE__NAME);
    createEReference(identityTypeEClass, IDENTITY_TYPE__DESCRIPTION);
    createEAttribute(identityTypeEClass, IDENTITY_TYPE__VERSION);
    createEAttribute(identityTypeEClass, IDENTITY_TYPE__REQUIRED_PRODUCT_VERSION);

    includeTypeEClass = createEClass(INCLUDE_TYPE);
    createEAttribute(includeTypeEClass, INCLUDE_TYPE__PATH);
    createEAttribute(includeTypeEClass, INCLUDE_TYPE__ROOT);

    langTypeEClass = createEClass(LANG_TYPE);
    createEAttribute(langTypeEClass, LANG_TYPE__VALUE);
    createEAttribute(langTypeEClass, LANG_TYPE__DEFAULT);

    messageSetTypeEClass = createEClass(MESSAGE_SET_TYPE);
    createEReference(messageSetTypeEClass, MESSAGE_SET_TYPE__LANG);
    createEAttribute(messageSetTypeEClass, MESSAGE_SET_TYPE__NAME);

    resourcesTypeEClass = createEClass(RESOURCES_TYPE);
    createEReference(resourcesTypeEClass, RESOURCES_TYPE__MESSAGE_SET);

    sabFilesTypeEClass = createEClass(SAB_FILES_TYPE);
    createEAttribute(sabFilesTypeEClass, SAB_FILES_TYPE__GROUP);
    createEReference(sabFilesTypeEClass, SAB_FILES_TYPE__INCLUDE);
    createEReference(sabFilesTypeEClass, SAB_FILES_TYPE__EXCLUDE);

    toolkitInfoModelTypeEClass = createEClass(TOOLKIT_INFO_MODEL_TYPE);
    createEReference(toolkitInfoModelTypeEClass, TOOLKIT_INFO_MODEL_TYPE__IDENTITY);
    createEReference(toolkitInfoModelTypeEClass, TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES);
    createEReference(toolkitInfoModelTypeEClass, TOOLKIT_INFO_MODEL_TYPE__RESOURCES);
    createEReference(toolkitInfoModelTypeEClass, TOOLKIT_INFO_MODEL_TYPE__SAB_FILES);

    // Create enums
    rootTypeEEnum = createEEnum(ROOT_TYPE);

    // Create data types
    rootTypeObjectEDataType = createEDataType(ROOT_TYPE_OBJECT);
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

    // Initialize classes and features; add operations and parameters
    initEClass(
        dependenciesTypeEClass,
        DependenciesType.class,
        "DependenciesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getDependenciesType_Toolkit(),
        theCommonPackage.getToolkitDependencyType(),
        null,
        "toolkit",
        null,
        0,
        -1,
        DependenciesType.class,
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
        getDocumentRoot_ToolkitInfoModel(),
        this.getToolkitInfoModelType(),
        null,
        "toolkitInfoModel",
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
        excludeTypeEClass,
        ExcludeType.class,
        "ExcludeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getExcludeType_Path(),
        theXMLTypePackage.getString(),
        "path",
        null,
        1,
        1,
        ExcludeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        identityTypeEClass,
        IdentityType.class,
        "IdentityType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getIdentityType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        IdentityType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getIdentityType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        1,
        1,
        IdentityType.class,
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
        getIdentityType_Version(),
        theCommonPackage.getVersionType(),
        "version",
        null,
        1,
        1,
        IdentityType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getIdentityType_RequiredProductVersion(),
        theCommonPackage.getVersionRangeType(),
        "requiredProductVersion",
        null,
        1,
        1,
        IdentityType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        includeTypeEClass,
        IncludeType.class,
        "IncludeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getIncludeType_Path(),
        theXMLTypePackage.getString(),
        "path",
        null,
        1,
        1,
        IncludeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getIncludeType_Root(),
        this.getRootType(),
        "root",
        null,
        0,
        1,
        IncludeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        langTypeEClass,
        LangType.class,
        "LangType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getLangType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        0,
        1,
        LangType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getLangType_Default(),
        theXMLTypePackage.getBoolean(),
        "default",
        null,
        0,
        1,
        LangType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        messageSetTypeEClass,
        MessageSetType.class,
        "MessageSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getMessageSetType_Lang(),
        this.getLangType(),
        null,
        "lang",
        null,
        1,
        -1,
        MessageSetType.class,
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
        getMessageSetType_Name(),
        theXMLTypePackage.getToken(),
        "name",
        null,
        1,
        1,
        MessageSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        resourcesTypeEClass,
        ResourcesType.class,
        "ResourcesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getResourcesType_MessageSet(),
        this.getMessageSetType(),
        null,
        "messageSet",
        null,
        1,
        -1,
        ResourcesType.class,
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
        sabFilesTypeEClass,
        SabFilesType.class,
        "SabFilesType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getSabFilesType_Group(),
        ecorePackage.getEFeatureMapEntry(),
        "group",
        null,
        0,
        -1,
        SabFilesType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getSabFilesType_Include(),
        this.getIncludeType(),
        null,
        "include",
        null,
        0,
        -1,
        SabFilesType.class,
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
        getSabFilesType_Exclude(),
        this.getExcludeType(),
        null,
        "exclude",
        null,
        0,
        -1,
        SabFilesType.class,
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
        toolkitInfoModelTypeEClass,
        ToolkitInfoModelType.class,
        "ToolkitInfoModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getToolkitInfoModelType_Identity(),
        this.getIdentityType(),
        null,
        "identity",
        null,
        1,
        1,
        ToolkitInfoModelType.class,
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
        getToolkitInfoModelType_Dependencies(),
        this.getDependenciesType(),
        null,
        "dependencies",
        null,
        1,
        1,
        ToolkitInfoModelType.class,
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
        getToolkitInfoModelType_Resources(),
        this.getResourcesType(),
        null,
        "resources",
        null,
        0,
        1,
        ToolkitInfoModelType.class,
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
        getToolkitInfoModelType_SabFiles(),
        this.getSabFilesType(),
        null,
        "sabFiles",
        null,
        0,
        1,
        ToolkitInfoModelType.class,
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
    initEEnum(rootTypeEEnum, RootType.class, "RootType");
    addEEnumLiteral(rootTypeEEnum, RootType.TOOLKIT_DIR);
    addEEnumLiteral(rootTypeEEnum, RootType.APPLICATION_DIR);
    addEEnumLiteral(rootTypeEEnum, RootType.OUTPUT_DIR);

    // Initialize data types
    initEDataType(
        rootTypeObjectEDataType,
        RootType.class,
        "RootTypeObject",
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
        dependenciesTypeEClass,
        source,
        new String[] {
          "name", "dependenciesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getDependenciesType_Toolkit(),
        source,
        new String[] {
          "kind", "element",
          "name", "toolkit",
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
        getDocumentRoot_ToolkitInfoModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "toolkitInfoModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        excludeTypeEClass,
        source,
        new String[] {
          "name", "excludeType",
          "kind", "empty"
        });
    addAnnotation(
        getExcludeType_Path(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "path"
        });
    addAnnotation(
        identityTypeEClass,
        source,
        new String[] {
          "name", "identityType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getIdentityType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getIdentityType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getIdentityType_Version(),
        source,
        new String[] {
          "kind", "element",
          "name", "version",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getIdentityType_RequiredProductVersion(),
        source,
        new String[] {
          "kind", "element",
          "name", "requiredProductVersion",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        includeTypeEClass,
        source,
        new String[] {
          "name", "includeType",
          "kind", "empty"
        });
    addAnnotation(
        getIncludeType_Path(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "path"
        });
    addAnnotation(
        getIncludeType_Root(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "root"
        });
    addAnnotation(
        langTypeEClass,
        source,
        new String[] {
          "name", "langType",
          "kind", "simple"
        });
    addAnnotation(
        getLangType_Value(),
        source,
        new String[] {
          "name", ":0",
          "kind", "simple"
        });
    addAnnotation(
        getLangType_Default(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "default"
        });
    addAnnotation(
        messageSetTypeEClass,
        source,
        new String[] {
          "name", "messageSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getMessageSetType_Lang(),
        source,
        new String[] {
          "kind", "element",
          "name", "lang",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getMessageSetType_Name(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "name"
        });
    addAnnotation(
        resourcesTypeEClass,
        source,
        new String[] {
          "name", "resourcesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getResourcesType_MessageSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "messageSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(rootTypeEEnum, source, new String[] {"name", "rootType"});
    addAnnotation(
        rootTypeObjectEDataType,
        source,
        new String[] {
          "name", "rootType:Object",
          "baseType", "rootType"
        });
    addAnnotation(
        sabFilesTypeEClass,
        source,
        new String[] {
          "name", "sabFilesType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getSabFilesType_Group(),
        source,
        new String[] {
          "kind", "group",
          "name", "group:0"
        });
    addAnnotation(
        getSabFilesType_Include(),
        source,
        new String[] {
          "kind", "element",
          "name", "include",
          "namespace", "##targetNamespace",
          "group", "#group:0"
        });
    addAnnotation(
        getSabFilesType_Exclude(),
        source,
        new String[] {
          "kind", "element",
          "name", "exclude",
          "namespace", "##targetNamespace",
          "group", "#group:0"
        });
    addAnnotation(
        toolkitInfoModelTypeEClass,
        source,
        new String[] {
          "name", "toolkitInfoModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitInfoModelType_Identity(),
        source,
        new String[] {
          "kind", "element",
          "name", "identity",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitInfoModelType_Dependencies(),
        source,
        new String[] {
          "kind", "element",
          "name", "dependencies",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitInfoModelType_Resources(),
        source,
        new String[] {
          "kind", "element",
          "name", "resources",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitInfoModelType_SabFiles(),
        source,
        new String[] {
          "kind", "element",
          "name", "sabFiles",
          "namespace", "##targetNamespace"
        });
  }
} // InfoPackageImpl
