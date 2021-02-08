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

package com.ibm.streams.controller.crds.jobs;

import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitted;
import static com.ibm.streams.controller.crds.jobs.EJobSubmissionStatus.Submitting;

public class JobStatusTransitions {

  public static boolean gainedId(Job pre, Job cur) {
    return pre.getSpec().getId() == null && cur.getSpec().getId() != null;
  }

  public static boolean becameSubmitted(Job pre, Job cur) {
    return pre.getSpec().getStatus() == Submitting && cur.getSpec().getStatus() == Submitted;
  }

  public static boolean revertedToSubmitting(Job pre, Job cur) {
    return pre.getSpec().getStatus() == Submitted && cur.getSpec().getStatus() == Submitting;
  }
}
