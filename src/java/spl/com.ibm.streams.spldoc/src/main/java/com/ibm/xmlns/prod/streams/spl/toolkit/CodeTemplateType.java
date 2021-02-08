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

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Code Template Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType#getTemplate
 *       <em>Template</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCodeTemplateType()
 * @model extendedMetaData="name='codeTemplateType' kind='elementOnly'"
 * @generated
 */
public interface CodeTemplateType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Template</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Template</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Template</em>' attribute.
   * @see #setTemplate(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCodeTemplateType_Template()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='template'"
   * @generated
   */
  String getTemplate();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CodeTemplateType#getTemplate <em>Template</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Template</em>' attribute.
   * @see #getTemplate()
   * @generated
   */
  void setTemplate(String value);
} // CodeTemplateType
