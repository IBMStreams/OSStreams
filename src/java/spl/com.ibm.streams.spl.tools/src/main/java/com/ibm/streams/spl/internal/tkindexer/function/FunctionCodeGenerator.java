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

package com.ibm.streams.spl.internal.tkindexer.function;

import com.ibm.streams.operator.Type.MetaType;
import java.io.File;
import java.io.FileWriter;

/**
 * Generate C++ glue code for calling toolkit Java native functions from SPL by using the C++ native
 * function call mechanism.
 */
public final class FunctionCodeGenerator {

  private static final boolean SIGNED_MODE = true;
  private static final boolean UNSIGNED_MODE = false;
  private static final boolean BOUNDED_LIST_MODE = true;
  private static final boolean UNBOUNDED_LIST_MODE = false;

  private final FunctionAnnotationProcessor fap;
  private final File glueCodeDir;
  private final String fileName;
  private final String SPLNamespace;

  private StringBuilder sb;
  private boolean hasFunctions = false;
  private int namespaces = 0;
  private int indent = 0;

  public FunctionCodeGenerator(
      FunctionAnnotationProcessor fap, String SPLNamespace, File glueCodeDir, String fileName)
      throws Exception {
    this.fap = fap;
    this.SPLNamespace = SPLNamespace;
    this.glueCodeDir = glueCodeDir;
    this.fileName = fileName;
    sb = new StringBuilder();
  }

  /**
   * Add a function to the glue code. For string parameters, we generate multiple functions to match
   * the SPL generic string prototype. For string returns only, we generate an rstring return
   * function. For integral parameters, we also generate a function with unsigned types. For array
   * parameters, we also generate a function with bounded lists.
   */
  public final void addFunction(AnnotatedFunction af) throws Exception {
    if (!hasFunctions) {
      startGlueCode();
      hasFunctions = true;
    }
    if (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()) {
      addFunction(af, MetaType.USTRING);
      addFunction(af, MetaType.RSTRING);
      addFunction(af, MetaType.BSTRING);
    } else {
      addFunction(af, MetaType.RSTRING);
    }
  }

  private final void addFunction(AnnotatedFunction af, MetaType mode) throws Exception {
    addFunction(af, mode, SIGNED_MODE);
    if (af.prototypeHasIntTypeParam()) addFunction(af, mode, UNSIGNED_MODE);
  }

  private final void addFunction(AnnotatedFunction af, MetaType mode, boolean signedMode)
      throws Exception {
    generateFunction(af, mode, signedMode, UNBOUNDED_LIST_MODE);
    if (af.prototypeHasArrayParam()) generateFunction(af, mode, signedMode, BOUNDED_LIST_MODE);
  }

  public final void writeFile() throws Exception {
    if (hasFunctions) {
      endGlueCode();
      fap.mkdirs(glueCodeDir);
      File file = new File(glueCodeDir, fileName);
      fap.fileToRemoveOnError(file);
      file.setWritable(true);
      FileWriter fw = new FileWriter(file);
      fw.write(sb.toString());
      fw.close();
      file.setReadOnly();
    }
  }

  private final void startGlueCode() {
    append("// " + fap.getDoNotEditThisFileString());
    String s = fileName.replace('.', '_');
    appendNewLine("#ifndef " + s);
    appendNewLine("#define " + s);
    appendNewLine();
    appendNewLine("#include <SPL/Runtime/Function/SPLJavaFunction.h>");
    appendNewLine("#include <SPL/Runtime/Type/SPLType.h>");
    appendNewLine();
    String[] names = fap.getCppNamespace(SPLNamespace).split("::");
    for (int i = 0; i < names.length - 1; i++) {
      appendNewLine("namespace " + names[i] + " {");
      namespaces++;
    }
    appendNewLine("class " + names[names.length - 1] + " {");
    appendNewLine("public:");
  }

  private final void generateFunction(
      AnnotatedFunction af, MetaType mode, boolean signedMode, boolean boundedListMode)
      throws Exception {
    generateFunctionHead(af, mode, signedMode, boundedListMode);
    generateFunctionLoad(af, mode, signedMode, boundedListMode);
    generateFunctionCall(af, mode, signedMode, boundedListMode);
  }

  private final void generateFunctionHead(
      AnnotatedFunction af, MetaType mode, boolean signedMode, boolean boundedListMode)
      throws Exception {
    appendNewLine();
    appendNewLine("// " + fap.getGeneratedFromString());
    if (mode == MetaType.BSTRING) {
      if (boundedListMode == BOUNDED_LIST_MODE)
        appendNewLine("template <SPL::int32 msize, SPL::int32 lsize>");
      else appendNewLine("template <SPL::int32 msize>");
    } else if (boundedListMode == BOUNDED_LIST_MODE) {
      appendNewLine("template <SPL::int32 lsize>");
    }
    appendNewLine("static ");
    Class<?> returnType = af.getMethod().getReturnType();
    if (!returnType.equals(void.class)) append("SPL::");
    append(javaToSPLType(returnType, mode, signedMode, boundedListMode));
    append(" " + af.getSPLName() + "(");
    Class<?>[] params = af.getMethod().getParameterTypes();
    for (int i = 0; i < params.length; i++) {
      if (i > 0) append(", ");
      append("SPL::" + javaToSPLType(params[i], mode, signedMode, boundedListMode));
      append(" const & arg" + (i + 1));
    }
    append(") {");
    startIndent();
  }

  private final void generateFunctionLoad(
      AnnotatedFunction af, MetaType mode, boolean signedMode, boolean boundedListMode)
      throws Exception {
    if ((mode == MetaType.RSTRING
            && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
        || mode == MetaType.BSTRING
        || signedMode == UNSIGNED_MODE
        || boundedListMode == BOUNDED_LIST_MODE) return;
    appendNewLine("static __thread void *func;");
    appendNewLine("static __thread jclass classGlobalRef;");
    appendNewLine("static __thread jmethodID mid;");
    if (af.prototypeHasStringArrayParam()) appendNewLine("static __thread jclass stringClass;");
    appendNewLine("if (func == NULL) {");
    startIndent();
    appendNewLine("func = SPL::SPLJavaFunction::loadFunction(");
    append("\"" + fap.getToolkitName() + "\", ");
    append("\"" + fap.getToolkitVersion() + "\", ");
    append("SPLJAVAFUNCTION_ADL_FILENAME, ");
    append("\"" + fap.getClassRelClasspath() + "\", ");
    append("\"" + fap.getClassOrigName() + "\", ");
    append("\"" + af.getMethod().getName() + "\", ");
    append("\"(");
    Class<?>[] params = af.getMethod().getParameterTypes();
    for (int i = 0; i < params.length; i++) append(fap.javaTypeSig(params[i]));
    append(")" + fap.javaTypeSig(af.getMethod().getReturnType()) + "\", ");
    append("\"" + af.getSPLNamespace() + "\", ");
    append("\"" + af.getSPLPrototype(signedMode, boundedListMode) + "\", ");
    append("&classGlobalRef, &mid);");
    if (af.prototypeHasStringArrayParam()) {
      appendNewLine("JNIEnv* env = ");
      append("SPL::SPLJavaFunction::getJNIEnv(func);");
      appendNewLine("stringClass = env->FindClass(\"java/lang/String\");");
    }
    endIndent();
    appendNewLine("}");
  }

  private final void generateFunctionCall(
      AnnotatedFunction af, MetaType mode, boolean signedMode, boolean boundedListMode)
      throws Exception {
    if ((mode == MetaType.RSTRING
            && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
        || mode == MetaType.BSTRING
        || signedMode == UNSIGNED_MODE
        || boundedListMode == BOUNDED_LIST_MODE) {
      generateModeFunctionCall(af, mode, signedMode, boundedListMode);
      return;
    }
    appendNewLine("JNIEnv* env = ");
    append("SPL::SPLJavaFunction::getJNIEnv(func);");
    Class<?>[] params = af.getMethod().getParameterTypes();
    for (int i = 0; i < params.length; i++) {
      if (params[i].equals(String.class)) generateCreateStringParam(i);
      else if (params[i].isArray()) {
        if (params[i].getComponentType().equals(String.class)) generateCreateStringArrayParam(i);
        else generateCreateArrayParam(params[i], i);
      }
    }
    Class<?> returnType = af.getMethod().getReturnType();
    if (returnType.equals(String.class)) {
      appendNewLine("jstring resObj = (jstring) ");
    } else if (returnType.isArray()) {
      String type = fap.javaToCppType(returnType.getComponentType()) + "Array";
      appendNewLine(type + " resObj = (" + type + ") ");
    } else if (returnType.equals(void.class)) {
      appendNewLine("");
    } else {
      appendNewLine("SPL::");
      append(fap.javaToSPLType(returnType, mode, signedMode));
      append(" result = ");
    }
    append("env->CallStatic");
    append(fap.javaMethodReturnType(returnType));
    append("Method(classGlobalRef, mid");
    for (int i = 0; i < params.length; i++) {
      if (params[i].equals(String.class) || params[i].isArray()) append(", " + "obj" + (i + 1));
      else append(", " + SPLToCppTypeCast(params[i]) + "arg" + (i + 1));
    }
    append(");");
    appendNewLine("if (env->ExceptionCheck())");
    appendWithIndent("SPL::SPLJavaFunction::throwCallError(func);");
    for (int i = 0; i < params.length; i++)
      if (params[i].equals(String.class) || params[i].isArray())
        appendNewLine("env->DeleteLocalRef(obj" + (i + 1) + ");");
    if (!returnType.equals(void.class)) {
      if (returnType.equals(String.class)) {
        generateReturnString(mode);
      } else if (returnType.isArray()) {
        if (returnType.getComponentType().equals(String.class))
          generateReturnStringArray(returnType, mode);
        else generateReturnArray(returnType, mode, signedMode);
      } else {
        appendNewLine("return result;");
      }
    }
    endIndent();
    appendNewLine("}");
  }

  private final void generateCreateStringParam(int idx) {
    appendNewLine("jstring obj" + (idx + 1));
    append(" = env->NewString((jchar *) ");
    append("arg" + (idx + 1));
    append(".getBuffer(), (jsize) ");
    append("arg" + (idx + 1));
    append(".length());");
    appendNewLine("if (env->ExceptionCheck())");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
  }

  private final void generateReturnString(MetaType mode) {
    appendNewLine("if (resObj != NULL) {");
    startIndent();
    appendNewLine("const jchar *chars = env->GetStringChars(resObj, NULL);");
    appendNewLine("if (chars == NULL)");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("SPL::ustring result((const UChar *) chars, ");
    append("(int32_t) env->GetStringLength(resObj));");
    appendNewLine("env->ReleaseStringChars(resObj, chars);");
    appendNewLine("env->DeleteLocalRef(resObj);");
    if (mode == MetaType.RSTRING) appendNewLine("return SPL::ustring::toUTF8(result);");
    else appendNewLine("return result;");
    endIndent();
    appendNewLine("} else {");
    startIndent();
    if (mode == MetaType.RSTRING) appendNewLine("return SPL::rstring(\"\");");
    else appendNewLine("return SPL::ustring::fromUTF8(\"\");");
    endIndent();
    appendNewLine("}");
  }

  private final void generateCreateArrayParam(Class<?> cls, int idx) throws Exception {
    Class<?> ctype = cls.getComponentType();
    appendNewLine("jsize len" + (idx + 1));
    append(" = arg" + (idx + 1) + ".getSize();");
    appendNewLine(fap.javaToCppType(ctype) + "Array obj" + (idx + 1));
    append(" = env->New");
    append(fap.javaMethodReturnType(ctype));
    append("Array(len" + (idx + 1) + ");");
    appendNewLine("if (obj" + (idx + 1) + " == NULL)");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("if (len" + (idx + 1) + " > 0) {");
    startIndent();
    if (ctype.equals(byte.class) || ctype.equals(boolean.class)) {
      // not necessary now, but might be if class for SPL type changes
      appendNewLine("for (jsize i = 0; i < len" + (idx + 1) + "; i++) {");
      startIndent();
      appendNewLine("env->Set");
      append(fap.javaMethodReturnType(ctype));
      append("ArrayRegion(obj" + (idx + 1));
      append(", i, 1, (" + fap.javaToCppType(ctype) + " *) &(");
      append("arg" + (idx + 1) + ".at((std::vector<");
      append(fap.javaToSPLType(ctype, null, SIGNED_MODE));
      append(">::size_type) i)));");
      appendNewLine("if (env->ExceptionCheck())");
      appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
      appendNewLine("}");
      endIndent();
    } else {
      appendNewLine("env->Set");
      append(fap.javaMethodReturnType(ctype));
      append("ArrayRegion(obj" + (idx + 1));
      append(", 0, len" + (idx + 1));
      append(", (" + fap.javaToCppType(ctype) + " *) &(");
      append("arg" + (idx + 1) + ".at(0)));");
      appendNewLine("if (env->ExceptionCheck())");
      appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    }
    appendNewLine("}");
    endIndent();
  }

  private final void generateReturnArray(Class<?> cls, MetaType mode, boolean signedMode)
      throws Exception {
    Class<?> ctype = cls.getComponentType();
    appendNewLine("if (resObj != NULL) {");
    startIndent();
    appendNewLine(fap.javaToCppType(ctype));
    append(" *elems = env->Get");
    append(fap.javaMethodReturnType(ctype));
    append("ArrayElements(resObj, NULL);");
    appendNewLine("if (elems == NULL)");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("SPL::");
    append(fap.javaToSPLType(cls, mode, signedMode));
    append(" result(elems, elems + env->GetArrayLength(resObj));");
    appendNewLine("env->Release");
    append(fap.javaMethodReturnType(ctype));
    append("ArrayElements(resObj, elems, JNI_ABORT);");
    appendNewLine("env->DeleteLocalRef(resObj);");
    appendNewLine("return result;");
    endIndent();
    appendNewLine("} else {");
    startIndent();
    appendNewLine("return SPL::");
    append(fap.javaToSPLType(cls, mode, signedMode));
    append("();");
    endIndent();
    appendNewLine("}");
  }

  private final void generateCreateStringArrayParam(int idx) throws Exception {
    appendNewLine("jsize len" + (idx + 1));
    append(" = arg" + (idx + 1) + ".getSize();");
    appendNewLine("jobjectArray obj" + (idx + 1));
    append(" = env->NewObjectArray(len" + (idx + 1));
    append(", stringClass, NULL);");
    appendNewLine("if (env->ExceptionCheck() || obj" + (idx + 1));
    append(" == NULL)");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("if (len" + (idx + 1) + " > 0) {");
    startIndent();
    appendNewLine("for (jsize i = 0; i < len" + (idx + 1) + "; i++) {");
    startIndent();
    appendNewLine("SPL::ustring ustr = arg" + (idx + 1));
    append(".at((std::vector<SPL::ustring>::size_type) i);");
    appendNewLine("jstring jstr = env->NewString(");
    append("(jchar *) ustr.getBuffer(), (jsize) ustr.length());");
    appendNewLine("if (env->ExceptionCheck())");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("env->SetObjectArrayElement(obj" + (idx + 1));
    append(", i, jstr);");
    appendNewLine("if (env->ExceptionCheck())");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("env->DeleteLocalRef(jstr);");
    appendNewLine("}");
    endIndent();
    appendNewLine("}");
    endIndent();
  }

  private final void generateReturnStringArray(Class<?> cls, MetaType mode) throws Exception {
    appendNewLine("SPL::");
    append(fap.javaToSPLType(cls, mode, SIGNED_MODE));
    append(" result;");
    appendNewLine("if (resObj != NULL) {");
    startIndent();
    appendNewLine("jsize len = env->GetArrayLength(resObj);");
    appendNewLine("for (jsize i = 0; i < len; i++) {");
    startIndent();
    appendNewLine("jstring jstr = (jstring) ");
    append("env->GetObjectArrayElement(resObj, i);");
    appendNewLine("if (env->ExceptionCheck())");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("if (jstr != NULL) {");
    startIndent();
    appendNewLine("const jchar *chars = env->GetStringChars(jstr, NULL);");
    appendNewLine("if (chars == NULL)");
    appendWithIndent("SPL::SPLJavaFunction::throwCreateError(func);");
    appendNewLine("SPL::ustring ustr((const UChar *) chars, ");
    append("(int32_t) env->GetStringLength(jstr));");
    appendNewLine("env->ReleaseStringChars(jstr, chars);");
    appendNewLine("env->DeleteLocalRef(jstr);");
    if (mode == MetaType.RSTRING) appendNewLine("result.add(SPL::ustring::toUTF8(ustr));");
    else appendNewLine("result.add(ustr);");
    endIndent();
    appendNewLine("} else {"); // jstr == NULL
    startIndent();
    if (mode == MetaType.RSTRING) appendNewLine("result.add(SPL::rstring(\"\"));");
    else appendNewLine("result.add(SPL::ustring::fromUTF8(\"\"));");
    endIndent();
    appendNewLine("}");
    endIndent();
    appendNewLine("}");
    appendNewLine("env->DeleteLocalRef(resObj);");
    endIndent();
    appendNewLine("}");
    appendNewLine("return result;");
  }

  /**
   * Generate a function call for rstring, bounded rstring, or unsigned integral parameters
   * (including lists of these types), or bounded lists.
   *
   * <p>For rstring, convert to ustring and call ustring function. For bounded rstring, convert to
   * rstring and call rstring function. For integral type, convert to unsigned and call signed
   * function. For bounded list, convert to unbounded and call unbounded function.
   */
  private final void generateModeFunctionCall(
      AnnotatedFunction af, MetaType mode, boolean signedMode, boolean boundedListMode)
      throws Exception {
    appendNewLine("");
    Class<?> returnType = af.getMethod().getReturnType();
    if (!returnType.equals(void.class)) {
      append("SPL::");
      if (boundedListMode == BOUNDED_LIST_MODE)
        append(javaToSPLType(returnType, mode, signedMode, UNBOUNDED_LIST_MODE));
      else
        append(
            fap.javaToSPLType(
                returnType,
                (mode == MetaType.RSTRING
                        && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
                    ? MetaType.USTRING
                    : mode == MetaType.BSTRING ? MetaType.RSTRING : mode,
                SIGNED_MODE));
      append(" result = ");
    }
    append(af.getSPLName() + "(");
    Class<?>[] params = af.getMethod().getParameterTypes();
    for (int i = 0; i < params.length; i++) {
      if (i > 0) append(", ");
      if (boundedListMode == BOUNDED_LIST_MODE) {
        if (params[i].isArray())
          append(
              "(SPL::"
                  + javaToSPLType(params[i], mode, signedMode, UNBOUNDED_LIST_MODE)
                  + ") arg"
                  + (i + 1));
        else append("arg" + (i + 1));
      } else {
        if (params[i].equals(String.class)
            && mode == MetaType.RSTRING
            && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
          append("SPL::ustring::fromUTF8(arg" + (i + 1) + ")");
        else if (params[i].isArray()
            && params[i].getComponentType().equals(String.class)
            && mode == MetaType.RSTRING
            && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
          append(
              "(SPL::"
                  + fap.javaToSPLType(params[i], MetaType.USTRING, signedMode)
                  + ") arg"
                  + (i + 1));
        else if ((params[i].equals(String.class)
                || (params[i].isArray() && params[i].getComponentType().equals(String.class)))
            && mode == MetaType.BSTRING)
          append(
              "(SPL::"
                  + fap.javaToSPLType(params[i], MetaType.RSTRING, signedMode)
                  + ") arg"
                  + (i + 1));
        else if (fap.javaMapsToUnsignedSPLType(params[i]) && signedMode == UNSIGNED_MODE)
          append("(SPL::" + fap.javaToSPLType(params[i], mode, SIGNED_MODE) + ") arg" + (i + 1));
        else append("arg" + (i + 1));
      }
    }
    append(");");
    if (!returnType.equals(void.class)) {
      appendNewLine("return ");
      if (boundedListMode == BOUNDED_LIST_MODE) {
        if (returnType.isArray())
          append(
              "(SPL::"
                  + javaToSPLType(returnType, mode, signedMode, boundedListMode)
                  + ") result;");
        else append("result;");
      } else if (returnType.equals(String.class)
          && mode == MetaType.RSTRING
          && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
        append("SPL::ustring::toUTF8(result);");
      else if ((returnType.isArray()
              && returnType.getComponentType().equals(String.class)
              && mode == MetaType.RSTRING
              && (af.prototypeHasStringParam() || af.prototypeHasStringArrayParam()))
          || (returnType.equals(String.class)
              || (returnType.isArray() && returnType.getComponentType().equals(String.class))
                  && mode == MetaType.BSTRING)
          || (fap.javaMapsToUnsignedSPLType(returnType) && signedMode == UNSIGNED_MODE))
        append(
            "(SPL::" + javaToSPLType(returnType, mode, signedMode, boundedListMode) + ") result;");
      else append("result;");
    }
    endIndent();
    appendNewLine("}");
  }

  private final void endGlueCode() {
    appendNewLine();
    appendNewLine("};");
    for (int i = 0; i < namespaces; i++) {
      appendNewLine("}");
    }
    appendNewLine("#endif");
    appendNewLine();
  }

  // map type to SPL C++ type, including BSTRING and BOUNDED_LIST_MODE cases
  private final String javaToSPLType(
      Class<?> cls, MetaType mode, boolean signedMode, boolean boundedListMode) throws Exception {
    if (cls.equals(String.class) && mode == MetaType.BSTRING) return "bstring<msize>";
    if (cls.isArray()) {
      Class<?> ctype = cls.getComponentType();
      if (ctype.equals(String.class) && mode == MetaType.BSTRING) {
        if (boundedListMode == BOUNDED_LIST_MODE) return "blist<bstring<msize>, lsize>";
        else return "list<bstring<msize> >";
      }
      if (boundedListMode == BOUNDED_LIST_MODE)
        return "blist<" + fap.javaToSPLType(ctype, mode, signedMode) + ", lsize>";
    }
    return fap.javaToSPLType(cls, mode, signedMode);
  }

  // provide cast for C++ primitive types implemented as classes in SPL
  private final String SPLToCppTypeCast(Class<?> cls) throws Exception {
    if (cls.equals(boolean.class) || cls.equals(byte.class))
      return "(" + fap.javaToCppType(cls) + ") ";
    else return "";
  }

  private final void startIndent() {
    indent++;
  }

  private final void endIndent() {
    indent--;
  }

  private final void appendWithIndent(String s) {
    startIndent();
    appendNewLine(s);
    endIndent();
  }

  private final void appendNewLine() {
    sb.append("\n");
  }

  private final void appendNewLine(String s) {
    sb.append("\n");
    for (int i = 0; i < indent; i++) {
      sb.append("    ");
    }
    sb.append(s);
  }

  private final void append(String s) {
    sb.append(s);
  }
}
