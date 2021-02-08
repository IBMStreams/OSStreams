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

package com.ibm.streams.operator.internal.model.processors;

import java.util.HashSet;
import java.util.Set;
import java.util.regex.Pattern;
import javax.annotation.processing.ProcessingEnvironment;
import javax.lang.model.element.Name;
import javax.lang.model.element.PackageElement;
import javax.lang.model.element.TypeElement;

public class Util {

  public static final String GENERATED_CLASS_SUFFIX = "$StreamsModel";

  private static final String[] _keywords = {
    "and",
    "and_eq",
    "asm",
    "auto",
    "bitand",
    "bitor",
    "bool",
    "case",
    "catch",
    "char",
    "class",
    "compl",
    "const",
    "const_cast",
    "default",
    "delete",
    "do",
    "double",
    "dynamic_cast",
    "enum",
    "explicit",
    "export",
    "extern",
    "float",
    "friend",
    "goto",
    "inline",
    "int",
    "long",
    "namespace",
    "new",
    "not",
    "not_eq",
    "operator",
    "or",
    "or_eq",
    "private",
    "protected",
    "public",
    "register",
    "reinterpret_cast",
    "short",
    "signed",
    "sizeof",
    "static",
    "static_cast",
    "struct",
    "switch",
    "template",
    "this",
    "throw",
    "try",
    "typedef",
    "typeid",
    "typename",
    "union",
    "unsigned",
    "using",
    "virtual",
    "void",
    "volatile",
    "wchar_t",
    "while",
    "xor",
    "xor_eq",
  };

  // Picked up from the SPL grammar definition.
  private static final String[] _splKeywords = {
    "as",
    "attribute",
    "blob",
    "boolean",
    "break",
    "complex128",
    "complex32",
    "complex64",
    "composite",
    "config",
    "continue",
    "decimal128",
    "decimal32",
    "decimal64",
    "else",
    "enum",
    "expression",
    "false",
    "float128",
    "float32",
    "float64",
    "for",
    "function",
    "graph",
    "if",
    "in",
    "input",
    "int16",
    "int32",
    "int64",
    "int8",
    "list",
    "logic",
    "map",
    "matrix",
    "mutable",
    "namespace",
    "onPunct",
    "onTuple",
    "operator",
    "output",
    "param",
    "public",
    "return",
    "set",
    "stateful",
    "static",
    "stream",
    "rstring",
    "ustring",
    "timestamp",
    "true",
    "tuple",
    "type",
    "uint16",
    "uint32",
    "uint64",
    "uint8",
    "use",
    "var",
    "void",
    "while",
    "window",
    "xml"
  };

  private static final String[] _reservedDirs = {
    "lib", "include", "samples", "output", "data", "etc", "doc", "opt", "bin", "impl", "license"
  };

  private static final Set<String> KEYWORDS = new HashSet<String>();
  private static final Set<String> RESERVED_DIR = new HashSet<String>();

  static {
    for (String keyword : _keywords) KEYWORDS.add(keyword);
    for (String keyword : _splKeywords) KEYWORDS.add(keyword);
    for (String dir : _reservedDirs) RESERVED_DIR.add(dir);
  }

  public static boolean isSPLIdentifier(String s) {
    return Pattern.matches("^([a-zA-Z_]\\w*)$", s);
  }

  public static boolean isRestrictedKeyword(String s) {
    return KEYWORDS.contains(s);
  }

  public static boolean isReservedDir(String s) {
    return RESERVED_DIR.contains(s);
  }

  public static String getBinarySimpleName(
      ProcessingEnvironment processingEnv,
      TypeElement classElement,
      PackageElement packageElement) {
    Name binaryName = processingEnv.getElementUtils().getBinaryName(classElement);
    Name packageName = packageElement.getQualifiedName();

    int packageLength = packageName.length();
    return binaryName.toString().substring(packageLength > 0 ? packageLength + 1 : 0);
  }
}
