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

import static org.junit.Assert.*;

import com.ibm.streams.admin.internal.api.StreamsException;
import com.ibm.streams.instance.sam.model.Job;
import java.math.BigInteger;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Properties;
import java.util.SortedMap;
import lombok.var;
import org.junit.Rule;
import org.junit.Test;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class PipelineTest {

  private static final Logger logger = LoggerFactory.getLogger(PipelineTest.class);

  @Rule public StreamsJobResource appsPar = new StreamsJobResource("apps.parallel.Parallel.adl");

  @Rule
  public StreamsJobResource splNestedUdp =
      new StreamsJobResource("spl.runtime.udpNestedPEBoundary.adl");

  /*
   * (2 + 2 + 2) == 6 parallel ops
   */
  private final Properties modestOps =
      new Properties() {
        {
          put("width1", "2");
          put("width2", "2");
          put("width3", "2");
        }
      };
  private final Map<String, BigInteger> modestOpsParallelRegion =
      new HashMap<String, BigInteger>() {
        {
          put("Result.Region1", BigInteger.valueOf(2));
          put("Result.Region2", BigInteger.valueOf(2));
          put("Result.Region3", BigInteger.valueOf(2));
        }
      };
  private final int numModestOps = 6;

  /*
   * (100 + 200 + 45) == 345 parallel ops
   */
  private final Properties largeOps =
      new Properties() {
        {
          put("width1", "100");
          put("width2", "200");
          put("width3", "45");
        }
      };
  private final Map<String, BigInteger> largeOpsParallelRegion =
      new HashMap<String, BigInteger>() {
        {
          put("Result.Region1", BigInteger.valueOf(100));
          put("Result.Region2", BigInteger.valueOf(200));
          put("Result.Region3", BigInteger.valueOf(45));
        }
      };
  private final int numLargeOps = 345;

  private final int sourceAndSink = 2;

  private void assertPortLabelsExist(Job job) {
    SortedMap<BigInteger, List<InputPortLabel>> portLabelMap =
        appsPar.getPipeline().getPortLabels(job);
    assertTrue(portLabelMap.size() > 0);
  }

  private void assertAADLsExact(Job job, int num) {
    SortedMap<BigInteger, String> aadlList = null;
    try {
      aadlList = appsPar.getPipeline().getAADLs(job);
    } catch (StreamsException e) {
      fail("error getting AADLs exact: " + e);
    }
    assertEquals(num, aadlList.size());
  }

  private void assertAADLsExist(Job job) {
    SortedMap<BigInteger, String> aadlList = null;
    try {
      aadlList = appsPar.getPipeline().getAADLs(job);
    } catch (StreamsException e) {
      fail("error getting AADLs exist: " + e);
    }
    assertTrue(aadlList.size() > 0);
  }

  @Test
  public void manualModestTest() {
    Job job = appsPar.createJobManual(modestOps, 5);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExact(job, 5);
  }

  @Test
  public void manualLargeTest() {
    Job job = appsPar.createJobManual(largeOps, 38);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExact(job, 38);
  }

  @Test
  public void automaticModestTest() {
    Job job = appsPar.createJobAutomatic(modestOps);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExist(job);
  }

  @Test
  public void automaticLargeTest() {
    Job job = appsPar.createJobAutomatic(largeOps);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExist(job);
  }

  @Test
  public void legacyModestTest() {
    Job job = appsPar.createJobLegacy(modestOps);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExact(job, numModestOps + sourceAndSink);
  }

  @Test
  public void legacyLargeTest() {
    Job job = appsPar.createJobLegacy(largeOps);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExact(job, numLargeOps + sourceAndSink);
  }

  @Test
  public void legacyLargeTestParallelRegion() {
    Job job = appsPar.createJobLegacy(largeOpsParallelRegion);
    assertNotNull(job);
    assertPortLabelsExist(job);
    assertAADLsExact(job, numLargeOps + sourceAndSink);
  }

  @Test
  public void parallelDiffIncreaseTest() {
    var originalJob =
        appsPar.createJobLegacy(modestOpsParallelRegion, Pipeline.RESET_OBJECT_TRACKER);
    assertNotNull(originalJob);
    assertPortLabelsExist(originalJob);
    assertAADLsExact(originalJob, numModestOps + sourceAndSink);

    var numOpIncrease = BigInteger.valueOf(3);
    final Map<String, BigInteger> updatedOpsParallelRegion =
        new HashMap<String, BigInteger>() {
          {
            put("Result.Region1", modestOpsParallelRegion.get("Result.Region1"));
            put("Result.Region2", modestOpsParallelRegion.get("Result.Region2").add(numOpIncrease));
            put("Result.Region3", modestOpsParallelRegion.get("Result.Region3"));
          }
        };

    var updatedJob =
        appsPar.createJobLegacy(updatedOpsParallelRegion, Pipeline.KEEP_OBJECT_TRACKER);
    assertNotNull(updatedJob);
    assertPortLabelsExist(updatedJob);
    assertAADLsExact(updatedJob, numModestOps + numOpIncrease.intValue() + sourceAndSink);

    var changedOperators = new HashSet<String>();
    var addedOperators = new HashSet<String>();
    var removedOperators = new HashSet<String>();

    try {
      appsPar
          .getPipeline()
          .transferParallelRegionDifference(
              originalJob,
              updatedJob,
              "Result.Region2",
              changedOperators,
              addedOperators,
              removedOperators);
    } catch (Exception e) {
      fail("error calling transferparallelRegionDifference: " + e);
    }

    assertTrue(changedOperators.size() == 6);
    assertTrue(addedOperators.size() == numOpIncrease.intValue());
    assertTrue(removedOperators.size() == 0);
  }

  @Test
  public void parallelDiffDecreaseTest() {
    var originalJob =
        appsPar.createJobLegacy(largeOpsParallelRegion, Pipeline.RESET_OBJECT_TRACKER);
    assertNotNull(originalJob);
    assertPortLabelsExist(originalJob);
    assertAADLsExact(originalJob, numLargeOps + sourceAndSink);

    var numOpDecrease = BigInteger.valueOf(50);
    final Map<String, BigInteger> updatedOpsParallelRegion =
        new HashMap<String, BigInteger>() {
          {
            put(
                "Result.Region1",
                largeOpsParallelRegion.get("Result.Region1").subtract(numOpDecrease));
            put("Result.Region2", largeOpsParallelRegion.get("Result.Region2"));
            put("Result.Region3", largeOpsParallelRegion.get("Result.Region3"));
          }
        };

    var updatedJob =
        appsPar.createJobLegacy(updatedOpsParallelRegion, Pipeline.KEEP_OBJECT_TRACKER);
    assertNotNull(updatedJob);
    assertPortLabelsExist(updatedJob);
    assertAADLsExact(updatedJob, numLargeOps - numOpDecrease.intValue() + sourceAndSink);

    var changedOperators = new HashSet<String>();
    var addedOperators = new HashSet<String>();
    var removedOperators = new HashSet<String>();

    try {
      appsPar
          .getPipeline()
          .transferParallelRegionDifference(
              originalJob,
              updatedJob,
              "Result.Region1",
              changedOperators,
              addedOperators,
              removedOperators);
    } catch (Exception e) {
      fail("error calling transferParallelRegionDifference: " + e);
    }

    assertTrue(changedOperators.size() == 251);
    assertTrue(addedOperators.size() == 0);
    assertTrue(removedOperators.size() == numOpDecrease.intValue());
  }
}
