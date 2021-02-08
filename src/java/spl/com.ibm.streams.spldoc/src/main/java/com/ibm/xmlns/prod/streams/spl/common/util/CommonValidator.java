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
import java.util.Map;
import org.eclipse.emf.common.util.DiagnosticChain;
import org.eclipse.emf.common.util.ResourceLocator;
import org.eclipse.emf.ecore.EPackage;
import org.eclipse.emf.ecore.util.EObjectValidator;
import org.eclipse.emf.ecore.xml.type.util.XMLTypeUtil;
import org.eclipse.emf.ecore.xml.type.util.XMLTypeValidator;

/**
 *
 * <!-- begin-user-doc -->
 * The <b>Validator</b> for the model.
 * <!-- end-user-doc -->
 *
 * @see com.ibm.xmlns.prod.streams.spl.common.CommonPackage
 * @generated
 */
public class CommonValidator extends EObjectValidator {
  /**
   * The cached model package
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public static final CommonValidator INSTANCE = new CommonValidator();

  /**
   * A constant for the {@link org.eclipse.emf.common.util.Diagnostic#getSource() source} of
   * diagnostic {@link org.eclipse.emf.common.util.Diagnostic#getCode() codes} from this package.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @see org.eclipse.emf.common.util.Diagnostic#getSource()
   * @see org.eclipse.emf.common.util.Diagnostic#getCode()
   * @generated
   */
  public static final String DIAGNOSTIC_SOURCE = "com.ibm.xmlns.prod.streams.spl.common";

  /**
   * A constant with a fixed name that can be used as the base value for additional hand written
   * constants.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  private static final int GENERATED_DIAGNOSTIC_CODE_COUNT = 0;

  /**
   * A constant with a fixed name that can be used as the base value for additional hand written
   * constants in a derived class.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected static final int DIAGNOSTIC_CODE_COUNT = GENERATED_DIAGNOSTIC_CODE_COUNT;

  /**
   * The cached base package validator.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  protected XMLTypeValidator xmlTypeValidator;

  /**
   * Creates an instance of the switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public CommonValidator() {
    super();
    xmlTypeValidator = XMLTypeValidator.INSTANCE;
  }

  /**
   * Returns the package of this validator switch.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  protected EPackage getEPackage() {
    return CommonPackage.eINSTANCE;
  }

  /**
   * Calls <code>validateXXX</code> for the corresponding classifier of the model.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  protected boolean validate(
      int classifierID, Object value, DiagnosticChain diagnostics, Map<Object, Object> context) {
    switch (classifierID) {
      case CommonPackage.DESCRIPTION_TYPE:
        return validateDescriptionType((DescriptionType) value, diagnostics, context);
      case CommonPackage.DOCUMENT_ROOT:
        return validateDocumentRoot((DocumentRoot) value, diagnostics, context);
      case CommonPackage.JAVA_OP_LIBRARY_TYPE:
        return validateJavaOpLibraryType((JavaOpLibraryType) value, diagnostics, context);
      case CommonPackage.JAVA_OP_MANAGED_LIBRARY_TYPE:
        return validateJavaOpManagedLibraryType(
            (JavaOpManagedLibraryType) value, diagnostics, context);
      case CommonPackage.LIBRARY_TYPE:
        return validateLibraryType((LibraryType) value, diagnostics, context);
      case CommonPackage.MANAGED_LIBRARY_TYPE:
        return validateManagedLibraryType((ManagedLibraryType) value, diagnostics, context);
      case CommonPackage.TOOLKIT_DEPENDENCY_TYPE:
        return validateToolkitDependencyType((ToolkitDependencyType) value, diagnostics, context);
      case CommonPackage.URI_TYPE:
        return validateUriType((UriType) value, diagnostics, context);
      case CommonPackage.VERSION_RANGE_TYPE:
        return validateVersionRangeType((String) value, diagnostics, context);
      case CommonPackage.VERSION_TYPE:
        return validateVersionType((String) value, diagnostics, context);
      default:
        return true;
    }
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateDescriptionType(
      DescriptionType descriptionType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(descriptionType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateDocumentRoot(
      DocumentRoot documentRoot, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(documentRoot, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateJavaOpLibraryType(
      JavaOpLibraryType javaOpLibraryType,
      DiagnosticChain diagnostics,
      Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(javaOpLibraryType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateJavaOpManagedLibraryType(
      JavaOpManagedLibraryType javaOpManagedLibraryType,
      DiagnosticChain diagnostics,
      Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(javaOpManagedLibraryType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateLibraryType(
      LibraryType libraryType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(libraryType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateManagedLibraryType(
      ManagedLibraryType managedLibraryType,
      DiagnosticChain diagnostics,
      Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(managedLibraryType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateToolkitDependencyType(
      ToolkitDependencyType toolkitDependencyType,
      DiagnosticChain diagnostics,
      Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(toolkitDependencyType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateUriType(
      UriType uriType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validate_EveryDefaultConstraint(uriType, diagnostics, context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateVersionRangeType(
      String versionRangeType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    boolean result = validateVersionRangeType_Pattern(versionRangeType, diagnostics, context);
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @see #validateVersionRangeType_Pattern
   */
  public static final PatternMatcher[][] VERSION_RANGE_TYPE__PATTERN__VALUES =
      new PatternMatcher[][] {
        new PatternMatcher[] {
          XMLTypeUtil.createPatternMatcher(
              "(\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?)|([\\[\\(]\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?,\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?[\\)\\]])")
        }
      };

  /**
   * Validates the Pattern constraint of '<em>Version Range Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateVersionRangeType_Pattern(
      String versionRangeType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validatePattern(
        CommonPackage.Literals.VERSION_RANGE_TYPE,
        versionRangeType,
        VERSION_RANGE_TYPE__PATTERN__VALUES,
        diagnostics,
        context);
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateVersionType(
      String versionType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    boolean result = validateVersionType_Pattern(versionType, diagnostics, context);
    return result;
  }

  /**
   *
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   * @see #validateVersionType_Pattern
   */
  public static final PatternMatcher[][] VERSION_TYPE__PATTERN__VALUES =
      new PatternMatcher[][] {
        new PatternMatcher[] {
          XMLTypeUtil.createPatternMatcher("\\d+(\\.\\d+(\\.\\d+(\\.[0-9a-zA-Z_\\-]+)?)?)?")
        }
      };

  /**
   * Validates the Pattern constraint of '<em>Version Type</em>'.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  public boolean validateVersionType_Pattern(
      String versionType, DiagnosticChain diagnostics, Map<Object, Object> context) {
    return validatePattern(
        CommonPackage.Literals.VERSION_TYPE,
        versionType,
        VERSION_TYPE__PATTERN__VALUES,
        diagnostics,
        context);
  }

  /**
   * Returns the resource locator that will be used to fetch messages for this validator's
   * diagnostics.
   * <!-- begin-user-doc -->
   * <!-- end-user-doc -->
   *
   * @generated
   */
  @Override
  public ResourceLocator getResourceLocator() {
    // TODO
    // Specialize this to return a resource locator for messages specific to this validator.
    // Ensure that you remove @generated or mark it @generated NOT
    return super.getResourceLocator();
  }
} // CommonValidator
