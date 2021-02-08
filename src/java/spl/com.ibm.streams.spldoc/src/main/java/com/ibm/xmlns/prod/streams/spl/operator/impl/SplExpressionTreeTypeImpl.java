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
package com.ibm.xmlns.prod.streams.spl.operator.impl;

import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.operator.SplExpressionTreeType;
import org.eclipse.emf.common.notify.Notification;
import org.eclipse.emf.ecore.EClass;
import org.eclipse.emf.ecore.impl.ENotificationImpl;
import org.eclipse.emf.ecore.impl.MinimalEObjectImpl;

/**
 *
 * <!-- begin-user-doc -->
 * An implementation of the model object '<em><b>Spl Expression Tree Type</b></em>'.
 * <!-- end-user-doc -->
 *
 * <p>The following features are implemented:
 *
 * <ul>
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl#isCppCode
 *       <em>Cpp Code</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl#isOutput
 *       <em>Output</em>}
 *   <li>{@link com.ibm.xmlns.prod.streams.spl.operator.impl.SplExpressionTreeTypeImpl#isParam
 *       <em>Param</em>}
 * </ul>
 *
 * @generated
 */
public class SplExpressionTreeTypeImpl extends MinimalEObjectImpl.Container
    implements SplExpressionTreeType {
  /**
   * The default value of the '{@link #isCppCode() <em>Cpp Code</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isCppCode()
   * @generated
   * @ordered
   */
  protected static final boolean CPP_CODE_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isCppCode() <em>Cpp Code</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isCppCode()
   * @generated
   * @ordered
   */
  protected boolean cppCode = CPP_CODE_EDEFAULT;

  /**
   * This is true if the Cpp Code attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean cppCodeESet;

  /**
   * The default value of the '{@link #isOutput() <em>Output</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isOutput()
   * @generated
   * @ordered
   */
  protected static final boolean OUTPUT_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isOutput() <em>Output</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isOutput()
   * @generated
   * @ordered
   */
  protected boolean output = OUTPUT_EDEFAULT;

  /**
   * This is true if the Output attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean outputESet;

  /**
   * The default value of the '{@link #isParam() <em>Param</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isParam()
   * @generated
   * @ordered
   */
  protected static final boolean PARAM_EDEFAULT = false;

  /**
   * The cached value of the '{@link #isParam() <em>Param</em>}' attribute.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see #isParam()
   * @generated
   * @ordered
   */
  protected boolean param = PARAM_EDEFAULT;

  /**
   * This is true if the Param attribute has been set.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @ordered
   */
  protected boolean paramESet;

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected SplExpressionTreeTypeImpl() {
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
    return OperatorPackage.Literals.SPL_EXPRESSION_TREE_TYPE;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isCppCode() {
    return cppCode;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setCppCode(boolean newCppCode) {
    boolean oldCppCode = cppCode;
    cppCode = newCppCode;
    boolean oldCppCodeESet = cppCodeESet;
    cppCodeESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE,
              oldCppCode,
              cppCode,
              !oldCppCodeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetCppCode() {
    boolean oldCppCode = cppCode;
    boolean oldCppCodeESet = cppCodeESet;
    cppCode = CPP_CODE_EDEFAULT;
    cppCodeESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE,
              oldCppCode,
              CPP_CODE_EDEFAULT,
              oldCppCodeESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetCppCode() {
    return cppCodeESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isOutput() {
    return output;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setOutput(boolean newOutput) {
    boolean oldOutput = output;
    output = newOutput;
    boolean oldOutputESet = outputESet;
    outputESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT,
              oldOutput,
              output,
              !oldOutputESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetOutput() {
    boolean oldOutput = output;
    boolean oldOutputESet = outputESet;
    output = OUTPUT_EDEFAULT;
    outputESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT,
              oldOutput,
              OUTPUT_EDEFAULT,
              oldOutputESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetOutput() {
    return outputESet;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isParam() {
    return param;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void setParam(boolean newParam) {
    boolean oldParam = param;
    param = newParam;
    boolean oldParamESet = paramESet;
    paramESet = true;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.SET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM,
              oldParam,
              param,
              !oldParamESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public void unsetParam() {
    boolean oldParam = param;
    boolean oldParamESet = paramESet;
    param = PARAM_EDEFAULT;
    paramESet = false;
    if (eNotificationRequired())
      eNotify(
          new ENotificationImpl(
              this,
              Notification.UNSET,
              OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM,
              oldParam,
              PARAM_EDEFAULT,
              oldParamESet));
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean isSetParam() {
    return paramESet;
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
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE:
        return isCppCode();
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT:
        return isOutput();
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM:
        return isParam();
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
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE:
        setCppCode((Boolean) newValue);
        return;
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT:
        setOutput((Boolean) newValue);
        return;
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM:
        setParam((Boolean) newValue);
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
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE:
        unsetCppCode();
        return;
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT:
        unsetOutput();
        return;
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM:
        unsetParam();
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
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__CPP_CODE:
        return isSetCppCode();
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__OUTPUT:
        return isSetOutput();
      case OperatorPackage.SPL_EXPRESSION_TREE_TYPE__PARAM:
        return isSetParam();
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
    result.append(" (cppCode: ");
    if (cppCodeESet) result.append(cppCode);
    else result.append("<unset>");
    result.append(", output: ");
    if (outputESet) result.append(output);
    else result.append("<unset>");
    result.append(", param: ");
    if (paramESet) result.append(param);
    else result.append("<unset>");
    result.append(')');
    return result.toString();
  }
} // SplExpressionTreeTypeImpl
