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

public class ComposingInstructionFactory {

  public static ComposingInstructions createConstraintFusingOnly() {
    return new ComposingInstructions(
        ComposingStyle.FUSE_TO_MAX_WITHIN_CONSTRAINTS,
        null,
        FusingStrategy._defaultFusingScript_forAutomatic);
  }

  public static ComposingInstructions createFuseToNum(Integer num) {
    return new ComposingInstructions(
        ComposingStyle.FUSE_TO_AVAIL, num, FusingStrategy._defaultFusingScript_forAutomatic);
  }

  public static ComposingInstructions createManualFuse(Integer num, String fusStrategy) {
    return new ComposingInstructions(ComposingStyle.FUSE_MANUAL, num, fusStrategy);
  }

  public static ComposingInstructions createAutomaticFuse(String fusStrategy) {
    return new ComposingInstructions(ComposingStyle.FUSE_AUTOMATIC, null, fusStrategy);
  }

  public static ComposingInstructions createLegacyFuse(String fusStrategy) {
    return new ComposingInstructions(ComposingStyle.FUSE_LEGACY, null, fusStrategy);
  }

  public static ComposingInstructions createFuseToPercent(Integer percent) {
    return new ComposingInstructions(
        ComposingStyle.FUSE_TO_AVAIL, null, FusingStrategy._defaultFusingScript_forAutomatic);
  }

  public static ComposingInstructions createFuseToOne() {
    return new ComposingInstructions(
        ComposingStyle.FUSE_TO_ONE, null, FusingStrategy._defaultFusingScript_forAutomatic);
  }

  public static ComposingInstructions createEmpty() {
    return new ComposingInstructions(null, null, FusingStrategy._defaultFusingScript_forAutomatic);
  }
}
