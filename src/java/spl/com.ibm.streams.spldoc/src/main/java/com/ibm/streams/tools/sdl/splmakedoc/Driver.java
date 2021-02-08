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

package com.ibm.streams.tools.sdl.splmakedoc;

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.generator.dita.GeneratorDita;
import com.ibm.streams.tools.doc.utilities.DriverDitaOT;
import com.ibm.streams.tools.doc.utilities.ErrorManager;
import com.ibm.streams.tools.doc.utilities.FileOutput;
import com.ibm.streams.tools.doc.utilities.InputParameters;
import com.ibm.streams.tools.doc.utilities.Logger;
import com.ibm.streams.tools.doc.utilities.ToolkitManager;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import com.ibm.streams.tools.sdl.reader.LoadModel;
import com.ibm.xmlns.prod.streams.spl.toolkit.ToolkitType;
import java.io.File;
import org.eclipse.core.runtime.Path;

/**
 * Main driver for spl-make-doc.
 *
 * <p>This driver processes the input parameters, loads the toolkit models, and generates the DITA
 * documentation and associated images for the specified toolkits.
 *
 * @author afrenk
 */
public class Driver {

  static Logger logger = Logger.getInstance();
  static ToolkitManager toolkitMgr = null;

  /**
   * The main method is invoked with one or more parameters, using the following syntax:
   *
   * <p>spl-make-doc "-<switch-1> value" "-<switch-2> value" etc ...
   *
   * <p>Where the -i or -t switch which specifies the path to the toolkit(s) must be specified.
   *
   * <p>The input parameters are assumed to be provided by the spl-make-doc CLI and to be
   * syntactically correct.
   *
   * @param argv
   */
  public static void main(String[] argv) {
    // Process input parameters.
    InputParameters inputParameters = InputParameters.processInputParameters(argv);
    if (inputParameters == null) {
      logger.printInfo(Key.CDISP7007I);
      System.exit(1);
    }
    logger.setVerbose(inputParameters.isVerbose());
    inputParameters.print();
    logger.printTrace(Key.CDISP7083I);
    try {
      // Load the specified toolkits.
      toolkitMgr = ToolkitManager.newInstance();
      for (String toolkitPath : inputParameters.getToolkitPaths()) {
        if (addToolkit(toolkitPath) == null) exitDriver();
      }

      // Generate documentation.
      GeneratorDita.newInstance().generate();
      DriverDitaOT.xlate();
    } catch (ErrorManager.FatalErrorException e) {
      // Handle known fatal error.
      if (inputParameters.isVerbose() || !ErrorManager.hasErrors()) logger.printException(e);
      exitDriver();
    } catch (Exception e) {
      // Handle unexpected error.
      logger.printException(e);
      exitDriver();
    }

    // Exit and report any errors.
    exitDriver();
  }

  /**
   * Exit the driver and report any errors.
   *
   * <p>If errors were encountered generating documentation, report the number of errors and exit
   * with a non-zero return code. If errors were encountered, and the top-level document exists,
   * output the path to that document. Otherwise, output a message that reports only the number of
   * errors encountered. If warnings were encountered, output a warning message and a count of the
   * number of warnings. If no errors were encountered, return a zero return code.
   */
  private static void exitDriver() {
    String topLevelDocument = DriverDitaOT.getTopLevelDocument();
    if (ErrorManager.hasErrors()) {
      // A top level document may not be available if a severe error was encountered.
      if (topLevelDocument != null) {
        File file = new File(topLevelDocument);
        if (file.exists()) {
          String[] args = {
            ErrorManager.getNumberErrors() + "",
            ErrorManager.getNumberWarnings() + "",
            topLevelDocument
          };
          if (ErrorManager.hasErrors()) {
            logger.printError(Key.CDISP7097E, args);
          }
        } else {
          String[] args = {
            ErrorManager.getNumberErrors() + "", ErrorManager.getNumberWarnings() + ""
          };
          logger.printError(Key.CDISP7093E, args);
        }
      } else {
        String[] args = {
          ErrorManager.getNumberErrors() + "", ErrorManager.getNumberWarnings() + ""
        };
        logger.printError(Key.CDISP7093E, args);
      }
      System.exit(1);
    } else if (ErrorManager.hasWarnings()) {
      if (topLevelDocument == null) {
        topLevelDocument = logger.getMessage("MSG_NOT_DEFINED");
      }
      String[] args = {ErrorManager.getNumberWarnings() + "", topLevelDocument};
      logger.printWarn(Key.CDISP7101W, null, args);
      System.exit(0);
    } else {
      // No errors or warnings encountered.
      // Output a message identifying the top level document and exit with a 0 return code.
      logger.printInfo(Key.CDISP7076I, new String[] {topLevelDocument});
      System.exit(0);
    }
  }

  /**
   * Add a toolkit for processing.
   *
   * <p>By loading the toolkit model here rather than in the toolkit manager we avoid a circular
   * dependency between the utilities and reader libraries. The toolkit model identifies all the
   * artifacts in a toolkit.
   */
  private static Toolkit addToolkit(String toolkitPath) {
    // Load the toolkit model.
    ToolkitType toolkitModel =
        LoadModel.loadToolkitModel(toolkitPath + Path.SEPARATOR + FileOutput.ToolkitModelFileName);
    if (toolkitModel == null) return null;

    // add the toolkit to the collection of toolkits to be processed
    return toolkitMgr.addToolkit(toolkitPath, toolkitModel);
  }
}
