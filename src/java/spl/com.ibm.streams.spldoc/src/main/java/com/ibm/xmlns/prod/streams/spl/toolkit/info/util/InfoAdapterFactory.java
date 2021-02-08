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
package com.ibm.xmlns.prod.streams.spl.toolkit.info.util;

import com.ibm.xmlns.prod.streams.spl.toolkit.info.*;
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
 * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.InfoPackage
 * @generated
 */
public class InfoAdapterFactory extends AdapterFactoryImpl {
  /**
   * The cached model package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static InfoPackage modelPackage;

  /**
   * Creates an instance of the adapter factory.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public InfoAdapterFactory() {
    if (modelPackage == null) {
      modelPackage = InfoPackage.eINSTANCE;
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
  protected InfoSwitch<Adapter> modelSwitch =
      new InfoSwitch<Adapter>() {
        @Override
        public Adapter caseDependenciesType(DependenciesType object) {
          return createDependenciesTypeAdapter();
        }

        @Override
        public Adapter caseDocumentRoot(DocumentRoot object) {
          return createDocumentRootAdapter();
        }

        @Override
        public Adapter caseExcludeType(ExcludeType object) {
          return createExcludeTypeAdapter();
        }

        @Override
        public Adapter caseIdentityType(IdentityType object) {
          return createIdentityTypeAdapter();
        }

        @Override
        public Adapter caseIncludeType(IncludeType object) {
          return createIncludeTypeAdapter();
        }

        @Override
        public Adapter caseLangType(LangType object) {
          return createLangTypeAdapter();
        }

        @Override
        public Adapter caseMessageSetType(MessageSetType object) {
          return createMessageSetTypeAdapter();
        }

        @Override
        public Adapter caseResourcesType(ResourcesType object) {
          return createResourcesTypeAdapter();
        }

        @Override
        public Adapter caseSabFilesType(SabFilesType object) {
          return createSabFilesTypeAdapter();
        }

        @Override
        public Adapter caseToolkitInfoModelType(ToolkitInfoModelType object) {
          return createToolkitInfoModelTypeAdapter();
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
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType <em>Dependencies Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DependenciesType
   * @generated
   */
  public Adapter createDependenciesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot <em>Document Root</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.DocumentRoot
   * @generated
   */
  public Adapter createDocumentRootAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType <em>Exclude Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ExcludeType
   * @generated
   */
  public Adapter createExcludeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType <em>Identity Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IdentityType
   * @generated
   */
  public Adapter createIdentityTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType <em>Include Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.IncludeType
   * @generated
   */
  public Adapter createIncludeTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType <em>Lang Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.LangType
   * @generated
   */
  public Adapter createLangTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType <em>Message Set Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.MessageSetType
   * @generated
   */
  public Adapter createMessageSetTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType <em>Resources Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ResourcesType
   * @generated
   */
  public Adapter createResourcesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType <em>Sab Files Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.SabFilesType
   * @generated
   */
  public Adapter createSabFilesTypeAdapter() {
    return null;
  }

  /**
   * Creates a new adapter for an object of class '{@link
   * com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType <em>Toolkit Info Model
   * Type</em>}'.
   * <!-- begin-user-doc -->
   * This default implementation returns null so that we can easily ignore cases; it's useful to
   * ignore a case when inheritance will catch all the cases anyway.
   * <!-- end-user-doc -->
   *
   * @return the new adapter.
   * @see com.ibm.xmlns.prod.streams.spl.toolkit.info.ToolkitInfoModelType
   * @generated
   */
  public Adapter createToolkitInfoModelTypeAdapter() {
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
} // InfoAdapterFactory
