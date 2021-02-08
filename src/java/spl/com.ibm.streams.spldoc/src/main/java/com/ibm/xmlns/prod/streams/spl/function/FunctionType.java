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
package com.ibm.xmlns.prod.streams.spl.function;

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionType#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.FunctionType#getJavaFunctionName <em>Java
 *       Function Name</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionType()
 * @model extendedMetaData="name='functionType' kind='elementOnly'"
 * @generated
 */
public interface FunctionType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionType#getDescription <em>Description</em>}'
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
   * Returns the value of the '<em><b>Prototype</b></em>' containment reference.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Prototype</em>' containment reference isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Prototype</em>' containment reference.
   * @see #setPrototype(PrototypeType)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionType_Prototype()
   * @model containment="true" required="true" extendedMetaData="kind='element' name='prototype'
   *     namespace='##targetNamespace'"
   * @generated
   */
  PrototypeType getPrototype();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.function.FunctionType#getPrototype
   * <em>Prototype</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Prototype</em>' containment reference.
   * @see #getPrototype()
   * @generated
   */
  void setPrototype(PrototypeType value);

  /**
   * Returns the value of the '<em><b>Java Function Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Java Function Name</em>' attribute isn't clear, there really
   * should be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Java Function Name</em>' attribute.
   * @see #setJavaFunctionName(String)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getFunctionType_JavaFunctionName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='element'
   *     name='javaFunctionName' namespace='##targetNamespace'"
   * @generated
   */
  String getJavaFunctionName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.function.FunctionType#getJavaFunctionName <em>Java Function
   * Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Java Function Name</em>' attribute.
   * @see #getJavaFunctionName()
   * @generated
   */
  void setJavaFunctionName(String value);
} // FunctionType
