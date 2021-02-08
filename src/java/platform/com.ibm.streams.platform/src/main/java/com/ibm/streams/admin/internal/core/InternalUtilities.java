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

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.HashMap;
import java.util.Map;

public class InternalUtilities {

  // ZooKeeper has a set of rules for znodes.  Our object rules need to take ZK rules into
  // consideration for our
  // enforcement
  // 1) The null character (\u0000) cannot be part of a path name. (This causes problems with the C
  // binding.)
  // 2) The following characters can't be used because they don't display well, or render in
  // confusing ways: u0001 -
  // u0019 and u007F - u009F.
  // 3) The following characters are not allowed: ud800 -uF8FF, uFFF0-uFFFF, uXFFFE - uXFFFF (where
  // X is a digit 1 - E),
  // uF0000 - uFFFFF.
  // 4) The "." character can be used as part of another name, but "." and ".." cannot alone be used
  // to indicate a node
  // along a path, because ZooKeeper doesn't use relative paths. The following would be invalid:
  // "/a/b/./c" or
  // "/a/b/../c".
  // 5) The token "zookeeper" is reserved.
  // static public final String namePattern =
  // "[^!#$%&\\'\\*\\+\\,\\/\\;\\<\\>\\=?@\\^\\[\\]`\\{|\\}~\u0000\u0001-\u0019\u007F-\u009F\ud800-\uF8FF\u1FFFE-\u1FFFF\u2FFFE-\u2FFFF\uF0000-\uFFFFF\\s]+";
  // ZK published rules - static public final String namePattern =
  // "[^!#$%&\\'\\*\\+\\,\\/\\;\\<\\>\\=?@\\^\\[\\]`\\{|\\}~\\u0000\\u0001-\\u0019\\u007F-\\u009F\\ud800-\\uF8FF\\x{1FFFE}-\\x{1FFFF}\\x{2FFFE}-\\x{2FFFF}\\x{3FFFE}-\\x{3FFFF}\\x{4FFFE}-\\x{4FFFF}\\x{5FFFE}-\\x{5FFFF}\\x{6FFFE}-\\x{6FFFF}\\x{7FFFE}-\\x{7FFFF}\\x{8FFFE}-\\x{8FFFF}\\x{9FFFE}-\\x{9FFFF}\\x{F0000}-\\x{FFFFF}\\s]+";

  private static Map<String, String> replaceHtml = new HashMap<String, String>();

  static {
    replaceHtml.put("&gt;", ">");
    replaceHtml.put("&lt;", "<");
    replaceHtml.put("&#91;", "[");
    replaceHtml.put("&#93;", "]");
    replaceHtml.put("&#123;", "{");
    replaceHtml.put("&#125;", "}");
    replaceHtml.put("&#124;", "|");
    replaceHtml.put("&#63;", "?");
    replaceHtml.put("&#39;", "'");
  }

  public static void cleanupProcess(Process proc) {
    cleanupProcess(proc, true);
  }

  public static void cleanupProcess(Process proc, boolean destroy) {
    try {
      InputStream in = proc.getInputStream();
      if (in != null) {
        in.close();
      }
    } catch (IOException ioe) {
    }
    try {
      InputStream err = proc.getErrorStream();
      if (err != null) {
        err.close();
      }
    } catch (IOException ioe) {
    }
    try {
      OutputStream os = proc.getOutputStream();
      if (os != null) {
        os.close();
      }
    } catch (IOException ioe) {
    }

    if (destroy) {
      proc.destroy();
    }
  }
}
