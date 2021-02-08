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
package com.ibm.xmlns.prod.streams.spl.function.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl;
import com.ibm.xmlns.prod.streams.spl.function.DependenciesType;
import com.ibm.xmlns.prod.streams.spl.function.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.function.FunctionFactory;
import com.ibm.xmlns.prod.streams.spl.function.FunctionModelType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionPackage;
import com.ibm.xmlns.prod.streams.spl.function.FunctionSetType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionsType;
import com.ibm.xmlns.prod.streams.spl.function.PrototypeType;
import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
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
public class FunctionPackageImpl extends EPackageImpl implements FunctionPackage {
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
  private EClass functionModelTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionSetTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionsTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass functionTypeEClass = null;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private EClass prototypeTypeEClass = null;

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
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#eNS_URI
   * @see #init()
   * @generated
   */
  private FunctionPackageImpl() {
    super(eNS_URI, FunctionFactory.eINSTANCE);
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
   * <p>This method is used to initialize {@link FunctionPackage#eINSTANCE} when that field is
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
  public static FunctionPackage init() {
    if (isInited)
      return (FunctionPackage) EPackage.Registry.INSTANCE.getEPackage(FunctionPackage.eNS_URI);

    // Obtain or create and register package
    FunctionPackageImpl theFunctionPackage =
        (FunctionPackageImpl)
            (EPackage.Registry.INSTANCE.get(eNS_URI) instanceof FunctionPackageImpl
                ? EPackage.Registry.INSTANCE.get(eNS_URI)
                : new FunctionPackageImpl());

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
    theFunctionPackage.createPackageContents();
    theCommonPackage.createPackageContents();

    // Initialize created meta-data
    theFunctionPackage.initializePackageContents();
    theCommonPackage.initializePackageContents();

    // Mark meta-data to indicate it can't be changed
    theFunctionPackage.freeze();

    // Update the registry and return the package
    EPackage.Registry.INSTANCE.put(FunctionPackage.eNS_URI, theFunctionPackage);
    return theFunctionPackage;
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
  public EReference getDependenciesType_Library() {
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
  public EReference getDocumentRoot_FunctionModel() {
    return (EReference) documentRootEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionModelType() {
    return functionModelTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionModelType_FunctionSet() {
    return (EReference) functionModelTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionSetType() {
    return functionSetTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionSetType_HeaderFileName() {
    return (EAttribute) functionSetTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionSetType_CppNamespaceName() {
    return (EAttribute) functionSetTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionSetType_Functions() {
    return (EReference) functionSetTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionSetType_Dependencies() {
    return (EReference) functionSetTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionsType() {
    return functionsTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionsType_Function() {
    return (EReference) functionsTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getFunctionType() {
    return functionTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionType_Description() {
    return (EReference) functionTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EReference getFunctionType_Prototype() {
    return (EReference) functionTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getFunctionType_JavaFunctionName() {
    return (EAttribute) functionTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EClass getPrototypeType() {
    return prototypeTypeEClass;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrototypeType_Value() {
    return (EAttribute) prototypeTypeEClass.getEStructuralFeatures().get(0);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrototypeType_Column() {
    return (EAttribute) prototypeTypeEClass.getEStructuralFeatures().get(1);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrototypeType_CppName() {
    return (EAttribute) prototypeTypeEClass.getEStructuralFeatures().get(2);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EAttribute getPrototypeType_Line() {
    return (EAttribute) prototypeTypeEClass.getEStructuralFeatures().get(3);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public FunctionFactory getFunctionFactory() {
    return (FunctionFactory) getEFactoryInstance();
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
    createEReference(dependenciesTypeEClass, DEPENDENCIES_TYPE__LIBRARY);

    documentRootEClass = createEClass(DOCUMENT_ROOT);
    createEAttribute(documentRootEClass, DOCUMENT_ROOT__MIXED);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XMLNS_PREFIX_MAP);
    createEReference(documentRootEClass, DOCUMENT_ROOT__XSI_SCHEMA_LOCATION);
    createEReference(documentRootEClass, DOCUMENT_ROOT__FUNCTION_MODEL);

    functionModelTypeEClass = createEClass(FUNCTION_MODEL_TYPE);
    createEReference(functionModelTypeEClass, FUNCTION_MODEL_TYPE__FUNCTION_SET);

    functionSetTypeEClass = createEClass(FUNCTION_SET_TYPE);
    createEAttribute(functionSetTypeEClass, FUNCTION_SET_TYPE__HEADER_FILE_NAME);
    createEAttribute(functionSetTypeEClass, FUNCTION_SET_TYPE__CPP_NAMESPACE_NAME);
    createEReference(functionSetTypeEClass, FUNCTION_SET_TYPE__FUNCTIONS);
    createEReference(functionSetTypeEClass, FUNCTION_SET_TYPE__DEPENDENCIES);

    functionsTypeEClass = createEClass(FUNCTIONS_TYPE);
    createEReference(functionsTypeEClass, FUNCTIONS_TYPE__FUNCTION);

    functionTypeEClass = createEClass(FUNCTION_TYPE);
    createEReference(functionTypeEClass, FUNCTION_TYPE__DESCRIPTION);
    createEReference(functionTypeEClass, FUNCTION_TYPE__PROTOTYPE);
    createEAttribute(functionTypeEClass, FUNCTION_TYPE__JAVA_FUNCTION_NAME);

    prototypeTypeEClass = createEClass(PROTOTYPE_TYPE);
    createEAttribute(prototypeTypeEClass, PROTOTYPE_TYPE__VALUE);
    createEAttribute(prototypeTypeEClass, PROTOTYPE_TYPE__COLUMN);
    createEAttribute(prototypeTypeEClass, PROTOTYPE_TYPE__CPP_NAME);
    createEAttribute(prototypeTypeEClass, PROTOTYPE_TYPE__LINE);
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
        getDependenciesType_Library(),
        theCommonPackage.getLibraryType(),
        null,
        "library",
        null,
        1,
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
        getDocumentRoot_FunctionModel(),
        this.getFunctionModelType(),
        null,
        "functionModel",
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
        functionModelTypeEClass,
        FunctionModelType.class,
        "FunctionModelType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionModelType_FunctionSet(),
        this.getFunctionSetType(),
        null,
        "functionSet",
        null,
        1,
        -1,
        FunctionModelType.class,
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
        functionSetTypeEClass,
        FunctionSetType.class,
        "FunctionSetType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getFunctionSetType_HeaderFileName(),
        theXMLTypePackage.getString(),
        "headerFileName",
        null,
        0,
        1,
        FunctionSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getFunctionSetType_CppNamespaceName(),
        theXMLTypePackage.getString(),
        "cppNamespaceName",
        null,
        0,
        1,
        FunctionSetType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEReference(
        getFunctionSetType_Functions(),
        this.getFunctionsType(),
        null,
        "functions",
        null,
        1,
        1,
        FunctionSetType.class,
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
        getFunctionSetType_Dependencies(),
        this.getDependenciesType(),
        null,
        "dependencies",
        null,
        0,
        1,
        FunctionSetType.class,
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
        functionsTypeEClass,
        FunctionsType.class,
        "FunctionsType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionsType_Function(),
        this.getFunctionType(),
        null,
        "function",
        null,
        1,
        -1,
        FunctionsType.class,
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
        functionTypeEClass,
        FunctionType.class,
        "FunctionType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEReference(
        getFunctionType_Description(),
        theCommonPackage.getDescriptionType(),
        null,
        "description",
        null,
        0,
        1,
        FunctionType.class,
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
        getFunctionType_Prototype(),
        this.getPrototypeType(),
        null,
        "prototype",
        null,
        1,
        1,
        FunctionType.class,
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
        getFunctionType_JavaFunctionName(),
        theXMLTypePackage.getString(),
        "javaFunctionName",
        null,
        0,
        1,
        FunctionType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

    initEClass(
        prototypeTypeEClass,
        PrototypeType.class,
        "PrototypeType",
        !IS_ABSTRACT,
        !IS_INTERFACE,
        IS_GENERATED_INSTANCE_CLASS);
    initEAttribute(
        getPrototypeType_Value(),
        theXMLTypePackage.getToken(),
        "value",
        null,
        0,
        1,
        PrototypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrototypeType_Column(),
        theXMLTypePackage.getUnsignedLong(),
        "column",
        null,
        0,
        1,
        PrototypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrototypeType_CppName(),
        theXMLTypePackage.getToken(),
        "cppName",
        null,
        0,
        1,
        PrototypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);
    initEAttribute(
        getPrototypeType_Line(),
        theXMLTypePackage.getUnsignedLong(),
        "line",
        null,
        0,
        1,
        PrototypeType.class,
        !IS_TRANSIENT,
        !IS_VOLATILE,
        IS_CHANGEABLE,
        !IS_UNSETTABLE,
        !IS_ID,
        IS_UNIQUE,
        !IS_DERIVED,
        IS_ORDERED);

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
        getDependenciesType_Library(),
        source,
        new String[] {
          "kind", "element",
          "name", "library",
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
        getDocumentRoot_FunctionModel(),
        source,
        new String[] {
          "kind", "element",
          "name", "functionModel",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionModelTypeEClass,
        source,
        new String[] {
          "name", "functionModelType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionModelType_FunctionSet(),
        source,
        new String[] {
          "kind", "element",
          "name", "functionSet",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionSetTypeEClass,
        source,
        new String[] {
          "name", "functionSetType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionSetType_HeaderFileName(),
        source,
        new String[] {
          "kind", "element",
          "name", "headerFileName",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionSetType_CppNamespaceName(),
        source,
        new String[] {
          "kind", "element",
          "name", "cppNamespaceName",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionSetType_Functions(),
        source,
        new String[] {
          "kind", "element",
          "name", "functions",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionSetType_Dependencies(),
        source,
        new String[] {
          "kind", "element",
          "name", "dependencies",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionsTypeEClass,
        source,
        new String[] {
          "name", "functionsType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionsType_Function(),
        source,
        new String[] {
          "kind", "element",
          "name", "function",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        functionTypeEClass,
        source,
        new String[] {
          "name", "functionType",
          "kind", "elementOnly"
        });
    addAnnotation(
        getFunctionType_Description(),
        source,
        new String[] {
          "kind", "element",
          "name", "description",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionType_Prototype(),
        source,
        new String[] {
          "kind", "element",
          "name", "prototype",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        getFunctionType_JavaFunctionName(),
        source,
        new String[] {
          "kind", "element",
          "name", "javaFunctionName",
          "namespace", "##targetNamespace"
        });
    addAnnotation(
        prototypeTypeEClass,
        source,
        new String[] {
          "name", "prototypeType",
          "kind", "simple"
        });
    addAnnotation(
        getPrototypeType_Value(),
        source,
        new String[] {
          "name", ":0",
          "kind", "simple"
        });
    addAnnotation(
        getPrototypeType_Column(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "column"
        });
    addAnnotation(
        getPrototypeType_CppName(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "cppName"
        });
    addAnnotation(
        getPrototypeType_Line(),
        source,
        new String[] {
          "kind", "attribute",
          "name", "line"
        });
  }
} // FunctionPackageImpl
