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

package com.ibm.streams.controller.instance.sam;

import static org.junit.Assert.fail;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.admin.internal.api.trace.StreamsLogger.StreamsLogLevel;
import com.ibm.streams.admin.internal.api.trace.TraceLogger;
import com.ibm.streams.instance.sam.model.Job;
import com.ibm.streams.instance.sam.model.logical.LogicalModel;
import com.ibm.streams.mock.instance.MockResource;
import com.ibm.streams.sch.composer.ComposingInstructions;
import java.io.FileInputStream;
import java.io.IOException;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;
import java.util.Properties;
import lombok.Getter;
import org.apache.commons.io.IOUtils;
import org.junit.rules.ExternalResource;

public class StreamsJobResource extends ExternalResource {
  @Getter private Pipeline pipeline;
  private String adlRelativePath;
  private String adl;

  public StreamsJobResource(String adlRelativePath) {
    this.adlRelativePath = adlRelativePath;
  }

  private String createADL() {
    String adl = null;
    String filePath = MockResource.APPS_PATH + adlRelativePath;
    try {
      FileInputStream adlFile = new FileInputStream(filePath);
      adl = IOUtils.toString(adlFile, "UTF-8");
    } catch (IOException e) {
      fail("can't open " + filePath + " when creating ADL: " + e);
    }
    return adl;
  }

  @Override
  protected void before() {
    adl = createADL();
    pipeline = new Pipeline(StreamsLogLevel.WARN, TraceLogger.Level.WARN);
  }

  public Job createJobAutomatic(Properties props) {
    return createJob(
        props,
        Pipeline.createManualFuse(1),
        new HashMap<String, BigInteger>(),
        Pipeline.RESET_OBJECT_TRACKER);
  }

  public Job createJobLegacy() {
    return createJob(
        new Properties(),
        Pipeline.createLegacyFuse(),
        new HashMap<String, BigInteger>(),
        Pipeline.RESET_OBJECT_TRACKER);
  }

  public Job createJobLegacy(Properties props) {
    return createJob(
        props,
        Pipeline.createLegacyFuse(),
        new HashMap<String, BigInteger>(),
        Pipeline.RESET_OBJECT_TRACKER);
  }

  public Job createJobLegacy(Map<String, BigInteger> parWidths) {
    return createJob(
        new Properties(), Pipeline.createLegacyFuse(), parWidths, Pipeline.RESET_OBJECT_TRACKER);
  }

  public Job createJobLegacy(Map<String, BigInteger> parWidths, boolean resetObjectTracker) {
    return createJob(new Properties(), Pipeline.createLegacyFuse(), parWidths, resetObjectTracker);
  }

  public Job createJobManual(Properties props, int numPes) {
    return createJob(
        props,
        Pipeline.createManualFuse(numPes),
        new HashMap<String, BigInteger>(),
        Pipeline.RESET_OBJECT_TRACKER);
  }

  private Job createJob(
      Properties props,
      ComposingInstructions compose,
      Map<String, BigInteger> parWidths,
      boolean resetObjectTracker) {
    Job job = null;
    try {
      LogicalModel logical =
          pipeline.createLogicalModel(
              BigInteger.ZERO, adl, props, new Properties(), resetObjectTracker);
      job =
          pipeline.createJob(
              "job" + Objects.hashCode(props) + Objects.hashCode(parWidths),
              adl,
              logical,
              compose,
              new Properties(),
              parWidths);
    } catch (StreamsException e) {
      fail("StreamsException when creating job: " + e);
    }
    return job;
  }
}
