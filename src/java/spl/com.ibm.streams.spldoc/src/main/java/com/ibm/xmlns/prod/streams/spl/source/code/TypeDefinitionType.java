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
package com.ibm.xmlns.prod.streams.spl.source.code;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Type Definition Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getSplDoc <em>Spl
 *       Doc</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getModifiers
 *       <em>Modifiers</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getName <em>Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getValue
 *       <em>Value</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getTypeDefinitionType()
 * @model extendedMetaData="name='typeDefinitionType' kind='elementOnly'"
 * @generated
 */
public interface TypeDefinitionType extends ExtendedSourceLocationType {
  /**
   * Returns the value of the '<em><b>Spl Doc</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Spl Doc</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Spl Doc</em>' containment reference.
   * @see #setSplDoc(SplDocType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getTypeDefinitionType_SplDoc()
   * @model containment="true" extendedMetaData="kind='element' name='splDoc'
   *     namespace='##targetNamespace'"
   * @generated
   */
  SplDocType getSplDoc();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getSplDoc <em>Spl Doc</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Spl Doc</em>' containment reference.
   * @see #getSplDoc()
   * @generated
   */
  void setSplDoc(SplDocType value);

  /**
   * Returns the value of the '<em><b>Modifiers</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Modifiers</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Modifiers</em>' containment reference.
   * @see #setModifiers(TypeModifiersType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getTypeDefinitionType_Modifiers()
   * @model containment="true" extendedMetaData="kind='element' name='modifiers'
   *     namespace='##targetNamespace'"
   * @generated
   */
  TypeModifiersType getModifiers();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getModifiers <em>Modifiers</em>}'
   * containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Modifiers</em>' containment reference.
   * @see #getModifiers()
   * @generated
   */
  void setModifiers(TypeModifiersType value);

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
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getTypeDefinitionType_Name()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='name'"
   * @generated
   */
  String getName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getName <em>Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Name</em>' attribute.
   * @see #getName()
   * @generated
   */
  void setName(String value);

  /**
   * Returns the value of the '<em><b>Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Value</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Value</em>' attribute.
   * @see #setValue(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getTypeDefinitionType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='attribute' name='value'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.TypeDefinitionType#getValue <em>Value</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);
} // TypeDefinitionType
