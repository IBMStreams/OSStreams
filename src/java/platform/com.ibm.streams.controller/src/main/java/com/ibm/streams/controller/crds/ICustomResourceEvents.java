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

package com.ibm.streams.controller.crds;

public interface ICustomResourceEvents {

  String JOB_SUBMISSION_SUCCESS = "SubmissionSucceeded";
  String JOB_SUBMISSION_FAILURE = "SubmissionFailed";

  String PARALLEL_REGION_WIDTH_CHANGE_SUCCESS = "ParallelRegionWidthChangeSuccess";
  String PARALLEL_REGION_WIDTH_CHANGE_FAILURE = "ParallelRegionWidthChangeFailure";

  String TESTSUITE_SELECTION_SUCCESS = "SelectionSucceeded";
  String TESTSUITE_SELECTION_FAILURE = "SelectionFailed";

  String TESTSUITE_ABORTION = "RunAborted";
  String TESTSUITE_SUCCESS = "RunSucceeded";
  String TESTSUITE_FAILURE = "RunFailed";
}
