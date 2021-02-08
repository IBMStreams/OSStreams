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

import com.ibm.xmlns.prod.streams.spl.common.*;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.impl.EFactoryImpl;
import org.eclipse.emf.ecore.plugin.EcorePlugin;
import org.eclipse.emf.ecore.xml.type.XMLTypeFactory;
import org.eclipse.emf.ecore.xml.type.XMLTypePackage;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public class CommonFactoryImpl extends EFactoryImpl implements CommonFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static CommonFactory init() {
    try {
      CommonFactory theCommonFactory =
          (CommonFactory) EPackage.Registry.INSTANCE.getEFactory(CommonPackage.eNS_URI);
      if (theCommonFactory != null) {
        return theCommonFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new CommonFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonFactoryImpl() {
    super();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public EObject create(EClass eClass) {
    switch (eClass.getClassifierID()) {
      case CommonPackage.DESCRIPTION_TYPE:
        return createDescriptionType();
      case CommonPackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case CommonPackage.JAVA_OP_LIBRARY_TYPE:
        return createJavaOpLibraryType();
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE:
        return createJavaOpManagedLibraryType();
      case CommonPackage.LIBRARY_TYPE:
        return createLibraryType();
      case CommonPackage.MANAGED_LIBRARY_TYPE:
        return createManagedLibraryType();
      case CommonPackage.TOOLKIT_DEPENDENCY_TYPE:
        return createToolkitDependencyType();
      case CommonPackage.URI_TYPE:
        return createUriType();
      default:
        throw new IllegalArgumentException(
            "The class '" + eClass.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public Object createFromString(EDataType eDataType, String initialValue) {
    switch (eDataType.getClassifierID()) {
      case CommonPackage.VERSION_RANGE_TYPE:
        return createVersionRangeTypeFromString(eDataType, initialValue);
      case CommonPackage.VERSION_TYPE:
        return createVersionTypeFromString(eDataType, initialValue);
      default:
        throw new IllegalArgumentException(
            "The datatype '" + eDataType.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public String convertToString(EDataType eDataType, Object instanceValue) {
    switch (eDataType.getClassifierID()) {
      case CommonPackage.VERSION_RANGE_TYPE:
        return convertVersionRangeTypeToString(eDataType, instanceValue);
      case CommonPackage.VERSION_TYPE:
        return convertVersionTypeToString(eDataType, instanceValue);
      default:
        throw new IllegalArgumentException(
            "The datatype '" + eDataType.getName() + "' is not a valid classifier");
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DescriptionType createDescriptionType() {
    DescriptionTypeImpl descriptionType = new DescriptionTypeImpl();
    return descriptionType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DocumentRoot createDocumentRoot() {
    DocumentRootImpl documentRoot = new DocumentRootImpl();
    return documentRoot;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpLibraryType createJavaOpLibraryType() {
    JavaOpLibraryTypeImpl javaOpLibraryType = new JavaOpLibraryTypeImpl();
    return javaOpLibraryType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public JavaOpManagedLibraryType createJavaOpManagedLibraryType() {
    JavaOpManagedLibraryTypeImpl javaOpManagedLibraryType = new JavaOpManagedLibraryTypeImpl();
    return javaOpManagedLibraryType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LibraryType createLibraryType() {
    LibraryTypeImpl libraryType = new LibraryTypeImpl();
    return libraryType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ManagedLibraryType createManagedLibraryType() {
    ManagedLibraryTypeImpl managedLibraryType = new ManagedLibraryTypeImpl();
    return managedLibraryType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitDependencyType createToolkitDependencyType() {
    ToolkitDependencyTypeImpl toolkitDependencyType = new ToolkitDependencyTypeImpl();
    return toolkitDependencyType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UriType createUriType() {
    UriTypeImpl uriType = new UriTypeImpl();
    return uriType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String createVersionRangeTypeFromString(EDataType eDataType, String initialValue) {
    return (String)
        XMLTypeFactory.eINSTANCE.createFromString(XMLTypePackage.Literals.STRING, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertVersionRangeTypeToString(EDataType eDataType, Object instanceValue) {
    return XMLTypeFactory.eINSTANCE.convertToString(XMLTypePackage.Literals.STRING, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String createVersionTypeFromString(EDataType eDataType, String initialValue) {
    return (String)
        XMLTypeFactory.eINSTANCE.createFromString(XMLTypePackage.Literals.STRING, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertVersionTypeToString(EDataType eDataType, Object instanceValue) {
    return XMLTypeFactory.eINSTANCE.convertToString(XMLTypePackage.Literals.STRING, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonPackage getCommonPackage() {
    return (CommonPackage) getEPackage();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @deprecated
   * @generated
   */
  @Deprecated
  public static CommonPackage getPackage() {
    return CommonPackage.eINSTANCE;
  }
} // CommonFactoryImpl
