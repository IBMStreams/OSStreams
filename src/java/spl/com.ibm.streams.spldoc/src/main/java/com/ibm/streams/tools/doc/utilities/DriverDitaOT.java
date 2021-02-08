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
import java.io.BufferedReader;
import java.io.File;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.util.HashMap;
import java.util.Map;
import java.util.Vector;
import java.util.regex.Pattern;
import org.eclipse.core.runtime.Path;

public class DriverDitaOT {

  // Regex patterns for DITA error messages.
  // Error messages contain: ...[<message id>][FATAL|ERROR] ... For example:
  // ... [DOTA002F][FATAL] Invalid input. This often means the input file is not specified
  // correctly. Please provide the
  // correct input.
  static final String DITA_ERROR_PATTERN = ".*\\[{1}.*\\]{1}\\[{1}((FATAL)|(ERROR))\\]{1}.*";
  static final Pattern ditaErrorPattern = Pattern.compile(DITA_ERROR_PATTERN);

  // Regex patterns for DITA topicpull error messages. For example:
  // [topicpull] ... : Error! java.io.FileNotFoundException: ...
  static final String TOPICPULL_ERROR_PATTERN = "\\[{1}topicpull\\]{1}.*: Error! .*";
  static final Pattern topicpullErrorPattern = Pattern.compile(TOPICPULL_ERROR_PATTERN);

  // Error messages of the form: [fop] SEVERE are output by the Apache Format Object Processor,
  // which is invoked by
  // DITA-OT.
  // An example message:s
  // [fop] SEVERE: Image not found. URI:
  // file:/homes/hny2/afrenk/development/spldoc_concord_2/HelloWorld/doc/spldoc/..
  // [fop] [ERROR] Anttask - Error rendering fo file:
  // /homes/hny9/nasgaard/test/spldoc/doc/spldoc/dita/temp/topic.fo
  static final String FOP_ERROR_PATTERN = ".*\\[fop\\]\\s(SEVERE:|\\[ERROR\\])\\s.*";
  static final Pattern fopErrorPattern = Pattern.compile(FOP_ERROR_PATTERN);

  // Regex patterns for ant build error messages.
  static final String ANT_ERROR_PATTERN =
      "^(BUILD FAILED|Build failed)$|.*The following error occurred.*";
  static final Pattern antErrorPattern = Pattern.compile(ANT_ERROR_PATTERN);

  // Regex patterns for Streams error messages (not used).
  // com.ibm.streams.examples/HelloWorld.spl: CDISP7025E ERROR: Missing beginning ..."
  // static final String STREAMS_ERROR_PATTERN = ".*(CDISP)\\d\\d\\d\\d(E)s.*\\s(ERROR:).*";

  static Logger logger = Logger.getInstance();
  static String DITA_HOME = Constants.getDitaHomePath();
  static final String DitaCommandRoot = "java -jar " + DITA_HOME + "/lib/dost.jar";
  static final String DitaTempDir = Path.SEPARATOR + "dita" + Path.SEPARATOR + "temp";
  static String topLevelDocument = null;

  // Directory for SPLDOC control files
  static String IBMSpldocPath = null;

  // IBM stylesheet for InfoCenter documents
  // static final String IBM_CSS_NAME = "swg_info_common.css";
  // TODO: Remove if not used
  static final String SPLDOC_CSS_NAME = "spldoc.css";

  // Map of output format to directory name
  static Map<String, String> transDirMap = new HashMap<String, String>();

  static {
    transDirMap.put("xhtml", "html");
    transDirMap.put("pdf", "pdf");
  }

  // Map of output format to file extension name
  static Map<String, String> transExtMap = new HashMap<String, String>();

  static {
    transExtMap.put("xhtml", "html");
    transExtMap.put("pdf", "pdf");
  }

  /**
   * Returns the fully qualified name of the top level generated SPLDOC document.
   *
   * @return
   */
  public static String getTopLevelDocument() {
    return topLevelDocument;
  }

  static boolean error = false;

  static void setError() {
    error = true;
  }

  static boolean isError() {
    return error;
  }

  /**
   * Executes the DITA-OT to translate DITA documents to output format (e.g. HTML, PDF, ...).
   *
   * @return 0 if no errors encountered, and non-zero otherwise.
   */
  public static int xlate() {
    String transType = InputParameters.getInputParameters().getTransType();
    int rc;
    if (InputParameters.DitaOnlyTransType.equals(transType)) {
      topLevelDocument = getDitaMapFileName();
      return 0;
    }
    String buildDir = InputParameters.getInputParameters().getBuildDir();
    IBMSpldocPath =
        buildDir == null
            ? System.getenv("STREAMS_INSTALL") + File.separator + "etc" + File.separator + "spldoc"
            : buildDir
                + File.separator
                + ".."
                + File.separator
                + "java"
                + File.separator
                + "SPLDOC"
                + File.separator
                + "com.ibm.streams.tools.doc.html";
    if ("xhtml".equals(transType)) {
      if ((rc = copyHtmlIndexUsingAnt()) != 0) return rc;
    }
    if ((rc = xlateUsingAnt()) != 0) return rc;
    topLevelDocument = getTopIndexFileName();
    return 0;
  }

  /** Returns the fully qualified name of the ditamap file. */
  public static String getDitaMapFileName() {
    String outputDir = InputParameters.getInputParameters().getOutputDir();
    return outputDir
        + Path.SEPARATOR
        + "dita"
        + Path.SEPARATOR
        + FileOutput.AllToolkitsFileName
        + "."
        + FileOutput.DitaMapFileExt;
  }

  /** Returns the fully qualified name of the top level document index file. */
  public static String getTopIndexFileName() {
    InputParameters inputParams = InputParameters.getInputParameters();
    String outputDir = inputParams.getOutputDir();
    String transType = inputParams.getTransType();
    return outputDir
        + Path.SEPARATOR
        + transDirMap.get(transType)
        + Path.SEPARATOR
        + FileOutput.TopIndexFileName
        + "."
        + transExtMap.get(transType);
  }

  /**
   * Execute ant to copy the index.html for the main toolkit page to the HTML output directory.
   *
   * @return 0 if no errors encountered, and non-zero otherwise.
   */
  private static int copyHtmlIndexUsingAnt() {
    InputParameters inputParams = InputParameters.getInputParameters();
    String docdir = inputParams.getOutputDir();
    String transtype = inputParams.getTransType();
    String outdir = docdir + Path.SEPARATOR + transDirMap.get(transtype);
    String builddir = IBMSpldocPath + File.separator + "build";

    File outdirFile = new File(outdir);
    if (!outdirFile.exists()) {
      outdirFile.mkdirs();
    }

    Vector<String> commandList = new Vector<String>();
    commandList.add(Constants.isWindowsOS ? "ant.bat" : "ant");
    commandList.add("-Doutput.dir=" + outdir);
    commandList.add("-f");
    commandList.add("build_copyhtmlindex.xml");

    try {
      ProcessBuilder pb = new ProcessBuilder(commandList);
      Map<String, String> env = pb.environment();
      env.put("DITA_HOME", DITA_HOME);
      pb.directory(new File(builddir));
      logger.printTrace(Key.CDISP7096I, new String[] {commandList.toString()});
      return execute(pb);
    } catch (Exception e) {
      logger.printException(e);
      return 1;
    }
  }

  /**
   * Executes the DITA-OT via ANT to translate DITA documents to output format (e.g. HTML, PDF,
   * ...).
   *
   * @return 0 if no errors encountered, and non-zero otherwise.
   */
  private static int xlateUsingAnt() {
    InputParameters inputParams = InputParameters.getInputParameters();
    String docdir = inputParams.getOutputDir();
    String transtype = inputParams.getTransType();
    String outdir = docdir + Path.SEPARATOR + transDirMap.get(transtype);
    String cssdir = IBMSpldocPath + File.separator + "css";

    // TODO: fix - dita subdirectory seems to be required to produce *.pdf - which
    // isn't obvious.  Try to fix.
    File outdirFile = new File(outdir + Path.SEPARATOR + "dita");
    if (!outdirFile.exists()) {
      outdirFile.mkdirs();
    }
    String DitaMapFile = getDitaMapFileName();

    logger.printTrace(Key.CDISP7067I, new String[] {docdir, transtype});
    logger.printTrace(Key.CDISP7068I, new String[] {DitaMapFile});

    Vector<String> commandList = new Vector<String>();
    commandList.add(Constants.isWindowsOS ? "ant.bat" : "ant");
    commandList.add("-Dargs.input=" + DitaMapFile);
    commandList.add("-Doutput.dir=" + outdir);
    commandList.add("-Ddita.temp.dir=" + docdir + DitaTempDir);
    commandList.add("-Dtranstype=" + transtype);
    commandList.add("-Ddita.dir=" + DITA_HOME);

    // An "Unable to find file" message is output to the DITA-OT trace if this
    // CSS can't be located. This IBM CSS should be in the Streams install image.
    commandList.add("-Dargs.css=" + SPLDOC_CSS_NAME);
    commandList.add("-Dargs.cssroot=" + cssdir);
    commandList.add("-Dargs.copycss=yes");
    commandList.add("-Dargs.csspath=../html");

    commandList.add("-Dargs.xhtml.toc=toc");

    if (inputParams.isVerbose()) {
      commandList.add("-v");
    }

    if (inputParams.isDebug()) {
      commandList.add("-Dargs.debug=yes");
      commandList.add("-Dclean.temp=no");
      commandList.add("-Dargs.logdir=" + docdir + DitaTempDir);
    }
    try {
      ProcessBuilder pb = new ProcessBuilder(commandList);
      Map<String, String> env = pb.environment();
      env.put("DITA_HOME", DITA_HOME);
      pb.directory(new File(DITA_HOME));
      logger.printTrace(Key.CDISP7096I, new String[] {commandList.toString()});
      return execute(pb);
    } catch (Exception e) {
      logger.printException(e);
      return 1;
    }
  }

  /**
   * Executes the command already set up in the ProcessBuilder. Errors are identified for ant and
   * DITA-OT error output.
   *
   * @return 0 if no errors encountered, and non-zero otherwise.
   */
  private static int execute(ProcessBuilder pb) {
    final StringBuilder infoMsg = new StringBuilder();
    try {
      pb.redirectErrorStream(true);
      final Process p = pb.start();

      (new Thread() {
            @Override
            public void run() {
              boolean verbose = InputParameters.getInputParameters().isVerbose();
              try {
                InputStream iStream = p.getInputStream();
                InputStreamReader isReader = new InputStreamReader(iStream, "US-ASCII");
                BufferedReader bReader = new BufferedReader(isReader);
                String oLine;
                String errorMessage = null;
                // DITA outputs messages in pairs - an error message followed by the error
                // explanation.
                while ((oLine = bReader.readLine()) != null) {
                  if (ditaErrorPattern.matcher(oLine).matches()
                      || fopErrorPattern.matcher(oLine).matches()
                      || antErrorPattern.matcher(oLine).matches()) {
                    // Save error message for output with next line.
                    if (errorMessage != null) errorMessage += "\n" + "  " + oLine;
                    else errorMessage = oLine;
                    setError();
                  } else if (errorMessage != null) {
                    // Add explanation to error message and output both.
                    errorMessage += "\n" + "  " + oLine;
                    logger.printError(Key.CDISP7102E, new String[] {errorMessage});
                    errorMessage = null;
                  } else if (topicpullErrorPattern.matcher(oLine).matches()) {
                    // Output single line error message.
                    logger.printError(Key.CDISP7102E, new String[] {oLine});
                    setError();
                  } else {
                    // Output non error message.
                    logger.printTrace(Key.CDISP7069I, new String[] {oLine});
                    if (!verbose) {
                      // This information may be useful if there is an unexpected failure.
                      if (infoMsg.length() > 0) {
                        infoMsg.append("\n" + "  " + oLine);
                      } else {
                        infoMsg.append(oLine);
                      }
                    }
                  }
                }
                if (errorMessage != null) {
                  // Output error message not followed by another line.
                  logger.printError(Key.CDISP7102E, new String[] {errorMessage});
                }
              } catch (Exception e) {
                logger.printException(e);
                setError();
              }
            }
          })
          .start();

      synchronized (p) {
        p.waitFor();
      }

      if (p.exitValue() != 0) {
        logger.printError(
            Key.CDISP7126E,
            new String[] {
              String.valueOf(p.exitValue())
                  + (isError() || infoMsg.length() <= 0 ? "" : " : " + infoMsg.toString())
            });
        return 1;
      } else {
        logger.printTrace(Key.CDISP7070I, new String[] {String.valueOf(p.exitValue())});
        return isError() ? 1 : 0;
      }
    } catch (Exception e) {
      logger.printException(e);
      return 1;
    }
  }
}
