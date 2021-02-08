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
package com.ibm.xmlns.prod.streams.spl.operator;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Code Template Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getTemplate
 *       <em>Template</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getName <em>Name</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCodeTemplateType()
 * @model extendedMetaData="name='codeTemplateType' kind='elementOnly'"
 * @generated
 */
public interface CodeTemplateType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCodeTemplateType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getDescription <em>Description</em>}'
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
   * Returns the value of the '<em><b>Template</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Template</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Template</em>' attribute.
   * @see #setTemplate(String)
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCodeTemplateType_Template()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='template' namespace='##targetNamespace'"
   * @generated
   */
  String getTemplate();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getTemplate <em>Template</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Template</em>' attribute.
   * @see #getTemplate()
   * @generated
   */
  void setTemplate(String value);

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
   * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCodeTemplateType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='name'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.operator.CodeTemplateType#getName
   * <em>Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);
} // CodeTemplateType
