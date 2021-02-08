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

package com.ibm.streams.instance.sam.model;

// Derived objects should override hashCode() for change detection
// Derived objects should not be used as keys in hash-based collections (e.g. HashSet, HashMap)
// since hashCode() contains mutable fields
public abstract class SamObject {

  private final boolean _isPreviewOnly;

  public SamObject(boolean isPreviewOnly) {
    _isPreviewOnly = isPreviewOnly;
  }

  public abstract boolean isJobConstructed();

  public boolean isPreviewOnly() {
    return _isPreviewOnly;
  }
}
