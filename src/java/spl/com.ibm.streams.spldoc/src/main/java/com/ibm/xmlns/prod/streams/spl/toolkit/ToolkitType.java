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
package com.ibm.xmlns.prod.streams.spl.toolkit;

import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.util.FeatureMap;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getGroup <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getNamespace <em>Namespace</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getDependency
 *       <em>Dependency</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getSabFiles <em>Sab Files</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources <em>Has NL
 *       Resources</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getRequiredProductVersion
 *       <em>Required Product Version</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getUri <em>Uri</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getVersion <em>Version</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType()
 * @model extendedMetaData="name='toolkitType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitType extends ToolkitBaseType {
  /**
   * Returns the value of the '<em><b>Group</b></em>' attribute list. The list contents are of type
   * {@link org.eclipse.emf.ecore.util.FeatureMap.Entry}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Group</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Group</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_Group()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='group' name='group:4'"
   * @generated
   */
  FeatureMap getGroup();

  /**
   * Returns the value of the '<em><b>Namespace</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitNamespaceType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Namespace</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Namespace</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_Namespace()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='namespace' namespace='##targetNamespace'
   *     group='#group:4'"
   * @generated
   */
  EList<ToolkitNamespaceType> getNamespace();

  /**
   * Returns the value of the '<em><b>Dependency</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Dependency</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Dependency</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_Dependency()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='dependency' namespace='##targetNamespace'
   *     group='#group:4'"
   * @generated
   */
  EList<ToolkitDependencyType> getDependency();

  /**
   * Returns the value of the '<em><b>Sab Files</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Sab Files</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Sab Files</em>' containment reference.
   * @see #setSabFiles(SabFilesType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_SabFiles()
   * @model containment="true" extendedMetaData="kind='element' name='sabFiles'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SabFilesType getSabFiles();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getSabFiles
   * <em>Sab Files</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Sab Files</em>' containment reference.
   * @see #getSabFiles()
   * @generated
   */
  void setSabFiles(SabFilesType value);

  /**
   * Returns the value of the '<em><b>Has NL Resources</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Has NL Resources</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Has NL Resources</em>' attribute.
   * @see #isSetHasNLResources()
   * @see #unsetHasNLResources()
   * @see #setHasNLResources(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_HasNLResources()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='hasNLResources'"
   * @generated
   */
  boolean isHasNLResources();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources <em>Has NL Resources</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Has NL Resources</em>' attribute.
   * @see #isSetHasNLResources()
   * @see #unsetHasNLResources()
   * @see #isHasNLResources()
   * @generated
   */
  void setHasNLResources(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources <em>Has NL Resources</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetHasNLResources()
   * @see #isHasNLResources()
   * @see #setHasNLResources(boolean)
   * @generated
   */
  void unsetHasNLResources();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#isHasNLResources <em>Has NL Resources</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Has NL Resources</em>' attribute is set.
   * @see #unsetHasNLResources()
   * @see #isHasNLResources()
   * @see #setHasNLResources(boolean)
   * @generated
   */
  boolean isSetHasNLResources();

  /**
   * Returns the value of the '<em><b>Required Product Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Required Product Version</em>' attribute isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Required Product Version</em>' attribute.
   * @see #setRequiredProductVersion(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_RequiredProductVersion()
   * @model dataType="com.ibm.xmlns.prod.streams.spl.common.VersionRangeType" required="true"
   *     extendedMetaData="kind='attribute' name='requiredProductVersion'"
   * @generated
   */
  String getRequiredProductVersion();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getRequiredProductVersion <em>Required
   * Product Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Required Product Version</em>' attribute.
   * @see #getRequiredProductVersion()
   * @generated
   */
  void setRequiredProductVersion(String value);

  /**
   * Returns the value of the '<em><b>Uri</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Uri</em>' attribute isn't clear, there really should be more of a
   * description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Uri</em>' attribute.
   * @see #setUri(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_Uri()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='uri'"
   * @generated
   */
  String getUri();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getUri
   * <em>Uri</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Uri</em>' attribute.
   * @see #getUri()
   * @generated
   */
  void setUri(String value);

  /**
   * Returns the value of the '<em><b>Version</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Version</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Version</em>' attribute.
   * @see #setVersion(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitType_Version()
   * @model dataType="com.ibm.xmlns.prod.streams.spl.common.VersionType" required="true"
   *     extendedMetaData="kind='attribute' name='version'"
   * @generated
   */
  String getVersion();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType#getVersion
   * <em>Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Version</em>' attribute.
   * @see #getVersion()
   * @generated
   */
  void setVersion(String value);
} // ToolkitType
