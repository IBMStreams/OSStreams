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

package com.ibm.streams.sch.composer;

import com.ibm.streams.admin.internal.api.trace.Trace;
import java.util.Arrays;
import java.util.HashSet;
import java.util.Set;

public class Composer_helper {

  public static String prepareConditionString(String str) {
    String str1 = str.replaceAll("\\s", "");
    str1 = str1.replace("[", "\\[");
    str1 = str1.replace("]", "\\]");
    return (str1.replace("*", "[\\S]*"));
  }

  public static Set<String> getMatchingNames(
      Set<String> names, String nameSpec, boolean ignoreNonMatching) {
    Trace.logTrace(
        "ENTER Composer_helper.getMatchingNames with "
            + Arrays.deepToString(new Object[] {names, nameSpec, ignoreNonMatching}));
    Set<String> matchingNames = new HashSet<>();
    for (String name : names) {
      if (name.matches(prepareConditionString(nameSpec))) {
        matchingNames.add(name);
      }
    }
    if (!ignoreNonMatching) {
      if (matchingNames.size() == 0) {
        // add back in original for error reporting purposes.
        matchingNames.add(nameSpec);
      }
    }
    Trace.logTrace(
        "EXIT Composer_helper.getMatchingNames with "
            + Arrays.deepToString(new Object[] {matchingNames}));
    return matchingNames;
  }
}
