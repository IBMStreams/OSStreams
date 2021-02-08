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

/*
 * JavaEGen ant task definition - for use in Distillery build.xml files.
 *
 * <p>Java Exception Generator - extracts exceptions from C++ code and creates the equivalent java
 * class.
 *
 * <p>srcPath The root directory for source C++ files that should be considered as templates for
 * java class generation. All .h files in the entire subtree will be considered. All files
 * containing the key DECL_EXCEPTION will be processed. Required, unless a nested <fileset> is
 * specified.
 *
 * <p>genPath The root directory where the generated java class files should be placed. Required.
 *
 * <p>genPackage The java package that should be used for the generated java classes. Optional. If
 * no package is specified, the task will attempt to use one of the pre-defined java class packages
 * based on the C++ Exception namespace. If none can be found, the task will fail.
 *
 * <p>clean If set, the generated files corresponding to the source set of files will be removed
 * prior to generating the files again. If not set, only the files that are new or have changed
 * since the last time will be created or updated.
 *
 * <p>failonerror If set, any problem encountered by the task will cause an ant failure. NOTE: Some
 * errors will cause a task failure regardless of the setting on this parameter.
 *
 * <p>verbose If set, prints extra information during processing.
 *
 * <p>The task also supports specifying a fileset (vs. a srcPath).
 *
 * <p>Examples:
 *
 * <p><taskdef name="javaegen" classname="JavaEGen" classpathref="${TOOLS}/lib/*.jar"/> <javaegen
 * srcPath="${VIEWROOT}/src/cpp/UTILS" genPath="test_gen" genPackage="com.ibm.distillery.utils.exc"
 * failonerror="true" verbose="true"/>
 */
package com.ibm.streams.build.exceptiongenerator;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.CharBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.Charset;
import java.nio.charset.CharsetDecoder;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Vector;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import org.apache.tools.ant.BuildException;
import org.apache.tools.ant.Task;
import org.apache.tools.ant.taskdefs.UpToDate;
import org.apache.tools.ant.types.FileSet;

public class JavaEGen extends Task {

  private static final Pattern linePattern = Pattern.compile(".*\r?\n");
  private static final Pattern hfilePattern = Pattern.compile(".*\\.h$");
  private static final Pattern DECL_EXCEPTIONPattern = Pattern.compile("DECL_EXCEPTION");
  private static final Pattern DECL_EXCEPTIONExcludePattern =
      Pattern.compile("\\s*(#ifdef|#ifndef|#define|\\/\\/)");
  private static final Pattern DECL_EXCEPTIONLinePattern =
      Pattern.compile(
          "\\s*DECL_EXCEPTION\\s*\\(\\s*([A-z0-9_]*)\\s*,\\s*([A-z0-9_]*)\\s*,\\s*([A-z0-9:_]*).*");
  private static final Pattern DECL_EXCEPTIONParentPattern =
      Pattern.compile("([A-z0-9]*)::([A-z0-9]*)");
  private static final Charset charset;
  private static final CharsetDecoder decoder;
  private static final Hashtable<String, String> NameSpaceHash;
  private String srcPath;
  private String genPath;
  private String genPackage;
  private boolean clean;
  private boolean failonerror;
  private boolean verbose;
  private final Vector<FileSet> filesets;

  static {
    charset = Charset.forName("ISO-8859-15");
    decoder = charset.newDecoder();
    NameSpaceHash = new Hashtable<>();
    NameSpaceHash.put("SAM_NAMESPACE", "com.ibm.streams.sam.exc");
    NameSpaceHash.put("SPC_NAMESPACE", "com.ibm.distillery.spc.exc");
    NameSpaceHash.put("SPC", "com.ibm.distillery.spc.exc");
    NameSpaceHash.put("UTILS_NAMESPACE", "com.ibm.distillery.utils.exc");
    NameSpaceHash.put("Distillery", "com.ibm.distillery.utils.exc");
    NameSpaceHash.put("NAM_NAMESPACE", "com.ibm.distillery.nam.exc");
    NameSpaceHash.put("NAM", "com.ibm.distillery.nam.exc");
    NameSpaceHash.put("TRC_NAMESPACE", "com.ibm.distillery.trc.exc");
    NameSpaceHash.put("SRM_NAMESPACE", "com.ibm.streams.srm.exc");
    NameSpaceHash.put("SRM", "com.ibm.streams.srm.exc");
    NameSpaceHash.put("SEC", "com.ibm.streams.sec.exc");
    NameSpaceHash.put("SCH", "com.ibm.streams.sch.exc");
    NameSpaceHash.put("SCH_NAMESPACE", "com.ibm.streams.sch.exc");
    NameSpaceHash.put("StreamsSim", "com.ibm.streams.sam.sim.exc");
    NameSpaceHash.put("SAMSIM_NAMESPACE", "com.ibm.streams.sam.sim.exc");
    NameSpaceHash.put("SPL", "com.ibm.streams.spl.internal.exc");
    NameSpaceHash.put("HC_NAMESPACE", "com.ibm.streams.hc.exc");
    NameSpaceHash.put("PEC_NAMESPACE", "com.ibm.streams.pec.exc");
  }

  public JavaEGen() {
    clean = false;
    failonerror = true;
    verbose = false;
    filesets = new Vector<>();
  }

  /*
   * Setter methods for ant task parameters - each method that starts with "set"
   * defines a parameter to the ant task
   */

  @SuppressWarnings("unused")
  public void setSrcPath(String srcPath) {
    this.srcPath = srcPath;
  }

  @SuppressWarnings("unused")
  public void setGenPath(String genPath) {
    this.genPath = genPath;
  }

  @SuppressWarnings("unused")
  public void setGenPackage(String genPackage) {
    this.genPackage = genPackage;
  }

  @SuppressWarnings("unused")
  public void setClean(boolean clean) {
    this.clean = clean;
  }

  @SuppressWarnings("unused")
  public void setFailonerror(boolean failonerror) {
    this.failonerror = failonerror;
  }

  @SuppressWarnings("unused")
  public void setVerbose(boolean verbose) {
    this.verbose = verbose;
  }

  @SuppressWarnings("unused")
  public void addFileset(FileSet fileset) {
    filesets.add(fileset);
  }

  /* Task interface overridden methods */
  public void init() {}

  protected void validate() {
    if (srcPath == null && filesets.size() < 1)
      throw new BuildException("Either srcPath parameter or nested fileset must be specified.");
    if (genPath == null) throw new BuildException("genPath parameter must be specified.");
  }

  public void execute() throws BuildException {
    if (verbose) {
      log("JavaEGen Execute");
      log(
          "Parameters:\n\n srcPath = "
              + srcPath
              + "\n genPath = "
              + genPath
              + "\n genPackage = "
              + genPackage
              + "\n clean = "
              + clean
              + "\n failonerror = "
              + failonerror
              + "\n verbose = "
              + verbose);
    }
    validate();
    if (verbose) log("JavaEGen Execute - determining qualified srcPath and genPath.");
    if (srcPath != null) {
      File srcPathRootDir = new File(srcPath);
      try {
        srcPath = srcPathRootDir.getCanonicalPath();
      } catch (IOException ioe) {
        throw new BuildException(ioe);
      }
      FileSet srcFileSet = new FileSet();
      srcFileSet.setDir(srcPathRootDir);
      srcFileSet.setIncludes("**/*.h");
      filesets.add(srcFileSet);
    }
    File genPathRootDir = new File(genPath);
    try {
      genPath = genPathRootDir.getCanonicalPath();
    } catch (IOException ioe) {
      throw new BuildException(ioe);
    }

    for (FileSet fs : filesets) {
      File sourceDir = fs.getDir(getProject());
      String[] includedFiles = fs.getDirectoryScanner(getProject()).getIncludedFiles();
      int i = 0;
      while (i < includedFiles.length) {
        String sourceFileName = includedFiles[i].replace('\\', '/');
        File sourceFile = new File(sourceDir, sourceFileName);
        Matcher m = hfilePattern.matcher(sourceFileName);
        if (!m.matches()) {
          if (verbose) log("JavaEGen Execute - skipping non-hfile " + sourceFile.getAbsolutePath());
        } else {
          if (verbose) log("JavaEGen Execute - source file is " + sourceFile.getAbsolutePath());
          ArrayList<CharSequence> declExceptions;
          try {
            declExceptions = grep(sourceFile, DECL_EXCEPTIONPattern);
          } catch (IOException ioe) {
            log("Error reading file " + sourceFileName + ": " + ioe.toString());
            throw new BuildException(ioe);
          }
          for (Object declException : declExceptions) {
            CharSequence line = (CharSequence) declException;
            if (verbose) log("Parsing matching line " + line);
            Matcher em = DECL_EXCEPTIONExcludePattern.matcher(line);
            if (em.find()) {
              if (verbose) {
                log(
                    "Excluding line: "
                        + line
                        + " because it starts with ifdef, ifndef, define, or is a comment.\n");
              }
              continue;
            }
            Matcher lm = DECL_EXCEPTIONLinePattern.matcher(line);
            if (!lm.find()) {
              log("Line:\n " + line + "\ndoes not match DECL_EXCEPTION line pattern.");
              log("Pattern is " + DECL_EXCEPTIONLinePattern.pattern());
              continue;
            }
            String exceptionNameSpace = lm.group(1);
            String exceptionName = lm.group(2);
            String exceptionParent = lm.group(3);
            if (verbose)
              log(
                  "JavaEGen Exception is --> "
                      + exceptionNameSpace
                      + "::"
                      + exceptionName
                      + " whose parent is "
                      + exceptionParent
                      + " (defined in "
                      + sourceFileName
                      + ")");
            exceptionName = exceptionName + "Exception";
            exceptionParent = exceptionParent + "Exception";
            String targetFileName = exceptionName + ".java";
            String targetPackage = genPackage;
            if (targetPackage == null) {
              if (!NameSpaceHash.containsKey(exceptionNameSpace))
                throw new BuildException(
                    "ERROR: package name and exception namespace are not defined (you must define one of them)");
              targetPackage = NameSpaceHash.get(exceptionNameSpace);
            }
            String targetFileDir = genPath + "/" + targetPackage.replace('.', '/');
            File targetFile = new File(targetFileDir + "/" + targetFileName);
            if (verbose) {
              log("JavaEGen Execute - target package is " + targetPackage);
              log("JavaEGen Execute - target file is " + targetFile);
            }
            if (clean) {
              log("Removing file " + targetFile);
              targetFile.delete();
              continue;
            }
            UpToDate uptodate = new UpToDate();
            uptodate.setSrcfile(sourceFile);
            uptodate.setTargetFile(targetFile);
            boolean isuptodate = uptodate.eval();
            if (isuptodate) {
              if (verbose) {
                log(
                    "JavaEGen Exception file "
                        + targetFile
                        + " is up-to-date with the source hfile "
                        + sourceFile
                        + ".");
              }
              continue;
            }
            if (verbose) log("Generating java exception file " + targetFile);
            try {
              targetFile.getParentFile().mkdirs();
              BufferedWriter out = new BufferedWriter(new FileWriter(targetFile));

              /*
               * TODO: write here the Apache license, maybe.
               */

              out.write("package " + targetPackage + ";");
              out.newLine();
              String parentPackage = targetPackage;
              Matcher pm = DECL_EXCEPTIONParentPattern.matcher(exceptionParent);
              if (pm.find()) {
                if (verbose)
                  log(
                      "JavaEGen Found extended parent package definition in declaration, translating.");
                String newpostfix = pm.group(1);
                newpostfix = newpostfix.replaceAll("UTILS_NAMESPACE", "utils");
                exceptionParent = pm.group(2);
                parentPackage = parentPackage.replace("\\.exc$", "");
                parentPackage = parentPackage.replace("\\.[A-Za-z0-9]*$", "." + newpostfix);
                if (verbose)
                  log(
                      "Parent package in this case is "
                          + parentPackage
                          + "(rather than "
                          + targetPackage
                          + ") (originally the C++ name space is "
                          + exceptionNameSpace
                          + ", exception is "
                          + exceptionName
                          + ", and parent is "
                          + exceptionParent
                          + ")");
              }
              out.newLine();
              out.write("import java.util.Vector;");
              out.newLine();
              out.write("import com.ibm.distillery.utils.DistilleryExceptionCode;");
              out.newLine();
              out.newLine();
              out.write("public class " + exceptionName);
              out.newLine();
              if (exceptionParent.equals("DistilleryException"))
                out.write("  extends com.ibm.distillery.utils.DistilleryException {");
              else out.write("  extends " + parentPackage + "." + exceptionParent + " {");
              out.newLine();
              out.newLine();
              out.write("  public static final long serialVersionUID = 1;");
              out.newLine();
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write("  * Construct a new exception with the specified detail message");
              out.newLine();
              out.write("  * @param message the exception related message");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write("  public " + exceptionName + "(String message) {");
              out.newLine();
              out.write("    this(message, new DistilleryExceptionCode(), new Vector<String>());");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write(
                  "  * Construct a new exception with the specified detail message, exception code and substitution text");
              out.newLine();
              out.write("  * @param message the exception related message");
              out.newLine();
              out.write(
                  "  * @param myExceptionCode the exception code that represents the exception");
              out.newLine();
              out.write(
                  "  * @param mySubstitutionText A vector of strings representing the substitution text related to the exception code");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write(
                  "  public "
                      + exceptionName
                      + "(String message, DistilleryExceptionCode myExceptionCode, Vector<String> mySubstitutionText) {");
              out.newLine();
              out.write("    super(message, myExceptionCode, mySubstitutionText);");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write(
                  "  * Constructs a new exception with the specified detail message and cause");
              out.newLine();
              out.write("  * @param message the exception related message");
              out.newLine();
              out.write("  * @param cause the exception cause");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write("  public " + exceptionName + "(String message, Throwable cause) {");
              out.newLine();
              out.write(
                  "    this(message, cause, new DistilleryExceptionCode(), new Vector<String>());");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.write("");
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write(
                  "  * Constructs a new exception with the specified detail message, cause, exception code and substitution text");
              out.newLine();
              out.write("  * @param message the exception related message");
              out.newLine();
              out.write("  * @param cause the exception cause");
              out.newLine();
              out.write(
                  "  * @param myExceptionCode the exception code that represents the exception");
              out.newLine();
              out.write(
                  "  * @param mySubstitutionText A vector of strings representing the substitution text related to the exception code");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write(
                  "  public "
                      + exceptionName
                      + "(String message, Throwable cause, DistilleryExceptionCode myExceptionCode, Vector<String> mySubstitutionText) {");
              out.newLine();
              out.write("    super(message, cause, myExceptionCode, mySubstitutionText);");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write(
                  "  * Construct a new exception with the specified cause and a detail message ");
              out.newLine();
              out.write(
                  "  * of (cause==null ? null : cause.toString()) (which typically contains ");
              out.newLine();
              out.write("  * the class and detail message of cause)");
              out.newLine();
              out.write("  * @param cause the exception cause");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write("  public " + exceptionName + "(Throwable cause) {");
              out.newLine();
              out.write("    this(cause, new DistilleryExceptionCode(), new Vector<String>());");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.newLine();
              out.write(" /**");
              out.newLine();
              out.write(
                  "  * Construct a new exception with the specified cause, a detail message, exception code and substitution text");
              out.newLine();
              out.write(
                  "  * of (cause==null ? null : cause.toString()) (which typically contains ");
              out.newLine();
              out.write("  * the class and detail message of cause)");
              out.newLine();
              out.write("  * @param cause the exception cause");
              out.newLine();
              out.write(
                  "  * @param myExceptionCode the exception code that represents the exception");
              out.newLine();
              out.write(
                  "  * @param mySubstitutionText A vector of strings representing the substitution text related to the exception code");
              out.newLine();
              out.write("  **/");
              out.newLine();
              out.write(
                  "  public "
                      + exceptionName
                      + "(Throwable cause, DistilleryExceptionCode myExceptionCode, Vector<String> mySubstitutionText) {");
              out.newLine();
              out.write("    super(cause, myExceptionCode, mySubstitutionText);");
              out.newLine();
              out.write("  }");
              out.newLine();
              out.newLine();
              out.write("}");
              out.newLine();
              out.close();
            } catch (IOException ioe) {
              throw new BuildException(ioe);
            }
            log("Created exception file " + targetFileName + ".");
          }
        }
        i++;
      }
    }
  }

  private static ArrayList<CharSequence> grep(CharBuffer cb, Pattern p) {
    ArrayList<CharSequence> matches = new ArrayList<>();
    Matcher lm = linePattern.matcher(cb);
    Matcher pm = null;
    do {
      if (!lm.find()) break;
      CharSequence cs = lm.group();
      if (pm == null) pm = p.matcher(cs);
      else pm.reset(cs);
      if (pm.find()) matches.add(cs);
    } while (lm.end() != cb.limit());
    return matches;
  }

  private static ArrayList<CharSequence> grep(File f, Pattern p) throws IOException {
    FileInputStream fis = new FileInputStream(f);
    FileChannel fc = fis.getChannel();
    int sz = (int) fc.size();
    java.nio.MappedByteBuffer bb = fc.map(java.nio.channels.FileChannel.MapMode.READ_ONLY, 0L, sz);
    CharBuffer cb = decoder.decode(bb);
    ArrayList<CharSequence> matches = grep(cb, p);
    fc.close();
    return matches;
  }
}
