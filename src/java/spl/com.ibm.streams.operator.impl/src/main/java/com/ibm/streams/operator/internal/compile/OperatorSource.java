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

import com.ibm.streams.operator.AbstractOperator;
import com.ibm.streams.operator.Attribute;
import com.ibm.streams.operator.StreamSchema;
import com.ibm.streams.operator.Type;
import com.ibm.streams.operator.Type.MetaType;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileWriter;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashSet;
import java.util.List;
import java.util.Set;
import javax.tools.JavaCompiler;
import javax.tools.JavaFileObject;
import javax.tools.StandardJavaFileManager;
import javax.tools.StandardLocation;
import javax.tools.ToolProvider;
import org.apache.commons.math.complex.Complex;

/**
 * Compile time action that generates source code for operator invocation for the spl.utility.JavaOp
 * when the parameter "generated" is true.
 *
 * <p>The source code is created in:
 *
 * <p>application_directory/opt/java/src
 *
 * <p>The compiled class files are created in
 *
 * <p>output_directory/java/bin
 *
 * <p>In each a full directory structure is created matching the package name of the operator class
 * file, e.g. com.example.ops.MyOp results in java/bin/com/example/ops
 */
public class OperatorSource {

  /** Location for compiled generated class files relative to SPL output directory. */
  public static final String BIN_DIR = "java/bin".replace('/', File.separatorChar);

  private static final class Env {
    final String concreteClassName;
    final String abstractClassName;
    /**
     * Package name is null when the className provided parameter is a simple class name (e.g.
     * MyClassName).
     */
    final String packageName;

    final StreamSchema[] inputSchemas;
    final StreamSchema[] outputSchemas;

    Env(CompileTimeSetup cts) {
      final String className = cts.getOperatorClassName();
      String acn = className;
      String ccn = className;
      String pn;
      int lastDot = acn.lastIndexOf('.');
      if (lastDot == -1) {
        acn = "Abstract" + className;
        pn = null;
      } else {
        pn = acn.substring(0, lastDot);
        ccn = acn.substring(lastDot + 1);
        acn = "Abstract" + ccn;
      }
      this.concreteClassName = ccn;
      this.abstractClassName = acn;
      this.packageName = pn;
      this.inputSchemas = cts.getInputPorts().getStreamSchemas();
      this.outputSchemas = cts.getOutputPorts().getStreamSchemas();
    }
  }

  /**
   * Generate Java source code templates for the current operator invocation. The invocation context
   * is entirely contained in CompileTimeSetup.
   */
  public static boolean doTheWork(final CompileTimeSetup cts) throws Exception {

    Env env = new Env(cts);
    StringBuilder ao = new StringBuilder();
    abstractOperator(ao, env);

    File appDir = new File(cts.getModel().getContext().getApplicationDirectory());
    if (!appDir.exists()) throw new FileNotFoundException(appDir.toString());

    File sourceRoot =
        new File(appDir, "opt" + File.separatorChar + "java" + File.separatorChar + "src");

    File src = sourceRoot;

    if (env.packageName != null)
      src = new File(sourceRoot, env.packageName.replace('.', File.separatorChar));

    if (!src.exists()) src.mkdirs();

    File aof = new File(src, env.abstractClassName + ".java");
    FileWriter afw = new FileWriter(aof);
    afw.write(ao.toString());
    afw.flush();
    afw.close();

    File cof = new File(src, env.concreteClassName + ".java");
    if (!cof.exists()) {
      StringBuilder co = new StringBuilder();
      concreteOperator(co, env);
      FileWriter cfw = new FileWriter(cof);
      cfw.write(co.toString());
      cfw.flush();
      cfw.close();
    }

    return compile(
        aof,
        cof,
        cts.getClassLibraries(),
        sourceRoot,
        cts.getModel().getContext().getOutputDirectory());
  }

  private static void indent(StringBuilder sb, int indentLevel) {
    for (int i = 0; i < indentLevel; i++) sb.append("    ");
  }

  public static void abstractOperator(StringBuilder sb, Env env) {

    classHeader(sb, env, false);

    addAbstractOperatorInitializationMethod(sb, env);

    for (int port = 0; port < env.inputSchemas.length; port++) addInputPort(sb, env, port);

    for (int port = 0; port < env.outputSchemas.length; port++) addOutputPort(sb, env, port);

    sb.append("\n");

    // Implementation code.
    processMethod(sb, env);

    sb.append("}\n");
  }

  public static void concreteOperator(StringBuilder sb, Env env) {

    classHeader(sb, env, true);

    // Not strictly required but helps the developer
    // and makes it clear that super.initialize is required.
    addConcreteOperatorInitializationMethod(sb, env);

    for (int port = 0; port < env.inputSchemas.length; port++) addInputPortConcrete(sb, env, port);

    sb.append("}\n");
  }

  private static void addConcreteOperatorInitializationMethod(StringBuilder sb, Env env) {

    StringBuilder contents = new StringBuilder();
    indent(contents, 2);
    contents.append("// TODO Auto-generated method stub\n");

    addInitializationMethod(sb, env, contents.toString());
  }

  private static void addAbstractOperatorInitializationMethod(StringBuilder main, Env env) {

    StringBuilder contents = new StringBuilder();
    indent(contents, 2);
    contents.append(
        "checkPorts(" + env.inputSchemas.length + "," + env.outputSchemas.length + ");\n");

    for (int port = 0; port < env.outputSchemas.length; port++) {
      addStreamingOutput(contents, 2, env, port);
    }

    addInitializationMethod(main, env, contents.toString());
  }

  private static void addStreamingOutput(StringBuilder sb, int indent, Env env, int port) {
    String tupleType = outputTupleTypeName(port);

    indent(sb, indent);
    sb.append("output" + port + " = new StreamingOutputWrapper<" + tupleType + ">");
    sb.append("(getOutput(" + port + ")) {\n");

    // unwrap method
    indent(sb, indent + 1);
    sb.append("protected OutputTuple unwrap(" + tupleType + " tuple) {\n");
    indent(sb, indent + 2);
    sb.append("return tuple.getOutputTuple();\n");
    indent(sb, indent + 1);
    sb.append("}\n");

    indent(sb, indent + 1);
    sb.append("public " + tupleType + " newTuple() {\n");

    indent(sb, indent + 2);
    sb.append("final OutputTuple tuple = getRealOutput().newTuple();\n");
    indent(sb, indent + 2);
    sb.append("return ");
    anonymousTuple(sb, indent + 2, env, port, false);
    sb.append(";\n");

    indent(sb, indent + 1);
    sb.append("}\n");

    // new Tuple method
    indent(sb, indent + 1);
    sb.append("};\n");
  }

  private static void addInitializationMethod(StringBuilder sb, Env env, String contents) {
    // Add the processN method
    sb.append("\n");
    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** Initialize the operator\n");
    indent(sb, 1);
    sb.append("*/\n");

    indent(sb, 1);
    sb.append("@Override\n");
    indent(sb, 1);
    sb.append("public void initialize(OperatorContext context) throws Exception {\n");
    indent(sb, 2);
    sb.append("super.initialize(context);\n");
    sb.append(contents);
    indent(sb, 1);
    sb.append("}\n");
  }

  /**
   * Create the class header which includes the package statement, the imports and the class
   * declaration.
   */
  private static void classHeader(StringBuilder sb, Env env, boolean concrete) {
    if (env.packageName != null) {
      sb.append("package ");
      sb.append(env.packageName);
      sb.append(";\n");
    }

    Set<Class<?>> typeImports = new HashSet<Class<?>>();
    for (StreamSchema schema : env.inputSchemas) {
      getImportsForSchema(typeImports, schema);
    }
    for (StreamSchema schema : env.outputSchemas) {
      getImportsForSchema(typeImports, schema);
    }

    sb.append("\n");
    for (Class<?> type : typeImports) {
      sb.append("import ");
      sb.append(type.getName());
      sb.append(";\n");
    }

    sb.append("\n");
    importStreamsClass(sb, "OperatorContext");
    if (!concrete) {
      importStreamsClass(sb, "AbstractOperator");
      importStreamsClass(sb, "OutputTuple");
      importStreamsClass(sb, "StreamingInput");
      importStreamsClass(sb, "Tuple");
      if (env.outputSchemas.length != 0) importStreamsClass(sb, "StreamingOutput");
      importStreamsClass(sb, "internal.operator.StreamingOutputWrapper");
    } else {

    }

    sb.append("\n");

    sb.append("public ");
    if (!concrete) sb.append("abstract");
    sb.append(" class ");
    sb.append(concrete ? env.concreteClassName : env.abstractClassName);
    sb.append(" extends ");
    sb.append(concrete ? env.abstractClassName : "AbstractOperator");
    sb.append(" {\n\n");
  }

  /**
   * Add required imports for the getters and setters corresponding to specific types in the schema.
   */
  private static void getImportsForSchema(Set<Class<?>> typeImports, StreamSchema schema) {
    for (Attribute attr : schema) {
      Class<?> typeClass = attr.getType().getObjectType();
      if (typeClass.getName().startsWith("java.lang.")) continue;
      if (typeClass.isArray()) continue;
      typeImports.add(typeClass);
    }
  }

  private static String inputTupleTypeName(int port) {
    return "IPort" + port;
  }

  private static String outputTupleTypeName(int port) {
    return "OPort" + port;
  }

  private static void processMethod(StringBuilder sb, Env env) {
    indent(sb, 1);
    sb.append("@Override\n");
    indent(sb, 1);
    sb.append(
        "public final synchronized void process(final StreamingInput input, final Tuple tuple) throws Exception {\n");
    indent(sb, 2);
    if (env.inputSchemas.length != 0) {
      sb.append("switch (input.getPortNumber()) {\n");
      for (int port = 0; port < env.inputSchemas.length; port++) {
        indent(sb, 2);
        sb.append("case " + port + ":\n");
        indent(sb, 3);
        sb.append("process" + port + "(");
        anonymousTuple(sb, 4, env, port, true);
        indent(sb, 3);
        sb.append(");\n");
        indent(sb, 3);
        sb.append("break;\n");
      }
      indent(sb, 2);
      sb.append("}\n"); // end of switch'
    }
    indent(sb, 1);
    sb.append("}\n"); // end of method
  }

  private static void anonymousTuple(
      StringBuilder sb, int indentLevel, Env env, int port, boolean input) {

    String tupleType = input ? inputTupleTypeName(port) : outputTupleTypeName(port);
    sb.append("new " + tupleType + "() {\n");
    addImplementationGetters(
        sb, indentLevel + 1, input ? env.inputSchemas[port] : env.outputSchemas[port]);

    // Add the implementation set_XXX methods
    if (!input) addImplementationSetters(sb, indentLevel + 1, env.outputSchemas[port]);

    // and now add the getTuple method
    String wrappedTuple = input ? "Tuple" : "OutputTuple";
    indent(sb, indentLevel + 1);
    sb.append("public " + wrappedTuple + " get" + wrappedTuple + "() {\n");
    indent(sb, indentLevel + 2);
    sb.append("return tuple;\n");
    indent(sb, indentLevel + 1);
    sb.append("}\n");

    indent(sb, indentLevel);
    sb.append("}\n");
  }

  private static void addInputPort(StringBuilder sb, Env env, int port) {

    String tupleType = inputTupleTypeName(port);
    StreamSchema schema = env.inputSchemas[port];

    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** Tuple interface for input port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("**\n");
    indent(sb, 1);
    sb.append("** Stream Schema\n");
    indent(sb, 1);
    sb.append("** ");
    sb.append(schema.toString());
    sb.append("\n");
    indent(sb, 1);
    sb.append("*/\n");

    inputTupleInterface(sb, 1, tupleType, schema);

    // Add the processN method
    sb.append("\n");
    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** process method for port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("*/\n");

    indent(sb, 1);
    sb.append("protected abstract void process");
    sb.append(port);
    sb.append("(");
    sb.append(tupleType);
    sb.append(" tuple) throws Exception;\n\n");
  }

  private static void addOutputPort(StringBuilder sb, Env env, int port) {

    String tupleType = outputTupleTypeName(port);
    StreamSchema schema = env.outputSchemas[port];

    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** Tuple interface for output port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("**\n");
    indent(sb, 1);
    sb.append("** Stream Schema\n");
    indent(sb, 1);
    sb.append("** ");
    sb.append(schema.toString());
    sb.append("\n");
    indent(sb, 1);
    sb.append("*/\n");

    outputTupleInterface(sb, 1, tupleType, schema);

    // Add the submitN method
    sb.append("\n");
    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** submit method for port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("*/\n");

    indent(sb, 1);
    sb.append("public final synchronized boolean submit");
    sb.append(port);
    sb.append("(");
    sb.append(tupleType);
    sb.append(" tuple) throws Exception {\n");
    indent(sb, 2);
    sb.append("return getOutput" + port + "().submit(tuple);\n");
    indent(sb, 1);
    sb.append("}\n");

    // Add the getOutputN method and field for the object.
    indent(sb, 1);
    sb.append("private StreamingOutput<" + tupleType + "> output" + port + ";\n");

    sb.append("\n");
    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** getOutput method for port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("*/\n");

    indent(sb, 1);
    sb.append("public final StreamingOutput<" + tupleType + "> getOutput");
    sb.append(port);
    sb.append("() {\n");

    indent(sb, 2);
    sb.append("return this.output" + port + ";\n");
    indent(sb, 1);
    sb.append("}\n");
  }

  private static void addInputPortConcrete(StringBuilder sb, Env env, int port) {

    String tupleType = inputTupleTypeName(port);

    // Add the processN method
    sb.append("\n");
    indent(sb, 1);
    sb.append("/**\n");
    indent(sb, 1);
    sb.append("** process method for port ");
    sb.append(port);
    sb.append(".\n");
    indent(sb, 1);
    sb.append("*/\n");

    indent(sb, 1);
    sb.append("@Override\n");
    indent(sb, 1);
    sb.append("protected void process");
    sb.append(port);
    sb.append("(");
    sb.append(tupleType);
    sb.append(" tuple) throws Exception {\n");
    indent(sb, 2);
    sb.append("// TODO Auto-generated method stub\n");
    indent(sb, 1);
    sb.append("}\n");
  }

  private static void importStreamsClass(StringBuilder sb, String name) {
    sb.append("import com.ibm.streams.operator.");
    sb.append(name);
    sb.append(";\n");
  }

  public static void inputTupleInterface(
      StringBuilder sb, int indentLevel, String ifName, StreamSchema schema) {

    indent(sb, indentLevel);
    sb.append("public interface ");
    sb.append(ifName);
    sb.append(" {\n");

    addInterfaceGetters(sb, indentLevel + 1, schema);

    indent(sb, indentLevel + 1);
    sb.append("/**\n");
    indent(sb, indentLevel + 1);
    sb.append("** Obtain the generic view of this tuple.\n");
    indent(sb, indentLevel + 1);
    sb.append("*/\n");
    indent(sb, indentLevel + 1);
    sb.append("public Tuple getTuple();\n");

    indent(sb, indentLevel);
    sb.append("}\n");
  }

  public static void outputTupleInterface(
      StringBuilder sb, int indentLevel, String ifName, StreamSchema schema) {

    indent(sb, indentLevel);
    sb.append("public interface ");
    sb.append(ifName);
    sb.append(" {\n");

    addInterfaceGetters(sb, indentLevel + 1, schema);
    addInterfaceSetters(sb, indentLevel + 1, schema);

    indent(sb, indentLevel + 1);
    sb.append("/**\n");
    indent(sb, indentLevel + 1);
    sb.append("** Obtain the generic view of this tuple.\n");
    indent(sb, indentLevel + 1);
    sb.append("*/\n");
    indent(sb, indentLevel + 1);
    sb.append("public OutputTuple getOutputTuple();\n");

    indent(sb, indentLevel);
    sb.append("}\n");
  }

  public static void addInterfaceGetters(StringBuilder sb, int indentLevel, StreamSchema schema) {

    for (Attribute attr : schema) {

      getterLeadIn(sb, indentLevel, attr);
      sb.append(";\n"); // interface method, no body
    }
  }

  public static void addImplementationGetters(
      StringBuilder sb, int indentLevel, StreamSchema schema) {

    for (Attribute attr : schema) {

      final Type type = attr.getType();
      final MetaType metaType = type.getMetaType();

      getterLeadIn(sb, indentLevel, attr);
      sb.append("{\n"); // interface method, no body
      indent(sb, indentLevel + 1);
      sb.append("return ");
      switch (metaType) {
        case BOOLEAN:
          sb.append("tuple.getBoolean");
          break;
        case UINT8:
        case INT8:
          sb.append("tuple.getByte");
          break;
        case UINT16:
        case INT16:
          sb.append("tuple.getShort");
          break;
        case UINT32:
        case INT32:
          sb.append("tuple.getInt");
          break;
        case UINT64:
        case INT64:
          sb.append("tuple.getLong");
          break;
        case FLOAT32:
          sb.append("tuple.getFloat");
          break;
        case FLOAT64:
          sb.append("tuple.getDouble");
          break;
        case USTRING:
          sb.append("tuple.getString");
          break;
        case TIMESTAMP:
          sb.append("tuple.getTimestamp");
          break;
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
          sb.append("tuple.getBigDecimal");
          break;
        case BLOB:
          sb.append("tuple.getBlob");
          break;
        case SET:
        case BSET:
          sb.append("tuple.getSet");
          break;
        case BMAP:
        case MAP:
          sb.append("tuple.getMap");
          break;
        case LIST:
        case BLIST:
          if (type.getObjectType().equals(List.class)) {
            sb.append("tuple.getList");
            break;
          }
        case OPTIONAL:
        default:
          // Insert a cast and use getObject
          sb.append("(");
          sb.append(type.getObjectType().getSimpleName());
          sb.append(") tuple.getObject");
          break;
      }
      sb.append("(");
      sb.append(attr.getIndex());
      sb.append(");\n");

      indent(sb, indentLevel);
      sb.append("}\n");
    }
  }

  public static void addInterfaceSetters(StringBuilder sb, int indentLevel, StreamSchema schema) {

    for (Attribute attr : schema) {

      setterLeadIn(sb, indentLevel, attr);
      sb.append(";\n"); // interface method, no body
    }
  }

  private static String getJavaType(Type type) {
    switch (type.getMetaType()) {
      case BOOLEAN:
        return "boolean";
      case UINT8:
      case INT8:
        return "byte";
      case UINT16:
      case INT16:
        return "short";
      case UINT32:
      case INT32:
        return "int";
      case UINT64:
      case INT64:
        return "long";
      case FLOAT32:
        return "float";
      case FLOAT64:
        return "double";
      default:
        return type.getObjectType().getSimpleName();
    }
  }

  public static void addImplementationSetters(
      StringBuilder sb, int indentLevel, StreamSchema schema) {

    for (Attribute attr : schema) {

      Type type = attr.getType();

      setterLeadIn(sb, indentLevel, attr);
      sb.append("{\n");
      indent(sb, indentLevel + 1);
      switch (type.getMetaType()) {
        case BOOLEAN:
          sb.append("tuple.setBoolean");
          break;
        case UINT8:
        case INT8:
          sb.append("tuple.setByte");
          break;
        case UINT16:
        case INT16:
          sb.append("tuple.setShort");
          break;
        case UINT32:
        case INT32:
          sb.append("tuple.setInt");
          break;
        case UINT64:
        case INT64:
          sb.append("tuple.setLong");
          break;
        case FLOAT32:
          sb.append("tuple.setFloat");
          break;
        case FLOAT64:
          sb.append("tuple.setDouble");
          break;
        case USTRING:
          sb.append("tuple.setString");
          break;
        case TIMESTAMP:
          sb.append("tuple.setTimestamp");
          break;
        case DECIMAL32:
        case DECIMAL64:
        case DECIMAL128:
          sb.append("tuple.setBigDecimal");
          break;
        case BLOB:
          sb.append("tuple.setBlob");
          break;
        case SET:
        case BSET:
          sb.append("tuple.setSet");
          break;
        case MAP:
        case BMAP:
          sb.append("tuple.setMap");
          break;
        case LIST:
        case BLIST:
          if (type.getObjectType().equals(List.class)) {
            sb.append("tuple.setList");
            break;
          }
        case OPTIONAL:
        default:
          sb.append("tuple.setObject");
          break;
      }
      sb.append("(");
      sb.append(attr.getIndex());
      sb.append(", value);\n");

      indent(sb, indentLevel);
      sb.append("}\n");
    }
  }

  /**
   * Produces the lead into a getter method, e.g. public int get_count()
   *
   * @param sb
   * @param attr
   */
  private static void getterLeadIn(StringBuilder sb, int indentLevel, Attribute attr) {

    String sourceType = getJavaType(attr.getType());

    indent(sb, indentLevel);
    sb.append("public ");
    sb.append(sourceType);
    sb.append(" get_");
    sb.append(attr.getName());
    sb.append("()");
  }

  /**
   * Produces the lead into a setter method, e.g. public void set_count(int count)
   *
   * @param sb
   * @param attr
   */
  private static void setterLeadIn(StringBuilder sb, int indentLevel, Attribute attr) {

    String sourceType = getJavaType(attr.getType());

    indent(sb, indentLevel);
    sb.append("public void set_");
    sb.append(attr.getName());
    sb.append("(final " + sourceType + " value)");
  }

  /*
   * Compile apis using Java 6.
   */

  private static boolean compile(
      File abstractOperator,
      File concreteOperator,
      List<String> libraries,
      File sourceDirectory,
      String outputDirectory)
      throws IOException {
    File output = new File(outputDirectory, BIN_DIR);
    if (!output.exists()) output.mkdirs();

    JavaCompiler jc = ToolProvider.getSystemJavaCompiler();
    StandardJavaFileManager sjfm = jc.getStandardFileManager(null, null, null);

    sjfm.setLocation(StandardLocation.CLASS_OUTPUT, Collections.singleton(output));

    Iterable<? extends JavaFileObject> fileObjects =
        sjfm.getJavaFileObjects(abstractOperator, concreteOperator);

    List<File> classPath = new ArrayList<File>();

    // Add the operator api classes, not the JavaCompiler
    // seems to ignore Class-Path manifest entries so
    // have to add in the dependent commons math jar.
    addClassPathEntry(classPath, AbstractOperator.class);
    addClassPathEntry(classPath, Complex.class); // Apache Commons Math
    for (String lib : libraries) classPath.add(new File(lib));

    sjfm.setLocation(StandardLocation.SOURCE_PATH, Collections.singleton(sourceDirectory));
    sjfm.setLocation(StandardLocation.CLASS_PATH, classPath);

    boolean ok = jc.getTask(null, sjfm, null, null, null, fileObjects).call();
    sjfm.close();
    return ok;
  }

  /** Add the File for a class path entry from a loaded class. */
  private static void addClassPathEntry(List<File> classPath, Class<?> clazz) {
    classPath.add(new File(clazz.getProtectionDomain().getCodeSource().getLocation().getPath()));
  }
}
