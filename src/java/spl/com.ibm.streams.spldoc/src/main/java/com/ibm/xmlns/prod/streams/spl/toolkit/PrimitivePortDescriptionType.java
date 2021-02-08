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

import com.ibm.xmlns.prod.streams.spl.common.DescriptionType;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Primitive Port Description Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts
 *       <em>Max Num Ports</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts
 *       <em>Min Num Ports</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitivePortDescriptionType()
 * @model extendedMetaData="name='primitivePortDescriptionType' kind='elementOnly'"
 * @generated
 */
public interface PrimitivePortDescriptionType extends EObject {
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
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitivePortDescriptionType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getDescription
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
   * Returns the value of the '<em><b>Max Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Max Num Ports</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Max Num Ports</em>' attribute.
   * @see #isSetMaxNumPorts()
   * @see #unsetMaxNumPorts()
   * @see #setMaxNumPorts(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitivePortDescriptionType_MaxNumPorts()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='maxNumPorts'"
   * @generated
   */
  int getMaxNumPorts();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts <em>Max Num
   * Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Max Num Ports</em>' attribute.
   * @see #isSetMaxNumPorts()
   * @see #unsetMaxNumPorts()
   * @see #getMaxNumPorts()
   * @generated
   */
  void setMaxNumPorts(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts <em>Max Num
   * Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetMaxNumPorts()
   * @see #getMaxNumPorts()
   * @see #setMaxNumPorts(int)
   * @generated
   */
  void unsetMaxNumPorts();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMaxNumPorts <em>Max Num
   * Ports</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Max Num Ports</em>' attribute is set.
   * @see #unsetMaxNumPorts()
   * @see #getMaxNumPorts()
   * @see #setMaxNumPorts(int)
   * @generated
   */
  boolean isSetMaxNumPorts();

  /**
   * Returns the value of the '<em><b>Min Num Ports</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Min Num Ports</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Min Num Ports</em>' attribute.
   * @see #isSetMinNumPorts()
   * @see #unsetMinNumPorts()
   * @see #setMinNumPorts(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitivePortDescriptionType_MinNumPorts()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='minNumPorts'"
   * @generated
   */
  int getMinNumPorts();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts <em>Min Num
   * Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Min Num Ports</em>' attribute.
   * @see #isSetMinNumPorts()
   * @see #unsetMinNumPorts()
   * @see #getMinNumPorts()
   * @generated
   */
  void setMinNumPorts(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts <em>Min Num
   * Ports</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetMinNumPorts()
   * @see #getMinNumPorts()
   * @see #setMinNumPorts(int)
   * @generated
   */
  void unsetMinNumPorts();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#getMinNumPorts <em>Min Num
   * Ports</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Min Num Ports</em>' attribute is set.
   * @see #unsetMinNumPorts()
   * @see #getMinNumPorts()
   * @see #setMinNumPorts(int)
   * @generated
   */
  boolean isSetMinNumPorts();

  /**
   * Returns the value of the '<em><b>Optional</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Optional</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Optional</em>' attribute.
   * @see #isSetOptional()
   * @see #unsetOptional()
   * @see #setOptional(boolean)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getPrimitivePortDescriptionType_Optional()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='attribute' name='optional'"
   * @generated
   */
  boolean isOptional();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional
   * <em>Optional</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Optional</em>' attribute.
   * @see #isSetOptional()
   * @see #unsetOptional()
   * @see #isOptional()
   * @generated
   */
  void setOptional(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional
   * <em>Optional</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetOptional()
   * @see #isOptional()
   * @see #setOptional(boolean)
   * @generated
   */
  void unsetOptional();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.PrimitivePortDescriptionType#isOptional
   * <em>Optional</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Optional</em>' attribute is set.
   * @see #unsetOptional()
   * @see #isOptional()
   * @see #setOptional(boolean)
   * @generated
   */
  boolean isSetOptional();
} // PrimitivePortDescriptionType
