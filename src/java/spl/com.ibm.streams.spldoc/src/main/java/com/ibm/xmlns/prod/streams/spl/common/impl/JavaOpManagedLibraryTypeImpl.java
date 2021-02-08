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
package com.ibm.xmlns.prod.streams.spl.common.impl;

import com.ibm.xmlns.prod.streams.spl.common.CommonPackage;
import com.ibm.xmlns.prod.streams.spl.common.JavaOpManagedLibraryType;
import java.util.Collection;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;
import org.eclipse.emf.ecore.util.EDataTypeEList;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Java Op Managed Library Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl#getLibPath
 *       <em>Lib Path</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.common.impl.JavaOpManagedLibraryTypeImpl#getCommand
 *       <em>Command</em>}
 * </ul>
 *
 * @generated
 */
public class JavaOpManagedLibraryTypeImpl extends MinimalEObjectImpl.Container
    implements JavaOpManagedLibraryType {
  /**
   * The cached value of the '{@link #getLibPath() <em>Lib Path</em>}' attribute list.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getLibPath()
   * @generated
   * @ordered
   */
  protected EList<String> libPath;

  /**
   * The default value of the '{@link #getCommand() <em>Command</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCommand()
   * @generated
   * @ordered
   */
  protected static final String COMMAND_EDEFAULT = null;

  /**
   * The cached value of the '{@link #getCommand() <em>Command</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #getCommand()
   * @generated
   * @ordered
   */
  protected String command = COMMAND_EDEFAULT;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected JavaOpManagedLibraryTypeImpl() {
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
    return CommonPackage.Literals.JAVA_OP_MANAGED_LIBRARY_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public EList<String> getLibPath() {
    if (libPath == null) {
      libPath =
          new EDataTypeEList<String>(
              String.class, this, CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH);
    }
    return libPath;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public String getCommand() {
    return command;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCommand(String newCommand) {
    String oldCommand = command;
    command = newCommand;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND,
              oldCommand,
              command));
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
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH:
        return getLibPath();
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND:
        return getCommand();
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
  @SuppressWarnings("unchecked")
  @Override
  public void eSet(int featureID, Object newValue) {
    switch (featureID) {
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH:
        getLibPath().clear();
        getLibPath().addAll((Collection<? extends String>) newValue);
        return;
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND:
        setCommand((String) newValue);
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
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH:
        getLibPath().clear();
        return;
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND:
        setCommand(COMMAND_EDEFAULT);
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
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__LIB_PATH:
        return libPath != null && !libPath.isEmpty();
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE__COMMAND:
        return COMMAND_EDEFAULT == null ? command != null : !COMMAND_EDEFAULT.equals(command);
    }
    return super.eIsSet(featureID);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public String toString() {
    if (eIsProxy()) return super.toString();

    StringBuffer result = new StringBuffer(super.toString());
    result.append(" (libPath: ");
    result.append(libPath);
    result.append(", command: ");
    result.append(command);
    result.append(')');
    return result.toString();
  }
} // JavaOpManagedLibraryTypeImpl
