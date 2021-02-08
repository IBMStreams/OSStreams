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

import com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Dependencies Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType#getToolkit
 *       <em>Toolkit</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getDependenciesType()
 * @model extendedMetaData="name='dependenciesType' kind='elementOnly'"
 * @generated
 */
public interface DependenciesType extends EObject {
  /**
   * Returns the value of the '<em><b>Toolkit</b></em>' containment reference list. The list
   * contents are of type {@link com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Toolkit</em>' containment reference list isn't clear, there
   * really should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Toolkit</em>' containment reference list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage#getDependenciesType_Toolkit()
   * @model containment="true" extendedMetaData="kind='element' name='toolkit'
   *     namespace='##targetNamespace'"
   * @generated
   */
  EList<ToolkitDependencyType> getToolkit();
} // DependenciesType
