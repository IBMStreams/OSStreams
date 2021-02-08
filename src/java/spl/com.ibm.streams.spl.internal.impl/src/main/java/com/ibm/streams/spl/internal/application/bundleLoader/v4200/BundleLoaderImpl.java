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

package com.ibm.streams.spl.internal.application.bundleLoader.v4200;

import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.platform.services.v4200.ApplicationSetType;
import com.ibm.streams.platform.services.v4200.ApplicationType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperDefinitionType;
import com.ibm.streams.platform.services.v4200.PrimitiveOperDefinitionsType;
import com.ibm.streams.platform.services.v4200.SubmissionTimeValueType;
import com.ibm.streams.platform.services.v4200.SubmissionTimeValuesType;
import com.ibm.streams.spl.internal.application.bundleLoader.BundleLoader;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.BundleInfoModelType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.FileType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.OutputType;
import com.ibm.streams.spl.internal.application.bundleLoader.v4000.bundleInfoModel.ToolkitType;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
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
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.cli.PosixParser;
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
  private final String helpOptionName = new String("help");
  private String helpOptDesc = new Message(Key.SPL_RUNTIME_OPTION_HELP, "").getLocalizedMessage();
  private String logToOptDesc =
      new Message(Key.SPL_RUNTIME_OPTION_CONSOLE, "").getLocalizedMessage();
  private String dataDirDesc =
      new Message(Key.SPL_RUNTIME_OPTION_DATA_DIRECTORY, "").getLocalizedMessage();
  private String execDirDesc =
      new Message(Key.SPL_RUNTIME_OPTION_EXECUTION_DIRECTORY, "").getLocalizedMessage();
  private String debugDesc = new Message(Key.SPL_RUNTIME_OPTION_DEBUGGER, "").getLocalizedMessage();
  private String killDesc =
      new Message(Key.SPL_RUNTIME_OPTION_KILL_AFTER, "").getLocalizedMessage();
  private String logDesc =
      new Message(Key.SPL_RUNTIME_OPTION_NEW_LOG_LEVEL, "").getLocalizedMessage();
  private String traceDesc =
      new Message(Key.SPL_RUNTIME_OPTION_TRACE_LEVEL, "").getLocalizedMessage();
  private final String dataDirOptionName = new String("data-directory");
  private final String exeDirOptionName = new String("execution-directory");
  private final String logToStdErr = new String("log-to-stderr");
  private final String debugOptionName = new String("debug");
  private final String killAfter = new String("kill-after");
  private final String logLevel = new String("log-level");
  private final String traceLevel = new String("trace-level");

  public void shutdown() {
    if (null != _p) {
      _p.destroy();
      // We need to wait for the process to stop before we delete the
      // directory or the delete below may fail
      try {
        _p.waitFor();
      } catch (Exception e) {
      }
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
            System.err.println(
                new Message(Key.SPL_RUNTIME_FAILED_TO_REMOVE_TEMP_DIR, _tempDir.toString())
                    .getLocalizedMessageAndPrefix());
          }
        } catch (Exception e) {
          System.err.println(
              new Message(Key.SPL_RUNTIME_FAILED_TO_REMOVE_TEMP_DIR, _tempDir.toString())
                  .getLocalizedMessageAndPrefix());
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
      throw new IOException("Can't read bundleInfo.xml", e);
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
                      int mode = entry.getMode();
                      // If any execute bits are on restore them
                      if ((mode | 0111) != 0) {
                        ArrayList<String> command = new ArrayList<String>();
                        command.add("/bin/chmod");
                        ;
                        command.add("750");
                        command.add(f.getAbsolutePath());
                        int rc = run(command);
                        if (rc != 0) {
                          throw new IOException(
                              "Failed to set permissions on " + f.getAbsolutePath().toString());
                        }
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
      } catch (InterruptedException e) {
        Trace.logError("IO exception.", e);
        // e.printStackTrace();
        throw new ApplicationBundleException("IO exception");
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

  private int run(ArrayList<String> command) throws IOException, InterruptedException {

    ProcessBuilder pb = new ProcessBuilder(command);
    pb.inheritIO();
    Process p = pb.start();
    return p.waitFor();
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

  private Options constructOptions() {
    Options options = new Options();

    Option helpOpt =
        OptionBuilder.withLongOpt(helpOptionName).withDescription(helpOptDesc).create("h");
    options.addOption(helpOpt);

    Option logStdErrOpt =
        OptionBuilder.withLongOpt(logToStdErr).withDescription(logToOptDesc).create("c");
    options.addOption(logStdErrOpt);

    Option dataDirOpt =
        OptionBuilder.withLongOpt(dataDirOptionName)
            .hasArg()
            .withDescription(dataDirDesc)
            .create("d");
    options.addOption(dataDirOpt);

    Option exeDirOpt =
        OptionBuilder.withLongOpt(exeDirOptionName)
            .hasArg()
            .withDescription(execDirDesc)
            .create("e");
    options.addOption(exeDirOpt);

    Option debugOpt =
        OptionBuilder.withLongOpt(debugOptionName).hasArg().withDescription(debugDesc).create("g");
    options.addOption(debugOpt);

    Option killAfterOpt =
        OptionBuilder.withLongOpt(killAfter).hasArg().withDescription(killDesc).create("k");
    options.addOption(killAfterOpt);

    Option logLevelOpt =
        OptionBuilder.withLongOpt(logLevel).hasArg().withDescription(logDesc).create("l");
    options.addOption(logLevelOpt);

    Option traceLevelOpt =
        OptionBuilder.withLongOpt(traceLevel).hasArg().withDescription(traceDesc).create("t");
    options.addOption(traceLevelOpt);

    return options;
  }

  private String computeExecutionDirectoryRoot(String value) throws Exception {

    // value is the location we need to use as a temporary root directory
    File root = new File(value).getAbsoluteFile();
    if (!root.exists()) {
      throw new Exception(
          new Message(Key.SPL_RUNTIME_EXEC_LOC_NOT_EXIST, value).getLocalizedMessageAndPrefix());
    } else if (!root.isDirectory()) {
      throw new Exception(
          new Message(Key.SPL_RUNTIME_EXEC_LOC_NOT_DIR, value).getLocalizedMessageAndPrefix());
    } else if (!(root.canExecute() && root.canRead() && root.canWrite())) {
      throw new Exception(
          new Message(Key.SPL_RUNTIME_EXEC_LOC_BAD_PERMS, value).getLocalizedMessageAndPrefix());
    }
    return root.toString();
  }

  private String computeDataDirectory(String value) throws Exception {
    File dir = new File(value).getAbsoluteFile();
    return dir.toString();
  }

  private com.ibm.streams.platform.services.v4200.ApplicationType getADL()
      throws ApplicationBundleException, IOException, JAXBException {
    String adlString = extractADLFileToString();
    InputStream iStream = IOUtils.toInputStream(adlString, "UTF-8");
    JAXBContext jc =
        JAXBContext.newInstance(com.ibm.streams.platform.services.v4200.ObjectFactory.class);
    Unmarshaller um = jc.createUnmarshaller();
    JAXBElement<com.ibm.streams.platform.services.v4200.ApplicationSetType> jbe =
        (JAXBElement<com.ibm.streams.platform.services.v4200.ApplicationSetType>)
            um.unmarshal(iStream);
    ApplicationSetType app = jbe.getValue();
    ApplicationType adlModel = app.getSplApplication().get(0);
    return adlModel;
  }

  private List<String> findCapabilities(ApplicationType adlModel) {
    List<String> caps = new ArrayList<>();
    PrimitiveOperDefinitionsType operDefs = adlModel.getPrimitiveOperDefinitions();
    for (PrimitiveOperDefinitionType operDef : operDefs.getPrimitiveOperDefinition()) {
      List<String> capabilities = operDef.getCapability();
      if (capabilities != null) {
        caps.addAll(capabilities);
      }
    }
    return caps;
  }

  private int usage(
      Options options, com.ibm.streams.platform.services.v4200.ApplicationType adlModel, int rc) {
    String version = adlModel.getProductVersion();
    SubmissionTimeValuesType subTimeValues = adlModel.getSubmissionTimeValues();
    List<String> requiredSubs = new ArrayList<>();
    List<String> optionalSubs = new ArrayList<>();
    if (subTimeValues != null) {
      for (SubmissionTimeValueType stv : subTimeValues.getSubmissionTimeValue()) {
        String compositeName = stv.getCompositeName();
        String valueName = stv.getName();
        String sub = "[" + compositeName + ".]" + valueName;
        if (stv.isRequired()) {
          requiredSubs.add(sub);
        } else {
          sub += " (" + stv.getDefaultValue() + ")";
          optionalSubs.add(sub);
        }
      }
    }

    String use =
        new String(
            "IBM Streams - "
                + new Message(Key.SPL_RUNTIME_STANDALONE_STREAMS_APPLICATION, "")
                    .getLocalizedMessage()
                + " (Streams "
                + version
                + ")\n\n");

    Iterator<?> it = options.getOptions().iterator();
    while (it.hasNext()) {
      Option o = (Option) it.next();
      if (null == o.getDescription()) continue;
      use += o.getDescription() + "\n";
    }
    use +=
        "\n"
            + new Message(Key.SPL_RUNTIME_STANDALONE_APPLICATION_ARGUMENT_HELP, "")
                .getLocalizedMessage();
    if (!requiredSubs.isEmpty()) {
      boolean first = true;
      String requiredSubString = "";
      for (String s : requiredSubs) {
        if (!first) {
          requiredSubString += ", ";
        } else {
          first = false;
        }
        requiredSubString += s;
      }
      use +=
          "\n"
              + new Message(
                      Key.SPL_RUNTIME_STANDALONE_APPLICATION_REQUIRED_ARGUMENT_HELP,
                      requiredSubString)
                  .getLocalizedMessage();
    }
    if (!optionalSubs.isEmpty()) {
      boolean first = true;
      String optionalSubString = "";
      for (String s : optionalSubs) {
        if (!first) {
          optionalSubString += ", ";
        } else {
          first = false;
        }
        optionalSubString += s;
      }
      use +=
          "\n"
              + new Message(
                      Key.SPL_RUNTIME_STANDALONE_APPLICATION_OPTIONAL_ARGUMENT_HELP,
                      optionalSubString)
                  .getLocalizedMessage();
    }
    System.out.println(use);
    return rc;
  }

  @Override
  public int runStandalone(ArrayList<String> args) throws IOException {

    String executionDirRoot = new String(File.separator + "tmp");
    ArrayList<String> commandArgs = new ArrayList<String>();
    Options options = constructOptions();
    String debugger = null;
    // Parse out the ADL file
    com.ibm.streams.platform.services.v4200.ApplicationType adlModel;
    try {
      adlModel = getADL();
    } catch (ApplicationBundleException | JAXBException e1) {
      System.err.println(e1.getMessage());
      return 1;
    }
    List<String> caps = findCapabilities(adlModel);
    try {
      // Walk the command line options building the command line for the standalone executable
      String[] argv = args.toArray(new String[args.size()]);

      PosixParser cmdLineParser = new PosixParser();
      CommandLine commandLine = cmdLineParser.parse(options, argv);
      for (Option opt : commandLine.getOptions()) {
        switch (opt.getOpt()) {
          case "d":
            String dataDir = computeDataDirectory(opt.getValue());
            commandArgs.add("-d");
            commandArgs.add(dataDir);
            break;
          case "e":
            executionDirRoot = computeExecutionDirectoryRoot(opt.getValue());
            break;
          case "g":
            debugger = opt.getValue();
            break;
          case "h":
            return usage(options, adlModel, 0);
          default:
            commandArgs.add("-" + opt.getOpt());
            String val = opt.getValue();
            if (val != null) commandArgs.add(val);
        }
      }
      List<String> rest = commandLine.getArgList();
      commandArgs.addAll(rest);

    } catch (final ParseException e) {
      return usage(options, adlModel, 1);
    } catch (Exception e) {
      System.err.println(e.getMessage());
      return 1;
    }

    int rc = 0;

    _shutdownHandler = new ShutdownHandler(this);
    Runtime.getRuntime().addShutdownHook(_shutdownHandler);

    // Create a temporary directory in which to run the standalone app
    File tmpDir = new File(executionDirRoot);
    _tempDir = Files.createTempDirectory(tmpDir.toPath(), "");
    String outDir = _bi.getOutputDirectory();
    String standaloneExe = _tempDir.toString() + "/" + outDir + "/bin/standalone";
    ArrayList<String> standaloneCommand = new ArrayList<String>();
    if (debugger != null) {
      standaloneCommand.add(debugger);
      standaloneCommand.add(standaloneExe);
    } else {
      standaloneCommand.add(standaloneExe);
      standaloneCommand.addAll(commandArgs);
    }

    try {
      extractFiles(_tempDir.toString());
      // If any operator has capabilities, then we need to run setcap on the executable
      if (!caps.isEmpty()) {
        ArrayList<String> setcapCommand = new ArrayList<String>();
        setcapCommand.add("/usr/bin/sudo");
        setcapCommand.add("/usr/sbin/setcap");
        for (String s : caps) {
          setcapCommand.add(s);
        }
        setcapCommand.add(standaloneExe);
        ProcessBuilder pb = new ProcessBuilder(setcapCommand);
        pb.inheritIO();
        _p = pb.start();
        rc = _p.waitFor();
        _p = null;
        if (rc != 0) {
          System.err.println("Unable to set permissions on " + standaloneExe);
          return 1;
        }
      }

      ProcessBuilder pb = new ProcessBuilder(standaloneCommand);
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
