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

package com.ibm.streams.tools.doc.utilities;

import com.ibm.streams.spl.messages.doc.StreamsSPLDOCMessagesKey.Key;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Artifact;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.ArtifactSubType;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.ArtifactType;
import com.ibm.streams.tools.doc.utilities.ToolkitManager.Toolkit;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Map;
import java.util.Set;

/**
 * Handle output of generated documents to file.
 *
 * @author andyfrenkiel
 */
public class FileOutput {

  // General variables
  private static Logger logger = Logger.getInstance();

  // Input file names.
  public static final String ToolkitModelFileName = "toolkit.xml";
  public static final String SourceModelFileName = ".sourceModel.xml";
  public static final String NamespaceDescFileName = "namespace-info.spl";
  public static final String functionModelDirName = "native.function";
  public static final String CppFunctionModelFileName = "function.xml";
  public static final String JavaFunctionModelFileName = "javaFunction.xml";

  // Input file name extensions.
  public static final String SplFileExt = "spl";

  // Name used internally to represent the DITA map file.
  public static final String DitaMapName = "ditamap$";

  // Output file names (without extensions).
  public static final String AllToolkitsFileName = "toolkits";
  public static final String TopIndexFileName = "index";

  // Output file name type prefixes.
  private static final String CppFunctionsFilePrefix = "fc$";
  private static final String IndexFilePrefix = "ix$";
  private static final String JavaFunctionsFilePrefix = "fj$";
  private static final String NamespaceFilePrefix = "ns$";
  private static final String OperatorFilePrefix = "op$";
  private static final String SPLFilePrefix = "spl$";
  private static final String ToolkitFilePrefix = "tk$";

  // Output file name extensions.
  public static final String DefaultFileExt = "xml";
  public static final String DitaFileExt = "xml";
  public static final String DitaMapFileExt = "ditamap";
  public static final String JPEGFileExt = "jpg";
  public static final String SVGFileExt = "svg";

  // Top-level output directory names.
  public static final String DefaultSplDocDirectory = new File("doc", "spldoc").toString();

  // Next-level output directory names.
  public static final String DitaFormat = "dita";
  private static final String TempSubDirectory = "temp";

  // Next-level output directory names.
  private static final String AllToolkitsSubDirectory = "toolkits";
  private static final String SampleToolkitsSubDirectory = "samples";
  private static final String ImageSubDirectory = "image";
  private static final String ReferencesSubDirectory = "references";

  // Next-level output directory names.
  private static final String CopySubDirectory = "copy";

  // SPLDOC output directory.
  private static File spldocDirectory = null;

  // Map each artifact to the number of new child pages it has.
  private static Map<Artifact, Integer> newPageCountMap = new HashMap<Artifact, Integer>();

  // Map each embedded image file to its copy in the output directory.
  private static Map<File, String> imageFileCopyMap = new HashMap<File, String>();

  // Map each copied image file name to the number of times the name has been used.
  private static Map<String, Integer> imageFileNameMap = new HashMap<String, Integer>();

  /** Set the SPLDOC output directory. */
  public static void setOutputDirectory(String directory) {
    spldocDirectory = new File(directory);
  }

  /** Return the source model file for the specified toolkit. */
  public static File getSourceModelFile(Toolkit toolkit) {
    File file = new File(spldocDirectory, TempSubDirectory);
    file = new File(file, new Integer(toolkit.getToolkitNumber()).toString());
    file = new File(file, SourceModelFileName);
    return file;
  }

  /**
   * Return the spldoc output directory for the specified subdirectory. Creates the directory path
   * if it doesn't already exist.
   */
  private static File getDocOutputDir(String subDir) {
    File outputDirFile = new File(spldocDirectory, subDir);
    if (!outputDirFile.exists()) {
      outputDirFile.mkdirs();
    }
    return outputDirFile;
  }

  /** Return the output subdirectory name for a toolkit. */
  private static String getDocSubDirToolkit(Toolkit toolkit) {
    return ToolkitFilePrefix
        + ToolkitManager.getSanitizedToolkitName(toolkit.getToolkitModel().getName());
  }

  /**
   * Return the path name relative to the dita output subdirectory for a dita document for reference
   * material.
   */
  public static String getDitaPathNameReference(String ditaFileName) {
    return new File(ReferencesSubDirectory, ditaFileName).toString();
  }

  /**
   * Return the path name relative to the dita output subdirectory for a generated dita document for
   * all the main or, if isSample is true, all the sample toolkits.
   */
  public static String getDitaPathNameToolkits(String ditaFileName, boolean isSample) {
    return new File((isSample ? SampleToolkitsSubDirectory : AllToolkitsSubDirectory), ditaFileName)
        .toString();
  }

  /**
   * Return the path name relative to the dita output subdirectory for a generated dita document for
   * a toolkit artifact or topic. The toolkit must not be null.
   */
  public static String getDitaPathName(Toolkit toolkit, String ditaFileName) {
    return new File(getDocSubDirToolkit(toolkit), ditaFileName).toString();
  }

  /**
   * Return the path name relative to the dita output subdirectory for a generated dita document for
   * a toolkit artifact or topic. If the toolkit is set to null, the document is for all the main
   * or, if isSample is true, all the sample toolkits.
   */
  public static String getDitaPathName(Toolkit toolkit, String ditaFileName, boolean isSample) {
    return (toolkit == null
        ? getDitaPathNameToolkits(ditaFileName, isSample)
        : getDitaPathName(toolkit, ditaFileName));
  }

  /**
   * Return the output file name (without extension) for a toolkit artifact or index of artifacts.
   */
  public static String getDocFileNameIndex(ArtifactType type) {
    return IndexFilePrefix + type.toString();
  }

  private static String getDocFileNameNamespaceInfix(String namespace) {
    // It seems that DITA does not always generate the HTML file if "$$" is part of the name.
    return (namespace.equals("") ? "." : namespace) + "$";
  }

  public static String getDocFileNameToolkit(Toolkit toolkit) {
    return ToolkitFilePrefix
        + ToolkitManager.getSanitizedToolkitName(toolkit.getToolkitModel().getName());
  }

  public static String getDocFileNameNamespace(String namespace) {
    return NamespaceFilePrefix + namespace;
  }

  public static String getDocFileNameSPLFile(String namespace, String fqSourceFile) {
    String name = new File(fqSourceFile).getName();
    if (name.endsWith("." + SplFileExt)) name = name.substring(0, name.lastIndexOf("."));
    return SPLFilePrefix + getDocFileNameNamespaceInfix(namespace) + name;
  }

  public static String getDocFileNameOp(String namespace, String name) {
    return OperatorFilePrefix + getDocFileNameNamespaceInfix(namespace) + name;
  }

  public static String getDocFileNameFunction(String namespace, String modelFileName) {
    assert (modelFileName.endsWith(CppFunctionModelFileName)
        || modelFileName.endsWith(JavaFunctionModelFileName));
    String prefix =
        modelFileName.endsWith(CppFunctionModelFileName)
            ? CppFunctionsFilePrefix
            : JavaFunctionsFilePrefix;
    return prefix + namespace;
  }

  public static String getDocFileNameArtifact(Artifact artifact) {
    if (artifact.type == ArtifactType.Toolkit) {
      return getDocFileNameToolkit(artifact.toolkit);
    } else if (artifact.type == ArtifactType.Namespace) {
      return getDocFileNameNamespace(artifact.name);
    } else if (artifact.type == ArtifactType.Function) {
      if (artifact.subtype == ArtifactSubType.NonNative) {
        return FileOutput.getDocFileNameSPLFile(artifact.namespace, artifact.uriValue);
      } else {
        return FileOutput.getDocFileNameFunction(artifact.namespace, artifact.uriValue);
      }
    } else if (artifact.type == ArtifactType.FunctionModel) {
      return FileOutput.getDocFileNameFunction(artifact.namespace, artifact.uriValue);
    } else if (artifact.type == ArtifactType.Operator) {
      if (artifact.subtype == ArtifactSubType.Composite) {
        return getDocFileNameSPLFile(artifact.namespace, artifact.uriValue);
      } else {
        return getDocFileNameOp(artifact.namespace, artifact.name);
      }
    } else if (artifact.type == ArtifactType.Type) {
      return getDocFileNameSPLFile(artifact.namespace, artifact.uriValue);
    } else if (artifact.type == ArtifactType.CompilationUnit) {
      return getDocFileNameSPLFile(artifact.namespace, artifact.uriValue);
    } else {
      logger.printError(
          Key.CDISP7124E,
          new String[] {"unexpected toolkit artifact type for a document file: " + artifact.type});
      ErrorManager.throwFatalErrorException();
      return null;
    }
  }

  /**
   * Generate and return the output file name (without extension) for a new child page for the given
   * toolkit artifact.
   */
  public static String generateDocFileNameNewPage(Artifact artifact) {
    Integer count = newPageCountMap.get(artifact);
    if (count == null) {
      // This is the first child page.
      count = new Integer(1);
    } else {
      count++;
    }
    newPageCountMap.put(artifact, count);
    return getDocFileNameArtifact(artifact) + "$" + count;
  }

  /**
   * Return the path name (without extension) relative to the spldoc output directory for a
   * generated image file for a composite or primitive operator.
   */
  public static String getImagePathNameOp(Toolkit toolkit, String namespace, String name) {
    File dir = new File(ImageSubDirectory, getDocSubDirToolkit(toolkit));
    return new File(dir, getDocFileNameOp(namespace, name)).toString();
  }

  /** Place images in "image" subdirectory of the spldoc output directory. */
  private static String getImageOutputFileName(
      Toolkit toolkit, String namespace, String artifactName) {
    File pathRelDoc = new File(getImagePathNameOp(toolkit, namespace, artifactName));
    String subDirs = pathRelDoc.getParentFile().toString();
    File dirPath = getDocOutputDir(subDirs);
    return new File(dirPath, pathRelDoc.getName()).toString();
  }

  public static String getSvgOutputFileName(
      Toolkit toolkit, String namespace, String artifactName) {
    return getImageOutputFileName(toolkit, namespace, artifactName) + "." + SVGFileExt;
  }

  public static String getJpegOutputFileName(
      Toolkit toolkit, String namespace, String artifactName) {
    return getImageOutputFileName(toolkit, namespace, artifactName) + "." + JPEGFileExt;
  }

  /**
   * Return the path relative to the documentation output directory for the copy of an embedded
   * image file of name fileName, where count is used to make the path name unique in case multiple
   * image files with the same name are copied.
   */
  private static String getImageCopyPathName(String fileName, Integer count) {
    return new File(
            new File(ImageSubDirectory, CopySubDirectory), count.toString() + "$" + fileName)
        .toString();
  }

  /**
   * Make a copy of the specified image file in the documentation output directory tree and return a
   * path to the file that is relative to the documentation output directory.
   */
  public static String copyImageFile(File file) {
    String copy = null;
    try {
      file = file.getCanonicalFile();
      copy = imageFileCopyMap.get(file);
      if (copy == null) {
        // This image file has not already been copied.
        // Since we use the file name to construct the name of the copy,
        // keep track of the number of times each name is used
        // so that each copy can have a unique name.
        String fileName = file.getName();
        Integer count = imageFileNameMap.get(fileName);
        if (count == null) {
          // This is the first use of this file name.
          count = new Integer(1);
        } else {
          count++;
        }
        imageFileNameMap.put(fileName, count);
        copy = getImageCopyPathName(fileName, count);
        File fileCopy = new File(spldocDirectory, copy);
        if (fileCopy.exists()) {
          fileCopy.delete();
        }
        if (imageFileCopyMap.isEmpty()) {
          File parent = fileCopy.getParentFile();
          if (!parent.exists()) parent.mkdirs();
        }
        // TODO: use java.nio.Files when we move off Java 6
        // Files.copy(file.toPath(), copyFile.toPath());
        copyFile(file, fileCopy);
        imageFileCopyMap.put(file, copy);
      }
    } catch (IOException e) {
      logger.printException(e);
      ErrorManager.throwFatalErrorException();
    }
    return copy;
  }

  // TODO: use java.nio.Files when we move off Java 6
  private static void copyFile(File fromFile, File toFile) throws IOException {
    FileInputStream inStream = null;
    FileOutputStream outStream = null;
    try {
      inStream = new FileInputStream(fromFile);
      outStream = new FileOutputStream(toFile);
      byte[] buffer = new byte[64 * 1024];
      int n;
      while ((n = inStream.read(buffer)) > 0) {
        outStream.write(buffer, 0, n);
      }
    } catch (IOException e) {
      throw e;
    } finally {
      inStream.close();
      outStream.close();
    }
  }

  /** Return the file name for the specific format. Format is "dita". */
  private static String getFileName(String format, String fileNameNoExt) {
    if (format.equals(DitaFormat)) {
      if (fileNameNoExt.equals(DitaMapName)) {
        return AllToolkitsFileName + "." + DitaMapFileExt;
      } else {
        return fileNameNoExt + "." + DitaFileExt;
      }
    }
    return fileNameNoExt + "." + DefaultFileExt;
  }

  /**
   * Output string buffers containing generated documents to the output directory for the given
   * format.
   */
  public static void write(Map<String, String> docSource, String format) {
    Set<String> fileNameNoExts = docSource.keySet();
    File outputDocDir = getDocOutputDir(format);
    for (String fileNameNoExt : fileNameNoExts) {
      File file;
      try {
        file = new File(outputDocDir.getCanonicalPath(), getFileName(format, fileNameNoExt));
        if (file.exists()) {
          file.delete();
        }
        File parent = file.getParentFile();
        if (!parent.exists()) parent.mkdirs();
        file.createNewFile();
      } catch (IOException e) {
        logger.printException(e);
        ErrorManager.throwFatalErrorException();
        return;
      }
      try {
        PrintWriter writer = new PrintWriter(file);
        writer.println(docSource.get(fileNameNoExt));
        writer.flush();
        writer.close();
      } catch (Exception e) {
        logger.printException(e);
        ErrorManager.throwFatalErrorException();
        return;
      }
    }
  }
}
