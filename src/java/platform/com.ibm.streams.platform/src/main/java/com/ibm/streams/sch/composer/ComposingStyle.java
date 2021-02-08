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

public enum ComposingStyle {
  FUSE_TO_ONE("fuse_to_one"),
  FUSE_TO_MAX_WITHIN_CONSTRAINTS("fuse_to_max"),
  FUSE_TO_MIN_WITHIN_CONSTRAINTS("fuse_to_min"),
  FUSE_LEGACY("legacy"),
  FUSE_AUTOMATIC("automatic"),
  FUSE_MANUAL("manual"),
  NO_FUSING_IGNORING_CONSTRAINTS("no_fusing"),
  FUSE_TO_AVAIL("fuse_to_avail");

  private String _style;

  public String toString() {
    return _style;
  }

  ComposingStyle(String style) {
    _style = style;
  }
}
