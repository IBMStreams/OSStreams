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

package com.ibm.streams.spl.internal.application.bundleLoader.v4000;

import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.spl.internal.application.bundleLoader.BundleLoader;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.BundleInfoModelType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.FileType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.OutputType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.ToolkitType;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.LinkOption;
import java.nio.file.Path;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import javax.xml.bind.JAXBContext;
import javax.xml.bind.JAXBElement;
import javax.xml.bind.JAXBException;
import javax.xml.bind.Marshaller;
import javax.xml.bind.Unmarshaller;
import javax.xml.namespace.QName;
import org.apache.commons.compress.archivers.ArchiveException;
import org.apache.commons.compress.archivers.ArchiveStreamFactory;
import org.apache.commons.compress.archivers.tar.TarArchiveEntry;
import org.apache.commons.compress.archivers.tar.TarArchiveInputStream;
import org.apache.commons.io.IOUtils;

public class BundleLoaderImpl implements BundleLoader {

  private Class _class;
  private ShutdownHandler _shutdownHandler = null;
  private Path _tempDir = null;
  private BundleInfoModelType _bi;
  private Process _p = null;

  public void shutdown() {
    if (null != _p) {
      _p.destroy();
      _p = null;
    }
    if (null != _tempDir) {
      if (_tempDir.toFile().exists()) {
        // Delete the temporary directory and all its contents
        try {
          String[] command = {"/bin/rm", "-fr", _tempDir.toString()};
          ProcessBuilder pb = new ProcessBuilder(command);
          Process p = pb.start();
          int status = p.waitFor();
          if (status != 0) {
            System.err.println("Failed to fully remove temporary directory " + _tempDir.toString());
          }
        } catch (Exception e) {
          System.err.println("Failed to fully remove temporary directory " + _tempDir.toString());
        }
      }
    }
  }

  class ShutdownHandler extends Thread {
    private BundleLoaderImpl _bl = null;
    private Thread _th = null;

    public ShutdownHandler(BundleLoaderImpl bl) {
      _bl = bl;
    }

    @Override
    public void run() {
      _bl.shutdown();
    }
  }

  public BundleLoaderImpl(Class c) throws IOException {
    _class = c;
    String resTag = "/bundleInfo.xml";
    InputStream iStream = _class.getResourceAsStream(resTag);
    if (null == iStream) {
      throw new IOException("Can't find bundleInfo.xml");
    }
    try {
      JAXBContext jc =
          JAXBContext.newInstance(
              com.ibm
                  .streams
                  .spl
                  .internal
                  .application
                  .bundleLoader
                  .v4000
                  .bundleInfoModel
                  .ObjectFactory
                  .class);
      Unmarshaller um = jc.createUnmarshaller();
      JAXBElement<BundleInfoModelType> jbe =
          (JAXBElement<BundleInfoModelType>) um.unmarshal(iStream);
      _bi = jbe.getValue();
    } catch (JAXBException e) {
      throw new IOException("Can't read bundleInfo.xml");
    } finally {
      iStream.close();
    }
  }

  private String getAdlFileName() throws ApplicationBundleException {
    return _bi.getAdlFile();
  }

  private void createDirIfNecessary(File dir) throws IOException {
    if (!dir.exists()) {
      boolean rc = dir.mkdirs();
      if (!rc) {
        throw new IOException("mkdirs[" + dir + "] failed.");
      }
    }
  }

  @Override
  public String extractADLFileToString() throws ApplicationBundleException {
    String adlFile = getOutputDirectory() + "/" + getAdlFileName();
    String resTag = "/tar/bundle.tar";
    InputStream iStream = _class.getResourceAsStream(resTag);
    if (iStream != null) {
      try {
        try {
          TarArchiveInputStream tStream =
              (TarArchiveInputStream)
                  new ArchiveStreamFactory().createArchiveInputStream("tar", iStream);
          try {
            TarArchiveEntry entry;
            while ((entry = (TarArchiveEntry) tStream.getNextEntry()) != null) {
              if (adlFile.equals(entry.getName())) {
                String xmlString = IOUtils.toString(tStream, "UTF-8");

                Trace.logDebug("adl unjarred successfully.");
                // Patch in the build ID
                String buildID = getBuildID();
                String newString =
                    xmlString.replaceFirst("buildId=\"\"", "buildId=\"" + buildID + "\"");
                return newString;
              }
            }
            // if reach here, then adl file not found in jar.
            Trace.logError("missing adl file in bundle.");
            throw new ApplicationBundleException("missing adl file in bundle.");
          } finally {
            tStream.close();
          }
        } finally {
          iStream.close();
        }
      } catch (ArchiveException e) {
        throw new ApplicationBundleException("failed to read archive");
      } catch (IOException e) {
        throw new ApplicationBundleException("failed to read archive");
      }
    } else {
      Trace.logError("getResourceAsStream[" + resTag + "] failed.");
      throw new ApplicationBundleException("getResourceAsStream failed.");
    }
  }

  @Override
  public void extractFiles(String targetAppDirName) throws ApplicationBundleException {
    String adlFile = getOutputDirectory() + "/" + getAdlFileName();
    String resTag = "/tar/bundle.tar";
    String outputDir = getOutputDirectory();
    String standalone = outputDir + "/bin/standalone";
    String standaloneExe = standalone + ".exe";

    InputStream iStream = _class.getResourceAsStream(resTag);
    if (iStream != null) {
      try {
        try {
          TarArchiveInputStream tStream =
              (TarArchiveInputStream)
                  new ArchiveStreamFactory().createArchiveInputStream("tar", iStream);
          try {
            TarArchiveEntry entry;
            while ((entry = (TarArchiveEntry) tStream.getNextEntry()) != null) {
              // Don't untar the standalone script, if it exists, because we will replace it with
              // the
              // standalone executable
              if (standalone.equals(entry.getName())) {
                continue;
              }
              File f = null;
              if (standaloneExe.equals(entry.getName())) {
                f = new File(targetAppDirName, standalone);
              } else {
                f = new File(targetAppDirName, entry.getName());
              }
              if (entry.isDirectory()) {
                createDirIfNecessary(f);
              } else {
                if (Files.exists(f.toPath(), LinkOption.NOFOLLOW_LINKS)) {
                  Files.delete(f.toPath());
                } else {
                  File parentPath = new File(f.getParent());
                  createDirIfNecessary(parentPath);
                }
                if (entry.isSymbolicLink()) {
                  File target = new File(entry.getLinkName());
                  Files.createSymbolicLink(f.toPath(), target.toPath());
                } else {
                  Trace.logTrace("untarring file " + f.getPath());
                  OutputStream oStream = new FileOutputStream(f);
                  try {
                    if (adlFile.equals(entry.getName())) {
                      String s = IOUtils.toString(tStream, "UTF-8");
                      // Patch in the build ID
                      String buildID = getBuildID();
                      String newString =
                          s.replaceFirst("buildId=\"\"", "buildId=\"" + buildID + "\"");
                      oStream.write(newString.getBytes());
                    } else {
                      IOUtils.copy(tStream, oStream);
                      // Set the executable bit if necessary
                      if ((entry.getMode() & 0100) != 0) {
                        f.setExecutable(true);
                      }
                    }
                  } finally {
                    oStream.close();
                  }
                }
              }
            }
          } finally {
            tStream.close();
          }
        } finally {
          iStream.close();
        }
      } catch (ArchiveException e) {
        Trace.logError("archival exception.", e);
        // e.printStackTrace();
        throw new ApplicationBundleException("archival exception");
      } catch (IOException e) {
        Trace.logError("IO exception.", e);
        // e.printStackTrace();
        throw new ApplicationBundleException("IO exception");
      }
    } else {
      Trace.logError("getResourceAsStream[" + resTag + "] failed.");
      throw new ApplicationBundleException("getResourceAsStream failed.");
    }
  }

  @Override
  public void extractADLFile(String targetDir) throws ApplicationBundleException {
    String adlFile = getOutputDirectory() + "/" + getAdlFileName();

    String resTag = "/tar/bundle.tar";
    InputStream iStream = _class.getResourceAsStream(resTag);
    if (iStream != null) {
      try {
        TarArchiveInputStream tStream =
            (TarArchiveInputStream)
                new ArchiveStreamFactory().createArchiveInputStream("tar", iStream);
        try {
          try {
            TarArchiveEntry entry;
            while ((entry = (TarArchiveEntry) tStream.getNextEntry()) != null) {
              if (adlFile.equals(entry.getName())) {
                String s = IOUtils.toString(tStream, "UTF-8");
                // Patch in the build ID
                String buildID = getBuildID();
                String newString = s.replaceFirst("buildId=\"\"", "buildId=\"" + buildID + "\"");
                File f = new File(targetDir, getAdlFileName());
                OutputStream oStream = new FileOutputStream(f);
                try {
                  oStream.write(newString.getBytes());
                } finally {
                  oStream.close();
                }
                Trace.logDebug("adl unjarred successfully.");
                return;
              }
            }

            // if reach here, then adl file not found in jar.
            Trace.logError("missing adl file in bundle.");
            throw new ApplicationBundleException("missing adl file in bundle.");

          } finally {
            tStream.close();
          }
        } finally {
          iStream.close();
        }
      } catch (ArchiveException e) {
        System.err.println("Oops");
      } catch (IOException e) {
        System.err.println("Oops");
      }
    } else {
      Trace.logError("getResourceAsStream[" + resTag + "] failed.");
      throw new ApplicationBundleException("getResourceAsStream failed.");
    }
  }

  @Override
  public InputStream getBundleInfo() throws ApplicationBundleException {
    ByteArrayOutputStream os = new ByteArrayOutputStream();
    try {
      JAXBContext jc =
          JAXBContext.newInstance(
              com.ibm
                  .streams
                  .spl
                  .internal
                  .application
                  .bundleLoader
                  .v4000
                  .bundleInfoModel
                  .ObjectFactory
                  .class);
      Marshaller m = jc.createMarshaller();
      m.setProperty(javax.xml.bind.Marshaller.JAXB_ENCODING, "UTF-8");
      m.setProperty(javax.xml.bind.Marshaller.JAXB_FORMATTED_OUTPUT, Boolean.TRUE);
      m.marshal(
          new JAXBElement<BundleInfoModelType>(
              new QName("bundleInfoModel"), BundleInfoModelType.class, _bi),
          os);
    } catch (JAXBException e) {
      throw new ApplicationBundleException(e.toString());
    }
    ByteArrayInputStream iStream = new ByteArrayInputStream(os.toByteArray());

    return iStream;
  }

  @Override
  public String getApplicationDirectory() throws ApplicationBundleException {
    return _bi.getApplicationDirectory();
  }

  @Override
  public String getOutputDirectory() throws ApplicationBundleException {
    return _bi.getOutputDirectory();
  }

  @Override
  public String getBuildID() throws ApplicationBundleException {
    return _bi.getBuildId();
  }

  @Override
  public String getBuildDate() throws ApplicationBundleException {
    return _bi.getBuildDate();
  }

  @Override
  public String getVersion() throws ApplicationBundleException {
    return _bi.getBundleVersion();
  }

  @Override
  public String getExecutionMode() throws ApplicationBundleException {
    return _bi.getExecutionMode().value();
  }

  @Override
  public String toolkitList(boolean filesToo) throws ApplicationBundleException {
    String s = new String("");
    List<ToolkitType> toolkits = _bi.getToolkit();
    Iterator<ToolkitType> it = toolkits.iterator();
    while (it.hasNext()) {
      ToolkitType tk = it.next();
      s += "Toolkit: " + tk.getName() + "  Version: " + tk.getVersion() + "\n";
      if (filesToo) {
        List<FileType> files = tk.getFile();
        Iterator<FileType> it1 = files.iterator();
        while (it1.hasNext()) {
          FileType ft = it1.next();
          s += "   " + ft.getName() + "\n";
        }
      }
    }
    if (filesToo) {
      OutputType ot = _bi.getOutput();
      if (null != ot) {
        s += "output directory files: \n";
        List<FileType> files = ot.getFile();
        Iterator<FileType> it1 = files.iterator();
        while (it1.hasNext()) {
          FileType ft = it1.next();
          s += "   " + ft.getName() + "\n";
        }
      }
    }
    return s;
  }

  @Override
  public int runStandalone(ArrayList<String> args) throws IOException {
    int rc = 0;
    _shutdownHandler = new ShutdownHandler(this);
    Runtime.getRuntime().addShutdownHook(_shutdownHandler);

    // Create a temporary directory in which to run the standalone app
    _tempDir = Files.createTempDirectory("");
    String outDir = _bi.getOutputDirectory();
    String standaloneExe = _tempDir.toString() + "/" + outDir + "/bin/standalone";

    try {
      extractFiles(_tempDir.toString());
      ArrayList<String> command = new ArrayList<String>();
      command.add(standaloneExe);
      command.addAll(args);
      ProcessBuilder pb = new ProcessBuilder(command);
      pb.inheritIO();
      _p = pb.start();
      rc = _p.waitFor();
      _p = null;
    } catch (InterruptedException e) {
      System.err.println("Execution was interrupted");
    } catch (ApplicationBundleException e) {
      throw new IOException(e.getMessage());
    }
    return rc;
  }
}
