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
package com.ibm.xmlns.prod.streams.spl.common.util;

import com.ibm.xmlns.prod.streams.spl.common.*;
import org.eclipse.emf.common.notify.Adapter;
import org.eclipse.emf.common.notify.Notifier;
import org.eclipse.emf.common.notify.impl.AdapterFactoryImpl;
import org.eclipse.emf.ecore.EObject;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Adapter Factory</b> for the model. It provides an adapter <code>createXXX</code> method
 * for each class of the model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage
 * @generated
 */
public class CommonAdapterFactory extends AdapterFactoryImpl {
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static CommonPackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonAdapterFactory() {
    if (modelPackage == null) {
      modelPackage = CommonPackage.eINSTANCE;
    }
  }

  /**
   * Returns whether this factory is applicable for the type of the object.
   * <!-- begin-user-doc -->
   * This implementation returns <code>true</code> if the object is either the model's package or is
   * an instance object of the model.
   * <!-- end-user-doc -->
   *
   * @return whether this factory is applicable for the type of the object.
   * @generated
   */
  @Override
  public boolean isFactoryForType(Object object) {
    if (object == modelPackage) {
      return true;
    }
    if (object instanceof EObject) {
      return ((EObject) object).eClass().getEPackage() == modelPackage;
    }
    return false;
  }

  /**
   * The switch that delegates to the <code>createXXX</code> methods.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CommonSwitch<Adapter> modelSwitch =
      new CommonSwitch<Adapter>() {
        @Override
        public Adapter caseDescriptionType(DescriptionType object) {
          return createDescriptionTypeAdapter();
        }

        @Override
        public Adapter caseDocumentRoot(DocumentRoot object) {
          return createDocumentRootAdapter();
        }

        @Override
        public Adapter caseJavaOpLibraryType(JavaOpLibraryType object) {
          return createJavaOpLibraryTypeAdapter();
        }

        @Override
        public Adapter caseJavaOpManagedLibraryType(JavaOpManagedLibraryType object) {
          return createJavaOpManagedLibraryTypeAdapter();
        }

        @Override
        public Adapter caseLibraryType(LibraryType object) {
          return createLibraryTypeAdapter();
        }

        @Override
        public Adapter caseManagedLibraryType(ManagedLibraryType object) {
          return createManagedLibraryTypeAdapter();
        }

        @Override
        public Adapter caseToolkitDependencyType(ToolkitDependencyType object) {
          return createToolkitDependencyTypeAdapter();
        }

        @Override
        public Adapter caseUriType(UriType object) {
          return createUriTypeAdapter();
        }

        @Override
        public Adapter defaultCase(EObject object) {
          return createEObjectAdapter();
        }
      };

  /**
   * Creates an adapter for the <code>target</code>.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @param target the object to adapt.
   * @return the adapter for the <code>target</code>.
   * @generated
   */
  @Override
  public Adapter createAdapter(Notifier target) {
    return modelSwitch.doSwitch((EObject) target);
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DescriptionType <em>Description Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.DescriptionType
   * @generated
   */
  public Adapter createDescriptionTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.DocumentRoot
   * @generated
   */
  public Adapter createDocumentRootAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType <em>Java Op Library Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpLibraryType
   * @generated
   */
  public Adapter createJavaOpLibraryTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType <em>Java Op Managed Library
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType
   * @generated
   */
  public Adapter createJavaOpManagedLibraryTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.LibraryType <em>Library Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.LibraryType
   * @generated
   */
  public Adapter createLibraryTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType <em>Managed Library Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.ManagedLibraryType
   * @generated
   */
  public Adapter createManagedLibraryTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType <em>Toolkit Dependency Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.ToolkitDependencyType
   * @generated
   */
  public Adapter createToolkitDependencyTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.common.UriType <em>Uri Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.common.UriType
   * @generated
   */
  public Adapter createUriTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for the default case.
   * <!-- begin-user-doc -->
   * This default implementation returns null.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @generated
   */
  public Adapter createEObjectAdapter() {
    return null;
  }
} // CommonAdapterFactory
