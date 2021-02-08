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
package com.ibm.xmlns.prod.streams.spl.common.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonFactory;
import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import com.ibm.xmlns.prod.streams.spl.common.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.LibraryType;
import com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType;
import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import com.ibm.xmlns.prod.streams.spl.common.UriType;
import com.ibm.xmlns.prod.streams.spl.common.util.CommonValidator;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.impl.OperatorPackageImpl;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;
import org.eclipse.emf.ecore.EValidator;
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
public class CommonPackageImpl extends EPackageImpl implements CommonPackage {
  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass descriptionTypeEClass = null;

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
  private EClass javaOpLibraryTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass javaOpManagedLibraryTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass libraryTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass managedLibraryTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass toolkitDependencyTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass uriTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType versionRangeTypeEDataType = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EDataType versionTypeEDataType = null;

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
   * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private CommonPackageImpl() {
    super(eNS_URI, CommonFactory.eINSTANCE);
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
   * <p>This method is used to initialize {@link CommonPackage#eINSTANCE} when that field is
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
  public static CommonPackage init() {
    if (isInited)
      return (CommonPackage) EPackage.Registry.INSTANCE.getEPackage(CommonPackage.eNS_URI);

    // Obtain or create and register package
    CommonPackageImpl theCommonPackage =
        (CommonPackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof CommonPackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new CommonPackageImpl());

    isInited = true;

    // Initialize simple dependencies
    XMLTypePackage.eINSTANCE.eClass();

    // Obtain or create and register interdependencies
    OperatorPackageImpl theOperatorPackage =
        (OperatorPackageImpl)
            (EPackage.Registry.INSTANCE.getEPackage(OperatorPackage.eNS_URI)
                    instanceof OperatorPackageImpl
                ? EPackage.Registry.INSTANCE.getEPackage(OperatorPackage.eNS_URI)
                : OperatorPackage.eINSTANCE);

    // Create package meta-data objects
    theCommonPackage.createPackageContents();
    theOperatorPackage.createPackageContents();

    // Initialize created meta-data
    theCommonPackage.initializePackageContents();
    theOperatorPackage.initializePackageContents();

    // Register package validator
    EValidator.Registry.INSTANCE.put(
        theCommonPackage,
        new EValidator.Descriptor() {
          public EValidator getEValidator() {
            return CommonValidator.INSTANCE;
          }
        });

    // Mark meta-data to indicate it can't be changed
    theCommonPackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(CommonPackage.eNS_URI, theCommonPackage);
    return theCommonPackage;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getDescriptionType() {
    return descriptionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getDescriptionType_Value() {
    return (EAttribute) descriptionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getDescriptionType_DocHref() {
    return (EAttribute) descriptionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getDescriptionType_SampleUri() {
    return (EAttribute) descriptionTypeEClass.getEStructuralFeatures().get(2);
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
  public EReference getDocumentRoot_JavaOpLibrary() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getDocumentRoot_Library() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(4);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpLibraryType() {
    return javaOpLibraryTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpLibraryType_Description() {
    return (EReference) javaOpLibraryTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getJavaOpLibraryType_ManagedLibrary() {
    return (EReference) javaOpLibraryTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getJavaOpManagedLibraryType() {
    return javaOpManagedLibraryTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpManagedLibraryType_LibPath() {
    return (EAttribute) javaOpManagedLibraryTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getJavaOpManagedLibraryType_Command() {
    return (EAttribute) javaOpManagedLibraryTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getLibraryType() {
    return libraryTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getLibraryType_Description() {
    return (EReference) libraryTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getLibraryType_ManagedLibrary() {
    return (EReference) libraryTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getManagedLibraryType() {
    return managedLibraryTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getManagedLibraryType_Lib() {
    return (EAttribute) managedLibraryTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getManagedLibraryType_LibPath() {
    return (EAttribute) managedLibraryTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getManagedLibraryType_IncludePath() {
    return (EAttribute) managedLibraryTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getManagedLibraryType_Command() {
    return (EAttribute) managedLibraryTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getToolkitDependencyType() {
    return toolkitDependencyTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitDependencyType_Name() {
    return (EAttribute) toolkitDependencyTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getToolkitDependencyType_Version() {
    return (EAttribute) toolkitDependencyTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getUriType() {
    return uriTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getUriType_Index() {
    return (EAttribute) uriTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getUriType_Value() {
    return (EAttribute) uriTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getVersionRangeType() {
    return versionRangeTypeEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EDataType getVersionType() {
    return versionTypeEDataType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonFactory getCommonFactory() {
    return (CommonFactory) getEFactoryInstance();
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
    descriptionTypeEClass = createEClass(DESCRIPTION_TYPE);
    createEAttribute(descriptionTypeEClass, DESCRIPTION_TYPE__VALUE);
    createEAttribute(descriptionTypeEClass, DESCRIPTION_TYPE__DOC_HREF);
    createEAttribute(descriptionTypeEClass, DESCRIPTION_TYPE__SAMPLE_URI);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__JAVA_OP_LIBRARY);
    createEReference(documentRootEClass, DOCUMENT_ROOT__LIBRARY);

    javaOpLibraryTypeEClass = createEClass(JAVA_OP_LIBRARY_TYPE);
    createEReference(javaOpLibraryTypeEClass, JAVA_OP_LIBRARY_TYPE__DESCRIPTION);
    createEReference(javaOpLibraryTypeEClass, JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY);

    javaOpManagedLibraryTypeEClass = createEClass(JAVA_OP_MANAGED_LIBRARY_TYPE);
    createEAttribute(javaOpManagedLibraryTypeEClass, JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH);
    createEAttribute(javaOpManagedLibraryTypeEClass, JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND);

    libraryTypeEClass = createEClass(LIBRARY_TYPE);
    createEReference(libraryTypeEClass, LIBRARY_TYPE__DESCRIPTION);
    createEReference(libraryTypeEClass, LIBRARY_TYPE__MANAGED_LIBRARY);

    managedLibraryTypeEClass = createEClass(MANAGED_LIBRARY_TYPE);
    createEAttribute(managedLibraryTypeEClass, MANAGED_LIBRARY_TYPE__LIB);
    createEAttribute(managedLibraryTypeEClass, MANAGED_LIBRARY_TYPE__LIB_PATH);
    createEAttribute(managedLibraryTypeEClass, MANAGED_LIBRARY_TYPE__INCLUDE_PATH);
    createEAttribute(managedLibraryTypeEClass, MANAGED_LIBRARY_TYPE__COMMAND);

    toolkitDependencyTypeEClass = createEClass(TOOLKIT_DEPENDENCY_TYPE);
    createEAttribute(toolkitDependencyTypeEClass, TOOLKIT_DEPENDENCY_TYPE__NAME);
    createEAttribute(toolkitDependencyTypeEClass, TOOLKIT_DEPENDENCY_TYPE__VERSION);

    uriTypeEClass = createEClass(URI_TYPE);
    createEAttribute(uriTypeEClass, URI_TYPE__INDEX);
    createEAttribute(uriTypeEClass, URI_TYPE__VALUE);

    // Create data types
    versionRangeTypeEDataType = createEDataType(VERSION_RANGE_TYPE);
    versionTypeEDataType = createEDataType(VERSION_TYPE);
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

    // Initialize classes, features, and operations; add parameters
    initEClass(
        descriptionTypeEClass,
        DescriptionType.class,
        "DescriptionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getDescriptionType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        0,
        1,
        DescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getDescriptionType_DocHref(),
        theXMLTypePackage.getString(),
        "docHref",
        null,
        0,
        1,
        DescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getDescriptionType_SampleUri(),
        theXMLTypePackage.getString(),
        "sampleUri",
        null,
        0,
        1,
        DescriptionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
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
        getDocumentRoot_JavaOpLibrary(),
        this.getJavaOpLibraryType(),
        null,
        "javaOpLibrary",
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
    initEReference(
        getDocumentRoot_Library(),
        this.getLibraryType(),
        null,
        "library",
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
        javaOpLibraryTypeEClass,
        JavaOpLibraryType.class,
        "JavaOpLibraryType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getJavaOpLibraryType_Description(),
        this.getDescriptionType(),
        null,
        "description",
        null,
        1,
        1,
        JavaOpLibraryType.class,
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
        getJavaOpLibraryType_ManagedLibrary(),
        this.getJavaOpManagedLibraryType(),
        null,
        "managedLibrary",
        null,
        1,
        1,
        JavaOpLibraryType.class,
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
        javaOpManagedLibraryTypeEClass,
        JavaOpManagedLibraryType.class,
        "JavaOpManagedLibraryType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getJavaOpManagedLibraryType_LibPath(),
        theXMLTypePackage.getString(),
        "libPath",
        null,
        0,
        -1,
        JavaOpManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getJavaOpManagedLibraryType_Command(),
        theXMLTypePackage.getString(),
        "command",
        null,
        0,
        1,
        JavaOpManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        libraryTypeEClass,
        LibraryType.class,
        "LibraryType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getLibraryType_Description(),
        this.getDescriptionType(),
        null,
        "description",
        null,
        1,
        1,
        LibraryType.class,
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
        getLibraryType_ManagedLibrary(),
        this.getManagedLibraryType(),
        null,
        "managedLibrary",
        null,
        1,
        1,
        LibraryType.class,
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
        managedLibraryTypeEClass,
        ManagedLibraryType.class,
        "ManagedLibraryType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getManagedLibraryType_Lib(),
        theXMLTypePackage.getString(),
        "lib",
        null,
        0,
        -1,
        ManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getManagedLibraryType_LibPath(),
        theXMLTypePackage.getString(),
        "libPath",
        null,
        0,
        -1,
        ManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getManagedLibraryType_IncludePath(),
        theXMLTypePackage.getString(),
        "includePath",
        null,
        0,
        -1,
        ManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        !IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getManagedLibraryType_Command(),
        theXMLTypePackage.getString(),
        "command",
        null,
        0,
        1,
        ManagedLibraryType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        toolkitDependencyTypeEClass,
        ToolkitDependencyType.class,
        "ToolkitDependencyType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getToolkitDependencyType_Name(),
        theXMLTypePackage.getString(),
        "name",
        null,
        1,
        1,
        ToolkitDependencyType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getToolkitDependencyType_Version(),
        this.getVersionRangeType(),
        "version",
        null,
        1,
        1,
        ToolkitDependencyType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        uriTypeEClass,
        UriType.class,
        "UriType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getUriType_Index(),
        theXMLTypePackage.getInt(),
        "index",
        null,
        1,
        1,
        UriType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getUriType_Value(),
        theXMLTypePackage.getString(),
        "value",
        null,
        1,
        1,
        UriType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    // Initialize data types
    initEDataType(
        versionRangeTypeEDataType,
        String.class,
        "VersionRangeType",
        IS_SERIALIZABLE,
        !IS_GENERATED_INSTANCE_CLASS);
    initEDataType(
        versionTypeEDataType,
        String.class,
        "VersionType",
        IS_SERIALIZABLE,
        !IS_GENERATED_INSTANCE_CLASS);

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
        descriptionTypeEClass,
        source,
        new String[] {
          "name", "descriptionType",
          "kind", "simple"
        });
    addAnnotation(
        getDescriptionType_Value(),
        source,
        new String[] {
          "name", ":0",
          "kind", "simple"
        });
    addAnnotation(
        getDescriptionType_DocHref(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "docHref"
        });
    addAnnotation(
        getDescriptionType_SampleUri(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "sampleUri"
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
        getDocumentRoot_JavaOpLibrary(),
        source,
        new String[] {
          "kind", "element",
          "name", "javaOpLibrary",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getDocumentRoot_Library(),
        source,
        new String[] {
          "kind", "element",
          "name", "library",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpLibraryTypeEClass,
        source,
        new String[] {
          "name", "javaOpLibraryType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpLibraryType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpLibraryType_ManagedLibrary(),
        source,
        new String[] {
          "kind", "element",
          "name", "managedLibrary",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        javaOpManagedLibraryTypeEClass,
        source,
        new String[] {
          "name", "javaOpManagedLibraryType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getJavaOpManagedLibraryType_LibPath(),
        source,
        new String[] {
          "kind", "element",
          "name", "libPath",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getJavaOpManagedLibraryType_Command(),
        source,
        new String[] {
          "kind", "element",
          "name", "command",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        libraryTypeEClass,
        source,
        new String[] {
          "name", "libraryType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getLibraryType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getLibraryType_ManagedLibrary(),
        source,
        new String[] {
          "kind", "element",
          "name", "managedLibrary",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        managedLibraryTypeEClass,
        source,
        new String[] {
          "name", "managedLibraryType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getManagedLibraryType_Lib(),
        source,
        new String[] {
          "kind", "element",
          "name", "lib",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getManagedLibraryType_LibPath(),
        source,
        new String[] {
          "kind", "element",
          "name", "libPath",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getManagedLibraryType_IncludePath(),
        source,
        new String[] {
          "kind", "element",
          "name", "includePath",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getManagedLibraryType_Command(),
        source,
        new String[] {
          "kind", "element",
          "name", "command",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        toolkitDependencyTypeEClass,
        source,
        new String[] {
          "name", "toolkitDependencyType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getToolkitDependencyType_Name(),
        source,
        new String[] {
          "kind", "element",
          "name", "name",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getToolkitDependencyType_Version(),
        source,
        new String[] {
          "kind", "element",
          "name", "version",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        uriTypeEClass,
        source,
        new String[] {
          "name", "uriType",
          "kind", "empty"
        });
    addAnnotation(
        getUriType_Index(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "index"
        });
    addAnnotation(
        getUriType_Value(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "value"
        });
    addAnnotation(
        versionRangeTypeEDataType,
        source,
        new String[] {
          "name", "versionRangeType",
          "baseType", "http://www.eclipse.org/emf/2003/XMLType#string",
          "pattern",
              "(\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?)|([\\[\\(]\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?,\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?[\\)\\]])"
        });
    addAnnotation(
        versionTypeEDataType,
        source,
        new String[] {
          "name", "versionType",
          "baseType", "http://www.eclipse.org/emf/2003/XMLType#string",
          "pattern", "\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?"
        });
  }
} // CommonPackageImpl
