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

import com.ibm.streams.function.model.Function;
import com.ibm.streams.spl.internal.tkindexer.AbstractAnnotationProcessor;
import com.ibm.streams.spl.internal.tkindexer.ModelGeneratorHelper;
import java.io.File;
import java.lang.reflect.Method;
import java.util.HashMap;

/**
 * Process toolkit Java native function "@Function" annotations.
 *
 * <p>This is the main class for generating the function model and C++ glue code so that Java native
 * functions can be called directly from SPL.
 *
 * <p>The generated glue code consists of C++ header files only. This avoids the need to build
 * platform-dependent shared libraries. We use a prefixed namespace to avoid name conflicts with
 * user C++ code. We generate classes so there is only one copy of the resulting object code.
 *
 * <p>There is one function model and one glue code file per SPL namespace. An SPL namespace is set
 * to the Java package name by default. Therefore each file usually contains all annotated functions
 * from a package. However the annotations can map functions to different namespaces.
 */
public final class FunctionAnnotationProcessor extends AbstractAnnotationProcessor {

  protected static final String FUNC_SUBDIR = "native.function";
  protected static final String MODEL_FILENAME = "javaFunction.xml";
  protected static final String HDR_DIR_REL_MODEL_DIR = ".";
  private static final String HDR_FILENAME_SUFFIX = ".h";
  private static final String CPP_NAMESPACE_PREFIX = "SPL::JNIFunctions";
  private static final String CPP_CLASSNAME = "SPL_JNIFunctions";

  // Map the SPL namespace to the model and glue code generators.
  private HashMap<String, FunctionFileGenerators> ffgMap;

  @Override
  public final void initialize() {
    ffgMap = new HashMap<String, FunctionFileGenerators>();
  }

  @Override
  public final void process(Class<?> cls) throws Exception {
    for (Method m : cls.getDeclaredMethods()) {
      if (!m.isAnnotationPresent(Function.class)) continue;
      AnnotatedFunction af = new AnnotatedFunction(this, m);
      if (hasErrors()) continue;
      addFunction(af);
    }
  }

  @Override
  public final void complete() throws Exception {
    for (FunctionFileGenerators ffg : ffgMap.values()) {
      ffg.fmg.writeFile();
      ffg.fcg.writeFile();
    }
  }

  public final String getCppNamespace(String SPLNamespace) {
    return CPP_NAMESPACE_PREFIX
        + (SPLNamespace.length() > 0 ? "::" + SPLNamespace.replaceAll("\\.", "::") : "")
        + "::"
        + CPP_CLASSNAME;
  }

  public final String getHeaderFilename(String SPLNamespace) {
    return CPP_NAMESPACE_PREFIX.replaceAll("::", "_")
        + (SPLNamespace.length() > 0 ? "_" + SPLNamespace.replace('.', '_') : "")
        + HDR_FILENAME_SUFFIX;
  }

  private final void addFunction(AnnotatedFunction af) throws Exception {
    String SPLNamespace = af.getSPLNamespace();
    FunctionFileGenerators ffg = ffgMap.get(SPLNamespace);
    if (ffg == null) {
      File namespaceDir =
          new File(getToolkitRootDir(), ModelGeneratorHelper.getNamespacePath(SPLNamespace));
      File functionDir = new File(namespaceDir, FUNC_SUBDIR);
      FunctionModelGenerator fmg =
          new FunctionModelGenerator(this, SPLNamespace, functionDir, MODEL_FILENAME);
      FunctionCodeGenerator fcg =
          new FunctionCodeGenerator(
              this, SPLNamespace, functionDir, getHeaderFilename(SPLNamespace));
      ffg = new FunctionFileGenerators(fmg, fcg);
      ffgMap.put(SPLNamespace, ffg);
    }
    ffg.fmg.addFunction(af);
    ffg.fcg.addFunction(af);
  }

  /* This class is used to store the generators for a given SPL namespace. */
  private static final class FunctionFileGenerators {
    FunctionModelGenerator fmg;
    FunctionCodeGenerator fcg;

    FunctionFileGenerators(FunctionModelGenerator fmg, FunctionCodeGenerator fcg) {
      this.fmg = fmg;
      this.fcg = fcg;
    }
  }
}
