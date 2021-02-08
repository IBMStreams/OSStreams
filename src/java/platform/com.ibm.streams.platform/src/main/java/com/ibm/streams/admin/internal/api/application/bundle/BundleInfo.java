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

package com.ibm.streams.admin.internal.api.application.bundle;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.api.trace.TraceLogger.Level;
import com.ibm.streams.messages.StreamsAdminMessagesKey.Key;
import java.io.BufferedInputStream;
import java.io.IOException;
import java.io.InputStream;
import java.util.jar.JarFile;
import java.util.zip.ZipEntry;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import org.apache.commons.compress.archivers.ArchiveEntry;
import org.apache.commons.compress.archivers.ArchiveInputStream;
import org.apache.commons.compress.archivers.ArchiveStreamFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.xml.sax.InputSource;

public class BundleInfo {
  protected String _buildID;
  protected String _version;
  protected String _adlFile;
  protected String _appDir;
  protected String _outputDir;
  protected String _execMode;
  protected long _buildDate;
  protected JarFile jarFile;

  public BundleInfo(String sabName, String instanceID) throws StreamsException {
    // System.out.println("Before parse");
    // memUsage();
    parse(sabName, instanceID);
    // System.out.println("After parse");
    // memUsage();
  }

  public String getBuildID() {
    return _buildID;
  }

  public String getVersion() {
    return _version;
  }

  public String getADLFile() {
    return _adlFile;
  }

  public String getApplicationDirectory() {
    return _appDir;
  }

  public String getOutputDirectory() {
    return _outputDir;
  }

  public long getBuildDate() {
    return _buildDate;
  }

  public String getExecutionMode() {
    return _execMode;
  }

  public void close() throws StreamsException {
    if (jarFile != null) {
      try {
        jarFile.close();
        jarFile = null;
      } catch (Exception e) {
        Trace.logError("Error closing jar file.", e);
        throw new StreamsException(e);
      }
    }
  }

  public String getADLXML() throws StreamsException {
    // System.out.println("Before xml");
    // memUsage();

    String xml = null;

    String adlFile = "/" + _adlFile;
    ZipEntry tarEntry = jarFile.getEntry("tar/bundle.tar");
    if (tarEntry != null) {
      // System.out.println("Tar entry found " + tarEntry.getName());
      // System.out.println("directory " + tarEntry.isDirectory());
      InputStream tarIn = null;
      try {
        tarIn = jarFile.getInputStream(tarEntry);
        if (tarIn != null) {
          ArchiveInputStream in =
              new ArchiveStreamFactory().createArchiveInputStream(new BufferedInputStream(tarIn));
          ArchiveEntry entry = null;
          while ((entry = in.getNextEntry()) != null) {
            // System.out.println("entry " + entry.getName());
            if (entry.getName().endsWith(adlFile)) {
              int contentLength = (int) entry.getSize();
              byte[] b = new byte[contentLength];
              int offset = 0;
              while ((b.length - offset) > 0) {
                int bytesRead = in.read(b, offset, contentLength - offset);
                offset = offset + bytesRead;
              }

              xml = new String(b, "UTF-8");
              break;
            }
          }
        } else {
          // System.out.println("Tar input stream is null");
          Trace.logError("Tar input stream is null");
        }
      } catch (IOException ioe) {
        Trace.logError("Error retrieving archive", ioe);
        throw new StreamsException(ioe);
      } catch (Throwable t) {
        t.printStackTrace();
        Trace.logError("Error retrieving archive ", t);
        throw new StreamsException(t);
      } finally {
        if (tarIn != null) {
          try {
            tarIn.close();
          } catch (IOException ioe) {
            Trace.logError("Unexpected error while closing tar input stream.", ioe);
          }
        }
      }
    }

    // System.out.println("After xml");
    // memUsage();

    return xml;
  }

  private void parse(String sabName, String instanceID) throws StreamsException {
    try {
      jarFile = new JarFile(sabName);
      ZipEntry bundleInfoEntry = jarFile.getEntry("bundleInfo.xml");
      if (bundleInfoEntry != null) {
        if (Trace.isEnabled(Level.TRACE)) {
          Trace.logTrace("Bundle info XML file found in sab.");
        }
        InputStream zipIn = null;
        try {
          zipIn = jarFile.getInputStream(bundleInfoEntry);
          InputSource inputSource = new InputSource(zipIn);

          DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
          DocumentBuilder docBuilder = factory.newDocumentBuilder();
          Document document = docBuilder.parse(inputSource);
          Element bundleInfoElement = document.getDocumentElement();
          Trace.logDebug("Element: " + bundleInfoElement);
          _buildID = bundleInfoElement.getAttribute("buildId");
          _version = bundleInfoElement.getAttribute("bundleVersion");
          _adlFile = bundleInfoElement.getAttribute("adlFile");
          _appDir = bundleInfoElement.getAttribute("applicationDirectory");
          _outputDir = bundleInfoElement.getAttribute("ouitputDirectory");
          _execMode = bundleInfoElement.getAttribute("executionMode");

          String bldDate = bundleInfoElement.getAttribute("buildDate");
          _buildDate = Long.parseLong(bldDate) * 1000;

          Trace.logDebug("Build ID: " + _buildID);
        } finally {
          if (zipIn != null) {
            try {
              zipIn.close();
            } catch (Throwable e) {
            }
          }
        }
      } else {
        if (Trace.isEnabled(Level.ERROR)) {
          Trace.logError("Bundle info XML file not found.");
        }
        // throw invalid version exception
        throw new StreamsException(Key.ERR_UNSUPPORTED_APPBUNDLE_VERSION, sabName, instanceID);
      }
    } catch (IOException ioe) {
      if (Trace.isEnabled(Level.ERROR)) {
        Trace.logError("Error reading SAB " + sabName, ioe);
      }
      throw new StreamsException(ioe);
    } catch (Exception e) {
      if (Trace.isEnabled(Level.ERROR)) {
        Trace.logError("Error reading SAB " + sabName, e);
      }
      throw new StreamsException(e);
    }
  }

  public static void memUsage() {
    Runtime runtime = Runtime.getRuntime();
    long free = runtime.freeMemory();
    long total = runtime.totalMemory();
    long used = total - free;
    System.out.println("Free memory: " + free + " total: " + total + " used: " + used);
  }

  public static void main(String[] args) {
    try {
      BundleInfo bundleInfo = new BundleInfo("testsst.Main.sab", "smstest");
      System.out.println("Build ID: " + bundleInfo.getBuildID());
      System.out.println("Version: " + bundleInfo.getVersion());
      System.out.println("ADL file: " + bundleInfo.getADLFile());
      System.out.println("Application directory: " + bundleInfo.getApplicationDirectory());
      System.out.println("Output directory: " + bundleInfo.getOutputDirectory());
      System.out.println("Build date: " + bundleInfo.getBuildDate());
      System.out.println("Execution mode: " + bundleInfo.getExecutionMode());

      // System.out.println(bundleInfo.getADLXML());
      bundleInfo.getADLXML();

      bundleInfo.close();

      System.gc();

      memUsage();
    } catch (Throwable t) {
      t.printStackTrace();
    }
  }
}
