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
 * A representation of the model object '<em><b>Composite Op Parameter Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType <em>Meta
 *       Type</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional
 *       <em>Optional</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpParameterType()
 * @model extendedMetaData="name='compositeOpParameterType' kind='elementOnly'"
 * @generated
 */
public interface CompositeOpParameterType extends ParameterType {
  /**
   * Returns the value of the '<em><b>Meta Type</b></em>' attribute. The literals are from the
   * enumeration {@link com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Meta Type</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Meta Type</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @see #isSetMetaType()
   * @see #unsetMetaType()
   * @see #setMetaType(CompositeParamEnumType)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpParameterType_MetaType()
   * @model unsettable="true" required="true" extendedMetaData="kind='attribute' name='metaType'"
   * @generated
   */
  CompositeParamEnumType getMetaType();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType <em>Meta
   * Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Meta Type</em>' attribute.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.CompositeParamEnumType
   * @see #isSetMetaType()
   * @see #unsetMetaType()
   * @see #getMetaType()
   * @generated
   */
  void setMetaType(CompositeParamEnumType value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType <em>Meta
   * Type</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetMetaType()
   * @see #getMetaType()
   * @see #setMetaType(CompositeParamEnumType)
   * @generated
   */
  void unsetMetaType();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#getMetaType <em>Meta
   * Type</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Meta Type</em>' attribute is set.
   * @see #unsetMetaType()
   * @see #getMetaType()
   * @see #setMetaType(CompositeParamEnumType)
   * @generated
   */
  boolean isSetMetaType();

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
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getCompositeOpParameterType_Optional()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean" required="true"
   *     extendedMetaData="kind='attribute' name='optional'"
   * @generated
   */
  boolean isOptional();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional <em>Optional</em>}'
   * attribute.
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional <em>Optional</em>}'
   * attribute.
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.CompositeOpParameterType#isOptional <em>Optional</em>}'
   * attribute is set.
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
} // CompositeOpParameterType
