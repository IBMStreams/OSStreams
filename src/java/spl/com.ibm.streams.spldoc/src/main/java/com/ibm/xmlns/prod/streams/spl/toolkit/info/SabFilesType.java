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

import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.util.FeatureMap;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Sab Files Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getGroup <em>Group</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getInclude
 *       <em>Include</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType#getExclude
 *       <em>Exclude</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getSabFilesType()
 * @model extendedMetaData="name='sabFilesType' kind='elementOnly'"
 * @generated
 */
public interface SabFilesType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getSabFilesType_Group()
   * @model unique="false" dataType="org.eclipse.emf.ecore.EFeatureMapEntry" many="true"
   *     extendedMetaData="kind='group' name='group:0'"
   * @generated
   */
  FeatureMap getGroup();

  /**
   * Returns the value of the '<em><b>Include</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Include</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Include</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getSabFilesType_Include()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='include' namespace='##targetNamespace'
   *     group='#group:0'"
   * @generated
   */
  EList<IncludeType> getInclude();

  /**
   * Returns the value of the '<em><b>Exclude</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Exclude</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Exclude</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getSabFilesType_Exclude()
   * @model containment="true" transient="true" volatile="true" derived="true"
   *     extendedMetaData="kind='element' name='exclude' namespace='##targetNamespace'
   *     group='#group:0'"
   * @generated
   */
  EList<ExcludeType> getExclude();
} // SabFilesType
