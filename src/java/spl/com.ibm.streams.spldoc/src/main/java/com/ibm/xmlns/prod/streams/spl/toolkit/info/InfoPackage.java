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
package com.ibm.xmlns.prod.streams.spl.toolkit.info;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
import org.eclipse.emf.ecore.EEnum;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.EReference;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Package</b> for the model. It contains accessors for the meta objects to represent
 *
 * <ul>
 *   <li>each class,
 *   <li>each feature of each class,
 *   <li>each enum,
 *   <li>and each data type
 * </ul>
 *
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoFactory
 * @model kind="package"
 * @generated
 */
public interface InfoPackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "info";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/toolkitInfo";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "info";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  InfoPackage eINSTANCE = com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DependenciesTypeImpl <em>Dependencies
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DependenciesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getDependenciesType()
   * @generated
   */
  int DEPENDENCIES_TYPE = 0;

  /**
   * The feature id for the '<em><b>Toolkit</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEPENDENCIES_TYPE__TOOLKIT = 0;

  /**
   * The number of structural features of the '<em>Dependencies Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DEPENDENCIES_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DocumentRootImpl <em>Document Root</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getDocumentRoot()
   * @generated
   */
  int DOCUMENT_ROOT = 1;

  /**
   * The feature id for the '<em><b>Mixed</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__MIXED = 0;

  /**
   * The feature id for the '<em><b>XMLNS Prefix Map</b></em>' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__XMLNS_PREFIX_MAP = 1;

  /**
   * The feature id for the '<em><b>XSI Schema Location</b></em>' map.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__XSI_SCHEMA_LOCATION = 2;

  /**
   * The feature id for the '<em><b>Toolkit Info Model</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__TOOLKIT_INFO_MODEL = 3;

  /**
   * The number of structural features of the '<em>Document Root</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ExcludeTypeImpl <em>Exclude Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ExcludeTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getExcludeType()
   * @generated
   */
  int EXCLUDE_TYPE = 2;

  /**
   * The feature id for the '<em><b>Path</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXCLUDE_TYPE__PATH = 0;

  /**
   * The number of structural features of the '<em>Exclude Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int EXCLUDE_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IdentityTypeImpl <em>Identity Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IdentityTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getIdentityType()
   * @generated
   */
  int IDENTITY_TYPE = 3;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IDENTITY_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IDENTITY_TYPE__DESCRIPTION = 1;

  /**
   * The feature id for the '<em><b>Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IDENTITY_TYPE__VERSION = 2;

  /**
   * The feature id for the '<em><b>Required Product Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IDENTITY_TYPE__REQUIRED_PRODUCT_VERSION = 3;

  /**
   * The number of structural features of the '<em>Identity Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int IDENTITY_TYPE_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl <em>Include Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getIncludeType()
   * @generated
   */
  int INCLUDE_TYPE = 4;

  /**
   * The feature id for the '<em><b>Path</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INCLUDE_TYPE__PATH = 0;

  /**
   * The feature id for the '<em><b>Root</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INCLUDE_TYPE__ROOT = 1;

  /**
   * The number of structural features of the '<em>Include Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int INCLUDE_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.LangTypeImpl <em>Lang Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.LangTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getLangType()
   * @generated
   */
  int LANG_TYPE = 5;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LANG_TYPE__VALUE = 0;

  /**
   * The feature id for the '<em><b>Default</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LANG_TYPE__DEFAULT = 1;

  /**
   * The number of structural features of the '<em>Lang Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LANG_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.MessageSetTypeImpl <em>Message Set Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.MessageSetTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getMessageSetType()
   * @generated
   */
  int MESSAGE_SET_TYPE = 6;

  /**
   * The feature id for the '<em><b>Lang</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MESSAGE_SET_TYPE__LANG = 0;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MESSAGE_SET_TYPE__NAME = 1;

  /**
   * The number of structural features of the '<em>Message Set Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MESSAGE_SET_TYPE_FEATURE_COUNT = 2;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ResourcesTypeImpl <em>Resources Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ResourcesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getResourcesType()
   * @generated
   */
  int RESOURCES_TYPE = 7;

  /**
   * The feature id for the '<em><b>Message Set</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int RESOURCES_TYPE__MESSAGE_SET = 0;

  /**
   * The number of structural features of the '<em>Resources Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int RESOURCES_TYPE_FEATURE_COUNT = 1;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl <em>Sab Files Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getSabFilesType()
   * @generated
   */
  int SAB_FILES_TYPE = 8;

  /**
   * The feature id for the '<em><b>Group</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SAB_FILES_TYPE__GROUP = 0;

  /**
   * The feature id for the '<em><b>Include</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SAB_FILES_TYPE__INCLUDE = 1;

  /**
   * The feature id for the '<em><b>Exclude</b></em>' containment reference list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SAB_FILES_TYPE__EXCLUDE = 2;

  /**
   * The number of structural features of the '<em>Sab Files Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int SAB_FILES_TYPE_FEATURE_COUNT = 3;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl <em>Toolkit Info
   * Model Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getToolkitInfoModelType()
   * @generated
   */
  int TOOLKIT_INFO_MODEL_TYPE = 9;

  /**
   * The feature id for the '<em><b>Identity</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_INFO_MODEL_TYPE__IDENTITY = 0;

  /**
   * The feature id for the '<em><b>Dependencies</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES = 1;

  /**
   * The feature id for the '<em><b>Resources</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_INFO_MODEL_TYPE__RESOURCES = 2;

  /**
   * The feature id for the '<em><b>Sab Files</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_INFO_MODEL_TYPE__SAB_FILES = 3;

  /**
   * The number of structural features of the '<em>Toolkit Info Model Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_INFO_MODEL_TYPE_FEATURE_COUNT = 4;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * <em>Root Type</em>}' enum.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getRootType()
   * @generated
   */
  int ROOT_TYPE = 10;

  /**
   * The meta object id for the '<em>Root Type Object</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getRootTypeObject()
   * @generated
   */
  int ROOT_TYPE_OBJECT = 11;

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType <em>Dependencies Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Dependencies Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType
   * @generated
   */
  EClass getDependenciesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType#getToolkit <em>Toolkit</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Toolkit</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType#getToolkit()
   * @see #getDependenciesType()
   * @generated
   */
  EReference getDependenciesType_Toolkit();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getToolkitInfoModel <em>Toolkit Info
   * Model</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Toolkit Info Model</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot#getToolkitInfoModel()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_ToolkitInfoModel();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType <em>Exclude Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Exclude Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType
   * @generated
   */
  EClass getExcludeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType#getPath <em>Path</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Path</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType#getPath()
   * @see #getExcludeType()
   * @generated
   */
  EAttribute getExcludeType_Path();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType <em>Identity Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Identity Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType
   * @generated
   */
  EClass getIdentityType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getName()
   * @see #getIdentityType()
   * @generated
   */
  EAttribute getIdentityType_Name();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getDescription()
   * @see #getIdentityType()
   * @generated
   */
  EReference getIdentityType_Description();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getVersion <em>Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getVersion()
   * @see #getIdentityType()
   * @generated
   */
  EAttribute getIdentityType_Version();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getRequiredProductVersion <em>Required
   * Product Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Required Product Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getRequiredProductVersion()
   * @see #getIdentityType()
   * @generated
   */
  EAttribute getIdentityType_RequiredProductVersion();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType <em>Include Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Include Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType
   * @generated
   */
  EClass getIncludeType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getPath <em>Path</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Path</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getPath()
   * @see #getIncludeType()
   * @generated
   */
  EAttribute getIncludeType_Path();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot <em>Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType#getRoot()
   * @see #getIncludeType()
   * @generated
   */
  EAttribute getIncludeType_Root();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType
   * <em>Lang Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Lang Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType
   * @generated
   */
  EClass getLangType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#getValue()
   * @see #getLangType()
   * @generated
   */
  EAttribute getLangType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault <em>Default</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Default</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType#isDefault()
   * @see #getLangType()
   * @generated
   */
  EAttribute getLangType_Default();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType <em>Message Set Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Message Set Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType
   * @generated
   */
  EClass getMessageSetType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getLang <em>Lang</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Lang</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getLang()
   * @see #getMessageSetType()
   * @generated
   */
  EReference getMessageSetType_Lang();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType#getName()
   * @see #getMessageSetType()
   * @generated
   */
  EAttribute getMessageSetType_Name();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType <em>Resources Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Resources Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType
   * @generated
   */
  EClass getResourcesType();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType#getMessageSet <em>Message Set</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Message Set</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType#getMessageSet()
   * @see #getResourcesType()
   * @generated
   */
  EReference getResourcesType_MessageSet();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType <em>Sab Files Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Sab Files Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType
   * @generated
   */
  EClass getSabFilesType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getGroup <em>Group</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Group</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getGroup()
   * @see #getSabFilesType()
   * @generated
   */
  EAttribute getSabFilesType_Group();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getInclude <em>Include</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Include</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getInclude()
   * @see #getSabFilesType()
   * @generated
   */
  EReference getSabFilesType_Include();

  /**
   * Returns the meta object for the containment reference list '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getExclude <em>Exclude</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference list '<em>Exclude</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getExclude()
   * @see #getSabFilesType()
   * @generated
   */
  EReference getSabFilesType_Exclude();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType <em>Toolkit Info Model
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Toolkit Info Model Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType
   * @generated
   */
  EClass getToolkitInfoModelType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getIdentity
   * <em>Identity</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Identity</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getIdentity()
   * @see #getToolkitInfoModelType()
   * @generated
   */
  EReference getToolkitInfoModelType_Identity();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getDependencies
   * <em>Dependencies</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Dependencies</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getDependencies()
   * @see #getToolkitInfoModelType()
   * @generated
   */
  EReference getToolkitInfoModelType_Dependencies();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getResources
   * <em>Resources</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Resources</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getResources()
   * @see #getToolkitInfoModelType()
   * @generated
   */
  EReference getToolkitInfoModelType_Resources();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getSabFiles <em>Sab
   * Files</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Sab Files</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType#getSabFiles()
   * @see #getToolkitInfoModelType()
   * @generated
   */
  EReference getToolkitInfoModelType_SabFiles();

  /**
   * Returns the meta object for enum '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * <em>Root Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for enum '<em>Root Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @generated
   */
  EEnum getRootType();

  /**
   * Returns the meta object for data type '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType <em>Root Type Object</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Root Type Object</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
   * @model instanceClass="com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType"
   *     extendedMetaData="name='rootType:Object' baseType='rootType'"
   * @generated
   */
  EDataType getRootTypeObject();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  InfoFactory getInfoFactory();

  /**
   *
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   *
   * <ul>
   *   <li>each class,
   *   <li>each feature of each class,
   *   <li>each enum,
   *   <li>and each data type
   * </ul>
   *
   * <!-- end-user-doc -->
   *
   * @generated
   */
  interface Literals {
    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DependenciesTypeImpl <em>Dependencies
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DependenciesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getDependenciesType()
     * @generated
     */
    EClass DEPENDENCIES_TYPE = eINSTANCE.getDependenciesType();

    /**
     * The meta object literal for the '<em><b>Toolkit</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DEPENDENCIES_TYPE__TOOLKIT = eINSTANCE.getDependenciesType_Toolkit();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DocumentRootImpl <em>Document Root</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getDocumentRoot()
     * @generated
     */
    EClass DOCUMENT_ROOT = eINSTANCE.getDocumentRoot();

    /**
     * The meta object literal for the '<em><b>Mixed</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute DOCUMENT_ROOT__MIXED = eINSTANCE.getDocumentRoot_Mixed();

    /**
     * The meta object literal for the '<em><b>XMLNS Prefix Map</b></em>' map feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__XMLNS_PREFIX_MAP = eINSTANCE.getDocumentRoot_XMLNSPrefixMap();

    /**
     * The meta object literal for the '<em><b>XSI Schema Location</b></em>' map feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__XSI_SCHEMA_LOCATION = eINSTANCE.getDocumentRoot_XSISchemaLocation();

    /**
     * The meta object literal for the '<em><b>Toolkit Info Model</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__TOOLKIT_INFO_MODEL = eINSTANCE.getDocumentRoot_ToolkitInfoModel();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ExcludeTypeImpl <em>Exclude Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ExcludeTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getExcludeType()
     * @generated
     */
    EClass EXCLUDE_TYPE = eINSTANCE.getExcludeType();

    /**
     * The meta object literal for the '<em><b>Path</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute EXCLUDE_TYPE__PATH = eINSTANCE.getExcludeType_Path();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IdentityTypeImpl <em>Identity Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IdentityTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getIdentityType()
     * @generated
     */
    EClass IDENTITY_TYPE = eINSTANCE.getIdentityType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute IDENTITY_TYPE__NAME = eINSTANCE.getIdentityType_Name();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference IDENTITY_TYPE__DESCRIPTION = eINSTANCE.getIdentityType_Description();

    /**
     * The meta object literal for the '<em><b>Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute IDENTITY_TYPE__VERSION = eINSTANCE.getIdentityType_Version();

    /**
     * The meta object literal for the '<em><b>Required Product Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute IDENTITY_TYPE__REQUIRED_PRODUCT_VERSION =
        eINSTANCE.getIdentityType_RequiredProductVersion();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl <em>Include Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.IncludeTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getIncludeType()
     * @generated
     */
    EClass INCLUDE_TYPE = eINSTANCE.getIncludeType();

    /**
     * The meta object literal for the '<em><b>Path</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INCLUDE_TYPE__PATH = eINSTANCE.getIncludeType_Path();

    /**
     * The meta object literal for the '<em><b>Root</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute INCLUDE_TYPE__ROOT = eINSTANCE.getIncludeType_Root();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.LangTypeImpl <em>Lang Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.LangTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getLangType()
     * @generated
     */
    EClass LANG_TYPE = eINSTANCE.getLangType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute LANG_TYPE__VALUE = eINSTANCE.getLangType_Value();

    /**
     * The meta object literal for the '<em><b>Default</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute LANG_TYPE__DEFAULT = eINSTANCE.getLangType_Default();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.MessageSetTypeImpl <em>Message Set
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.MessageSetTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getMessageSetType()
     * @generated
     */
    EClass MESSAGE_SET_TYPE = eINSTANCE.getMessageSetType();

    /**
     * The meta object literal for the '<em><b>Lang</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference MESSAGE_SET_TYPE__LANG = eINSTANCE.getMessageSetType_Lang();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute MESSAGE_SET_TYPE__NAME = eINSTANCE.getMessageSetType_Name();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ResourcesTypeImpl <em>Resources Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ResourcesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getResourcesType()
     * @generated
     */
    EClass RESOURCES_TYPE = eINSTANCE.getResourcesType();

    /**
     * The meta object literal for the '<em><b>Message Set</b></em>' containment reference list
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference RESOURCES_TYPE__MESSAGE_SET = eINSTANCE.getResourcesType_MessageSet();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl <em>Sab Files Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.SabFilesTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getSabFilesType()
     * @generated
     */
    EClass SAB_FILES_TYPE = eINSTANCE.getSabFilesType();

    /**
     * The meta object literal for the '<em><b>Group</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute SAB_FILES_TYPE__GROUP = eINSTANCE.getSabFilesType_Group();

    /**
     * The meta object literal for the '<em><b>Include</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SAB_FILES_TYPE__INCLUDE = eINSTANCE.getSabFilesType_Include();

    /**
     * The meta object literal for the '<em><b>Exclude</b></em>' containment reference list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference SAB_FILES_TYPE__EXCLUDE = eINSTANCE.getSabFilesType_Exclude();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl <em>Toolkit Info
     * Model Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.ToolkitInfoModelTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getToolkitInfoModelType()
     * @generated
     */
    EClass TOOLKIT_INFO_MODEL_TYPE = eINSTANCE.getToolkitInfoModelType();

    /**
     * The meta object literal for the '<em><b>Identity</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_INFO_MODEL_TYPE__IDENTITY = eINSTANCE.getToolkitInfoModelType_Identity();

    /**
     * The meta object literal for the '<em><b>Dependencies</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_INFO_MODEL_TYPE__DEPENDENCIES =
        eINSTANCE.getToolkitInfoModelType_Dependencies();

    /**
     * The meta object literal for the '<em><b>Resources</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_INFO_MODEL_TYPE__RESOURCES = eINSTANCE.getToolkitInfoModelType_Resources();

    /**
     * The meta object literal for the '<em><b>Sab Files</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference TOOLKIT_INFO_MODEL_TYPE__SAB_FILES = eINSTANCE.getToolkitInfoModelType_SabFiles();

    /**
     * The meta object literal for the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
     * <em>Root Type</em>}' enum.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getRootType()
     * @generated
     */
    EEnum ROOT_TYPE = eINSTANCE.getRootType();

    /**
     * The meta object literal for the '<em>Root Type Object</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.RootType
     * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.impl.InfoPackageImpl#getRootTypeObject()
     * @generated
     */
    EDataType ROOT_TYPE_OBJECT = eINSTANCE.getRootTypeObject();
  }
} // InfoPackage
