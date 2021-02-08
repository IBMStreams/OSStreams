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
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeBodyType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeDefinitionType;
import com.ibm.xmlns.prod.streams.spl.source.code.CompositeHeadType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.notify.NotificationChain;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.InternalEObject;
import org.eclipse.emf.ecore.impl.ENotificationImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Composite Definition Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl#getCompositeHead
 *       <em>Composite Head</em>}
 *   <li>{@link
 *       com.ibm.xmlns.prod.streams.spl.source.code.impl.CompositeDefinitionTypeImpl#getCompositeBody
 *       <em>Composite Body</em>}
 * </ul>
 *
 * @generated
 */
public class CompositeDefinitionTypeImpl extends ExtendedSourceLocationTypeImpl
    implements CompositeDefinitionType {
  /**
   * The cached value of the '{@link #getCompositeHead() <em>Composite Head</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCompositeHead()
   * @generated
   * @ordered
   */
  protected CompositeHeadType compositeHead;

  /**
   * The cached value of the '{@link #getCompositeBody() <em>Composite Body</em>}' containment
   * reference.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCompositeBody()
   * @generated
   * @ordered
   */
  protected CompositeBodyType compositeBody;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected CompositeDefinitionTypeImpl() {
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
    return CodePackage.Literals.COMPOSITE_DEFINITION_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeHeadType getCompositeHead() {
    return compositeHead;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCompositeHead(
      CompositeHeadType newCompositeHead, NotificationChain msgs) {
    CompositeHeadType oldCompositeHead = compositeHead;
    compositeHead = newCompositeHead;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD,
              oldCompositeHead,
              newCompositeHead);
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
  public void setCompositeHead(CompositeHeadType newCompositeHead) {
    if (newCompositeHead != compositeHead) {
      NotificationChain msgs = null;
      if (compositeHead != null)
        msgs =
            ((InternalEObject) compositeHead)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD,
                    null,
                    msgs);
      if (newCompositeHead != null)
        msgs =
            ((InternalEObject) newCompositeHead)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD,
                    null,
                    msgs);
      msgs = basicSetCompositeHead(newCompositeHead, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD,
              newCompositeHead,
              newCompositeHead));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CompositeBodyType getCompositeBody() {
    return compositeBody;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public NotificationChain basicSetCompositeBody(
      CompositeBodyType newCompositeBody, NotificationChain msgs) {
    CompositeBodyType oldCompositeBody = compositeBody;
    compositeBody = newCompositeBody;
    if (eNotificationRequired()) {
      ENotificationImpl notification =
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY,
              oldCompositeBody,
              newCompositeBody);
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
  public void setCompositeBody(CompositeBodyType newCompositeBody) {
    if (newCompositeBody != compositeBody) {
      NotificationChain msgs = null;
      if (compositeBody != null)
        msgs =
            ((InternalEObject) compositeBody)
                .eInverseRemove(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY,
                    null,
                    msgs);
      if (newCompositeBody != null)
        msgs =
            ((InternalEObject) newCompositeBody)
                .eInverseAdd(
                    this,
                    EOPPOSITE_FEATURE_BASE - CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY,
                    null,
                    msgs);
      msgs = basicSetCompositeBody(newCompositeBody, msgs);
      if (msgs != null) msgs.dispatch();
    } else if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY,
              newCompositeBody,
              newCompositeBody));
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
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD:
        return basicSetCompositeHead(null, msgs);
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY:
        return basicSetCompositeBody(null, msgs);
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
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD:
        return getCompositeHead();
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY:
        return getCompositeBody();
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
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD:
        setCompositeHead((CompositeHeadType) newValue);
        return;
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY:
        setCompositeBody((CompositeBodyType) newValue);
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
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD:
        setCompositeHead((CompositeHeadType) null);
        return;
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY:
        setCompositeBody((CompositeBodyType) null);
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
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_HEAD:
        return compositeHead != null;
      case CodePackage.COMPOSITE_DEFINITION_TYPE__COMPOSITE_BODY:
        return compositeBody != null;
    }
    return super.eIsSet(featureID);
  }
} // CompositeDefinitionTypeImpl
