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

package com.ibm.streams.instance.sam.exception;

import com.ibm.streams.messages.MessageKey;
import java.util.ArrayList;
import java.util.List;

@SuppressWarnings("serial")
public class SubmissionTimeErrorException extends SAMException {
  private List<String> duplicateList = null,
      missingList = null,
      ambiguousList = null,
      extraValues = null;

  public SubmissionTimeErrorException(MessageKey key, Object... subst) {
    super(key, subst);
  }

  public List<String> getDuplicatesList() {
    if (duplicateList == null) {
      duplicateList = new ArrayList<String>();
    }
    return duplicateList;
  }

  public List<String> getMissingList() {
    if (missingList == null) {
      missingList = new ArrayList<String>();
    }
    return missingList;
  }

  public List<String> getAmbiguousList() {
    if (ambiguousList == null) {
      ambiguousList = new ArrayList<String>();
    }
    return ambiguousList;
  }

  public List<String> getExtraValues() {
    if (extraValues == null) {
      extraValues = new ArrayList<String>();
    }
    return extraValues;
  }

  public String toPrintableString() {
    String ret = getMessage() + "\n";
    for (String s : getExtraValues()) {
      ret += "CDISR1149W EXTRA_PARAM_WARNING: " + s + "\n";
    }
    for (String s : getMissingList()) {
      ret += "CDISR1146E MISSING_PARAM: " + s + "\n";
    }
    for (String s : getAmbiguousList()) {
      ret += "CDISR1148E AMBIGUOUS_PARAM: " + s + "\n";
    }
    for (String s : getDuplicatesList()) {
      ret += "CDISR1147E DUPLICATE_PARAM: " + s + "\n";
    }
    return ret;
  }
}
