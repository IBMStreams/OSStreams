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

package com.ibm.streams.spl.internal.tkindexer;

import com.ibm.streams.operator.internal.model.ShadowClass;
import com.ibm.streams.operator.internal.model.processors.Util;
import com.ibm.streams.spl.internal.tkindexer.function.FunctionAnnotationProcessor;
import com.ibm.streams.spl.internal.tkindexer.operator.OperatorAnnotationProcessor;
import java.io.File;
import java.io.FilenameFilter;
import java.net.URL;
import java.net.URLClassLoader;
import java.text.MessageFormat;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;

/**
 * Toolkit indexer Java annotation processor for annotated Java native functions and operators. This
 * processor generates files for the SPL compilation step based on the annotations.
 *
 * <p>Prior to this step, annotation verifiers are run as part of the Java compilation step. These
 * verifiers verify the annotations and generate shadow class files. The shadow class files contain
 * the information needed by this processor that has been extracted from the original source files.
 *
 * <p>This processor will normally be run by spl-make-toolkit.
 *
 * <p>The undocumented "-b" command option is used for source tree builds, indicating that the
 * toolkit may become part of the streams installation. (This information is no longer used, so this
 * command option is obsolete.)
 */
public final class ToolkitAnnotationProcessor {

  /* toolkit annotation processors */
  private static final AbstractAnnotationProcessor[] ANNOTATION_PROCESSORS = {
    new FunctionAnnotationProcessor(), new OperatorAnnotationProcessor()
  };

  /* look for Java classes in these subdirs of toolkit root dir */
  private static final String[] CLASS_DIRS_TO_SEARCH = {"impl/java/bin"};
  private static final String[] JAR_DIRS_TO_SEARCH = {"impl/lib", "lib"};

  /*
   * Strings used to match class and jar file names.
   * Exclude streams jar files that are samples or do not have annotations.
   */
  private static final String CLASS_FILE_SUFFIX = ".class";
  private static final String JAR_FILE_MATCH = ".*\\.(jar|JAR)$";
  private static final String JAR_FILE_EXCLUDE = "com\\.ibm\\.streams\\.operator\\..*";

  /* other fields */
  private static File toolkitRootDir;
  private static String toolkitName;
  private static String toolkitVersion;
  private static boolean isSourceTreeBuild = false; // no longer used
  private static boolean verbose = false;
  private static int errorCount = 0;
  private static HashMap<String, String> classesProcessed = new HashMap<String, String>();

  /** Main method. */
  public static void main(String[] args) throws Exception {

    /* process arguments */
    int offset;
    optionsLoop:
    for (offset = 0; offset < args.length; offset++) {
      switch (args[offset].charAt(0)) {
        case '-':
          if (args[offset].equals("-b")) isSourceTreeBuild = true;
          else if (args[offset].equals("-v") || args[offset].equals("--verbose")) verbose = true;
          else usageExit("Invalid option.");
          break;
        default:
          break optionsLoop;
      }
    }
    if (args.length < offset + 3) usageExit("Too few arguments.");
    if (args.length > offset + 3) usageExit("Too many arguments.");
    toolkitRootDir = new File(args[offset]);
    if (!toolkitRootDir.isDirectory())
      usageExit(
          MessageFormat.format("''{0}'' is not a directory.", toolkitRootDir.getCanonicalPath()));
    toolkitName = args[offset + 1];
    toolkitVersion = args[offset + 2];

    /* process annotations in jar and class files */
    try {
      initialize();
      processClassFiles();
      processJarFiles();
      complete();
    } catch (Exception e) {
      handle(e);
    } finally {
      cleanUpAndExit();
    }
  }

  /** Look for jar files and process the classes in them. */
  private static void processJarFiles() throws Exception {
    for (String path : JAR_DIRS_TO_SEARCH) {
      File dir = new File(toolkitRootDir, path);
      if (!dir.isDirectory()) continue;
      File[] files =
          dir.listFiles(
              new FilenameFilter() {
                public boolean accept(File dir, String name) {
                  return name.matches(JAR_FILE_MATCH) && !name.matches(JAR_FILE_EXCLUDE);
                }
              });
      for (File file : files) {
        JarFile jf = new JarFile(file);
        URLClassLoader loader = new URLClassLoader(new URL[] {file.toURI().toURL()});
        Enumeration<JarEntry> e = jf.entries();
        while (e.hasMoreElements()) {
          JarEntry je = e.nextElement();
          String en = je.getName();
          if (!en.endsWith(CLASS_FILE_SUFFIX)) continue;
          processClass(
              loader, pathToClassName(en), getToolkitRelPath(file), getToolkitRelPath(file));
        }
        jf.close();
      }
    }
  }

  /** Look for class files and process the classes in them. */
  private static void processClassFiles() throws Exception {
    for (String path : CLASS_DIRS_TO_SEARCH) {
      File dir = new File(toolkitRootDir, path);
      if (!dir.isDirectory()) continue;
      URLClassLoader loader = new URLClassLoader(new URL[] {dir.toURI().toURL()});
      processClassFilesUnderDir(loader, dir, dir);
    }
  }

  /** Find and process class files within a directory tree. */
  private static void processClassFilesUnderDir(URLClassLoader loader, File clsRootDir, File curDir)
      throws Exception {
    String[] fileNames = curDir.list();
    for (String fileName : fileNames) {
      File file = new File(curDir, fileName);
      if (file.isDirectory()) {
        processClassFilesUnderDir(loader, clsRootDir, file);
        continue;
      }
      if (!fileName.endsWith(CLASS_FILE_SUFFIX)) continue;
      processClass(
          loader,
          pathToClassName(getRelPath(clsRootDir, file)),
          getToolkitRelPath(clsRootDir),
          getToolkitRelPath(file));
    }
  }

  /** Initialise annotation processors. */
  private static void initialize() throws Exception {
    for (AbstractAnnotationProcessor ap : ANNOTATION_PROCESSORS) {
      ap.setToolkitRootDir(toolkitRootDir);
      ap.setToolkitName(toolkitName);
      ap.setToolkitVersion(toolkitVersion);
      ap.initialize();
    }
  }

  /** Call all annotation processors to process the shadow classes. */
  private static void processClass(
      URLClassLoader loader,
      String className,
      String classpathRelTkDir,
      String classContainerRelPath)
      throws Exception {
    if (!className.endsWith(Util.GENERATED_CLASS_SUFFIX)) return;
    Class<?> cls = loader.loadClass(className);
    String classOrigName = cls.getAnnotation(ShadowClass.class).value();
    if (verbose)
      System.out.println(
          MessageFormat.format(
              "Processing class {0} in file {1}.",
              classOrigName, new File(toolkitRootDir, classContainerRelPath)));
    String classErrorContext =
        MessageFormat.format("in file {0}: class {1}", classContainerRelPath, classOrigName);
    if (classesProcessed.containsKey(classOrigName)) {
      System.err.println(
          MessageFormat.format(
              "Class {0} in file {1} was skipped: a class with "
                  + "the same name in file {2} was already processed.",
              classOrigName, classContainerRelPath, classesProcessed.get(classOrigName)));
      return;
    }
    classesProcessed.put(classOrigName, classContainerRelPath);
    for (AbstractAnnotationProcessor ap : ANNOTATION_PROCESSORS) {
      ap.setClassOrigName(classOrigName);
      ap.setClassContainerRelPath(classContainerRelPath);
      ap.setClassErrorContext(classErrorContext);
      ap.setClassRelClasspath(classpathRelTkDir);
      ap.process(cls);
    }
  }

  /** Call all annotation processors to complete their processing. */
  private static void complete() throws Exception {
    for (AbstractAnnotationProcessor ap : ANNOTATION_PROCESSORS) {
      ap.complete();
    }
  }

  /** Check for errors, call processors to clean up if needed, and exit. */
  private static void cleanUpAndExit() {
    for (AbstractAnnotationProcessor ap : ANNOTATION_PROCESSORS) {
      errorCount += ap.numErrors();
    }
    if (errorCount > 0) {
      for (AbstractAnnotationProcessor ap : ANNOTATION_PROCESSORS) {
        ap.cleanOnError();
      }
      System.err.println(MessageFormat.format("{0} error(s) encountered.", errorCount));
    }
    if (errorCount > 0) System.exit(1);
  }

  /** Get pathname of file relative to a higher level directory. */
  private static String getRelPath(File dir, File file) throws Exception {
    String path = file.getCanonicalPath();
    return path.substring(dir.getCanonicalPath().length() + 1, path.length());
  }

  /** Get pathname relative to toolkit root directory. */
  private static String getToolkitRelPath(File file) throws Exception {
    return getRelPath(toolkitRootDir, file);
  }

  /** Convert class file relative pathname to a fully qualified class name. */
  private static String pathToClassName(String path) {
    String name = path.substring(0, path.length() - CLASS_FILE_SUFFIX.length());
    return name.replace(File.separatorChar, '.');
  }

  /** Print usage message and exit. */
  private static void usageExit(String message) {
    System.err.println(message);
    System.err.println(
        "Usage: java "
            + ToolkitAnnotationProcessor.class.getName()
            + " [-v | --verbose]"
            + " <toolkit root directory> <toolkit name> <toolkit version>");
    System.exit(1);
  }

  /** Print error message and exit. */
  private static void errorExit(String message) {
    System.err.println(message);
    System.exit(1);
  }

  /** Print details of thrown error or exception. */
  private static void handle(Throwable e) {
    System.err.println(ToolkitAnnotationProcessor.class.getName() + ": Internal error.");
    e.printStackTrace();
    errorCount++;
  }
}
