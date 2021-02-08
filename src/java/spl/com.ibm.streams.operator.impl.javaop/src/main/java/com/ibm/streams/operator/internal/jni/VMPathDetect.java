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

package com.ibm.streams.operator.internal.jni;

import java.io.File;

/**
 * Helper class for the JavaOp that determines the library paths for the Java Virtual Machine for
 * the JNI invocation API (creating the JVM from C++).
 */
public class VMPathDetect {

  /** Enumeration of VM types. */
  enum VMType {
    /** IBM's SDK (J9). */
    IBM {

      @Override
      String getVMType() {
        return "j9vm";
      }
    },

    /** Sun's (now Oracle's) original Hotspot Server VM. */
    ORACLE_SUN {

      @Override
      String getVMType() {
        return "server";
      }
    },

    /** Unknown VM. */
    UNKNOWN {
      @Override
      String getVMType() {
        return "";
      }
    };

    /**
     * Get the vmType which is typically used as the name of the directory that contains the 'jvm'
     * library. For IBM's and ORACLE_SUN VMs it defines the "style" of the JVM.
     */
    abstract String getVMType();

    /**
     * From the current environment determine the type of the VM.
     *
     * @param vpmd VMPathDetect object containing system information.
     */
    static VMType whichVM(VMPathDetect vmpd) {
      if (vmpd.javaVendor.startsWith("IBM")) return IBM;

      if (vmpd.javaVendor.startsWith("Sun ") || vmpd.javaVendor.startsWith("Oracle "))
        return ORACLE_SUN;

      return UNKNOWN;
    }
  }

  /** Typically lead in for the jvm lib path relative to java.home for the JRE. */
  private static final String JRE_LIB = "/lib".replace('/', File.separatorChar);

  /** Enumeration of commands requested by the SPL compiler. */
  enum Command {
    /** Library definitions. Nothing to do, 'jvm' is defined in the operator model XML. */
    lib,

    /** Library path for libjvm.so */
    libPath {

      @Override
      void emit(VMPathDetect vmpd) {
        vmpd.emitLibraryPaths();
      }
    },
    /**
     * Include path definitions. Nothing to do, $JAVA_HOME/include/jni.h is defined in the operator
     * model XML.
     */
    includePath {
      @Override
      void emit(VMPathDetect vmpd) {
        vmpd.emitIncludePaths();
      }
    };

    /**
     * Emit lines for this JVM for the given command. This implementation does nothing, specific
     * enumeration values can override.
     */
    void emit(VMPathDetect vmpd) {}
  }

  /** Value of system property java.home. */
  final String javaHome;
  /** Value of system property java.vendor. */
  final String javaVendor;

  /** The VMType enumeration type we have identified. */
  final VMType vmType;

  /** Mapped name of the 'jvm' library. */
  final String libraryName;

  /**
   * Utility to get the paths for the JavaOp. Expects one of three arguments lib, includePath and
   * libPath which correspond to the Command enums.
   *
   * @param args
   */
  public static void main(String[] args) {

    VMPathDetect vmpd = new VMPathDetect();

    Command.valueOf(args[0]).emit(vmpd);
  }

  private VMPathDetect() {
    javaHome = getSystemProperty("java.home");
    javaVendor = getSystemProperty("java.vendor");
    vmType = VMType.whichVM(this);
    System.out.println("# VMType=" + vmType);
    libraryName = System.mapLibraryName("jvm");
    System.out.println("# Mapped library: " + libraryName);
  }

  /**
   * Read a system property and report it as a comment in the output for debugging.
   *
   * @param name System property name.
   * @return Value of the property.
   */
  private static String getSystemProperty(String name) {
    String value = System.getProperty(name);
    System.out.println("# " + name + "=" + value);
    return value;
  }

  /**
   * Look for a path of the form:
   *
   * <p>$JAVA_HOME/lib/?arch?/vmtype/jvm_lib
   *
   * <p>where ?arch? is the unknown portion and jvm_lib is the platform specific mapped name of the
   * 'jvm' library
   */
  private void emitLibjvmPath() {
    File base = new File(javaHome + JRE_LIB);

    File[] contents = base.listFiles();

    if (contents == null) return;

    for (File archDir : contents) {
      if (!archDir.isDirectory()) continue;

      File libPath = new File(archDir, vmType.getVMType());
      if (libPath.isDirectory()) {
        File lib = new File(libPath, libraryName);
        if (lib.exists()) System.out.println(libPath.getAbsolutePath());
      }
    }
  }

  /**
   * Emit library paths required for the JavaOp and its invocation of the JVM through the JNI
   * invocation api.
   */
  protected void emitLibraryPaths() {
    // Currently just the 'jvm' library.
    emitLibjvmPath();
  }

  /**
   * Emit the include paths required. $JAVA_HOME/include is specified by the operator model. This
   * adds any additional include paths specific to the virtual machine.
   */
  protected void emitIncludePaths() {
    // Conflict in definition of jboolean requires the inclusion
    // of $JAVA_HOME/include/linux on Linux systems.
    // http://gcc.gnu.org/bugzilla/show_bug.cgi?id=35089
    if (System.getProperty("os.name").startsWith("Linux")) {
      // Note java.home property points to the JRE location not the JDK.
      System.out.println(System.getenv("JAVA_HOME") + "/include/linux");
    }
  }
}
