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

import com.ibm.xmlns.prod.streams.spl.toolkit.info.*;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.impl.EFactoryImpl;
import org.eclipse.emf.ecore.plugin.EcorePlugin;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model <b>Factory</b>.
 * <!-- end-user-doc -->
 *
 * @generated
 */
public class InfoFactoryImpl extends EFactoryImpl implements InfoFactory {
  /**
   * Creates the default factory implementation.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static InfoFactory init() {
    try {
      InfoFactory theInfoFactory =
          (InfoFactory)
              EPackage.Registry.INSTANCE.getEFactory(
                  "http://www.ibm.com/xmlns/prod/streams/spl/toolkitInfo");
      if (theInfoFactory != null) {
        return theInfoFactory;
      }
    } catch (Exception exception) {
      EcorePlugin.INSTANCE.log(exception);
    }
    return new InfoFactoryImpl();
  }

  /**
   * Creates an instance of the factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InfoFactoryImpl() {
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
      case InfoPackage.DEPENDENCIES_TYPE:
        return createDependenciesType();
      case InfoPackage.DOCUMENT_ROOT:
        return createDocumentRoot();
      case InfoPackage.EXCLUDE_TYPE:
        return createExcludeType();
      case InfoPackage.IDENTITY_TYPE:
        return createIdentityType();
      case InfoPackage.INCLUDE_TYPE:
        return createIncludeType();
      case InfoPackage.LANG_TYPE:
        return createLangType();
      case InfoPackage.MESSAGE_SET_TYPE:
        return createMessageSetType();
      case InfoPackage.RESOURCES_TYPE:
        return createResourcesType();
      case InfoPackage.SAB_FILES_TYPE:
        return createSabFilesType();
      case InfoPackage.TOOLKIT_INFO_MODEL_TYPE:
        return createToolkitInfoModelType();
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
      case InfoPackage.ROOT_TYPE:
        return createRootTypeFromString(eDataType, initialValue);
      case InfoPackage.ROOT_TYPE_OBJECT:
        return createRootTypeObjectFromString(eDataType, initialValue);
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
      case InfoPackage.ROOT_TYPE:
        return convertRootTypeToString(eDataType, instanceValue);
      case InfoPackage.ROOT_TYPE_OBJECT:
        return convertRootTypeObjectToString(eDataType, instanceValue);
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
  public DependenciesType createDependenciesType() {
    DependenciesTypeImpl dependenciesType = new DependenciesTypeImpl();
    return dependenciesType;
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
  public ExcludeType createExcludeType() {
    ExcludeTypeImpl excludeType = new ExcludeTypeImpl();
    return excludeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IdentityType createIdentityType() {
    IdentityTypeImpl identityType = new IdentityTypeImpl();
    return identityType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public IncludeType createIncludeType() {
    IncludeTypeImpl includeType = new IncludeTypeImpl();
    return includeType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public LangType createLangType() {
    LangTypeImpl langType = new LangTypeImpl();
    return langType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public MessageSetType createMessageSetType() {
    MessageSetTypeImpl messageSetType = new MessageSetTypeImpl();
    return messageSetType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ResourcesType createResourcesType() {
    ResourcesTypeImpl resourcesType = new ResourcesTypeImpl();
    return resourcesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SabFilesType createSabFilesType() {
    SabFilesTypeImpl sabFilesType = new SabFilesTypeImpl();
    return sabFilesType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public ToolkitInfoModelType createToolkitInfoModelType() {
    ToolkitInfoModelTypeImpl toolkitInfoModelType = new ToolkitInfoModelTypeImpl();
    return toolkitInfoModelType;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public RootType createRootTypeFromString(EDataType eDataType, String initialValue) {
    RootType result = RootType.get(initialValue);
    if (result == null)
      throw new IllegalArgumentException(
          "The value '"
              + initialValue
              + "' is not a valid enumerator of '"
              + eDataType.getName()
              + "'");
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertRootTypeToString(EDataType eDataType, Object instanceValue) {
    return instanceValue == null ? null : instanceValue.toString();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public RootType createRootTypeObjectFromString(EDataType eDataType, String initialValue) {
    return createRootTypeFromString(InfoPackage.Literals.ROOT_TYPE, initialValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String convertRootTypeObjectToString(EDataType eDataType, Object instanceValue) {
    return convertRootTypeToString(InfoPackage.Literals.ROOT_TYPE, instanceValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InfoPackage getInfoPackage() {
    return (InfoPackage) getEPackage();
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
  public static InfoPackage getPackage() {
    return InfoPackage.eINSTANCE;
  }
} // InfoFactoryImpl
