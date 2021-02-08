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

import java.math.BigInteger;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * A representation of the model object '<em><b>Prototype Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are supported:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getValue <em>Value</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getColumn <em>Column</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getCppName <em>Cpp Name</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getLine <em>Line</em>}
 * </ul>
 *
 * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getPrototypeType()
 * @model extendedMetaData="name='prototypeType' kind='simple'"
 * @generated
 */
public interface PrototypeType extends EObject {
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
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getPrototypeType_Value()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" extendedMetaData="name=':0'
   *     kind='simple'"
   * @generated
   */
  String getValue();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getValue
   * <em>Value</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Value</em>' attribute.
   * @see #getValue()
   * @generated
   */
  void setValue(String value);

  /**
   * Returns the value of the '<em><b>Column</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Column</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Column</em>' attribute.
   * @see #setColumn(BigInteger)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getPrototypeType_Column()
   * @model dataType="org.eclipse.emf.ecore.xml.type.UnsignedLong"
   *     extendedMetaData="kind='attribute' name='column'"
   * @generated
   */
  BigInteger getColumn();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getColumn
   * <em>Column</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Column</em>' attribute.
   * @see #getColumn()
   * @generated
   */
  void setColumn(BigInteger value);

  /**
   * Returns the value of the '<em><b>Cpp Name</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Cpp Name</em>' attribute isn't clear, there really should be more
   * of a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Cpp Name</em>' attribute.
   * @see #setCppName(String)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getPrototypeType_CppName()
   * @model dataType="org.eclipse.emf.ecore.xml.type.Token" extendedMetaData="kind='attribute'
   *     name='cppName'"
   * @generated
   */
  String getCppName();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getCppName
   * <em>Cpp Name</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Cpp Name</em>' attribute.
   * @see #getCppName()
   * @generated
   */
  void setCppName(String value);

  /**
   * Returns the value of the '<em><b>Line</b></em>' attribute.
   * <!-- begin-user-doc -->
   *
   * <p>If the meaning of the '<em>Line</em>' attribute isn't clear, there really should be more of
   * a description here...
   * <!-- end-user-doc -->
   *
   * @return the value of the '<em>Line</em>' attribute.
   * @see #setLine(BigInteger)
   * @see com.ibm.xmlns.prod.streams.spl.function.FunctionPackage#getPrototypeType_Line()
   * @model dataType="org.eclipse.emf.ecore.xml.type.UnsignedLong"
   *     extendedMetaData="kind='attribute' name='line'"
   * @generated
   */
  BigInteger getLine();

  /**
   * Sets the value of the '{@link com.ibm.xmlns.prod.streams.spl.function.PrototypeType#getLine
   * <em>Line</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param value the new value of the '<em>Line</em>' attribute.
   * @see #getLine()
   * @generated
   */
  void setLine(BigInteger value);
} // PrototypeType
