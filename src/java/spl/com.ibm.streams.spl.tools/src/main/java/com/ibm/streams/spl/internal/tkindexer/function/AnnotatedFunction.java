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
import com.ibm.streams.operator.Type.MetaType;
import com.ibm.streams.operator.internal.model.MethodParameters;
import java.lang.reflect.Method;

/**
 * Class to represent an annotated Java native function.
 *
 * <p>This class reads the annotations for a Java native function, and provides defaults. The
 * annotations have already been verified by the compile-time verifier.
 */
public final class AnnotatedFunction {

  private FunctionAnnotationProcessor fap;
  private Method method;
  private Function annotation;
  private String SPLName;
  private String description;
  private boolean prototypeHasArrayParam = false;
  private boolean prototypeHasIntTypeParam = false;
  private boolean prototypeHasStringParam = false;
  private boolean prototypeHasStringArrayParam = false;

  public AnnotatedFunction(FunctionAnnotationProcessor fap, Method method) throws Exception {
    this.fap = fap;
    this.method = method;
    annotation = method.getAnnotation(Function.class);
    determineSPLName();
    determineFunctionDescription();
    determinePrototypeDataTypes();
  }

  protected final String getDescription() {
    return description;
  }

  protected final Method getMethod() {
    return method;
  }

  protected final String getSPLName() {
    return SPLName;
  }

  protected final String getSPLNamespace() {
    return annotation.namespace();
  }

  /**
   * Return the SPL function prototype. Use an SPL generic string type for functions with string
   * parameters. Use an SPL rstring for functions with only a string return type. Use signed or
   * unsigned integral types, depending on the mode. Use bounded or unbounded list types, depending
   * on the mode.
   */
  protected final String getSPLPrototype(boolean signedMode, boolean boundedListMode)
      throws Exception {
    StringBuilder sb = new StringBuilder();
    if (prototypeHasStringParam || prototypeHasStringArrayParam) sb.append("<string T> ");
    if (prototypeHasArrayParam && boundedListMode) sb.append("[N] ");
    sb.append("public ");
    if (annotation.stateful()) sb.append("stateful ");
    Class<?> returnType = method.getReturnType();
    if (returnType.equals(String.class)
        && (prototypeHasStringParam || prototypeHasStringArrayParam)) sb.append("T");
    else if (returnType.isArray()
        && returnType.getComponentType().equals(String.class)
        && (prototypeHasStringParam || prototypeHasStringArrayParam)) sb.append("list<T>");
    else sb.append(fap.javaToSPLType(returnType, MetaType.RSTRING, signedMode));
    if (returnType.isArray() && prototypeHasArrayParam && boundedListMode) sb.append("[N]");
    sb.append(" " + SPLName + "(");
    Class<?>[] params = method.getParameterTypes();
    // Look for method parameter names so that the prototype does not revert to default values
    MethodParameters mps = method.getAnnotation(MethodParameters.class);
    boolean hasValues = mps != null ? (mps.value().length > 0) : false;
    for (int i = 0; i < params.length; i++) {
      if (i > 0) sb.append(", ");
      if (params[i].equals(String.class)) sb.append("T");
      else if (params[i].isArray() && params[i].getComponentType().equals(String.class))
        sb.append("list<T>");
      else sb.append(fap.javaToSPLType(params[i], null, signedMode));
      if (params[i].isArray() && boundedListMode) sb.append("[N]");
      if (hasValues) sb.append(" " + mps.value()[i]);
    }
    sb.append(")");
    return sb.toString();
  }

  protected final boolean prototypeHasArrayParam() {
    return prototypeHasArrayParam;
  }

  protected final boolean prototypeHasIntTypeParam() {
    return prototypeHasIntTypeParam;
  }

  protected final boolean prototypeHasStringParam() {
    return prototypeHasStringParam;
  }

  protected final boolean prototypeHasStringArrayParam() {
    return prototypeHasStringArrayParam;
  }

  private final void determineFunctionDescription() {
    description = annotation.description();
  }

  private final void determinePrototypeDataTypes() throws Exception {
    Class<?>[] params = method.getParameterTypes();
    for (int i = 0; i < params.length; i++) {
      if (fap.javaMapsToUnsignedSPLType(params[i])) prototypeHasIntTypeParam = true;
      if (params[i].equals(String.class)) prototypeHasStringParam = true;
      if (params[i].isArray()) prototypeHasArrayParam = true;
      if (params[i].isArray() && params[i].getComponentType().equals(String.class))
        prototypeHasStringArrayParam = true;
    }
  }

  private final void determineSPLName() {
    SPLName = annotation.name();
    if (SPLName.equals("")) SPLName = method.getName();
  }
}
