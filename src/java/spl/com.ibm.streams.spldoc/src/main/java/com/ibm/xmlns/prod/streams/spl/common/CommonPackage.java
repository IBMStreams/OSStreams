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
package com.ibm.xmlns.prod.streams.spl.common;

import org.eclipse.emf.ecore.EAttribute;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.EDataType;
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
 *   <li>each operation of each class,
 *   <li>each enum,
 *   <li>and each data type
 * </ul>
 *
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonFactory
 * @model kind="package"
 * @generated
 */
public interface CommonPackage extends EPackage {
  /**
   * The package name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNAME = "common";

  /**
   * The package namespace URI.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_URI = "http://www.ibm.com/xmlns/prod/streams/spl/common";

  /**
   * The package namespace name.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  String eNS_PREFIX = "common";

  /**
   * The singleton instance of the package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  CommonPackage eINSTANCE = com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl.init();

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl <em>Description Type</em>}'
   * class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getDescriptionType()
   * @generated
   */
  int DESCRIPTION_TYPE = 0;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DESCRIPTION_TYPE__VALUE = 0;

  /**
   * The feature id for the '<em><b>Doc Href</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DESCRIPTION_TYPE__DOC_HREF = 1;

  /**
   * The feature id for the '<em><b>Sample Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DESCRIPTION_TYPE__SAMPLE_URI = 2;

  /**
   * The number of structural features of the '<em>Description Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DESCRIPTION_TYPE_FEATURE_COUNT = 3;

  /**
   * The number of operations of the '<em>Description Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DESCRIPTION_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl
   * <em>Document Root</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getDocumentRoot()
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
   * The feature id for the '<em><b>Java Op Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__JAVA_OP_LIBRARY = 3;

  /**
   * The feature id for the '<em><b>Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT__LIBRARY = 4;

  /**
   * The number of structural features of the '<em>Document Root</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT_FEATURE_COUNT = 5;

  /**
   * The number of operations of the '<em>Document Root</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int DOCUMENT_ROOT_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl <em>Java Op Library
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getJavaOpLibraryType()
   * @generated
   */
  int JAVA_OP_LIBRARY_TYPE = 2;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Managed Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY = 1;

  /**
   * The number of structural features of the '<em>Java Op Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_LIBRARY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl <em>Java Op Managed
   * Library Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getJavaOpManagedLibraryType()
   * @generated
   */
  int JAVA_OP_MANAGED_LIBRARY_TYPE = 3;

  /**
   * The feature id for the '<em><b>Lib Path</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH = 0;

  /**
   * The feature id for the '<em><b>Command</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND = 1;

  /**
   * The number of structural features of the '<em>Java Op Managed Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MANAGED_LIBRARY_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Java Op Managed Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int JAVA_OP_MANAGED_LIBRARY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.common.impl.LibraryTypeImpl
   * <em>Library Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.LibraryTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getLibraryType()
   * @generated
   */
  int LIBRARY_TYPE = 4;

  /**
   * The feature id for the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_TYPE__DESCRIPTION = 0;

  /**
   * The feature id for the '<em><b>Managed Library</b></em>' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_TYPE__MANAGED_LIBRARY = 1;

  /**
   * The number of structural features of the '<em>Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int LIBRARY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.impl.ManagedLibraryTypeImpl <em>Managed Library
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.ManagedLibraryTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getManagedLibraryType()
   * @generated
   */
  int MANAGED_LIBRARY_TYPE = 5;

  /**
   * The feature id for the '<em><b>Lib</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE__LIB = 0;

  /**
   * The feature id for the '<em><b>Lib Path</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE__LIB_PATH = 1;

  /**
   * The feature id for the '<em><b>Include Path</b></em>' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE__INCLUDE_PATH = 2;

  /**
   * The feature id for the '<em><b>Command</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE__COMMAND = 3;

  /**
   * The number of structural features of the '<em>Managed Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE_FEATURE_COUNT = 4;

  /**
   * The number of operations of the '<em>Managed Library Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int MANAGED_LIBRARY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link
   * com.ibm.xmlns.prod.streams.spl.common.impl.ToolkitDependencyTypeImpl <em>Toolkit Dependency
   * Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.ToolkitDependencyTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getToolkitDependencyType()
   * @generated
   */
  int TOOLKIT_DEPENDENCY_TYPE = 6;

  /**
   * The feature id for the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_DEPENDENCY_TYPE__NAME = 0;

  /**
   * The feature id for the '<em><b>Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_DEPENDENCY_TYPE__VERSION = 1;

  /**
   * The number of structural features of the '<em>Toolkit Dependency Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_DEPENDENCY_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Toolkit Dependency Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int TOOLKIT_DEPENDENCY_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '{@link com.ibm.xmlns.prod.streams.spl.common.impl.UriTypeImpl
   * <em>Uri Type</em>}' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.UriTypeImpl
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getUriType()
   * @generated
   */
  int URI_TYPE = 7;

  /**
   * The feature id for the '<em><b>Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TYPE__INDEX = 0;

  /**
   * The feature id for the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TYPE__VALUE = 1;

  /**
   * The number of structural features of the '<em>Uri Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TYPE_FEATURE_COUNT = 2;

  /**
   * The number of operations of the '<em>Uri Type</em>' class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  int URI_TYPE_OPERATION_COUNT = 0;

  /**
   * The meta object id for the '<em>Version Range Type</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see java.lang.String
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getVersionRangeType()
   * @generated
   */
  int VERSION_RANGE_TYPE = 8;

  /**
   * The meta object id for the '<em>Version Type</em>' data type.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see java.lang.String
   * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getVersionType()
   * @generated
   */
  int VERSION_TYPE = 9;

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.common.DescriptionType
   * <em>Description Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Description Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DescriptionType
   * @generated
   */
  EClass getDescriptionType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getValue()
   * @see #getDescriptionType()
   * @generated
   */
  EAttribute getDescriptionType_Value();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getDocHref <em>Doc Href</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Doc Href</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getDocHref()
   * @see #getDescriptionType()
   * @generated
   */
  EAttribute getDescriptionType_DocHref();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getSampleUri <em>Sample Uri</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Sample Uri</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DescriptionType#getSampleUri()
   * @see #getDescriptionType()
   * @generated
   */
  EAttribute getDescriptionType_SampleUri();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.common.DocumentRoot
   * <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Document Root</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot
   * @generated
   */
  EClass getDocumentRoot();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getMixed <em>Mixed</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Mixed</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getMixed()
   * @see #getDocumentRoot()
   * @generated
   */
  EAttribute getDocumentRoot_Mixed();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXMLNSPrefixMap <em>XMLNS Prefix
   * Map</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XMLNS Prefix Map</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXMLNSPrefixMap()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XMLNSPrefixMap();

  /**
   * Returns the meta object for the map '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXSISchemaLocation <em>XSI Schema
   * Location</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the map '<em>XSI Schema Location</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getXSISchemaLocation()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_XSISchemaLocation();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getJavaOpLibrary <em>Java Op Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Java Op Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getJavaOpLibrary()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_JavaOpLibrary();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getLibrary <em>Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot#getLibrary()
   * @see #getDocumentRoot()
   * @generated
   */
  EReference getDocumentRoot_Library();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType <em>Java Op Library Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Library Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType
   * @generated
   */
  EClass getJavaOpLibraryType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getDescription()
   * @see #getJavaOpLibraryType()
   * @generated
   */
  EReference getJavaOpLibraryType_Description();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getManagedLibrary <em>Managed
   * Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Managed Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType#getManagedLibrary()
   * @see #getJavaOpLibraryType()
   * @generated
   */
  EReference getJavaOpLibraryType_ManagedLibrary();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType <em>Java Op Managed Library
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Java Op Managed Library Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType
   * @generated
   */
  EClass getJavaOpManagedLibraryType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType#getLibPath <em>Lib Path</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Lib Path</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType#getLibPath()
   * @see #getJavaOpManagedLibraryType()
   * @generated
   */
  EAttribute getJavaOpManagedLibraryType_LibPath();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType#getCommand <em>Command</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Command</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType#getCommand()
   * @see #getJavaOpManagedLibraryType()
   * @generated
   */
  EAttribute getJavaOpManagedLibraryType_Command();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.common.LibraryType
   * <em>Library Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Library Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.LibraryType
   * @generated
   */
  EClass getLibraryType();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.LibraryType#getDescription <em>Description</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Description</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.LibraryType#getDescription()
   * @see #getLibraryType()
   * @generated
   */
  EReference getLibraryType_Description();

  /**
   * Returns the meta object for the containment reference '{@link
   * com.ibm.xmlns.prod.streams.spl.common.LibraryType#getManagedLibrary <em>Managed Library</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the containment reference '<em>Managed Library</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.LibraryType#getManagedLibrary()
   * @see #getLibraryType()
   * @generated
   */
  EReference getLibraryType_ManagedLibrary();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType <em>Managed Library Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Managed Library Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType
   * @generated
   */
  EClass getManagedLibraryType();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLib <em>Lib</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Lib</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLib()
   * @see #getManagedLibraryType()
   * @generated
   */
  EAttribute getManagedLibraryType_Lib();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLibPath <em>Lib Path</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Lib Path</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getLibPath()
   * @see #getManagedLibraryType()
   * @generated
   */
  EAttribute getManagedLibraryType_LibPath();

  /**
   * Returns the meta object for the attribute list '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getIncludePath <em>Include
   * Path</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute list '<em>Include Path</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getIncludePath()
   * @see #getManagedLibraryType()
   * @generated
   */
  EAttribute getManagedLibraryType_IncludePath();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getCommand <em>Command</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Command</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType#getCommand()
   * @see #getManagedLibraryType()
   * @generated
   */
  EAttribute getManagedLibraryType_Command();

  /**
   * Returns the meta object for class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType <em>Toolkit Dependency Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Toolkit Dependency Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType
   * @generated
   */
  EClass getToolkitDependencyType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getName <em>Name</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Name</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getName()
   * @see #getToolkitDependencyType()
   * @generated
   */
  EAttribute getToolkitDependencyType_Name();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getVersion <em>Version</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Version</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType#getVersion()
   * @see #getToolkitDependencyType()
   * @generated
   */
  EAttribute getToolkitDependencyType_Version();

  /**
   * Returns the meta object for class '{@link com.ibm.xmlns.prod.streams.spl.common.UriType <em>Uri
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for class '<em>Uri Type</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.UriType
   * @generated
   */
  EClass getUriType();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex <em>Index</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Index</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.UriType#getIndex()
   * @see #getUriType()
   * @generated
   */
  EAttribute getUriType_Index();

  /**
   * Returns the meta object for the attribute '{@link
   * com.ibm.xmlns.prod.streams.spl.common.UriType#getValue <em>Value</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for the attribute '<em>Value</em>'.
   * @see com.ibm.xmlns.prod.streams.spl.common.UriType#getValue()
   * @see #getUriType()
   * @generated
   */
  EAttribute getUriType_Value();

  /**
   * Returns the meta object for data type '{@link java.lang.String <em>Version Range Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Version Range Type</em>'.
   * @see java.lang.String
   * @model instanceClass="java.lang.String" extendedMetaData="name='versionRangeType'
   *     baseType='http://www.eclipse.org/emf/2003/XMLType#string'
   *     pattern='(\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?)|([\\[\\(]\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?,\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?[\\)\\]])'"
   * @generated
   */
  EDataType getVersionRangeType();

  /**
   * Returns the meta object for data type '{@link java.lang.String <em>Version Type</em>}'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the meta object for data type '<em>Version Type</em>'.
   * @see java.lang.String
   * @model instanceClass="java.lang.String" extendedMetaData="name='versionType'
   *     baseType='http://www.eclipse.org/emf/2003/XMLType#string'
   *     pattern='\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?'"
   * @generated
   */
  EDataType getVersionType();

  /**
   * Returns the factory that creates the instances of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return the factory that creates the instances of the model.
   * @generated
   */
  CommonFactory getCommonFactory();

  /**
   *
   * <!-- begin-user-doc -->
   * Defines literals for the meta objects that represent
   *
   * <ul>
   *   <li>each class,
   *   <li>each feature of each class,
   *   <li>each operation of each class,
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
     * com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl <em>Description Type</em>}'
     * class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.DescriptionTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getDescriptionType()
     * @generated
     */
    EClass DESCRIPTION_TYPE = eINSTANCE.getDescriptionType();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute DESCRIPTION_TYPE__VALUE = eINSTANCE.getDescriptionType_Value();

    /**
     * The meta object literal for the '<em><b>Doc Href</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute DESCRIPTION_TYPE__DOC_HREF = eINSTANCE.getDescriptionType_DocHref();

    /**
     * The meta object literal for the '<em><b>Sample Uri</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute DESCRIPTION_TYPE__SAMPLE_URI = eINSTANCE.getDescriptionType_SampleUri();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl <em>Document Root</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.DocumentRootImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getDocumentRoot()
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
     * The meta object literal for the '<em><b>Java Op Library</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__JAVA_OP_LIBRARY = eINSTANCE.getDocumentRoot_JavaOpLibrary();

    /**
     * The meta object literal for the '<em><b>Library</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference DOCUMENT_ROOT__LIBRARY = eINSTANCE.getDocumentRoot_Library();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl <em>Java Op Library
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpLibraryTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getJavaOpLibraryType()
     * @generated
     */
    EClass JAVA_OP_LIBRARY_TYPE = eINSTANCE.getJavaOpLibraryType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_LIBRARY_TYPE__DESCRIPTION = eINSTANCE.getJavaOpLibraryType_Description();

    /**
     * The meta object literal for the '<em><b>Managed Library</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference JAVA_OP_LIBRARY_TYPE__MANAGED_LIBRARY =
        eINSTANCE.getJavaOpLibraryType_ManagedLibrary();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl <em>Java Op Managed
     * Library Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl
     * @see
     *     com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getJavaOpManagedLibraryType()
     * @generated
     */
    EClass JAVA_OP_MANAGED_LIBRARY_TYPE = eINSTANCE.getJavaOpManagedLibraryType();

    /**
     * The meta object literal for the '<em><b>Lib Path</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH =
        eINSTANCE.getJavaOpManagedLibraryType_LibPath();

    /**
     * The meta object literal for the '<em><b>Command</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND =
        eINSTANCE.getJavaOpManagedLibraryType_Command();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.LibraryTypeImpl <em>Library Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.LibraryTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getLibraryType()
     * @generated
     */
    EClass LIBRARY_TYPE = eINSTANCE.getLibraryType();

    /**
     * The meta object literal for the '<em><b>Description</b></em>' containment reference feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference LIBRARY_TYPE__DESCRIPTION = eINSTANCE.getLibraryType_Description();

    /**
     * The meta object literal for the '<em><b>Managed Library</b></em>' containment reference
     * feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EReference LIBRARY_TYPE__MANAGED_LIBRARY = eINSTANCE.getLibraryType_ManagedLibrary();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.ManagedLibraryTypeImpl <em>Managed Library
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.ManagedLibraryTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getManagedLibraryType()
     * @generated
     */
    EClass MANAGED_LIBRARY_TYPE = eINSTANCE.getManagedLibraryType();

    /**
     * The meta object literal for the '<em><b>Lib</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute MANAGED_LIBRARY_TYPE__LIB = eINSTANCE.getManagedLibraryType_Lib();

    /**
     * The meta object literal for the '<em><b>Lib Path</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute MANAGED_LIBRARY_TYPE__LIB_PATH = eINSTANCE.getManagedLibraryType_LibPath();

    /**
     * The meta object literal for the '<em><b>Include Path</b></em>' attribute list feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute MANAGED_LIBRARY_TYPE__INCLUDE_PATH = eINSTANCE.getManagedLibraryType_IncludePath();

    /**
     * The meta object literal for the '<em><b>Command</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute MANAGED_LIBRARY_TYPE__COMMAND = eINSTANCE.getManagedLibraryType_Command();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.ToolkitDependencyTypeImpl <em>Toolkit Dependency
     * Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.ToolkitDependencyTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getToolkitDependencyType()
     * @generated
     */
    EClass TOOLKIT_DEPENDENCY_TYPE = eINSTANCE.getToolkitDependencyType();

    /**
     * The meta object literal for the '<em><b>Name</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_DEPENDENCY_TYPE__NAME = eINSTANCE.getToolkitDependencyType_Name();

    /**
     * The meta object literal for the '<em><b>Version</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute TOOLKIT_DEPENDENCY_TYPE__VERSION = eINSTANCE.getToolkitDependencyType_Version();

    /**
     * The meta object literal for the '{@link
     * com.ibm.xmlns.prod.streams.spl.common.impl.UriTypeImpl <em>Uri Type</em>}' class.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.UriTypeImpl
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getUriType()
     * @generated
     */
    EClass URI_TYPE = eINSTANCE.getUriType();

    /**
     * The meta object literal for the '<em><b>Index</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute URI_TYPE__INDEX = eINSTANCE.getUriType_Index();

    /**
     * The meta object literal for the '<em><b>Value</b></em>' attribute feature.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @generated
     */
    EAttribute URI_TYPE__VALUE = eINSTANCE.getUriType_Value();

    /**
     * The meta object literal for the '<em>Version Range Type</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see java.lang.String
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getVersionRangeType()
     * @generated
     */
    EDataType VERSION_RANGE_TYPE = eINSTANCE.getVersionRangeType();

    /**
     * The meta object literal for the '<em>Version Type</em>' data type.
     * <!-- begin-user-doc -->
     * <!-- end-user-doc -->
     *
     * @see java.lang.String
     * @see com.ibm.xmlns.prod.streams.spl.common.impl.CommonPackageImpl#getVersionType()
     * @generated
     */
    EDataType VERSION_TYPE = eINSTANCE.getVersionType();
  }
} // CommonPackage
