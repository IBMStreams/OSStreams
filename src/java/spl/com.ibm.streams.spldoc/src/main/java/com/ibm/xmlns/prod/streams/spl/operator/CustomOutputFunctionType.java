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
 * A representation of the model object '<em><b>Custom Output Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getPrototype
 *       <em>Prototype</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction
 *       <em>Pseudo Function</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCustomOutputFunctionType()
 * @model extendedMetaData="name='customOutputFunctionType' kind='elementOnly'"
 * @generated
 */
public interface CustomOutputFunctionType extends EObject {
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
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCustomOutputFunctionType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getDescription
   * <em>Description</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Description</em>' containment reference.
   * @see #getDescription()
   * @generated
   */
  void setDescription(DescriptionType value);

  /**
   * Returns the value of the '<em><b>Prototype</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Prototype</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Prototype</em>' attribute.
   * @see #setPrototype(String)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCustomOutputFunctionType_Prototype()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" required="true"
   *     extendedMetaData="kind='element' name='prototype' namespace='##targetNamespace'"
   * @generated
   */
  String getPrototype();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#getPrototype
   * <em>Prototype</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Prototype</em>' attribute.
   * @see #getPrototype()
   * @generated
   */
  void setPrototype(String value);

  /**
   * Returns the value of the '<em><b>Pseudo Function</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Pseudo Function</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Pseudo Function</em>' attribute.
   * @see #isSetPseudoFunction()
   * @see #unsetPseudoFunction()
   * @see #setPseudoFunction(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage#getCustomOutputFunctionType_PseudoFunction()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='pseudoFunction'"
   * @generated
   */
  boolean isPseudoFunction();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction <em>Pseudo
   * Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Pseudo Function</em>' attribute.
   * @see #isSetPseudoFunction()
   * @see #unsetPseudoFunction()
   * @see #isPseudoFunction()
   * @generated
   */
  void setPseudoFunction(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction <em>Pseudo
   * Function</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPseudoFunction()
   * @see #isPseudoFunction()
   * @see #setPseudoFunction(boolean)
   * @generated
   */
  void unsetPseudoFunction();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.operator.CustomOutputFunctionType#isPseudoFunction <em>Pseudo
   * Function</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Pseudo Function</em>' attribute is set.
   * @see #unsetPseudoFunction()
   * @see #isPseudoFunction()
   * @see #setPseudoFunction(boolean)
   * @generated
   */
  boolean isSetPseudoFunction();
} // CustomOutputFunctionType
