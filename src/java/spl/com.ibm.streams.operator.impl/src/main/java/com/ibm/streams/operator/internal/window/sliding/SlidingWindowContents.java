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

package com.ibm.streams.operator.internal.window.sliding;

import java.util.LinkedList;

/**
 * Maintains the state of a sliding window partition including the tuples and if an initial full has
 * been seen.
 */
public class SlidingWindowContents<W> extends LinkedList<W> {
  /** */
  private static final long serialVersionUID = -2371039230060720506L;

  private boolean seenWindowFull;

  /**
   * Notify the window partition as becoming full returns true if this is the first time, otherwise
   * false.
   *
   * @return
   */
  boolean isWindowFullForFirstTime() {
    if (seenWindowFull) return false;
    return seenWindowFull = true;
  }

  boolean seenWindowFull() {
    return seenWindowFull;
  }
}
