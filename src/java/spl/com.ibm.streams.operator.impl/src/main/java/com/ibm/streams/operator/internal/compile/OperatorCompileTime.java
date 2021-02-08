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

package com.ibm.streams.operator.internal.compile;

import com.ibm.streams.operator.Operator;
import com.ibm.streams.operator.OperatorContext;
import com.ibm.streams.operator.ProcessingElement;
import com.ibm.streams.operator.internal.logtrace.Log;
import com.ibm.streams.operator.internal.logtrace.Setup;
import com.ibm.streams.operator.internal.logtrace.log4j.LogAppender;
import com.ibm.streams.operator.internal.logtrace.log4j.TraceAppender;
import com.ibm.streams.operator.internal.runtime.OperatorAnnotations;
import com.ibm.streams.operator.internal.runtime.api.OperatorAdapter;
import com.ibm.streams.operator.logging.LoggerNames;
import com.ibm.streams.operator.model.PrimitiveOperator;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import com.ibm.streams.spl.model.OperatorInstanceModelType;
import java.io.File;
import java.io.IOException;
import java.net.MalformedURLException;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Unmarshaller;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.transform.stream.StreamSource;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

/**
 * Execute compile time actions for a Java primitive operator or a spl.utility.JavaOp. Java
 * primitives are implemented by the SPL compiler refactoring the Java primitive operator model as
 * an invocation of JavaOp.
 */
public class OperatorCompileTime {

  static {
    System.setProperty(
        TraceAppender.APPENDER_PROPERTY,
        "com.ibm.streams.operator.internal.compile.CompileTraceAppender"); //$NON-NLS-1$
    System.setProperty(
        LogAppender.APPENDER_PROPERTY,
        "com.ibm.streams.operator.internal.compile.CompileLogAppender"); // $NON-NLS-1$
    Setup.setup(new CompileTimeTrace(), new CompileTimeLog());
  }

  private static Logger trace = Logger.getLogger(OperatorCompileTime.class.getName());
  private static Logger logger = Logger.getLogger(LoggerNames.LOG_FACILITY);

  /** Set to true if an error has been logged. */
  static boolean seenError;

  /** Set to true if the SPL compiler is in verbose mode */
  static boolean verboseMode;

  public static void main(String[] args) throws Exception {

    Setup.setLogLevel(Log.warn);

    String cppClass = args[2];
    String xmlPath = cppClass.replace("::", File.separator).replace(":", File.separator);

    File outDir = new File(args[1]);
    File opSource = new File(outDir, "src/operator".replace('/', File.separatorChar));
    File classModel = new File(opSource, xmlPath + ".xml");
    OperatorInstanceModelType model = loadOperatorInstanceModel(classModel);

    final CompileTimeSetup cts = CompileTimeSetup.newCompileTimeSetup(model, args[0]);

    verboseMode = cts.isVerboseMode();
    if (!verboseMode) {
      trace.setLevel(Level.SEVERE);
    }

    int rc = 0;

    boolean generated = Boolean.valueOf(cts.getProperty(OperatorAdapter.OPERATOR_GENERATED));
    if (generated) {
      rc = OperatorSource.doTheWork(cts) ? 0 : 1;
    }

    if (rc == 0) {
      try {
        // If we cannot determine the class name and/or class
        // libraries at compile time then do not perform any
        // compile time checks
        if (cts.isValidClassSetup())
          if (!executeCompileTimeChecks(cts)) {
            rc = 2;
          }
      } catch (Exception e) {
        e.printStackTrace(System.err);
        logger.severe(e.toString());
      }

      // Any message logged at error level
      // results in the compile failing.
      if (OperatorCompileTime.seenError) rc = 2;
    }

    System.exit(rc);
  }

  /**
   * Execute compile time checks.
   *
   * @return True if the context is value, false otherwise.
   */
  public static boolean executeCompileTimeChecks(final CompileTimeSetup cts) throws Exception {

    // Compile time version of a ProcessingElement
    // that will be exposed to user code through
    // annotated compile time check methods.
    final ProcessingElement pe = new CompileTimePE(cts);

    final Class<? extends Operator> operatorClass = verifyClassNameAndLibraries(cts, pe);

    boolean ok = true;
    // Run additional checks if:
    // - we are running as a Java primitive
    // - and the operator model is not defined by @PrimitiveOperator
    //
    // In the case the operator model is defined by @PrimitiveOperator
    // then by definition the model is consistent.

    final OperatorContext context = new CompileTimeContext(cts);

    Document model = null;
    if (!cts.isJavaOp() && operatorClass.getAnnotation(PrimitiveOperator.class) == null)
      model = loadJavaPrimitiveModel(cts, operatorClass, context.getKind());

    final OperatorAnnotations annotations = new OperatorAnnotations(context, operatorClass);

    if (model != null) {
      annotations.verifyParameters(model);
      annotations.verifyCustomMetric(model);
    }

    if (!annotations.applyCompileTimeAnnotations()) ok = false;

    return ok;
  }

  /** First step is to load the class from its defined class path. */
  private static Class<? extends Operator> verifyClassNameAndLibraries(
      CompileTimeSetup cts, ProcessingElement pe)
      throws MalformedURLException, ClassNotFoundException {
    boolean internalOp = cts.getOperatorClassName().startsWith("com.ibm.streams.control.");

    final ClassLoader loader =
        OperatorAdapter.newOperatorClassLoader(cts.getClassLibraries(), pe, internalOp);
    return OperatorAdapter.loadOperatorClass(loader, cts.getOperatorClassName());
  }

  /**
   * Verify the Java primitive operator model XML file.
   *
   * @throws ParserConfigurationException
   * @throws IOException
   * @throws SAXException
   */
  private static Document loadJavaPrimitiveModel(
      final CompileTimeSetup cts, Class<? extends Operator> operatorClass, final String kind)
      throws ParserConfigurationException, SAXException, IOException {

    final String modelDir = cts.getModel().getContext().getJavaOperatorModelDirectory();

    return loadJavaPrimitiveModel(kind, modelDir);
  }

  public static Document loadJavaPrimitiveModel(String kind, String modelDir)
      throws ParserConfigurationException, SAXException, IOException {

    String primitiveName;
    if (kind.contains("::")) // $NON-NLS-1$
    primitiveName = kind.substring(kind.indexOf("::") + 2); // $NON-NLS-1$
    else primitiveName = kind;

    final File modelFile = new File(modelDir, primitiveName + ".xml"); // $NON-NLS-1$

    if (!modelFile.exists() || !modelFile.canRead())
      throw new IllegalStateException(
          new Message(Key.SPL_JAVA_OCT_MODEL_FILE, kind, modelFile.getAbsolutePath())
              .getLocalizedMessageAndPrefix());

    DocumentBuilderFactory docFactory = DocumentBuilderFactory.newInstance();
    docFactory.setValidating(false);
    DocumentBuilder docBuilder = docFactory.newDocumentBuilder();
    return docBuilder.parse(modelFile);
  }

  private static OperatorInstanceModelType loadOperatorInstanceModel(File xml)
      throws JAXBException {
    JAXBContext jc = JAXBContext.newInstance("com.ibm.streams.spl.model");

    Unmarshaller u = jc.createUnmarshaller();
    JAXBElement<OperatorInstanceModelType> jbt =
        u.unmarshal(new StreamSource(xml), OperatorInstanceModelType.class);
    return jbt.getValue();
  }

  public static int verify(String[] args) throws Exception {

    Setup.setLogLevel(Log.warn);

    String cppClass = args[2];
    String xmlPath = cppClass.replace("::", File.separator).replace(":", File.separator);

    File outDir = new File(args[1]);
    File opSource = new File(outDir, "src/operator".replace('/', File.separatorChar));
    File classModel = new File(opSource, xmlPath + ".xml");
    OperatorInstanceModelType model = loadOperatorInstanceModel(classModel);

    final CompileTimeSetup cts = CompileTimeSetup.newCompileTimeSetup(model, args[0]);

    verboseMode = cts.isVerboseMode();
    if (!verboseMode) {
      trace.setLevel(Level.SEVERE);
    }

    int rc = 0;

    boolean generated = Boolean.valueOf(cts.getProperty(OperatorAdapter.OPERATOR_GENERATED));
    if (generated) {
      rc = OperatorSource.doTheWork(cts) ? 0 : 1;
    }

    if (rc == 0) {
      try {
        // If we cannot determine the class name and/or class
        // libraries at compile time then do not perform any
        // compile time checks
        if (cts.isValidClassSetup())
          if (!executeCompileTimeChecks(cts)) {
            rc = 2;
          }
      } catch (Exception e) {
        e.printStackTrace(System.err);
        logger.severe(e.toString());
      }

      // Any message logged at error level
      // results in the compile failing.
      if (OperatorCompileTime.seenError) rc = 2;
    }

    return rc;
  }
}
