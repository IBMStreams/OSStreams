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
 * A representation of the model object '<em><b>Window Policy Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getAttribute
 *       <em>Attribute</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind <em>Kind</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getSize <em>Size</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowPolicyType()
 * @model extendedMetaData="name='windowPolicyType' kind='empty'"
 * @generated
 */
public interface WindowPolicyType extends SourceLocationType {
  /**
   * Returns the value of the '<em><b>Attribute</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Attribute</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Attribute</em>' attribute.
   * @see #setAttribute(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowPolicyType_Attribute()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='attribute'"
   * @generated
   */
  String getAttribute();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getAttribute <em>Attribute</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Attribute</em>' attribute.
   * @see #getAttribute()
   * @generated
   */
  void setAttribute(String value);

  /**
   * Returns the value of the '<em><b>Kind</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Kind</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Kind</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @see #isSetKind()
   * @see #unsetKind()
   * @see #setKind(WindowPolicyKindEnumType)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowPolicyType_Kind()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute' name='kind'"
   * @generated
   */
  WindowPolicyKindEnumType getKind();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Kind</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyKindEnumType
   * @see #isSetKind()
   * @see #unsetKind()
   * @see #getKind()
   * @generated
   */
  void setKind(WindowPolicyKindEnumType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind <em>Kind</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetKind()
   * @see #getKind()
   * @see #setKind(WindowPolicyKindEnumType)
   * @generated
   */
  void unsetKind();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getKind <em>Kind</em>}' attribute
   * is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Kind</em>' attribute is set.
   * @see #unsetKind()
   * @see #getKind()
   * @see #setKind(WindowPolicyKindEnumType)
   * @generated
   */
  boolean isSetKind();

  /**
   * Returns the value of the '<em><b>Size</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Size</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Size</em>' attribute.
   * @see #setSize(String)
   * @see com.ibm.xmlns.prod.streams.spl.source.code.CodePackage#getWindowPolicyType_Size()
   * @model dataType="org.eclipse.emf.ecore.xml.type.String" extendedMetaData="kind='attribute'
   *     name='size'"
   * @generated
   */
  String getSize();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.source.code.WindowPolicyType#getSize <em>Size</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Size</em>' attribute.
   * @see #getSize()
   * @generated
   */
  void setSize(String value);
} // WindowPolicyType
