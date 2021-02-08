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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Identity Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getVersion
 *       <em>Version</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getRequiredProductVersion
 *       <em>Required Product Version</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIdentityType()
 * @model extendedMetaData="name='identityType' kind='elementOnly'"
 * @generated
 */
public interface IdentityType extends EObject {
  /**
   * Returns the value of the '<em><b>Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Name</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Name</em>' attribute.
   * @see #setName(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIdentityType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='name' namespace='##targetNamespace'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getName
   * <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Description</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Description</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Description</em>' containment reference.
   * @see #setDescription(DescriptionType)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIdentityType_Description()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getDescription <em>Description</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIdentityType_Version()
   * @model dataType="com.ibm.xmlns.prod.streams.spl.common.VersionType" required="true"
   *     extendedMetaData="kind='element' name='version' namespace='##targetNamespace'"
   * @generated
   */
  String getVersion();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getVersion <em>Version</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Version</em>' attribute.
   * @see #getVersion()
   * @generated
   */
  void setVersion(String value);

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
   *     com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getIdentityType_RequiredProductVersion()
   * @model dataType="com.ibm.xmlns.prod.streams.spl.common.VersionRangeType" required="true"
   *     extendedMetaData="kind='element' name='requiredProductVersion'
   *     namespace='##targetNamespace'"
   * @generated
   */
  String getRequiredProductVersion();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType#getRequiredProductVersion <em>Required
   * Product Version</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Required Product Version</em>' attribute.
   * @see #getRequiredProductVersion()
   * @generated
   */
  void setRequiredProductVersion(String value);
} // IdentityType
