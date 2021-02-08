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
 * A representation of the model object '<em><b>Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getDefaultValue <em>Default
 *       Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable <em>Mutable</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getType <em>Type</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getParameterType()
 * @model extendedMetaData="name='parameterType' kind='elementOnly'"
 * @generated
 */
public interface ParameterType extends ToolkitElementType {
  /**
   * Returns the value of the '<em><b>Default Value</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Default Value</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Default Value</em>' attribute.
   * @see #setDefaultValue(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getParameterType_DefaultValue()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='defaultValue'"
   * @generated
   */
  String getDefaultValue();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getDefaultValue <em>Default Value</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Default Value</em>' attribute.
   * @see #getDefaultValue()
   * @generated
   */
  void setDefaultValue(String value);

  /**
   * Returns the value of the '<em><b>Mutable</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Mutable</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Mutable</em>' attribute.
   * @see #isSetMutable()
   * @see #unsetMutable()
   * @see #setMutable(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getParameterType_Mutable()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='mutable'"
   * @generated
   */
  boolean isMutable();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable
   * <em>Mutable</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Mutable</em>' attribute.
   * @see #isSetMutable()
   * @see #unsetMutable()
   * @see #isMutable()
   * @generated
   */
  void setMutable(boolean value);

  /**
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable
   * <em>Mutable</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetMutable()
   * @see #isMutable()
   * @see #setMutable(boolean)
   * @generated
   */
  void unsetMutable();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#isMutable <em>Mutable</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Mutable</em>' attribute is set.
   * @see #unsetMutable()
   * @see #isMutable()
   * @see #setMutable(boolean)
   * @generated
   */
  boolean isSetMutable();

  /**
   * Returns the value of the '<em><b>Type</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type</em>' attribute.
   * @see #setType(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getParameterType_Type()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='type'"
   * @generated
   */
  String getType();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.ParameterType#getType
   * <em>Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Type</em>' attribute.
   * @see #getType()
   * @generated
   */
  void setType(String value);
} // ParameterType
