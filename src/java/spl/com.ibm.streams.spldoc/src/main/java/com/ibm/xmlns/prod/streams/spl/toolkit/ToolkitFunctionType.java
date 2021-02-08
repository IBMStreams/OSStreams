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
 * A representation of the model object '<em><b>Function Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn
 *       <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex
 *       <em>Model Uri Index</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative <em>Native</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic <em>Public</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful
 *       <em>Stateful</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex <em>Uri
 *       Index</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType()
 * @model extendedMetaData="name='toolkitFunctionType' kind='elementOnly'"
 * @generated
 */
public interface ToolkitFunctionType extends BaseFunctionType {
  /**
   * Returns the value of the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Column</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Column</em>' attribute.
   * @see #isSetColumn()
   * @see #unsetColumn()
   * @see #setColumn(int)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_Column()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='column'"
   * @generated
   */
  int getColumn();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn <em>Column</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Column</em>' attribute.
   * @see #isSetColumn()
   * @see #unsetColumn()
   * @see #getColumn()
   * @generated
   */
  void setColumn(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn <em>Column</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetColumn()
   * @see #getColumn()
   * @see #setColumn(int)
   * @generated
   */
  void unsetColumn();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getColumn <em>Column</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Column</em>' attribute is set.
   * @see #unsetColumn()
   * @see #getColumn()
   * @see #setColumn(int)
   * @generated
   */
  boolean isSetColumn();

  /**
   * Returns the value of the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Line</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Line</em>' attribute.
   * @see #isSetLine()
   * @see #unsetLine()
   * @see #setLine(int)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_Line()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='line'"
   * @generated
   */
  int getLine();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine <em>Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Line</em>' attribute.
   * @see #isSetLine()
   * @see #unsetLine()
   * @see #getLine()
   * @generated
   */
  void setLine(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine <em>Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetLine()
   * @see #getLine()
   * @see #setLine(int)
   * @generated
   */
  void unsetLine();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getLine <em>Line</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Line</em>' attribute is set.
   * @see #unsetLine()
   * @see #getLine()
   * @see #setLine(int)
   * @generated
   */
  boolean isSetLine();

  /**
   * Returns the value of the '<em><b>Model Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Model Uri Index</em>' attribute isn't clear, there really should
   * be more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Model Uri Index</em>' attribute.
   * @see #isSetModelUriIndex()
   * @see #unsetModelUriIndex()
   * @see #setModelUriIndex(int)
   * @see
   *     com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_ModelUriIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int"
   *     extendedMetaData="kind='attribute' name='modelUriIndex'"
   * @generated
   */
  int getModelUriIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex <em>Model Uri
   * Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Model Uri Index</em>' attribute.
   * @see #isSetModelUriIndex()
   * @see #unsetModelUriIndex()
   * @see #getModelUriIndex()
   * @generated
   */
  void setModelUriIndex(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex <em>Model Uri
   * Index</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetModelUriIndex()
   * @see #getModelUriIndex()
   * @see #setModelUriIndex(int)
   * @generated
   */
  void unsetModelUriIndex();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getModelUriIndex <em>Model Uri
   * Index</em>}' attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Model Uri Index</em>' attribute is set.
   * @see #unsetModelUriIndex()
   * @see #getModelUriIndex()
   * @see #setModelUriIndex(int)
   * @generated
   */
  boolean isSetModelUriIndex();

  /**
   * Returns the value of the '<em><b>Native</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Native</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Native</em>' attribute.
   * @see #isSetNative()
   * @see #unsetNative()
   * @see #setNative(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_Native()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='native'"
   * @generated
   */
  boolean isNative();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative <em>Native</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Native</em>' attribute.
   * @see #isSetNative()
   * @see #unsetNative()
   * @see #isNative()
   * @generated
   */
  void setNative(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative <em>Native</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetNative()
   * @see #isNative()
   * @see #setNative(boolean)
   * @generated
   */
  void unsetNative();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isNative <em>Native</em>}' attribute
   * is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Native</em>' attribute is set.
   * @see #unsetNative()
   * @see #isNative()
   * @see #setNative(boolean)
   * @generated
   */
  boolean isSetNative();

  /**
   * Returns the value of the '<em><b>Public</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Public</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Public</em>' attribute.
   * @see #isSetPublic()
   * @see #unsetPublic()
   * @see #setPublic(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_Public()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='public'"
   * @generated
   */
  boolean isPublic();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic <em>Public</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Public</em>' attribute.
   * @see #isSetPublic()
   * @see #unsetPublic()
   * @see #isPublic()
   * @generated
   */
  void setPublic(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic <em>Public</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetPublic()
   * @see #isPublic()
   * @see #setPublic(boolean)
   * @generated
   */
  void unsetPublic();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isPublic <em>Public</em>}' attribute
   * is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Public</em>' attribute is set.
   * @see #unsetPublic()
   * @see #isPublic()
   * @see #setPublic(boolean)
   * @generated
   */
  boolean isSetPublic();

  /**
   * Returns the value of the '<em><b>Stateful</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Stateful</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Stateful</em>' attribute.
   * @see #isSetStateful()
   * @see #unsetStateful()
   * @see #setStateful(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_Stateful()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='stateful'"
   * @generated
   */
  boolean isStateful();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful <em>Stateful</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Stateful</em>' attribute.
   * @see #isSetStateful()
   * @see #unsetStateful()
   * @see #isStateful()
   * @generated
   */
  void setStateful(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful <em>Stateful</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetStateful()
   * @see #isStateful()
   * @see #setStateful(boolean)
   * @generated
   */
  void unsetStateful();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#isStateful <em>Stateful</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Stateful</em>' attribute is set.
   * @see #unsetStateful()
   * @see #isStateful()
   * @see #setStateful(boolean)
   * @generated
   */
  boolean isSetStateful();

  /**
   * Returns the value of the '<em><b>Uri Index</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Uri Index</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Uri Index</em>' attribute.
   * @see #isSetUriIndex()
   * @see #unsetUriIndex()
   * @see #setUriIndex(int)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getToolkitFunctionType_UriIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='uriIndex'"
   * @generated
   */
  int getUriIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex <em>Uri Index</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Uri Index</em>' attribute.
   * @see #isSetUriIndex()
   * @see #unsetUriIndex()
   * @see #getUriIndex()
   * @generated
   */
  void setUriIndex(int value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex <em>Uri Index</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetUriIndex()
   * @see #getUriIndex()
   * @see #setUriIndex(int)
   * @generated
   */
  void unsetUriIndex();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitFunctionType#getUriIndex <em>Uri Index</em>}'
   * attribute is set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Uri Index</em>' attribute is set.
   * @see #unsetUriIndex()
   * @see #getUriIndex()
   * @see #setUriIndex(int)
   * @generated
   */
  boolean isSetUriIndex();
} // ToolkitFunctionType
