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

package com.ibm.streams.admin.internal.core;

import java.io.File;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;

public class LogFileNameComparator implements Comparator<File> {
  /* (non-Javadoc)
   * @see java.util.Comparator#compare(java.lang.Object, java.lang.Object)
   */
  public int compare(File source, File target) {
    try {
      FileInfo srcInfo = new FileInfo(source.getName());
      FileInfo tgtInfo = new FileInfo(target.getName());

      int cmp = srcInfo._name.compareTo(tgtInfo._name);
      if (cmp == 0) {
        // source.<num>.log  <  tgt.<num>.log
        if (srcInfo._logNum < tgtInfo._logNum) {
          cmp = -1;
        }
        // source.<num>.log  >  tgt.<num>.log
        else if (srcInfo._logNum > tgtInfo._logNum) {
          cmp = 1;
        }
        // source.<num>.log.<set>  <  tgt.<num>.log.<set>
        else if (srcInfo._logSet < tgtInfo._logSet) {
          cmp = -1;
        }
        // source.<num>.log.<set>  >  tgt.<num>.log.<set>
        else if (srcInfo._logSet > tgtInfo._logSet) {
          cmp = 1;
        }
        // must be comparing with a .lck file
        else {
          cmp = srcInfo._fullName.compareTo(tgtInfo._fullName);
        }
      }

      return cmp;
    } catch (Throwable t) {
      // anything goes wrong, return equals
      return 0;
    }
  }

  //////////////////////////
  // FileInfo
  //////////////////////////
  private class FileInfo {
    private String _name;
    private String _fullName;
    private int _logNum = -1;
    private int _logSet = -1;

    public FileInfo(String filename) {
      _name = _fullName = filename;

      // is this a .log file ?
      int logDot = filename.lastIndexOf(".log");
      if (logDot > 0) {
        // filename.<num>.log
        int numDot = filename.lastIndexOf(".", logDot - 1);
        if (numDot > 0 && numDot < logDot) {
          _name = filename.substring(0, numDot);
          try {
            String num = filename.substring(numDot + 1, logDot);
            _logNum = Integer.parseInt(num);
          } catch (NumberFormatException nfe) {
          }

          // filename.<num>.log.<set>
          if (!filename.endsWith(".log")) {
            int setDot = filename.lastIndexOf(".");
            if (setDot > 0) {
              try {
                String set = filename.substring(setDot + 1);
                _logSet = Integer.parseInt(set);
              } catch (NumberFormatException nfe) {
              }
            }
          }
        }
      }
    }
  }

  //////////////////////////
  // Main
  //////////////////////////
  public static void main(String[] args) {
    try {
      // assume first argument is file path
      if (args.length > 0) {
        File dir = new File(args[0]);
        if (dir.isDirectory()) {
          File[] dirContent = dir.listFiles();
          ArrayList<File> files = new ArrayList<File>();
          for (int i = 0; i < dirContent.length; i++) {
            System.out.println(dirContent[i]);
            files.add(dirContent[i]);
          }

          LogFileNameComparator comparator = new LogFileNameComparator();
          Collections.sort(files, comparator);

          System.out.println("Sorted list:");
          for (File file : files) {
            System.out.println(file);
          }
        } else {
          System.out.println("Path is not a directory");
        }
      } else {
        System.out.println("File path not specified");
      }
    } catch (Throwable t) {
      t.printStackTrace();
    }
  }
}
