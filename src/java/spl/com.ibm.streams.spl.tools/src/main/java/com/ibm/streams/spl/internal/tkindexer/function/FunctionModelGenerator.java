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

import com.ibm.streams.operator.internal.model.ShadowClass;
import com.ibm.streams.operator.internal.model.processors.Util;
import com.ibm.streams.spl.internal.tkindexer.ModelGeneratorHelper;
import java.io.File;
import java.lang.reflect.Method;
import org.w3c.dom.Document;
import org.w3c.dom.Element;

/** Generate a function model XML file for toolkit Java native functions. */
public final class FunctionModelGenerator {

  private final FunctionAnnotationProcessor fap;
  private final ModelGeneratorHelper mgh;
  private final Document doc;
  private final File functionModelDir;
  private final String fileName;
  private final String SPLNamespace;

  private Element mod;
  private Element fns;
  private boolean hasFunctions = false;

  public FunctionModelGenerator(
      FunctionAnnotationProcessor fap, String SPLNamespace, File functionModelDir, String fileName)
      throws Exception {
    this.fap = fap;
    this.SPLNamespace = SPLNamespace;
    this.functionModelDir = functionModelDir;
    this.fileName = fileName;
    mgh = new ModelGeneratorHelper(fap);
    doc = mgh.newDocument();
    startModel();
  }

  /**
   * Add a function to the function model. For integral parameters, we also generate a function with
   * unsigned types.
   */
  public final void addFunction(AnnotatedFunction af) throws Exception {
    addFunction(af, true);
    if (af.prototypeHasIntTypeParam()) addFunction(af, false);
    hasFunctions = true;
  }

  /**
   * Add a function to the function model. For array parameters, we also generate a function with
   * bounded lists.
   */
  private final void addFunction(AnnotatedFunction af, boolean signedMode) throws Exception {
    addFunction(af, signedMode, false);
    if (af.prototypeHasArrayParam()) addFunction(af, signedMode, true);
    hasFunctions = true;
  }

  public final void writeFile() throws Exception {
    if (hasFunctions) mgh.writeModelFile(functionModelDir.getCanonicalPath(), fileName, doc);
  }

  private final void startModel() throws Exception {
    mod = doc.createElement("functionModel");
    mod.setAttribute("xmlns", "http://www.ibm.com/xmlns/prod/streams/spl/function");
    mod.setAttribute("xmlns:cmn", "http://www.ibm.com/xmlns/prod/streams/spl/common");
    mod.setAttribute("xmlns:xsi", ModelGeneratorHelper.W3C_XML_SCHEMA_INSTANCE);
    mod.setAttribute(
        "xsi:schemaLocation",
        "http://www.ibm.com/xmlns/prod/streams/spl/function " + "functionModel.xsd");
    mod.appendChild(mgh.getDoNotEditThisFileComment(doc));
    mod.appendChild(createFunctionSet());
    doc.appendChild(mod);
  }

  private final void addFunction(AnnotatedFunction af, boolean signedMode, boolean boundedListMode)
      throws Exception {
    Element fn = doc.createElement("function");
    fn.appendChild(mgh.createGeneratedFromComment(doc));
    String desc = af.getDescription();
    if (!desc.isEmpty()) fn.appendChild(mgh.createTextElement(doc, "description", desc));
    fn.appendChild(
        mgh.createCDATAElement(doc, "prototype", af.getSPLPrototype(signedMode, boundedListMode)));

    Method meth = af.getMethod();
    ShadowClass shadowAnn = meth.getDeclaringClass().getAnnotation(ShadowClass.class);
    if (shadowAnn != null) {
      String methodAsString = meth.toString();
      methodAsString = methodAsString.replace(Util.GENERATED_CLASS_SUFFIX, "");
      fn.appendChild(mgh.createTextElement(doc, "javaFunctionName", methodAsString));
    }
    fns.appendChild(fn);
  }

  // a function set contains all the annotated functions in an SPL namespace
  private final Element createFunctionSet() throws Exception {
    Element set = doc.createElement("functionSet");
    set.appendChild(
        mgh.createTextElement(doc, "headerFileName", fap.getHeaderFilename(SPLNamespace)));
    set.appendChild(
        mgh.createTextElement(doc, "cppNamespaceName", fap.getCppNamespace(SPLNamespace)));
    set.appendChild(fns = doc.createElement("functions"));
    set.appendChild(createDependencies());
    return set;
  }

  private final Element createDependencies() throws Exception {
    Element dep = doc.createElement("dependencies");
    Element lib = doc.createElement("library");
    lib.appendChild(
        mgh.createTextElement(doc, "cmn:description", "C++ interface to Java native functions."));
    Element mlib = doc.createElement("cmn:managedLibrary");
    mlib.appendChild(mgh.createTextElement(doc, "cmn:lib", "streams-stdtk-javaop"));
    mlib.appendChild(mgh.createTextElement(doc, "cmn:lib", "streams-stdtk-runtime"));
    mlib.appendChild(
        mgh.createTextElement(
            doc, "cmn:includePath", FunctionAnnotationProcessor.HDR_DIR_REL_MODEL_DIR));
    lib.appendChild(mlib);
    dep.appendChild(lib);
    lib = doc.createElement("library");
    lib.appendChild(mgh.createTextElement(doc, "cmn:description", "Java JNI."));
    mlib = doc.createElement("cmn:managedLibrary");
    mlib.appendChild(mgh.createTextElement(doc, "cmn:lib", "pthread"));
    mlib.appendChild(mgh.createTextElement(doc, "cmn:lib", "dl"));
    mlib.appendChild(mgh.createTextElement(doc, "cmn:includePath", "@JAVA_HOME@/include"));
    mlib.appendChild(
        mgh.createTextElement(
            doc, "cmn:command", "@STREAMS_TOOLKIT_DIR@/impl/bin/JavaOp_vmpath.sh"));
    lib.appendChild(mlib);
    dep.appendChild(lib);
    return dep;
  }
}
