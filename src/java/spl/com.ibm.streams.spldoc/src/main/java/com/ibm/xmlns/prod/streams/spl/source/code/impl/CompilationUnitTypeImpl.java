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
package com.ibm.xmlns.prod.streams.spl.source.code.impl;

import com.ibm.xmlns.prod.streams.spl.source.code.CodePackage;
import com.ibm.xmlns.prod.streams.spl.source.code.CompilationUnitType;
import com.ibm.xmlns.prod.streams.spl.source.code.DefinitionsType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplDocType;
import com.ibm.xmlns.prod.streams.spl.source.code.SplNamespaceType;
import com.ibm.xmlns.prod.streams.spl.source.code.UseDirectivesType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.EObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Compilation Unit Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl#getSplDoc
 *       <em>Spl Doc</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl#getSplNamespace
 *       <em>Spl Namespace</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl#getUseDirectives
 *       <em>Use Directives</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompilationUnitTypeImpl#getDefinitions
 *       <em>Definitions</em>}
 * </ul>
 *
 * @generated
 */
public class CompilationUnitTypeImpl extends EObjectImpl implements CompilationUnitType {
  /**
   * The cached value of the '{@link #getSplDoc() <em>Spl Doc</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSplDoc()
   * @generated
   * @ordered
   */
  protected SplDocType splDoc;

  /**
   * The cached value of the '{@link #getSplNamespace() <em>Spl Namespace</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getSplNamespace()
   * @generated
   * @ordered
   */
  protected SplNamespaceType splNamespace;

  /**
   * The cached value of the '{@link #getUseDirectives() <em>Use Directives</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getUseDirectives()
   * @generated
   * @ordered
   */
  protected UseDirectivesType useDirectives;

  /**
   * The cached value of the '{@link #getDefinitions() <em>Definitions</em>}' containment reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getDefinitions()
   * @generated
   * @ordered
   */
  protected DefinitionsType definitions;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompilationUnitTypeImpl() {
    super();
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  protected EClass eStaticClass() {
    return CodePackage.Literals.COMPILATION_UNIT_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplDocType getSplDoc() {
    return splDoc;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSplDoc(SplDocType newSplDoc, NotificationChain msgs) {
    SplDocType oldSplDoc = splDoc;
    splDoc = newSplDoc;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC,
              oldSplDoc,
              newSplDoc);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSplDoc(SplDocType newSplDoc) {
    if (newSplDoc != splDoc) {
      NotificationChain msgs = null;
      if (splDoc != null)
        msgs =
            ((InternalEObject) splDoc)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC,
                    null,
                    msgs);
      if (newSplDoc != null)
        msgs =
            ((InternalEObject) newSplDoc)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC,
                    null,
                    msgs);
      msgs = basicSetSplDoc(newSplDoc, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC,
              newSplDoc,
              newSplDoc));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public SplNamespaceType getSplNamespace() {
    return splNamespace;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetSplNamespace(
      SplNamespaceType newSplNamespace, NotificationChain msgs) {
    SplNamespaceType oldSplNamespace = splNamespace;
    splNamespace = newSplNamespace;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE,
              oldSplNamespace,
              newSplNamespace);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setSplNamespace(SplNamespaceType newSplNamespace) {
    if (newSplNamespace != splNamespace) {
      NotificationChain msgs = null;
      if (splNamespace != null)
        msgs =
            ((InternalEObject) splNamespace)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE,
                    null,
                    msgs);
      if (newSplNamespace != null)
        msgs =
            ((InternalEObject) newSplNamespace)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE,
                    null,
                    msgs);
      msgs = basicSetSplNamespace(newSplNamespace, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE,
              newSplNamespace,
              newSplNamespace));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public UseDirectivesType getUseDirectives() {
    return useDirectives;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetUseDirectives(
      UseDirectivesType newUseDirectives, NotificationChain msgs) {
    UseDirectivesType oldUseDirectives = useDirectives;
    useDirectives = newUseDirectives;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES,
              oldUseDirectives,
              newUseDirectives);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setUseDirectives(UseDirectivesType newUseDirectives) {
    if (newUseDirectives != useDirectives) {
      NotificationChain msgs = null;
      if (useDirectives != null)
        msgs =
            ((InternalEObject) useDirectives)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES,
                    null,
                    msgs);
      if (newUseDirectives != null)
        msgs =
            ((InternalEObject) newUseDirectives)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES,
                    null,
                    msgs);
      msgs = basicSetUseDirectives(newUseDirectives, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES,
              newUseDirectives,
              newUseDirectives));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public DefinitionsType getDefinitions() {
    return definitions;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetDefinitions(
      DefinitionsType newDefinitions, NotificationChain msgs) {
    DefinitionsType oldDefinitions = definitions;
    definitions = newDefinitions;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS,
              oldDefinitions,
              newDefinitions);
      if (msgs == null) msgs = notification;
      else msgs.add(notification);
    }
    return msgs;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setDefinitions(DefinitionsType newDefinitions) {
    if (newDefinitions != definitions) {
      NotificationChain msgs = null;
      if (definitions != null)
        msgs =
            ((InternalEObject) definitions)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS,
                    null,
                    msgs);
      if (newDefinitions != null)
        msgs =
            ((InternalEObject) newDefinitions)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS,
                    null,
                    msgs);
      msgs = basicSetDefinitions(newDefinitions, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS,
              newDefinitions,
              newDefinitions));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public NotificationChain eInverseRemove(
      InternalEObject otherEnd, int featureID, NotificationChain msgs) {
    switch (featureID) {
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC:
        return basicSetSplDoc(null, msgs);
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE:
        return basicSetSplNamespace(null, msgs);
      case CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES:
        return basicSetUseDirectives(null, msgs);
      case CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS:
        return basicSetDefinitions(null, msgs);
    }
    return super.eInverseRemove(otherEnd, featureID, msgs);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public Object eGet(int featureID, boolean resolve, boolean coreType) {
    switch (featureID) {
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC:
        return getSplDoc();
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE:
        return getSplNamespace();
      case CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES:
        return getUseDirectives();
      case CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS:
        return getDefinitions();
    }
    return super.eGet(featureID, resolve, coreType);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC:
        setSplDoc((SplDocType) newValue);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE:
        setSplNamespace((SplNamespaceType) newValue);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES:
        setUseDirectives((UseDirectivesType) newValue);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS:
        setDefinitions((DefinitionsType) newValue);
        return;
    }
    super.eSet(featureID, newValue);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public void eUnset(int featureID) {
    switch (featureID) {
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC:
        setSplDoc((SplDocType) null);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE:
        setSplNamespace((SplNamespaceType) null);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES:
        setUseDirectives((UseDirectivesType) null);
        return;
      case CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS:
        setDefinitions((DefinitionsType) null);
        return;
    }
    super.eUnset(featureID);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public boolean eIsSet(int featureID) {
    switch (featureID) {
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_DOC:
        return splDoc != null;
      case CodePackage.COMPILATION_UNIT_TYPE__SPL_NAMESPACE:
        return splNamespace != null;
      case CodePackage.COMPILATION_UNIT_TYPE__USE_DIRECTIVES:
        return useDirectives != null;
      case CodePackage.COMPILATION_UNIT_TYPE__DEFINITIONS:
        return definitions != null;
    }
    return super.eIsSet(featureID);
  }
} // CompilationUnitTypeImpl
