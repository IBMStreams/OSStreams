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

package com.ibm.streams.splappinfo.core;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.StreamsInstall;
import com.ibm.streams.admin.internal.api.VersionInfo;
import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleException;
import com.ibm.streams.admin.internal.api.application.bundle.ApplicationBundleFileException;
import com.ibm.streams.admin.internal.api.application.bundle.Bundle;
import com.ibm.streams.spl.messages.Message;
import com.ibm.streams.spl.messages.general.StreamsSPLJavaMessagesKey.Key;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.MalformedURLException;
import java.net.URI;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.Date;
import java.util.Iterator;
import java.util.ServiceLoader;
import org.apache.commons.cli.CommandLine;
import org.apache.commons.cli.CommandLineParser;
import org.apache.commons.cli.GnuParser;
import org.apache.commons.cli.Option;
import org.apache.commons.cli.OptionBuilder;
import org.apache.commons.cli.Options;
import org.apache.commons.cli.ParseException;
import org.apache.commons.io.IOUtils;

public class Main {

  private static final String streamsInstall = System.getenv("STREAMS_INSTALL");
  // None of these option strings should be translated.
  private static final String filesOption = "files";
  private static final String toolkitsOption = "toolkits";
  private static final String xmlOption = "xml";
  private static final String buildIdOption = "build-id";
  private static final String buildDateOption = "build-date";
  private static final String unbundleOption = "unbundle";
  private static final String adlOption = "adl";
  private static final String helpOption = "help";
  private static final String bundleOption = "bundle";
  private static final String executionModeOption = "execution-mode";

  private static final Options options = constructOptions();
  private static final CommandLineParser cmdLineParser = new GnuParser();

  private static Bundle loadBundle(File file)
      throws MalformedURLException, ApplicationBundleFileException {
    String splJar =
        streamsInstall
            + File.separator
            + "system"
            + File.separator
            + "impl"
            + File.separator
            + "lib"
            + File.separator
            + "com.ibm.streams.spl.internal.impl.jar";
    File splJarFile = new File(splJar);
    URI splJarURI = splJarFile.toURI();
    URL splJarURL = splJarURI.toURL();

    URI uri = file.toURI();
    URL url = uri.toURL();
    URL[] urls = new URL[] {splJarURL, url};
    ClassLoader loader = new URLClassLoader(urls);
    ServiceLoader<Bundle> bundleServiceLoader = ServiceLoader.load(Bundle.class, loader);
    bundleServiceLoader.reload();
    for (Bundle bundle : bundleServiceLoader) {
      return bundle;
    }
    throw new ApplicationBundleFileException(file.toString());
  }

  private void usage(int rc) {
    String use = new Message(Key.SPL_APP_INFO_USAGE, "").getLocalizedMessageAndPrefix() + "\n\n";

    Iterator it = options.getOptions().iterator();
    while (it.hasNext()) {
      Option o = (Option) it.next();
      if (null == o.getDescription()) continue;
      use += o.getDescription() + "\n";
    }
    use += "\n" + new Message(Key.SPL_APP_INFO_ABSTRACT, "").getLocalizedMessage();
    System.out.println(use);
    System.exit(rc);
  }

  /* The bundle version string is "VR00", without the dots, where "V" and "R" are from
   * "V.R.M.F". This is a convention that we follow, but it is not documented anywhere.
   */
  private static long generateBundleVersionFromVRMF(String version) {
    String[] vrmf = version.split("\\.");
    return Long.parseLong(vrmf[0] + vrmf[1] + "00");
  }

  public void run(String[] argv) {
    int rc = 0;
    try {
      CommandLine commandLine = cmdLineParser.parse(options, argv);
      String[] remainingArgs = commandLine.getArgs();
      if (remainingArgs.length != 1) {
        usage(1);
      }

      if (commandLine.hasOption(helpOption)) {
        usage(0);
      }

      String file = remainingArgs[0];
      File bundleFile = new File(file);
      if (!bundleFile.exists()) {
        System.err.println(
            new Message(Key.SPL_APP_INFO_FILE_NOT_EXIST, file).getLocalizedMessageAndPrefix());
        System.exit(1);
      }
      Bundle bundle = loadBundle(bundleFile);

      long bundleVersion = Long.parseLong(bundle.getVersion());
      long installedVersion =
          generateBundleVersionFromVRMF(
              StreamsInstall.getDefaultStreamsInstall().getVersionInfo().getVersion());
      long minimumAllowedVersion = generateBundleVersionFromVRMF(VersionInfo.VERSION_40);
      long firstSubTimeFusionVersion = generateBundleVersionFromVRMF(VersionInfo.VERSION_42);
      if (bundleVersion < minimumAllowedVersion || bundleVersion > installedVersion) {
        System.err.println(
            new Message(Key.SPL_APP_INFO_UNSUPPORTED_BUNDLE_VERSION, "")
                .getLocalizedMessageAndPrefix());
        System.exit(1);
      }

      if (commandLine.hasOption(buildIdOption)) {
        String buildId = bundle.getBuildID();
        System.out.println(buildId);
      }
      if (commandLine.hasOption(buildDateOption)) {
        String date = bundle.getBuildDate();
        long buildDate = Long.parseLong(date);
        Date d = new Date(buildDate * 1000);
        System.out.println(d.toLocaleString());
      }
      if (commandLine.hasOption(filesOption) || commandLine.hasOption(toolkitsOption)) {
        String s = bundle.toolkitList(commandLine.hasOption(filesOption));
        System.out.println(s);
      }
      if (commandLine.hasOption(xmlOption)) {
        String xmlFileName = commandLine.getOptionValue(xmlOption);
        File f = new File(xmlFileName);
        OutputStream oStream = new FileOutputStream(f);
        IOUtils.copy(bundle.getBundleInfo(), oStream);
        oStream.close();
      }
      if (commandLine.hasOption(unbundleOption)) {
        String unbundleDirectory = commandLine.getOptionValue(unbundleOption);
        bundle.unpack(unbundleDirectory);
      }
      if (commandLine.hasOption(executionModeOption)) {
        int mode = bundle.getExecutionMode();
        if (bundleVersion >= firstSubTimeFusionVersion) {
          System.out.println("N/A");
        } else {
          System.out.println(
              mode == Bundle.EXECUTION_MODE_DISTRIBUTED ? "distributed" : "standalone");
        }
      }
      if (commandLine.hasOption(adlOption)) {
        String adl = bundle.unpackAdlFileToXml();
        System.out.println(adl);
      }
    } catch (final ParseException e) {
      usage(1);
    } catch (MalformedURLException e) {
      System.err.println(
          new Message(Key.SPL_APP_INFO_FILE_ERROR, e.getLocalizedMessage())
              .getLocalizedMessageAndPrefix());
      rc = 1;
    } catch (ApplicationBundleException e) {
      System.err.println(
          new Message(Key.SPL_APP_INFO_BUNDLE_ERROR, e.getLocalizedMessage())
              .getLocalizedMessageAndPrefix());
      rc = 1;
    } catch (StreamsException e) {
      System.err.println(
          new Message(Key.SPL_APP_INFO_BUNDLE_ERROR, e.getLocalizedMessage())
              .getLocalizedMessageAndPrefix());
      rc = 1;
    } catch (NumberFormatException e) {
      System.err.println(
          new Message(Key.SPL_APP_INFO_BUNDLE_ERROR, e.getLocalizedMessage())
              .getLocalizedMessageAndPrefix());
      rc = 1;
    } catch (IOException e) {
      System.err.println(
          new Message(Key.SPL_APP_INFO_BUNDLE_ERROR, e.getLocalizedMessage())
              .getLocalizedMessageAndPrefix());
      rc = 1;
    }
    if (rc != 0) {
      System.exit(rc);
    }
  }

  public static void main(String[] argv) {
    Main m = new Main();
    m.run(argv);
  }

  private static Options constructOptions() {
    String helpOptDesc = new Message(Key.SPL_APP_INFO_HELP_OPTION, "").getLocalizedMessage();
    String buildIdDesc = new Message(Key.SPL_APP_INFO_BUILDID_OPTION, "").getLocalizedMessage();
    String buildDateDesc = new Message(Key.SPL_APP_INFO_BUILDDATE_OPTION, "").getLocalizedMessage();
    String toolkitsDesc = new Message(Key.SPL_APP_INFO_TOOLKITS_OPTION, "").getLocalizedMessage();
    String filesDesc = new Message(Key.SPL_APP_INFO_FILES_OPTION, "").getLocalizedMessage();
    String xmlDesc = new Message(Key.SPL_APP_INFO_XML_OPTION, "").getLocalizedMessage();
    String unbundleDesc = new Message(Key.SPL_APP_INFO_UNBUNDLE_OPTION, "").getLocalizedMessage();
    String executionModeDesc =
        new Message(Key.SPL_APP_INFO_EXECUTIONMODE_OPTION, "").getLocalizedMessage();

    Option helpOpt = OptionBuilder.withLongOpt(helpOption).withDescription(helpOptDesc).create('h');
    Option buildIdOpt =
        OptionBuilder.withLongOpt(buildIdOption).withDescription(buildIdDesc).create();
    Option buildDateOpt =
        OptionBuilder.withLongOpt(buildDateOption).withDescription(buildDateDesc).create();
    Option toolkitsOpt =
        OptionBuilder.withLongOpt(toolkitsOption).withDescription(toolkitsDesc).create();
    Option filesOpt = OptionBuilder.withLongOpt(filesOption).withDescription(filesDesc).create();
    Option xmlOpt = OptionBuilder.withLongOpt(xmlOption).hasArg().withDescription(xmlDesc).create();
    Option unbundleOpt =
        OptionBuilder.withLongOpt(unbundleOption).hasArg().withDescription(unbundleDesc).create();
    Option executionModeOpt =
        OptionBuilder.withLongOpt(executionModeOption).withDescription(executionModeDesc).create();
    Option adlOpt = OptionBuilder.withLongOpt(adlOption).create();

    Options options = new Options();
    options.addOption(helpOpt);
    options.addOption(buildIdOpt);
    options.addOption(buildDateOpt);
    options.addOption(toolkitsOpt);
    options.addOption(filesOpt);
    options.addOption(xmlOpt);
    options.addOption(unbundleOpt);
    options.addOption(executionModeOpt);
    options.addOption(adlOpt);
    return options;
  }
}
