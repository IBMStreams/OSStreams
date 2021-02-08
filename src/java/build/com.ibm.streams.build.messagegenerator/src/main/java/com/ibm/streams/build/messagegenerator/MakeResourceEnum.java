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

package com.ibm.streams.build.messagegenerator;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Reader;
import java.io.StringReader;
import java.io.StringWriter;
import java.io.Writer;
import java.text.MessageFormat;
import java.util.ArrayList;
import java.util.List;
import java.util.Properties;
import java.util.PropertyResourceBundle;
import java.util.Set;

public class MakeResourceEnum {
  private static File templateFile;
  private static List<File> resourceFiles = new ArrayList<File>();
  private static String insertionMarker;
  private static File generatedFile;

  private static void copy(Reader reader, Writer writer) throws IOException {
    int character = reader.read();
    while (character != -1) {
      writer.write(character);
      character = reader.read();
    }
  }

  private static String readFile(Reader reader) throws IOException {
    StringWriter writer = new StringWriter();
    copy(reader, writer);
    writer.close();
    return writer.toString();
  }

  private static void writeFile(Writer writer, String contents) throws IOException {
    StringReader reader = new StringReader(contents);
    copy(reader, writer);
  }

  private static String generateEnum(List<PropertyResourceBundle> bundles) throws IOException {
    StringBuffer output = new StringBuffer();

    for (PropertyResourceBundle bundle : bundles) {
      Set<String> keys = bundle.keySet();
      String separator = ",";
      String newline = System.getProperty("line.separator");
      for (String key : keys) {
        output.append(key).append(separator).append(newline);
      }
    }
    if (output.length() > 0) {
      // replace last comma with semicolon
      int lastCommaPosition = output.lastIndexOf(",");
      output.setCharAt(lastCommaPosition, ';');
    }
    return output.toString();
  }

  private static class DuplicatePropertyException extends RuntimeException {
    public DuplicatePropertyException(String bundleName, String propertyName) {
      super(
          MessageFormat.format(
              "Property file {0} contains duplicate definitions for property {1}",
              bundleName, propertyName));
    }
  }

  private static class PropertiesDuplicateTester extends Properties {
    private String propertiesFileName;

    public PropertiesDuplicateTester(String propertiesFileName) {
      this.propertiesFileName = propertiesFileName;
    }

    @Override
    public Object put(Object key, Object value) {
      Object oldValue = super.put(key, value);
      if (oldValue != null) {
        // Property had old value;
        throw new DuplicatePropertyException(propertiesFileName, key.toString());
      }
      return oldValue;
    }
  }

  /** Test a properties file for duplicate keys */
  private static void testForDuplicateProperties(File resourceFile) throws IOException {
    Reader propReader = new FileReader(resourceFile);
    new PropertiesDuplicateTester(resourceFile.getAbsolutePath()).load(propReader);
  }

  private static void makeEnum() throws IOException {
    List<PropertyResourceBundle> bundles = new ArrayList<PropertyResourceBundle>();
    for (File resourceFile : resourceFiles) {
      testForDuplicateProperties(resourceFile);
      bundles.add(new PropertyResourceBundle(new FileReader(resourceFile)));
    }

    Writer writer = new BufferedWriter(new FileWriter(generatedFile));
    Reader reader = new BufferedReader(new FileReader(templateFile));

    try {
      // quick-and-dirty
      String templateContents = readFile(reader);
      String enumContents = generateEnum(bundles);
      String outputContents = templateContents.replace(insertionMarker, enumContents);
      writeFile(writer, outputContents);
    } finally {
      if (writer != null) {
        try {
          writer.close();
          writer = null;
        } catch (IOException e) {
        }
      }
      if (reader != null) {
        try {
          reader.close();
          reader = null;
        } catch (IOException e) {
        }
      }
    }
  }

  private static void conditionallyMakeEnum() throws IOException {
    if (!templateFile.canRead()) {
      System.err.println("Cannot read " + templateFile.getAbsolutePath());
      System.exit(1);
    }
    for (File resourceFile : resourceFiles) {
      if (!resourceFile.canRead()) {
        System.err.println("Cannot read " + resourceFile.getAbsolutePath());
        System.exit(1);
      }
    }

    boolean generationNeeded = false;
    if (!generatedFile.exists()) {
      generationNeeded = true;
    } else if (isResourceFileNewer()) {
      generationNeeded = true;
    } else if (generatedFile.lastModified() < templateFile.lastModified()) {
      generationNeeded = true;
    }

    if (generationNeeded) {
      System.out.println("Generating resource messages file");
      makeEnum();
    } else {
      System.out.println("Resource file up to date.");
    }
  }

  private static boolean isResourceFileNewer() {
    boolean newer = false;

    for (File resourceFile : resourceFiles) {
      if (generatedFile.lastModified() < resourceFile.lastModified()) {
        newer = true;
        break;
      }
    }

    return newer;
  }

  public static void main(String[] args) {
    try {
      System.out.println("Create resource enums");
      templateFile = new File(args[0]);
      insertionMarker = args[1];
      generatedFile = new File(args[2]);
      for (int i = 3; i < args.length; i++) {
        System.out.println("Input resource file - " + args[i]);
        resourceFiles.add(new File(args[i]));
      }

      conditionallyMakeEnum();
    } catch (DuplicatePropertyException dpe) {
      System.err.println(dpe.getMessage());
      System.exit(1);
    } catch (IOException e) {
      e.printStackTrace();
      System.exit(1);
    }
  }
}
