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
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Enum Literals Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getDescription
 *       <em>Description</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getEnum <em>Enum</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine <em>Line</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic <em>Static</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getTypeName <em>Type
 *       Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex <em>Uri
 *       Index</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType()
 * @model extendedMetaData="name='enumLiteralsType' kind='elementOnly'"
 * @generated
 */
public interface EnumLiteralsType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_Description()
   * @model containment="true" extendedMetaData="kind='element' name='description'
   *     namespace='##targetNamespace'"
   * @generated
   */
  DescriptionType getDescription();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getDescription <em>Description</em>}'
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
   * Returns the value of the '<em><b>Enum</b></em>' attribute list. The list contents are of type
   * {@link java.lang.String}.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Enum</em>' attribute list isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Enum</em>' attribute list.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_Enum()
   * @model unique="false" dataType="org.eclipse.emf.ecore.xml.type.Token"
   *     extendedMetaData="kind='element' name='enum' namespace='##targetNamespace'"
   * @generated
   */
  EList<String> getEnum();

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_Column()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='column'"
   * @generated
   */
  int getColumn();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn
   * <em>Column</em>}' attribute.
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn <em>Column</em>}' attribute.
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getColumn <em>Column</em>}' attribute
   * is set.
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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_Line()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='line'"
   * @generated
   */
  int getLine();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine
   * <em>Line</em>}' attribute.
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
   * Unsets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine
   * <em>Line</em>}' attribute.
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getLine <em>Line</em>}' attribute is
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
   * Returns the value of the '<em><b>Static</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Static</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Static</em>' attribute.
   * @see #isSetStatic()
   * @see #unsetStatic()
   * @see #setStatic(boolean)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_Static()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Boolean"
   *     extendedMetaData="kind='attribute' name='static'"
   * @generated
   */
  boolean isStatic();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic
   * <em>Static</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Static</em>' attribute.
   * @see #isSetStatic()
   * @see #unsetStatic()
   * @see #isStatic()
   * @generated
   */
  void setStatic(boolean value);

  /**
   * Unsets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic <em>Static</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isSetStatic()
   * @see #isStatic()
   * @see #setStatic(boolean)
   * @generated
   */
  void unsetStatic();

  /**
   * Returns whether the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#isStatic <em>Static</em>}' attribute is
   * set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @return whether the value of the '<em>Static</em>' attribute is set.
   * @see #unsetStatic()
   * @see #isStatic()
   * @see #setStatic(boolean)
   * @generated
   */
  boolean isSetStatic();

  /**
   * Returns the value of the '<em><b>Type Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Type Name</em>' attribute isn't clear, there really should be
   * more of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Type Name</em>' attribute.
   * @see #setTypeName(String)
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_TypeName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" extendedMetaData="kind='attribute'
   *     name='typeName'"
   * @generated
   */
  String getTypeName();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getTypeName <em>Type Name</em>}'
   * attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Type Name</em>' attribute.
   * @see #getTypeName()
   * @generated
   */
  void setTypeName(String value);

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
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage#getEnumLiteralsType_UriIndex()
   * @model unsettable="true" dataType="org.eclipse.emf.ecore.xml.type.Int" required="true"
   *     extendedMetaData="kind='attribute' name='uriIndex'"
   * @generated
   */
  int getUriIndex();

  /**
   * Sets the value of the '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex <em>Uri Index</em>}'
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex <em>Uri Index</em>}'
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.EnumLiteralsType#getUriIndex <em>Uri Index</em>}'
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
} // EnumLiteralsType
