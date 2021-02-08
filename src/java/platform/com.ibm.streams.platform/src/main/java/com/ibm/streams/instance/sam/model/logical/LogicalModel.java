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

package com.ibm.streams.instance.sam.model.logical;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.instance.sam.exception.SAMInternalErrorException;
import com.ibm.streams.instance.sam.exception.SubmissionTimeErrorException;
import com.ibm.streams.instance.sam.exception.TooManyStartOperatorsException;
import com.ibm.streams.instance.sam.model.topology.TopologyApplication;
import java.math.BigInteger; // SCOTT_HACK
import java.util.Map; // SCOTT_HACK
import java.util.Properties;

public interface LogicalModel {

  @Override
  public abstract String toString();

  public abstract void performTransforms() throws StreamsException;

  public abstract void computeCCRegions()
      throws SAMInternalErrorException, TooManyStartOperatorsException;

  // This method is a temporary hack so that Scott can work on transformer and topology diff logic
  // before we
  // figure out exactly how to pass this information through the configs. Once we do, we will go
  // back to
  // a single version of generateTopology.
  // SCOTT_HACK
  public abstract TopologyApplication generateTopology(
      Properties cfgs, Map<String, BigInteger> parRegionNameToWidth) throws StreamsException;

  public abstract TopologyApplication generateTopology(Properties cfgs) throws StreamsException;

  // Function to verify instance setup for this application.
  // Current, with two kind of verification:
  // 1. When application need checkpoint enabled (with operator or Consistent Region),
  //    verify that instance has correct checkpoint setup.
  // 2. When application operator need setcap, verify that instance is set up for it.
  public abstract void instanceVerificationForApp() throws StreamsException;

  public abstract String toJSON();

  public abstract SubmissionTimeErrorException getSubmissionTimeWarning();

  public abstract void forPreviewSubmitjob();
}
