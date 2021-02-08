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

package com.ibm.streams.admin.internal.core.zip;

import com.ibm.streams.admin.internal.api.trace.Trace;
import com.ibm.streams.admin.internal.core.AdminMessageBundle;
import com.ibm.streams.admin.internal.core.LogFileNameComparator;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.nio.file.Files;
import java.nio.file.Path;
import java.nio.file.StandardCopyOption;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Locale;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;
import java.util.zip.ZipOutputStream;

public class Zip extends Thread {
  // data members
  ZipInputStream _zin;

  private Zip(ZipInputStream zin) {
    super("ZipClose");
    _zin = zin;
  }

  public static void zipDirectory(File dir, OutputStream out, FileFilter filter)
      throws IOException {
    if (!dir.isDirectory()) {
      throw new IOException(
          AdminMessageBundle.getPlainFormattedString(
              Locale.getDefault(),
              AdminMessageBundle.Id.EXC_ZIP_DIRECTORY.toString(),
              dir.getPath()));
    }

    ZipOutputStream zipOut = new ZipOutputStream(out);
    String relativePath = null;
    zipDir(dir, zipOut, filter, relativePath);
    zipOut.flush();
    zipOut.close();
  }

  private static void zipDir(
      File dir, ZipOutputStream zipOut, FileFilter filter, String relativePath) throws IOException {
    File[] files;
    if (filter != null) {
      files = dir.listFiles(filter);
    } else {
      files = dir.listFiles();
    }

    if (files != null) {
      byte[] buf = new byte[1024 * 1024];
      for (File f : sortFiles(files)) {
        String relPath;
        if (relativePath != null) {
          relPath = relativePath + "/" + f.getName();
        } else {
          relPath = f.getName();
        }

        ZipEntry zipEntry;
        if (f.isDirectory()) {
          try {
            zipEntry = new ZipEntry(relPath + "/");
            long lastModified = f.lastModified();
            zipEntry.setTime(lastModified);
            zipOut.putNextEntry(zipEntry);
            zipOut.closeEntry();
            zipDir(f, zipOut, filter, relPath);
          } catch (IOException ioe) {
            // ignore this entry if failure
          }
        } else {

          File tempFile = null;
          try {
            Trace.logTrace("In Zip for: " + relPath);
            zipEntry = new ZipEntry(relPath);
            zipEntry.setMethod(ZipEntry.DEFLATED);
            long lastModified = f.lastModified();
            zipEntry.setTime(lastModified);

            Path filePath = f.toPath();
            tempFile = File.createTempFile(f.getName(), "streams");
            tempFile.deleteOnExit();
            Path tempFilePath = tempFile.toPath();
            // copy contents
            Files.copy(filePath, tempFilePath, StandardCopyOption.REPLACE_EXISTING);

            long size = tempFile.length();
            zipEntry.setSize(size);
            zipOut.putNextEntry(zipEntry);

            FileInputStream fin = null;
            try {
              // write file content
              fin = new FileInputStream(tempFile);
              int bytesRead;
              while ((bytesRead = fin.read(buf)) != -1) {
                zipOut.write(buf, 0, bytesRead);
              }
            } catch (IOException ioe2) {
              // ignore error
            } finally {
              if (fin != null) {
                fin.close();
              }
            }

            // close entry and proceed to next
            zipOut.closeEntry();
          } catch (IOException ioe) {
            // ignore this entry if error
            Trace.logError("Zip error", ioe);
          } finally {
            if (tempFile != null) {
              tempFile.delete();
            }
          }
        }
      }
    }
  }

  public static void unzipDirectory(File targetDir, InputStream in) throws IOException {
    if (!targetDir.exists()) {
      targetDir.mkdirs();
    }
    if (!targetDir.isDirectory()) {
      throw new IOException(
          AdminMessageBundle.getPlainFormattedString(
              Locale.getDefault(),
              AdminMessageBundle.Id.EXC_ZIP_DIRECTORY.toString(),
              targetDir.getPath()));
    }

    ZipInputStream zin = new ZipInputStream(in);

    byte[] buf = new byte[16 * 1024];
    ZipEntry zipEntry;
    while ((zipEntry = zin.getNextEntry()) != null) {
      File f = new File(targetDir, zipEntry.getName());
      if (zipEntry.isDirectory()) {
        f.mkdirs();
      } else {
        FileOutputStream fout = new FileOutputStream(f);
        int bytesRead;
        while ((bytesRead = zin.read(buf)) != -1) {
          fout.write(buf, 0, bytesRead);
        }
        fout.close();
        long t = zipEntry.getTime();
        f.setLastModified(t);
        zin.closeEntry();
      }
    }

    // other end of zip stream can sometimes complain that
    // we close too soon; so delay a bit before closing
    Zip zip = new Zip(zin);
    zip.start();
  }

  private static List<File> sortFiles(File[] files) {
    List<File> dirContent = new ArrayList<File>();
    for (int i = 0; i < files.length; i++) {
      dirContent.add(files[i]);
    }
    Collections.sort(dirContent, new LogFileNameComparator());
    return dirContent;
  }

  public void run() {
    try {
      // other end of zip stream can sometimes complain that
      // we close too soon; so delay a bit before closing
      Thread.sleep(500);
    } catch (Exception e) {
    }

    try {
      _zin.close();
    } catch (IOException ioe) {
      Trace.logError(ioe.getMessage(), ioe);
    }
  }

  public static void main(String[] args) {
    try {
      File antDir = new File("/apache-ant-1.9.0");
      FileOutputStream fout = new FileOutputStream("/temp/apache-ant.zip");
      zipDirectory(antDir, fout, null);

      File testDir = new File("/temp/test-ant");
      FileInputStream fin = new FileInputStream("/temp/apache-ant.zip");
      unzipDirectory(testDir, fin);
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
