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

package com.ibm.streams.tools.doc.utilities;

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import java.io.File;
import java.util.LinkedList;
import java.util.List;
import java.util.Scanner;

/**
 * Validates and caches switches provided by spl-make-doc CLI. Supplies some default values.
 * Provides accessor methods for parameters.
 *
 * @author afrenk
 */
public class InputParameters {

  // The input parameters are a singleton. There's one set of parameters for each execution
  // of this command.
  static InputParameters inputParams = null;

  // Document output format types.
  public static final String DefaultTransType = "xhtml";
  public static final String DitaOnlyTransType = "ditaonly";
  public static final String PDFTransType = "pdf"; // Not supported.

  // Image file format types.
  public static final String DefaultImageType = "svg";
  public static final String AddJpegImageType = "addjpeg";
  public static final String JpegOnlyImageType = "jpegonly";

  // Look for "-<switch> .." or " --<switch> .."
  static final String SwitchListPattern = "(^\\-)";

  static final Logger logger = Logger.getInstance();

  LinkedList<String> toolkitPaths = new LinkedList<String>();
  String author = null;
  String buildDir = null;
  String imageType = DefaultImageType;
  String outputDir = null;
  String transType = DefaultTransType;
  boolean i_switch = false;
  boolean t_switch = false;
  boolean checkTags = false;
  boolean copyImageFiles = false;
  boolean debug = false;
  boolean includeAll = false;
  boolean includeCompositeOperatorDiagram = false;
  boolean includeExcludedArtifacts = false;
  boolean includeFunctionSetHeader = false;
  boolean includePrivateArtifacts = false;
  boolean includeSource = false;
  boolean suppressPrimitiveOperatorDiagram = false;
  boolean suppressReferenceDocs = false;
  boolean verbose = false;
  boolean warnNoComments = false;
  boolean warnOnCrossLinkError = false;
  String docTitle = "";

  public static InputParameters getInputParameters() {
    assert (inputParams != null);
    return inputParams;
  }

  public List<String> getToolkitPaths() {
    return toolkitPaths;
  }

  public String getAuthor() {
    return author;
  }

  public String getBuildDir() {
    return buildDir;
  }

  public String getDocTitle() {
    return docTitle;
  }

  public String getImageType() {
    return imageType;
  }

  public String getOutputDir() {
    return outputDir;
  }

  public String getTransType() {
    return transType;
  }

  public boolean isDebug() {
    return debug;
  }

  public boolean isCheckTags() {
    return checkTags;
  }

  public boolean isCopyImageFiles() {
    return copyImageFiles;
  }

  public boolean isIncludeCompositeOperatorDiagram() {
    return includeCompositeOperatorDiagram;
  }

  public boolean isIncludeExcludedArtifacts() {
    return includeExcludedArtifacts;
  }

  public boolean isIncludeFunctionSetHeader() {
    return includeFunctionSetHeader;
  }

  public boolean isIncludePrivateArtifacts() {
    return includePrivateArtifacts;
  }

  public boolean isIncludeSource() {
    return includeSource;
  }

  public boolean isSuppressPrimitiveOperatorDiagram() {
    return suppressPrimitiveOperatorDiagram;
  }

  public boolean isSuppressReferenceDocs() {
    return suppressReferenceDocs;
  }

  public boolean isVerbose() {
    return verbose;
  }

  public boolean isWarnNoComments() {
    return warnNoComments;
  }

  public boolean isWarnOnCrossLinkError() {
    return warnOnCrossLinkError;
  }

  public void print() {
    String paths = toolkitPaths.toString();
    assert (paths.startsWith("[") && paths.endsWith("]"));
    logger.printTrace(Key.CDISP7060I, new String[] {paths.substring(1, paths.length() - 1)});
    logger.printTrace(Key.CDISP7061I, new String[] {outputDir});
    logger.printTrace(Key.CDISP7062I, new String[] {transType});
    logger.printTrace(Key.CDISP7071I, new String[] {imageType});
    logger.printTrace(Key.CDISP7063I, new String[] {author});
    logger.printTrace(Key.CDISP7125I, new String[] {docTitle});
    logger.printTrace(Key.CDISP7077I, new String[] {String.valueOf(checkTags)});
    logger.printTrace(Key.CDISP7079I, new String[] {String.valueOf(copyImageFiles)});
    logger.printTrace(Key.CDISP7095I, new String[] {String.valueOf(debug)});
    logger.printTrace(
        Key.CDISP7122I, new String[] {String.valueOf(includeCompositeOperatorDiagram)});
    logger.printTrace(Key.CDISP7089I, new String[] {String.valueOf(includeExcludedArtifacts)});
    logger.printTrace(Key.CDISP7090I, new String[] {String.valueOf(includeFunctionSetHeader)});
    logger.printTrace(Key.CDISP7091I, new String[] {String.valueOf(includePrivateArtifacts)});
    logger.printTrace(Key.CDISP7092I, new String[] {String.valueOf(includeSource)});
    logger.printTrace(
        Key.CDISP7123I, new String[] {String.valueOf(suppressPrimitiveOperatorDiagram)});
    logger.printTrace(Key.CDISP7118I, new String[] {String.valueOf(suppressReferenceDocs)});
    logger.printTrace(Key.CDISP7064I, new String[] {String.valueOf(verbose)});
    logger.printTrace(Key.CDISP7078I, new String[] {String.valueOf(warnNoComments)});
    logger.printTrace(Key.CDISP7066I, new String[] {String.valueOf(warnOnCrossLinkError)});
  }

  // Assembles space separated values of a switch
  // into a single string.
  static String getValue(Scanner scanner) {
    String switchValue = "";
    while (scanner.hasNext()) {
      switchValue += scanner.next() + " ";
    }
    return switchValue.trim();
  }

  // Scan the directory name, removing enclosing quotes if need be.
  // Allow for spaces in an unquoted string.
  static String getDir(Scanner scanner) {
    String path = "";
    if (scanner.hasNext()) path = scanner.next();
    while (scanner.hasNext()) path += " " + scanner.next();
    if (path.length() > 1 && path.charAt(0) == '\'' && path.charAt(path.length() - 1) == '\'')
      path = path.substring(1, path.length() - 1);
    return path;
  }

  // Scan multiple directory names with separators.
  static LinkedList<String> getDirs(LinkedList<String> paths, Scanner scanner) {
    scanner.useDelimiter(Constants.isWindowsOS ? ";" : ":");
    while (scanner.hasNext()) {
      String path = scanner.next();
      Scanner pathScanner = new Scanner(path);
      paths.add(getDir(pathScanner));
      pathScanner.close();
    }
    return paths;
  }

  // Process the input parameters.  It's assumed that the
  // spl-make-doc CLI verified the syntax.
  public static InputParameters processInputParameters(String[] argv) {
    inputParams = new InputParameters();
    for (String arg : argv) {
      arg = arg.trim();
      Scanner switchListScanner = new Scanner(arg);
      // Create scanner where each -<switch> <value> is a separate token
      switchListScanner.useDelimiter(SwitchListPattern);
      while (switchListScanner.hasNext()) {
        // Create scanner for parsing a switch
        // Assume that switch and value is separated by space
        Scanner switchScanner = new Scanner(switchListScanner.next());
        String switchName = switchScanner.next();
        if ("-author".equals(switchName)) {
          inputParams.author = getValue(switchScanner);
        } else if ("b".equals(switchName)) {
          inputParams.buildDir = getDir(switchScanner);
        } else if ("-debug".equals(switchName)) {
          inputParams.debug = true;
        } else if ("-doc-title".equals(switchName)) {
          inputParams.docTitle = getValue(switchScanner);
        } else if ("-check-tags".equals(switchName)) {
          inputParams.checkTags = true;
        } else if ("-copy-image-files".equals(switchName)) {
          inputParams.copyImageFiles = true;
        } else if ("-directory".equals(switchName) || "i".equals(switchName)) {
          inputParams.i_switch = true;
          inputParams.toolkitPaths.clear();
          inputParams.toolkitPaths.add(getDir(switchScanner));
        } else if ("-image-format".equals(switchName)) {
          inputParams.imageType = getValue(switchScanner);
        } else if ("-include-all".equals(switchName)) {
          inputParams.includeAll = true;
        } else if ("-include-composite-operator-diagram".equals(switchName)) {
          inputParams.includeCompositeOperatorDiagram = true;
        } else if ("-include-excluded-artifacts".equals(switchName)) {
          inputParams.includeExcludedArtifacts = true;
        } else if ("-include-function-set-header".equals(switchName)) {
          inputParams.includeFunctionSetHeader = true;
        } else if ("-include-private-artifacts".equals(switchName)) {
          inputParams.includePrivateArtifacts = true;
        } else if ("-include-source".equals(switchName)) {
          inputParams.includeSource = true;
        } else if ("-output-format".equals(switchName) || "x".equals(switchName)) {
          inputParams.transType = getValue(switchScanner);
        } else if ("-output-directory".equals(switchName)) {
          inputParams.outputDir = getDir(switchScanner);
        } else if ("-suppress-primitive-operator-diagram".equals(switchName)) {
          inputParams.suppressPrimitiveOperatorDiagram = true;
        } else if ("-suppress-reference-docs".equals(switchName)) {
          inputParams.suppressReferenceDocs = true;
        } else if ("-toolkit-path".equals(switchName) || "t".equals(switchName)) {
          inputParams.t_switch = true;
          inputParams.toolkitPaths = getDirs(inputParams.toolkitPaths, switchScanner);
        } else if ("v".equals(switchName)) {
          inputParams.verbose = true;
        } else if ("-warn-no-comments".equals(switchName)) {
          inputParams.warnNoComments = true;
        } else if ("-warn-on-cross-link-error".equals(switchName)) {
          inputParams.warnOnCrossLinkError = true;
        } else {
          logger.printError(Key.CDISP7003E, new String[] {arg});
          switchScanner.close();
          switchListScanner.close();
          return null;
        }
        switchScanner.close();
      }
      switchListScanner.close();
    } // for

    // Do not allow both -i and -t switches.
    if (inputParams.i_switch && inputParams.t_switch) {
      logger.printError(Key.CDISP7004E);
      return null;
    }

    // If neither -i nor -t switch is specified,
    // use current directory as default toolkit directory.
    if (!(inputParams.i_switch || inputParams.t_switch)) {
      inputParams.toolkitPaths.add(System.getProperty("user.dir"));
    }

    // Check that a toolkit directory has been specified.
    if (inputParams.toolkitPaths.size() <= 0) {
      logger.printError(Key.CDISP7000E);
      return null;
    }

    // Check that the specified toolkit directories exist.
    for (String path : inputParams.toolkitPaths) {
      if (!new File(path).isDirectory()) {
        logger.printError(Key.CDISP7005E, new String[] {path});
        return null;
      }
    }

    // If no output directory specified,
    // output doc to to current directory if -t switch, or
    // to toolkit's doc directory.
    if (inputParams.outputDir == null) {
      inputParams.outputDir =
          new File(
                  inputParams.t_switch
                      ? System.getProperty("user.dir")
                      : inputParams.toolkitPaths.getFirst(),
                  FileOutput.DefaultSplDocDirectory)
              .toString();
    }

    // Include all is a shorthand for including all optional "include" items.
    if (inputParams.includeAll) {
      inputParams.includeCompositeOperatorDiagram = true;
      inputParams.includeExcludedArtifacts = true;
      inputParams.includeFunctionSetHeader = true;
      inputParams.includePrivateArtifacts = true;
      inputParams.includeSource = true;
    }

    if (inputParams.debug) {
      inputParams.verbose = true;
    }
    return inputParams;
  }
}
