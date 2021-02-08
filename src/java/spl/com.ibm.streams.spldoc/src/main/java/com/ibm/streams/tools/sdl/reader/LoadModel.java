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

package com.ibm.streams.tools.sdl.reader;

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import com.ibm.xmlns.prod.streams.spl.function.FunctionModelType;
import com.ibm.xmlns.prod.streams.spl.function.FunctionPackage;
import com.ibm.xmlns.prod.streams.spl.operator.JavaOpModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorModelType;
import com.ibm.xmlns.prod.streams.spl.operator.OperatorPackage;
import com.ibm.xmlns.prod.streams.spl.source.code.CodePackage;
import com.ibm.xmlns.prod.streams.spl.source.code.SourceModelType;
import com.ibm.xmlns.prod.streams.spl.source.code.util.CodeResourceFactoryImpl;
import com.ibm.xmlns.prod.streams.spl.toolkit.DocumentRoot;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitModelType;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitPackage;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import com.ibm.xmlns.prod.streams.spl.toolkit.util.ToolkitResourceFactoryImpl;
import java.io.File;
import org.eclipse.emf.common.util.EList;
import org.eclipse.emf.common.util.URI;
import org.eclipse.emf.ecore.EObject;
import org.eclipse.emf.ecore.resource.Resource;
import org.eclipse.emf.ecore.resource.ResourceSet;
import org.eclipse.emf.ecore.resource.impl.ResourceSetImpl;

/**
 * Loads various model objects.
 *
 * @author afrenk
 */
public class LoadModel {

  private static Logger logger = Logger.getInstance();

  /**
   * Load the source model for the specified toolkit. This model contains the SPL compilation units
   * in the SPL files (*.spl).
   *
   * @return model object for toolkit's source model
   */
  public static SourceModelType loadSourceModel(Toolkit toolkit) {
    File file = FileOutput.getSourceModelFile(toolkit);
    URI uri = URI.createFileURI(file.getAbsolutePath());

    ResourceSet resourceSet = new ResourceSetImpl();
    resourceSet
        .getResourceFactoryRegistry()
        .getExtensionToFactoryMap()
        .put(Resource.Factory.Registry.DEFAULT_EXTENSION, new CodeResourceFactoryImpl());

    // Register the package to ensure it is available during loading.
    resourceSet.getPackageRegistry().put(CodePackage.eNS_URI, CodePackage.eINSTANCE);

    try {
      // Load the source model
      Resource resource;
      try {
        resource = resourceSet.getResource(uri, true);
      } catch (RuntimeException e) {
        // Error loading source model
        String[] args = {uri.toString(), e.getMessage()};
        logger.printError(Key.CDISP7006E, args);
        return null;
      }
      EList<EObject> eobjects = resource.getContents();
      if (eobjects == null || eobjects.isEmpty()) {
        // Error reading source model - seems empty
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // First object should be a DocumentRoot
      EObject eobject = eobjects.get(0);
      if (eobject == null
          || !(eobject instanceof com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot)) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }

      com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot docRoot =
          (com.ibm.xmlns.prod.streams.spl.source.code.DocumentRoot) eobject;

      // Verify that the source model is not empty
      SourceModelType sourceModel = docRoot.getSourceModel();
      if (sourceModel == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      return sourceModel;

    } catch (Exception e) {
      String[] args = {uri.toString()};
      logger.printError(Key.CDISP7013E, args, e);
    }
    return null;
  }

  /**
   * Load toolkit model.
   *
   * @param toolkitPath fully qualified path to toolkit index (toolkit.xml).
   * @return model object for toolkit model
   */
  public static ToolkitType loadToolkitModel(String toolkitPath) {
    File file = new File(toolkitPath);
    URI uri =
        file.isFile() ? URI.createFileURI(file.getAbsolutePath()) : URI.createURI(toolkitPath);

    ResourceSet resourceSet = new ResourceSetImpl();
    resourceSet
        .getResourceFactoryRegistry()
        .getExtensionToFactoryMap()
        .put(Resource.Factory.Registry.DEFAULT_EXTENSION, new ToolkitResourceFactoryImpl());

    // Register the package to ensure it is available during loading.
    resourceSet.getPackageRegistry().put(ToolkitPackage.eNS_URI, ToolkitPackage.eINSTANCE);

    try {
      // Load the toolkit model
      Resource resource;
      try {
        resource = resourceSet.getResource(uri, true);
      } catch (RuntimeException e) {
        // Error loading toolkit model
        String[] args = {uri.toString(), e.getMessage()};
        logger.printError(Key.CDISP7006E, args);
        return null;
      }
      EList<EObject> eobjects = resource.getContents();
      if (eobjects == null || eobjects.isEmpty()) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // First object should be ToolkitModel
      EObject eobject = eobjects.get(0);
      if (eobject == null || !(eobject instanceof DocumentRoot)) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }

      ToolkitModelType toolkitModel = ((DocumentRoot) eobject).getToolkitModel();
      if (toolkitModel == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      EList<ToolkitType> toolkitTypes = toolkitModel.getToolkit();
      if (toolkitTypes == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      return toolkitTypes.get(0);
    } catch (Exception e) {
      String[] args = {uri.toString()};
      logger.printError(Key.CDISP7013E, args, e);
    }
    return null;
  }

  /**
   * Loads function model object.
   *
   * @param functionPath fully qualified path to function's model file
   * @return function model object
   */
  public static FunctionModelType loadFunctionModel(String functionPath) {
    File file = new File(functionPath);
    URI uri =
        file.isFile() ? URI.createFileURI(file.getAbsolutePath()) : URI.createURI(functionPath);

    ResourceSet resourceSet = new ResourceSetImpl();
    resourceSet
        .getResourceFactoryRegistry()
        .getExtensionToFactoryMap()
        .put(Resource.Factory.Registry.DEFAULT_EXTENSION, new CodeResourceFactoryImpl());

    // Register the package to ensure it is available during loading.
    resourceSet.getPackageRegistry().put(FunctionPackage.eNS_URI, FunctionPackage.eINSTANCE);

    try {
      // Load the function model
      Resource resource;
      try {
        resource = resourceSet.getResource(uri, true);
      } catch (RuntimeException e) {
        // Error loading function model
        String[] args = {uri.toString(), e.getMessage()};
        logger.printError(Key.CDISP7006E, args);
        return null;
      }
      EList<EObject> eobjects = resource.getContents();
      if (eobjects.isEmpty()) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // First object should be functionModel
      EObject eobject = eobjects.get(0);
      if (eobject == null
          || !(eobject instanceof com.ibm.xmlns.prod.streams.spl.function.DocumentRoot)) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      com.ibm.xmlns.prod.streams.spl.function.DocumentRoot root =
          (com.ibm.xmlns.prod.streams.spl.function.DocumentRoot) eobject;
      FunctionModelType functionModel = root.getFunctionModel();
      if (functionModel == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // Sanity check - funtionModel must have at least one function set
      if (functionModel.getFunctionSet() == null || functionModel.getFunctionSet().isEmpty()) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      return functionModel;
    } catch (Exception e) {
      String[] args = {uri.toString()};
      logger.printError(Key.CDISP7013E, args, e);
    }
    return null;
  }

  /**
   * Loads operator model object.
   *
   * @param operatorPath fully qualified path to operator's model file
   * @return operator model object
   */
  public static OperatorModelType loadOperatorModel(String operatorPath) {
    File file = new File(operatorPath);
    URI uri =
        file.isFile() ? URI.createFileURI(file.getAbsolutePath()) : URI.createURI(operatorPath);

    ResourceSet resourceSet = new ResourceSetImpl();
    resourceSet
        .getResourceFactoryRegistry()
        .getExtensionToFactoryMap()
        .put(Resource.Factory.Registry.DEFAULT_EXTENSION, new CodeResourceFactoryImpl());

    // Register the package to ensure it is available during loading.
    resourceSet.getPackageRegistry().put(OperatorPackage.eNS_URI, OperatorPackage.eINSTANCE);

    try {
      // Load the operator model
      Resource resource;
      try {
        resource = resourceSet.getResource(uri, true);
      } catch (RuntimeException e) {
        // Error loading operator model
        String[] args = {uri.toString(), e.getMessage()};
        logger.printError(Key.CDISP7006E, args);
        return null;
      }
      EList<EObject> eobjects = resource.getContents();
      if (eobjects.isEmpty()) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // First object should be OperatorModel
      EObject eobject = eobjects.get(0);
      if (!(eobject instanceof com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot)) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot root =
          (com.ibm.xmlns.prod.streams.spl.operator.DocumentRoot) eobject;
      OperatorModelType operatorModel = root.getOperatorModel();
      if (operatorModel == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      // Sanity check content
      if (operatorModel.getCppOperatorModel() == null
          && operatorModel.getJavaOperatorModel() == null) {
        String[] args = {uri.toString()};
        logger.printError(Key.CDISP7008E, args);
        return null;
      }
      return operatorModel;
    } catch (Exception e) {
      String[] args = {uri.toString()};
      logger.printError(Key.CDISP7013E, args, e);
    }
    return null;
  }

  /**
   * Test code.
   *
   * @param args
   */
  public static void main(String[] args) {
    OperatorModelType opModel = LoadModel.loadOperatorModel(args[0]);
    JavaOpModelType javaOpModel;
    if ((javaOpModel = opModel.getJavaOperatorModel()) != null) {
      System.out.println("Java Op Model: " + javaOpModel.getContext().getDescription());
    }
  }
}
