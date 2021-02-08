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

package com.ibm.streams.application.models.spl.logical;

import java.util.List;

public class Annotation {
  private final String tag;
  private final Boolean isInternal;
  private final List<AnnotationKeyValue> keyValues;

  public Annotation(String tag, Boolean isInternal, List<AnnotationKeyValue> keyValues) {
    super();
    this.tag = tag;
    this.isInternal = isInternal;
    this.keyValues = keyValues;
  }

  public String getTag() {
    return tag;
  }

  public Boolean getIsInternal() {
    return isInternal;
  }

  public List<AnnotationKeyValue> getKeyValues() {
    return keyValues;
  }
}
